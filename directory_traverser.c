/**
 * https://github.com/schmkls/do-with-all-files
 * 
 * Module used for traversing files in parallell with a given number of threads, 
 * and calling a given function with given argument with each encountered file's
 * path as input. 
 * 
 * The interface consists only of the function "do_with_all_files" 
 */
#include "get_opts_help.h"
#include "directory_traverser.h"
#include "queue.h"     
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>


//used to help store function and argument
struct Func_and_arg {
    void (*do_with_file)(char *file_path, void *arg);
    void *arg;
};

//used to coordinate work so each file is only traversed once
struct Traverser {
    Queue *traversed_files;

    Func_and_arg *do_with_file; 

    int t_waiting;              //count of waiting threads
    int thread_size;            //num of threads working on directory 
    bool finished;

    pthread_mutex_t modify_lock; //used to lock so only one thread modifies dir at a time
    sem_t work_to_do;
};

//used to hold all info needed for threads to be sent to work
struct Options {
    Traverser **traversers;                 //will be equal to files_size
    int files_size;                           
    int nr_threads_to_use;

    int success_status;
};


static bool is_navigationfile(char *path) {
    if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0) {
        return true;
    }
    return false;
}


static char *append_path(char *destination, char *path, char *sub_path) {
    int len = strlen(path) + strlen(sub_path) + strlen("/") + 1;
    destination = (char*)realloc(destination, len * sizeof(char));
    strcpy(destination, path);
    strcat(destination, "/");
    strcat(destination, sub_path);

    return destination;
}

/**
 * Enqueues sub-directories of given directory to given queue
 * 
 * Calls the function stored in given function-and-argument-struct on 
 * each non-directory sub-file with sub-file and argument
 * stored in the struct as input.
 * 
 * @param dir_path path to directory
 * @param fq file-queue to be filled with sub-directories
 * @param func_and_arg stores users function and argument
 * @return int 0 on success, anything else indicates error
 */
static int enqueue_sub_dirs_do_with_sub_files(char *dir_path, Queue *fq, Func_and_arg *func_and_arg) {
    DIR *dir;
    struct dirent *dir_pointer;
    char *temp_file = NULL;
    struct stat temp_file_stats;
    int ret_status = SUCCESS;

    if ((dir = opendir(dir_path)) == NULL) {
        fprintf(stderr, "do-with-all-files: cannot read files in directory '%s'\n", dir_path); 
        closedir(dir);
        return FAILURE;
    }

    while ((dir_pointer = readdir(dir)) != NULL) {
        if (!is_navigationfile(dir_pointer->d_name)) {
            temp_file = append_path(temp_file, dir_path, dir_pointer->d_name);

            if ((lstat(temp_file, &temp_file_stats)) < 0) {
                fprintf(stderr, "do-with-all-files: can not traverse '%s'\n", temp_file);
                closedir(dir);
                free(temp_file);
                ret_status = FAILURE;
            }
            
            if (S_ISDIR(temp_file_stats.st_mode)) { 
                queue_enqueue(fq, temp_file);   
            } else {
                func_and_arg->do_with_file(temp_file, func_and_arg->arg);
            }
        }
    }
    
    closedir(dir);
    free(temp_file);

    return ret_status;   
}


/**
 * Calls the function stored in func_and_arg with file_path and argument stored
 * in func_and_arg as input. If file is a directory, returns queue with sub_directories
 * and does the function to all non-directory sub-files.  
 * 
 * @param func_and_arg function-and-argument struct storing users function and argument
 * @param file_path 
 * @return Queue* containing sub-directories of given file
 */
static Queue *do_to_file_and_get_subfiles(Func_and_arg *func_and_arg, char *file_path) {
    struct stat temp_file_stats;

    Queue *sub_dirs;
    if ((sub_dirs = queue_create()) == NULL) {
        fprintf(stderr, "do-with-all-files: can not traverse '%s'\n", file_path);
        return NULL;
    }

    if ((lstat(file_path, &temp_file_stats)) < 0) {
        fprintf(stderr, "do-with-all-files: can not traverse '%s'\n", file_path);
        queue_destroy(sub_dirs);
        return NULL;
    }

    func_and_arg->do_with_file(file_path, func_and_arg->arg);

    if (S_ISDIR(temp_file_stats.st_mode)) {
        if (enqueue_sub_dirs_do_with_sub_files(file_path, sub_dirs, func_and_arg) != 0) {
            return NULL;
        }
    }

    return sub_dirs;
}


/**
 * Work-loop for one thread. The loop synchronizes traversal
 * thread-safely between threads, and lets thread quit when
 * all threads has seen that there is no more work.
 * 
 * See the image 'worker_loop' on git to understand abstraction:
 * (https://github.com/schmkls/do-with-all-files/blob/main/worker_loop.jpeg)
 * 
 * @param trav traverser-struct 
 * @return int 0 on success, anything else indicates an error
 */
static int traverse_file(Traverser *trav) {
    char *temp_file;
    Queue *tmp_file_queue = queue_create();
    
    while (1) {        
        pthread_mutex_lock(&trav->modify_lock);

        if (!queue_is_empty(trav->traversed_files)) {
            
            if ((temp_file = queue_dequeue(trav->traversed_files)) == NULL) {
                free(temp_file);
                queue_destroy(tmp_file_queue);
                sem_post(&trav->work_to_do);      //lure someone to work
                return FAILURE;
            }
            
        
            pthread_mutex_unlock(&trav->modify_lock);

            queue_destroy(tmp_file_queue);
            if ((tmp_file_queue = do_to_file_and_get_subfiles(trav->do_with_file, temp_file)) == NULL) {
                free(temp_file);
                sem_post(&trav->work_to_do);      //lure someone to work
                return FAILURE;
            }
            
            free(temp_file);

            pthread_mutex_lock(&trav->modify_lock);

            //get directories from temporary directory-usage-queue
            while (!queue_is_empty(tmp_file_queue)) {
                char *tmp = queue_dequeue(tmp_file_queue);
                queue_enqueue(trav->traversed_files, tmp);
                sem_post(&trav->work_to_do);      //work to do since file was enqueued
                trav->t_waiting--;                //one less thread expected to wait since work to do
                free(tmp);
            }
        }

        trav->t_waiting++;                                   //thread goes home and waits until someone calls from work

        if (trav->t_waiting > trav->thread_size) {           //thread wont work if all other threads have gone home
            trav->finished = true;
            sem_post(&trav->work_to_do);                     //lures another thread to work to check if someone is there
            pthread_mutex_unlock(&trav->modify_lock);
            break;
        }

        pthread_mutex_unlock(&trav->modify_lock);

        sem_wait(&trav->work_to_do);           //wait until someone at work made more work

        if (trav->finished) {                  //work is no more
            sem_post(&trav->work_to_do);       //tell the next worker to come see they too are unemployed
            break;
        }
    }

    queue_destroy(tmp_file_queue);
    
    return SUCCESS;
}

/**
 * Traverses the files stored in given options-struct
 * Sets opts's success-status to indicate error or success. 
 * 
 * @param arg options-struct
 * @return NULL
 */
static void *traverse_directories(void *arg) {
    Options *opts = (Options*)arg;

    for (int i = 0; i < opts->files_size; i++) {
        if (traverse_file(opts->traversers[i]) != 0) {
            fprintf(stderr, "do-with-all-files: error traversing files\n");
            opts->success_status = FAILURE;
        }
    }

    return NULL;
}


/**
 * Sends threads to traverse files with the help of info stored in opts. 
 * Sets opts's success-status to indicate error or success. 
 * 
 * @param opts where options for traversal are stored (what files to traverse, number of threads to use, etc.)
 */
static void send_threads_to_do_with_files(Options *opts) {
    pthread_t threads[opts->nr_threads_to_use];

    for (int i = 0; i < opts->nr_threads_to_use; i++) {
        if ((pthread_create(&threads[i], NULL, traverse_directories, (void*)opts)) != 0) {
            perror("pthread create");
            opts->success_status = FAILURE;
        }
    }

    for (int i = 0; i < opts->nr_threads_to_use; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            opts->success_status = FAILURE;
        }
    }
}



//------------------------------creation and destruction of structs--------------------------------//

static Traverser *create_Traverser(char *top_dir_path, int thread_size, Func_and_arg *func_and_arg) {
    Traverser *trav = calloc(1, sizeof(Traverser));
    
    trav->traversed_files = queue_create();
    queue_enqueue(trav->traversed_files, top_dir_path);
    trav->do_with_file = func_and_arg;    
    trav->t_waiting = 0;
    trav->thread_size = thread_size;
    trav->finished = false;
    if (pthread_mutex_init(&trav->modify_lock, NULL) != 0) {
        return NULL;
    }
    sem_init(&trav->work_to_do, 0, 1);
    
    return trav;
}


static void destroy_Func_and_arg(Func_and_arg *func_and_arg) {
    free(func_and_arg);
    func_and_arg = NULL;
}


static void destroy_Traverser(Traverser *trav) {
    queue_destroy(trav->traversed_files);
    pthread_mutex_destroy(&trav->modify_lock);
    sem_destroy(&trav->work_to_do);

    free(trav);
}


static Func_and_arg *create_Func_and_arg(void (*do_with_file)(char *file_path, void *arg), void *arg) {
    Func_and_arg *func_and_arg;
    if ((func_and_arg = calloc(1, sizeof(Func_and_arg))) == NULL) {
        return NULL;
    }
    func_and_arg->do_with_file = do_with_file;
    func_and_arg->arg = arg;
    return func_and_arg;
}


static void destroy_Options(Options *opts) {
    if (opts != NULL) {
        free(opts->traversers[0]->do_with_file);    //all traversers share the same do_with_file so it only has to be freed once
        for (int i = 0; i < opts->files_size; i++) { 
            destroy_Traverser(opts->traversers[i]);
        }
        free(opts->traversers);
        free(opts);
    }
}

//returns NULL if options could not be created
static Options *create_Options(int files_size, char **files, void (*do_with_file)(char *file_path, void *arg), void *arg, int nr_threads_to_use) {
    Options *user_opts;
    Func_and_arg *func_and_arg;

    if ((func_and_arg = create_Func_and_arg(do_with_file, arg)) == NULL) {
        return NULL;
    }

    if ((user_opts = calloc(1, sizeof(Options))) == NULL) {
        destroy_Func_and_arg(func_and_arg);
        return NULL;
    }

    user_opts->nr_threads_to_use = nr_threads_to_use;       //one thread by default
    user_opts->files_size = files_size;   
    user_opts->success_status = SUCCESS; 
    
    if ((user_opts->traversers = calloc(files_size, sizeof(Traverser*))) == NULL) {
        return NULL;
    }

    for (int i = 0; i < user_opts->files_size; i++){     
        if ((user_opts->traversers[i] = create_Traverser(files[i], nr_threads_to_use, func_and_arg)) == NULL) {
            destroy_Options(user_opts);
            return NULL;
        } 
    }

    return user_opts;
}



//------------------------------the function/interface--------------------------------//

int do_with_all_files(void (*do_with_file)(char *file_path, void *arg), void *arg, char **files, 
                        int files_size, int num_threads) {
    
    int success_status = SUCCESS;

    Options *user_opts;                       //stores user options and info for threads work/coordination
    if ((user_opts = create_Options(files_size, files, do_with_file, arg, num_threads)) == NULL) {
        fprintf(stderr, "do-with-all-files: can not run\n");
        return FAILURE;
    }

    send_threads_to_do_with_files(user_opts);   //the traversing and doing, sets user_opt's success-status
    success_status = user_opts->success_status;
    destroy_Options(user_opts);                 //freeing allocated memory
    return success_status;
}










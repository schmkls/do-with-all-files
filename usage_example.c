/**
 * GIT-link
 * Example to show how do-something-with-all-files can be used. 
 * 
 * The example emulates the command "du -ls -B512", that
 * estimates file space usage of given files. 
 * 
 * How to use (this example) on Linux:
 * 1. Make program ready for use by running 'make' in same catalog where source code (from GIT-link) is located
 * 2. Run ./dswaf [file name] [number of threads to use]"
 * 
 * Explanation of arguments:
 * [file name] files whos space usage will be estimated, for directory-files 
 *                all files in directory and subdirectories will be included
 * 
 * [-j number of threads to use] number of threads that will be used to traverse given file in paralell
 */

#include "directory_traverser.h"
#include "get_opts_help.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>

//success-statuses
#define SUCCESS 0
#define FAILURE 1


/**
 * Struct that will be used as argument to 'do_with_all_files'  
 * 
 * Holds usage of one one file and has objects used to 
 * coordinate threads to count up the usage
 */
typedef struct file_usage {
    int success_status;
    char *file;
    long usage;
    pthread_mutex_t modify_lock;
} file_usage;


/**
 * The function that will be used in 'do_with_all_files' 
 * 
 * Counts up given file-usage with usage of given file
 * 
 * @param file_path path to file
 * @param arg file-usage struct
 */
void count_up_file_size(char *file_path, void *arg) {
    file_usage *fu = (file_usage*)arg;
    struct stat temp_file_stats;
    
    if ((lstat(file_path, &temp_file_stats)) < 0) {
        fprintf(stderr, "Could not get usage of '%s'\n", file_path);
        fu->success_status = FAILURE;
        return;
    }

    pthread_mutex_lock(&fu->modify_lock);
    fu->usage += temp_file_stats.st_blocks;     //one thread at a time counts up fu with file's usage
    pthread_mutex_unlock(&fu->modify_lock);
}

/**
 * Checks arguments to program
 * If incorrect arguments: exits on failure
 * 
 * @param argc 
 * @param argv 
 */
static void arg_check(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage_example: How to use the example: ./dswaf [file name] [number of threads to use]\n");
        exit(EXIT_FAILURE);
    }

    if (!is_number_above(argv[2], 0)) {
        fprintf(stderr, "usage_example: How to use the example: ./dswaf [file name] [number of threads to use]\n");
        fprintf(stderr, "(third argument should be positive integer)\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Free allocated memory for file-usage struct
 * @param fu file-usage whos memory should be freed 
 */
static void destroy_file_usage(file_usage *fu) {
    pthread_mutex_destroy(&fu->modify_lock);
    free(fu->file);
    free(fu);
}

/**
 * Create a file-usage struct
 * 
 * @param file path to file whos usage is stored in file-usage struct
 * @return file_usage* created file-usage
 */
static file_usage *create_file_usage(char *file) {
    file_usage *fu;
    if ((fu = calloc(1, sizeof(file_usage))) == NULL) {
        return NULL;
    }
    if (pthread_mutex_init(&fu->modify_lock, NULL) != 0) {
        return NULL;
    }
    fu->success_status = SUCCESS;
    fu->usage = 0;
    fu->file = strdup(file);

    return fu;
}

/**
 * Creates files-list (used in 'do_with_all_files' as files argument),  
 * containing only the file of given file-usage
 * 
 * On failure: frees allocated memory for file-usage and exits
 * 
 * @param fu file-usage whos file will be in created file-list
 * @return char** 
 */
static char **create_single_files_args(file_usage *fu) {
    char **files_args;

    if ((files_args = calloc(1, sizeof(char*))) == NULL) {
        destroy_file_usage(fu);
        fprintf(stderr, "usage_example: error1 creating file arguments used to show example usage of 'do_with_all_files'");
        exit(EXIT_FAILURE);
    }

    if ((files_args[0] = strdup(fu->file)) == NULL) {
        free(files_args);
        destroy_file_usage(fu);
        fprintf(stderr, "usage_example: error2 creating file arguments used to show example usage of 'do_with_all_files'");
        exit(EXIT_FAILURE);
    }

    return files_args;
}


static void print_file_usage(file_usage *fu) {
    printf("Space usage of '%s':\t%ld\n", fu->file, fu->usage);
}


int main(int argc, char **argv) {
    file_usage *fu; 
    char **files_args;
    int exit_status = EXIT_SUCCESS;

    arg_check(argc, argv);

    if ((fu = create_file_usage(argv[1])) == NULL) {
        fprintf(stderr, "usage_example: error1 creating arguments used to show example usage of 'do_with_all_files'");
        exit(EXIT_FAILURE);
    }

    files_args = create_single_files_args(fu);

    //the function that do-with-all-files provides
    if ((do_with_all_files(count_up_file_size, (void*)fu, files_args, 1, atoi(argv[2])) != 0)) {
        exit_status = EXIT_FAILURE;
    }

    if (fu->success_status != SUCCESS) {
        printf("usage_example: usage of '%s' could not be estimated succesfully\n", fu->file);
    }

    print_file_usage(fu);
    
    free(files_args[0]);
    free(files_args);
    destroy_file_usage(fu);
    
    return exit_status;
}
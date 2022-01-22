/**
 * https://github.com/schmkls/do-with-all-files
 * 
 * Module used for traversing files in parallell with a given number of threads, 
 * and calling a given function with given argument with each encountered file's
 * path as input. 
 * 
 * The interface consists only of the function "do_with_all_files" 
 */

#ifndef DIR_TRAV_H
#define DIR_TRAV_H

#define FAILURE 1
#define SUCCESS 0

typedef struct Traverser Traverser;
typedef struct Func_and_arg Func_and_arg;
typedef struct Options Options;

int do_with_all_files(void (*do_with_file)(char *file_path, void *arg), void *arg, char **files, int directories_size, int num_threads);

#endif

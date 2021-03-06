# do-with-all-files
C-function

__```int do_with_all_files(void (*do_with_file)(char *file_path, void *arg), void *arg, char **files, int files_size, int num_threads)```__
>
for calling a function *do_with_file* with each file in *files* as input, recursively for directories. Inspired by the Linux *du* command. 

### How to use *do_with_all_files*
1. Get all the files in the Github-repository. *Makefile* and *usage_example.c* are not needed (they are only used for showing example usage). 
2. Include *directory_traverser.h* in your program
3. Use the function in your program
4. Compile like: \
  ```gcc -o out yourpgrogram.c -pthread directory_traverser.c queue.c list.c get_opts_help.c```

### Explanation of parameters
* ```void (*do_with_file)(char *file_path, void *arg)```: function to be called for each file
* ```void *arg```: argument for the *do_with_file*-function
* ```char **files```: files to traverse (directories are traversed recursively)
* ```int file_size```: number of *files* 
* ```int num_threads```: number of threads used for traversal

### Return value and errors
0 on success. Anything else indicates an error. \
Error messages are printed to *stderr*.  

### What *do_with_all_files* does
*do_with_all_files* 

*do_with_all_files* traverses the given *files*, and calls the given function *do_with_file* with each encountered file and the given argument *arg* as input. If encountered files is directory, its sub-files is traversed in parallell using *num_threads* threads. 

### Thread safety
The traversing is thread safe, but several threads can be calling given function *do_with_file* at the same time. Therefore, for thread safe usage of *do_with_all_files*, threads must be synchronized in given function *do_with_file* to avoid data-races when accessing the given *arg*. In the example *usage_example.c* the function *count_up_file_size* is coordinated to be thread safe with the help of mutex-locks. 

### Try the example (on Linux)
*usage_example.c* provides an example of how to use *do_with_all_files* with explanatory comments. Use it like: 
1. Get all the files in the Github-repository.
2. Run ([gcc](https://gcc.gnu.org/) requiered): \
  ``` make ```
3. Run: \
  ``` ./dwaf [file] [number of threads] ```

### Please give feedback in Discussions->General

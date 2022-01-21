# do-with-all-files
C-function 

> int do_with_all_files(void (\*do_with_file)(char \*file_path, void \*arg), void \*arg, char \*\*directories, int directories_size, int num_threads)
> 
for traversing files/directories in parallell and calling a given function with each encountered file and a void-pointer as parameters. 

### How to use the function
1. Get all the files in the Github-repository. *Makefile* and *usage_example.c* are not needed (they are only used for showing example usage). 
2. Use the function in your program and compile like:
  > gcc -o out yourpgrogram.c -pthread -directory_traverser.c queue.c list.c get_opts_help.c

### Explanation of parameters


### What the function does

### Try the example

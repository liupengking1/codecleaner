### INFORMATION:          

This is a program to remove C/C++ comments and empty lines in several files at the same time.
Every file is processed in a separate process.
The cleaned code is stored to another file named filename.cleaned.
The log information is stored to file named logger.txt. 

### BULID & RUN & CLEAN:  

In the home directory, type "make" to build.
$make

Usage of the program:
$./code_cleaner file1 [file2] ...
There should be at least 1 file to be processed or there will be a notice of usage in the screen.
The cleaned code is stored to another file named filename.cleaned.
The log information is stored to file named logger.txt. 

The cleaning work is also simply type "make clean" in the terminal.
$make clean
Cleaning work will remove all generated files, including the log file and cleaned files.


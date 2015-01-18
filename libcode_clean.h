#ifndef LIBCODE_CLEAN_H
#define LIBCODE_CLEAN_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdarg.h>
#define LOG_FILE "logger.txt"

/*this function is to set C/C++ comments ' ' in the string, input is string pointor and string size */

void remove_comment(char *buf, size_t size);

/*this function is clean the empty lines in the string and save the result to another string, inputs are original string pointor and new string pointor*/

void clean_line(char *buf1,char *buf2);

/*this function is to process the file remove comments and empty lines and save the result to a new file, inputs are original file name, new file name and signal handle_signal*/

int process_file(char* filename, 
		 char* new_filename, 
		 volatile sig_atomic_t* handle_signal);

/*this function is to print the message on the screen and write the same message and time stamp in the log file, input is the message string*/

void print_log(char * msg, ...);

/*this function is used for allocing memory for new filenames and adding ".cleaned" to filenames, inputs are size of array; array for holding filenames. and the return is  memory pointer to array of new filenames */

char** init_new_filenames(int file_num, char** filenames);

/* this function is used for freeing memory in new filenames array, first looping inner arrays and then outer array.inputs are pointer to filename array and amount of filenames in array */

void free_new_filenames(char** new_filenames, int file_num);

/*this function is used for allocating memory for child pids. input is amount of child. and return is memory pointer to array */

pid_t* init_child(int file_num);

/*this function is used for allocating memory child activity status.input is amount of child. and return is memory pointer to array */

int* init_child_active(int file_num);

/*this function is used for checking if any child is still active. inputs are array of child statuses and amount of child. and return is 0 if no active child, 1 if even one of childr is active */

int is_child_active(int* child_active, int amount);
#endif
                                                                                                                                               

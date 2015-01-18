#include "libcode_clean.h"
/*this is the collection of subfunctions used in the main function */
void remove_comment(char *buf, size_t size)
{
 char *p, *end, c;
/*define lc_start the start address of line comment, bc_start is the start address of block comment */
 char *lc_start, *bc_start;
 
 p = buf;
 end = p + size;
 lc_start = bc_start = NULL;
/*if the pointor has not reached the end of the string*/
 while (p < end)
 {
  c = *p;  
  switch (c)
  {

/*deal with the '/' */
   case '/':
   {
    if (lc_start || bc_start)
    {
     p++;
     continue;
    }
/*when comments like line comment and block comment are met */
    c = *(p + 1);
    if (c == '/')
    {
     lc_start = p;
     p += 2;
    }
    else if (c == '*')
    {
     bc_start = p;
     p += 2;
    }
    else
    { 

     p++;
    }
   }
/*deal with the '*' */
   case '*':
   {
    if (lc_start)
    {
     p++;
     continue;
    }
/*if the symbol is not the end of a block comment, skip */
    if (*(p + 1) != '/')
    {
     p++;
     continue;
    }
    p += 2; 
/*set the block comment empty */
    memset(bc_start, ' ', p-bc_start);
    bc_start = NULL;
    break;
   }
/*deal with the new line symbol */
   case '\n':
   {
    if (lc_start == NULL)
    {
     p++;
     continue; 

    }
/*if line comment is met, set the line comment emmpty */
    memset(lc_start, ' ',p++ - lc_start);
    lc_start = NULL;
    break;
   }


   default:
    p++;
    break;
  }
/*this is for the case that the last line is a line comment without a '\n', the comment should be set empty as well */
  if (lc_start)
  {
   memset(lc_start, ' ', p - lc_start);
  }
 }
}

void clean_line(char *buf1,char *buf2){
  int i=0;
/*set starting flag */
  int emp_line = 1; 
  int size2 = 0;
  int line = 1;
  int space_num = 0;
  for (i = 0;  i < strlen(buf1); i++) {
    buf2[size2] = buf1[i];
    size2++;
/*set the flag when line starts and clear flag when run into something else than space */
    if ((emp_line == 0) && (buf1[i] == '\n')) {
      emp_line = 1;
      space_num = 0;
    }
    else if ((emp_line == 1) && (buf1[i] == ' ')) {
      space_num++;
    }
    else if ((emp_line == 1) && (buf1[i] == '\n')) {
/*rewind index as much as space_num. */
      space_num++;
      size2 -= space_num;
      space_num = 0;
    }
    else if ((emp_line == 1) && (buf1[i] != ' ')) {
      emp_line = 0;
      space_num = 0;
    }
    
    if (buf1[i] == '\n') {
      line++;
    }
    
  }
}

int process_file(char* filename, 
		 char* new_filename, 
		 volatile sig_atomic_t* handle_signal) {
  

  struct timeval start_time;
  struct timeval end_time;
  double time_diff = 0.0;
  FILE* fp = NULL;
  long size = 0;
  char* buffer = NULL;
  char* buffer2 = NULL;
  long read_size = 0;
  long write_size = 0;

  print_log("begin to process: %s", filename);
 
/*start file process timer */
  if (gettimeofday(&start_time, NULL) != 0) {
    perror("process_file(): gettimeofday() failed");
    return EXIT_FAILURE;
  }

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("process_file(): fopen() failed");
    return EXIT_FAILURE;
  }

/*check file size and read the file into buffer */
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);
  
  buffer = (char*)malloc(size*sizeof(char));
  if (buffer == NULL) {
    perror("process_file(): malloc() failed");
    return EXIT_FAILURE;
  }
  read_size = fread(buffer, sizeof(char), size, fp);
  if (read_size != size) {
    perror("process_file(): fread() failed");
    return EXIT_FAILURE;
  }
  fclose(fp);

/*call the function to remove comments. */
  print_log("removing comments from: %s", filename);
  remove_comment(buffer,size);
/*check if ctrl+c is pressed */
  if (*handle_signal == SIGINT) {
    print_log("%s: abort processing", filename);
    return EXIT_FAILURE;
  }
/*malloc for the second buffer*/
  buffer2 = (char*)malloc(size*sizeof(char));
  if (buffer2 == NULL) {
    perror("process_file(): malloc() failed");
    return EXIT_FAILURE;
  }
/*call the function to remove empty lines */
  print_log("removing empty lines from: %s", filename);
  clean_line(buffer, buffer2);
  /* Check if Ctrl-C pressed. */
  if (*handle_signal == SIGINT) {
    print_log("%s: abort processing", filename);
    return EXIT_FAILURE;
  }
/*free memory and write the buffer to newfile */
  free(buffer);
  fp = fopen(new_filename, "w");
  if (fp == NULL) {
    perror("process_file(): fopen() failed");
    return EXIT_FAILURE;
  }
  

  print_log("writing uncommented code from %s to: %s", filename, new_filename);
  write_size = fwrite(buffer2, sizeof(char), strlen(buffer2), fp);
  if (write_size != strlen(buffer2)) {
    perror("process_file(): fwrite() failed");
    return EXIT_FAILURE;
  }
  
  fclose(fp);
  free(buffer2);

/*stop file process timer */
  if (gettimeofday(&end_time, NULL) != 0) {
    perror("process_file(): gettimeofday() failed");
    return EXIT_FAILURE;
  }
  
/* Calculate time interval of the file process */
  time_diff = 
    (((end_time.tv_sec - start_time.tv_sec)*1000000.0) 
     + (end_time.tv_usec - start_time.tv_usec))/1000000.0;
  
  print_log("%s is processed in %.2f s", filename, time_diff);

  return EXIT_SUCCESS;
  
}

void print_log(char* msg, ...){

  char *time_string;
  FILE* fp = NULL;
  struct flock fl = { F_WRLCK, SEEK_SET, 0, 0, 0 };
  char buffer[512];
  va_list args;
  time_t now;
  time(&now);
/*handle argument list and put it to buffer */
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  fl.l_pid = getpid();
  printf("%s\n", buffer);
  fp = fopen(LOG_FILE, "a");
  if (fp == NULL) {
    perror("print_log(): fopen() failed");
    return;
  }
/* acquire file lock */
  if (fcntl(fileno(fp), F_SETLKW, &fl) == -1) {
    perror("print_log(): fcntl() failed");
    fclose(fp);
    return;
  }
  time_string=asctime(localtime(&now));
  *(time_string+24) = '\0';
  fprintf(fp, "[%s] %s\n",time_string , buffer);
  
/*release file lock */
  fl.l_type = F_UNLCK;
  fcntl(fileno(fp), F_SETLK, &fl);
  fclose(fp);
}

char** init_new_filenames(int file_num, char** filenames) {
  
  int i = 0;
  int length = 0;
  char** new_filenames = NULL;
/*malloc for new filenames */
  new_filenames = (char**)malloc(file_num*sizeof(char*));
  if (new_filenames == NULL) {
    perror("malloc_new_filenames(): malloc() failed");
    return NULL;
  }
  
  for (i = 0; i < file_num; i++) {
    
    new_filenames[i] = (char*)malloc((strlen(filenames[i])+8)*sizeof(char));
    if (new_filenames[i] == NULL) {
      perror("malloc_new_filenaames(): malloc() failed");
      return NULL;
    }
    
/* add suffix to the file name*/
    length = strlen(filenames[i]);
    strncpy(new_filenames[i], filenames[i], length);
    new_filenames[i][length] = '.';
    new_filenames[i][length+1] = 'c';
    new_filenames[i][length+2] = 'l';
    new_filenames[i][length+3] = 'e';
    new_filenames[i][length+4] = 'a';
    new_filenames[i][length+5] = 'n';
    new_filenames[i][length+6] = 'e';
    new_filenames[i][length+7] = 'd';
    new_filenames[i][length+8] = '\0';
  }
  return new_filenames;
}

/*free memory */
void free_new_filenames(char** new_filenames, int file_num) {
  int i = 0;
  for (i = 0; i < file_num; i++) {
    free(new_filenames[i]);
  }
  free(new_filenames);
}

pid_t* init_child(int file_num) {
  pid_t* child = NULL;
  int i = 0;
/*malloc for child process id*/
  child = (pid_t*)malloc(file_num*sizeof(pid_t));
  if (child == NULL) {
    perror("init_child(): malloc() failed");
    return NULL;
  }
/*set child pid*/
  for (i = 0; i < file_num; i++)
    child[i] = 0;
  return child;
}

int* init_child_active(int file_num) {
  int* child_active = NULL;
  int i = 0;
/*malloc for child_active */
  child_active = (int*)malloc(file_num*sizeof(int));
  if (child_active == NULL) {
    perror("init_child_active(): malloc() failed");
    return NULL;
  }
  for (i = 0; i < file_num; i++)
    child_active[i] = 0;
  return child_active;
}

int is_child_active(int* child_active, int amount) {
  int i = 0;
  int active = 0;
/*check if any of the child is active */
  for (i = 0; i < amount; i++)
    if (child_active[i] == 1)
      active = 1;
  return active;
  
}

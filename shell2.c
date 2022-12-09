#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
char* cwd;
bool background;
pid_t  pid;
int child;

void handlesig(int signum){
  signal(SIGINT, handlesig);
  fflush(stdout);
}
void handalarm(int signum){
  kill(child, SIGKILL);
}

int main() {
  signal(SIGINT, handlesig);
  signal(SIGALRM, handalarm);
  // Stores the string typed into the command line.
  char command_line[MAX_COMMAND_LINE_LEN];
  char cmd_bak[MAX_COMMAND_LINE_LEN];

  // Stores the tokenized command line input.
  char *arguments[MAX_COMMAND_LINE_ARGS];
  int temp = 256;
  

  while (true) {
    do{ 
      // Print the shell prompt.
      cwd = getcwd(cwd, temp);
      printf("%s%s", cwd, prompt);
      fflush(stdout);

      // Read input from stdin and store it in command_line. If there's an
      // error, exit immediately. (If you want to learn more about this line,
      // you can Google "man fgets")

      if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
          fprintf(stderr, "fgets error");
          exit(0);
        }

    }while(command_line[0] == 0x0A);  // while just ENTER pressed
      // If the user input was EOF (ctrl+d), exit the shell.
    if (feof(stdin)) {
        printf("\n");
        fflush(stdout);
        fflush(stderr);
        return 0;
    }
      // TODO:
      // 
      
      // 0. Modify the prompt to print the current working directory
      //done
    
      // 1. Tokenize the command line input (split it on whitespace)
    int i = 0;
    arguments[i] = strtok(command_line, delimiters);
    while(arguments[i] != NULL){
      arguments[++i] = strtok(NULL, delimiters);
    }
    if(strcmp(arguments[i - 1], "&") == 0){
      background = true;
      arguments[i - 1] = '\0';
    }

    // 2. Implement Built-In Commands
    if (strcmp(arguments[0], "cd") == 0){
      if (arguments[1] == NULL){
        printf("Please input directory.\n");
        }
      else{
        chdir(arguments[1]);
      }
    }

    else if (strcmp(arguments[0], "pwd") == 0){
      printf("%s\n", cwd);
    }

    else if (strcmp(arguments[0], "echo") == 0){
      int i = 1;
      while(arguments[i] != NULL){
        if(arguments[i][0] == '$'){
          printf("%s ", getenv(arguments[i]+1));
        }
        else{
          printf("%s ", arguments[i]);
        }
        i++;
      }
      printf("\n");
    }

    else if (strcmp(arguments[0], "exit") == 0){
      printf("exiting.\n");
      exit(0);
    }

    else if (strcmp(arguments[0], "env") == 0){
      for (i = 0; environ[i]!=NULL;i++){
        printf("%s\n", environ[i]);
      }
    }

    else if (strcmp(arguments[0], "setenv") == 0){
      char* val[2];
      char* first = arguments[1];
      for(i = 0; i < 2; i++){
        char *temp = strsep(&first, "=");
        val[i] = temp;
      }
      setenv(val[0], val[1], 1);
    }

    // 3. Create a child process which will execute the command line input
    else{
      int i;
      pid = fork();
      if (pid < 0){
        printf("Error: Fork Op \n");
        exit(-1);
      }
      else if (pid == 0){
        if(execvp(arguments[i], arguments) == -1){
          printf("error: execvp failed.\n");
        }
        exit(0);
      }

      // 4. The parent process should wait for the child to complete unless its a background process
      else{
        child = pid;
        if(!background){
          wait(NULL);
        }
      }
    }
  
    // Hints (put these into Google):
    // man fork
    // man execvp
    // man wait
    // man strtok
    // man environ
    // man signals
    
    // Extra Credit
    // man dup2
    // man open
    // man pipes
  }
  // This should never be reached.
  return -1;
}
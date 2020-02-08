#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h>

void convertToArray (char **arguments, char *input, int *size) {
  // need to prase my initialinput which is just a char array into individual words
  // place it into arguments []
  // going to use tokens

  int numOfWords = 0;

  char *parse = strtok(input, " \n\t"); // parse base on spaces
  arguments[numOfWords] = parse; // put the first word into arguments
  numOfWords++;
  while (parse != NULL) {
    parse = strtok(NULL, " \n\t"); // grab next token
    arguments[numOfWords] = parse;
    numOfWords++;

    // need this because there will be an extra addition since token accounts for null
    //avoid seg fault
    // NOTE: whitespace problem might occur, if all else fails just do -- outside :)
    // fixed.
    if (parse == NULL) {
      numOfWords--;
    }
  }

  // store the size of the array here and pass it back by reference
  *size = numOfWords;
}
//============================================================================================================
// debugging function purposes
void printArray (char **arguments, char*input, int which1, int size) {

  if (which1 == 1) {
    int i;
    for (i=0; i <size; i++ ) {
      printf("%s\n", arguments[i] );
    }
  }

  else if (which1 == 2) {
    printf("%s\n",input );
  }
}

//====================================================================================
// handle sig int
void sigint_handler(int sig ){
  write(1, "ctrl+z caught!\n",13 );
}

// handle sig stp
void sigstp_handler(int sig ){


}

//============================================================================================

// this function will check for a symbol in the input
// >
// <
// ;
// |
// file name pass by reference as usual
char checkSymbol (char **arguments, int size, char **file_name){

  int i;
  for (i=0; i <size; i++) {
    if (strcmp(arguments[i], ">") == 0)
      return '>';
    else if (strcmp(arguments[i], "<") == 0)
      return '<';
    else if (strcmp(arguments[i], ";") == 0)
      return ';';
    else if (strcmp(arguments[i], "|") == 0)
      return '|';
  }

  // nothing special found so we know it's just a regular command

  // how to pass by reference in c for strings and it works!
  char*new = "not needed";
  *file_name = new;
  return '$';

}

//===================================================================================================

// do the main work of the shell, i/o stuff
void runCommand(char **arguments, int size ) {

  char *file_name;

  // file name is now working
  char symbolChecker = checkSymbol(arguments, size, &file_name);
  //printf("the symbol we got here was %c\n", symbolChecker);

  // $ means just a regular normal command, other cases should be easy to figure out from looking
  switch (symbolChecker) {
    case '$' :
    {
      execvp(arguments[0], arguments); // anything below doesnt get printed out for some reason
      //printf("hello\n");
      //printf("mee\n" );
    }
      break;
    case '>' :
      printf("> was called \n");
      break;
    case '<' :
      printf("< was called\n");
      break;
    case ';' :
      printf("; was called \n");
      break;
    case '|' :
      printf("| was called \n");
      break;

  }

}
//=======================================================================================
int main() {

  // make a singal for ctrl z too
  //signal (SIGINT, sigint_handler); // ctrl c

  char userInput[100]; // intial userinput

  // array of arguments, [0] is the command the the following are extra that comes along
  // for now set it to 100, change later base off initial input
  char *arguments[100];

  int numOfWords = 0; // useed in arguments where how many arguments we have in total

  // grab user input
  printf("361 >");
  fgets(userInput, 100, stdin);
  //printf("Your input was %s\n", userInput);

  convertToArray(arguments, userInput, &numOfWords);

  // now arguments hold each individual words at each index.
  // numOfWords now holds the size of the words in arguments
  //printArray(arguments,userInput,1,numOfWords);

  pid_t pid;
  int child_status; // status for WEEXITSTATUS
  pid = fork();

  // child process should do the main work
  if (pid == 0) {
    //printf("child was called!\n" );
    runCommand(arguments, numOfWords);
    exit(0);
  }

  // parent part here. need to print out pid and status
  else {
    wait (&child_status);

    // if the child terminated normally
    if (WIFEXITED(child_status)) {
      printf("pid:%d status:%d\n", pid, WEXITSTATUS(child_status) );
    }

  }

  // while (1) {
  //
  //   if (strcmp("exit\n", userInput) == 0) {
  //     write(1, "Exiting the program...",7 );
  //     exit(0);
  //   }
  //
  //
  // } // infinite while

   return 0;
}

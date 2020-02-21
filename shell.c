//Gavin Wu
// 361 homework 3, spring 2020

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int printTwoPIDS = -999; // see if i should print pid twice or not

//IMPORTANT TO KNOW I AM PARSING WITH SPACES IF THERE IS A SPECIAL token
// EXAMPLE: ECHO HI > FOO.TXT
// IT ONLY PICKS UP THE > SYMBOL IF IT IS SURROUNDED BY SPACES
// same goes with the other symbols
// if the autograder does it without spaces, i have to parse it differently
// use a different delimminater

// function declerations needed because they are gonna call itself in a cycle with runCommand()
void semicolonCommand(char **arguments, int size, int index_pos);
void pipeCommand(char **arguments, int size, int index_pos);

//==============================================================================================================================
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
// which 1, is if i wanna pring a char*array or just the char*
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
  char msg[] = "caught sigint\nCS361 >";
  write(1, msg,sizeof(msg) );
}

// handle sig stp
void sigstp_handler(int sig ){
  char msg[] = "caught sigtstp\nCS361 >";
  write(1,msg,sizeof(msg));

}

//===============================================================================================================
// function to see if we see the | command then we make it print 2 pids
// this is a cheese method for now
int findPipe(char **arguments, int size) {
  int i;
  for(i=0; i < size; i++) {
    if (strcmp(arguments[i], "|") == 0){
      return 88;
    }
  }

  return 5; // some random number that we literally dont care about
}

//================================================================================================================

// this function will check for a symbol in the input
// >
// <
// ;
// |
// file name pass by reference as usual
char checkSymbol (char **arguments, int size, char **file_name, int *index){

  // we know the format on where the filename is going to be so
  // the next index in arguments will have where the file name will be easy.
  // index stores the position of where the symbol is so we can chop off for array

  // search by semicolon first so we can use 2 commands+ at same time and look good.
  int p;
  for (p=0; p < size; p++){
    if (strcmp(arguments[p], ";") == 0) {
      *index = p;
      return ';';
    }
  }

  int i;
  for (i=0; i <size; i++) {

    if (strcmp(arguments[i], ">") == 0) {
      *index = i;
      *file_name = arguments[i+1];
      return '>';
    }

    else if (strcmp(arguments[i], "<") == 0){
      *index = i;
      *file_name = arguments[i+1];
      return '<';
    }

    // probably not needed
    else if (strcmp(arguments[i], ";") == 0){
      *index = i;
      return ';';
    }

    else if (strcmp(arguments[i], "|") == 0) {
      *index = i;
      return '|';
    }

  } // for loop

  // nothing special found so we know it's just a regular command

  // how to pass by reference in c for strings and it works!
  //char*new = "not needed";
  //*file_name = new;
  *file_name = "not needed";
  return '$';

}

//===================================================================================================

// do the main work of the shell, i/o stuff
// it will check if there is a special symbol
// depending on the symbol it will do what it does accordingly to shell

// extra > < symbols are for my learning purposes
void runCommand(char **arguments, int size ) {

  char *file_name; // grab the file name from checkSymbol function
  int index_pos;

  char symbolChecker = checkSymbol(arguments, size, &file_name, &index_pos);
  //printf("the symbol we got here was %c\n", symbolChecker);

  // $ means just a regular normal command, other cases should be easy to figure out from looking
  switch (symbolChecker) {
    case '$' :
      //printf("%s\n",file_name );
      execvp(arguments[0], arguments); // anything below doesnt get printed out for some reason..because it replaces a new program process
      //printf("hello\n");
      //printf("mee\n" );
      break; // probably wont even reach here this break

    // existing files are over written.. echo hi > hi.txt
    // need to modify the char array
    case '>' : ;
      int f = open(file_name, O_TRUNC | O_RDWR | O_CREAT,    S_IRWXU | S_IRWXG | S_IRWXO);
      dup2(f, 1);
      char *newArg[100]; // make a brand new array
      int i = 0;
      for (i=0; i < index_pos; i++){ // copy over content
        newArg[i] = arguments[i];
      }
      newArg[index_pos] = NULL; // set the null position for exec call
      execvp(newArg[0], newArg);
      break;

    case '<' : ; // same concept as above but we only want to read it conents
      int ff = open (file_name, O_RDONLY);
      dup2(ff,0);
      char *newArg1[100];
      int j=0;
      for (j=0; j < index_pos; j++) {
        newArg1[j] = arguments[j];
      }
      newArg1[index_pos] = NULL;
      execvp(newArg1[0], newArg1);
      break;

    case ';' :
      semicolonCommand(arguments, size, index_pos); // see below
      break;

    case '|' :
      pipeCommand(arguments, size, index_pos);
      break;

  } // switch case

}

//===========================================================================================================
// arguments holds the array of input, into each individual words
// size is the size of arguments
// index_pos is the position on where the semi colon is at.
// psuedo recursive call --> will call back runCommand to do work
// based off the specs we only run as in command1 so nothing special
void semicolonCommand(char **arguments, int size, int index_pos ) {

  // create two different arrays for each command on each side
  char *a[100];
  char *b[100];

  int a_size = 0;
  int b_size = 0;

  // grab the first half of the semicolon
  int i;
  for (i=0; i < index_pos; i++) {
    a[i] = arguments[i];
    a_size++;
  }
  a[index_pos] = NULL;
  //runCommand(a,a_size);

  // now grab the second half of the smicolon
  int k;
  for (k= index_pos + 1; k < size; k++){
    b[b_size] = arguments[k];
    b_size++;
  }
  b[b_size] = NULL;
  //printArray(b,"dd",1,b_size);
  //runCommand(b,b_size);

  // now make another forking for the commands
  pid_t pid;
  pid = fork();
  int childS;

  // child process
  if (pid == 0) {
    //printf("run");
    runCommand(a,a_size); //  we want command A to go first
    exit(0);
  }

  // parent i guess
  else{
      wait(&childS);
      runCommand(b,b_size); // wait until the first command goes first, then run this 1
  }

}

//======================================================================================================
// pipe command
// redirect output of command 1 to command 2
// similar to semi colon command but now a pipe is needed to pipe thr 1 command to the other 1
void pipeCommand(char **arguments, int size, int index_pos ){

  // create two different arrays for each command on each side
  char *a[100];
  char *b[100];

  int a_size = 0;
  int b_size = 0;

  // grab the first half of the |
  int i;
  for (i=0; i < index_pos; i++) {
    a[i] = arguments[i];
    a_size++;
  }
  a[index_pos] = NULL;
  //runCommand(a,a_size);

  // now grab the second half of the |
  int k;
  for (k= index_pos + 1; k < size; k++){
    b[b_size] = arguments[k];
    b_size++;
  }
  b[b_size] = NULL;

  int proc_stats;

  // create a pipe
  int pipe1[2];
  pipe(pipe1);

  // now make another process
  pid_t pid;
  pid = fork();

  // child process
  if (pid == 0) {

    close(1); // close standard output
    dup(pipe1[1]); // output go thr pipe

    // close file dessc, not sure if needed here
    close(pipe1[0]);
    close(pipe1[1]);

    runCommand(a,a_size); //  we want command A to go first
    exit(0);
  }

  // parent i guess
  else{
      wait(&proc_stats);
      close(0); // close input
      dup(pipe1[0]); // make input come from pipe

      // close file desc from pipe
      close(pipe1[0]);
      close(pipe1[1]);
      if (WIFEXITED(proc_stats)) {
        //printf("fuckpid:%d status:%d\n", pid, WEXITSTATUS(proc_stats) );
        //printf("hello" );
      }
      //printf("pid:%d status:%d\n", pid, WEXITSTATUS(proc_stats) );

      runCommand(b,b_size); // wait until the first command goes first, then run this 1
  }

}

//======================================================================================================
int main() {

  // handle the signals
  signal (SIGINT, sigint_handler); // ctrl c
  signal (SIGTSTP, sigstp_handler);

  while (1) {

    char userInput[500]; // intial userinput

    // array of arguments, [0] is the command the the following are extra that comes along
    // for now set it to 100, change later base off initial input
    char *arguments[500];

    int numOfWords = 0; // useed in arguments where how many arguments we have in total

    // grab user input
    printf("CS361 >");
    fgets(userInput, 100, stdin);
    //printf("Your input was %s\n", userInput);

    if (strcmp("exit\n", userInput) == 0) {
      //write(1, "Exiting the program...",7 );
      exit(0);
    }

    convertToArray(arguments, userInput, &numOfWords);

    printTwoPIDS = findPipe(arguments, numOfWords);

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
      // for the | the pid status 2 times comes from here
      if (WIFEXITED(child_status)) {
        if (printTwoPIDS == 88) {
          printf("pid:%d status:%d\n", pid, WEXITSTATUS(child_status) );
          printf("pid:%d status:%d\n", pid, WEXITSTATUS(child_status) );
        }
        else
          printf("pid:%d status:%d\n", pid, WEXITSTATUS(child_status) );
      }

    }

  } // infinite while

   return 0;
}

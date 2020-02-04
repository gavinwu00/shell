#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void convertToArray (char **arguments, char *input, int *size) {
  // need to prase my initialinput which is just a char array into individual words
  // place it into arguments []
  // going to use tokens

  int numOfWords = 0;

  char *parse = strtok(input, " "); // parse base on spaces
  arguments[numOfWords] = parse; // put the first word into arguments
  numOfWords++;
  while (parse != NULL) {
    parse = strtok(NULL, " "); // grab next token
    arguments[numOfWords] = parse;
    numOfWords++;

    // need this because there will be an extra addition
    //avoid seg fault
    if (parse == NULL) {
      numOfWords--;
    }
  }

  // store the size of the array here and pass it back by reference
  *size = numOfWords;
}

//====================================================================================
// handle sig int
void sigint_handler(int sig ){
  write(1, "ctrl+z caught!\n",13 );
}

// handle sig stp
void sigstp_handler(int sig ){


}

//=======================================================================================
int main() {
  //signal (SIGINT, sigint_handler); // ctrl c


  char userInput[100]; // intial userinput

  // array of arguments, [0] is the command the the following are extra that comes along
  // for now set it to 100, change later base off initial input
  char *arguments[100];

  int numOfWords = 0; // useed in arguments where how many arguments we have in total

  // grab user input
  fgets(userInput, 100, stdin);
  printf("Enter a command\n");
  printf("Your input was %s\n", userInput);

  convertToArray(arguments, userInput, &numOfWords);

  // // need to prase my initialinput which is just a char array into individual words
  // // place it into arguments []
  // // going to use tokens
  //
  // char *parse = strtok(userInput, " "); // parse base on spaces
  // arguments[numOfWords] = parse; // put the first word into arguments
  // numOfWords++;
  // while (parse != NULL) {
  //   parse = strtok(NULL, " "); // grab next token
  //   arguments[numOfWords] = parse;
  //   numOfWords++;
  //
  //   // need this because there will be an extra addition
  //   //avoid seg fault
  //   if (parse == NULL) {
  //     numOfWords--;
  //   }
  // }

  // now arguments hold each individual words at each index.
  printf("size of array is %d\n", numOfWords);


  while (1) {

    if (strcmp("exit\n", userInput) == 0) {
      write(1, "Exiting the program...",7 );
      exit(0);
    }


  } // infinite while

   return 0;
}

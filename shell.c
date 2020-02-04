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

//=======================================================================================
int main() {
  //signal (SIGINT, sigint_handler); // ctrl c


  char userInput[100]; // intial userinput

  // array of arguments, [0] is the command the the following are extra that comes along
  // for now set it to 100, change later base off initial input
  char *arguments[100];

  int numOfWords = 0; // useed in arguments where how many arguments we have in total

  // grab user input
  printf("361 >");
  fgets(userInput, 100, stdin);
  printf("Your input was %s\n", userInput);

  convertToArray(arguments, userInput, &numOfWords);

  // now arguments hold each individual words at each index.
  // numOfWords now holds the size of the words in arguments




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

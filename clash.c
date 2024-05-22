#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <unistd.h>
#include <errno.h>
#define INITIAL_CWD_CAPACITY 64
#define MAX_INPUT_LENGTH 1337
#define DEFAULT_ARGS 5

static void die(char* message){
	perror(message);
	exit(EXIT_FAILURE);
}
/* Draws the current directory for the terminal
 * the buffer stores the directory name as a char *
 * and reallocates itself, if the buffer is to small 
 */
static void showPrompt(){
	int capacity = INITIAL_CWD_CAPACITY;
	char* buf = NULL;
	while(1){
		buf = realloc(buf, sizeof(char)*capacity);	//dynamically increase buf size till it is enough
		if(buf == NULL){				//to fit the current working directory
			die("realloc");
		}
		char* retVal = getcwd(buf, capacity);
		if(retVal != NULL){				//check if getcwd failed
			break;
		}
		if(errno != ERANGE){				//only acceptable error, that the buf wasnt big 
			die("getcwd");				// enough, all else are fatal
		}
		capacity *= 1.5;
	}
	if(printf("%s: ", buf) < 0){
		die("printf");
	}
	if(fflush(stdout) < 0 ){				//have to flush to ensure it gets written
		die("fflush");
	}
	free(buf);
}
// Reads the command from stdin as saves it in the char * it receives
static void getInput(char* buf){
	if(fgets(buf, MAX_INPUT_LENGTH + 1, stdin) == NULL){	
		if(ferror(stdin)){
			die("fgets");
		}
		else{
			exit(EXIT_SUCCESS);
		}
	}
}
/* creates the argv for the command execution, out of the
 * input pointer it receives
 */
static void saveInArgV(char **argv, int *args, int i, char *argument) {
	if(argv == NULL || argument == NULL) {
		return;
	}
	if (*args <= i) {
		*args = (int)1.5 * *args;	
		argv = realloc(argv, *args * sizeof(char*));
		if (argv == NULL) { die("realloc");}
	}
	argv[i] = argument;
	return;
}

static char **createArgs(char *input) {
	if (input == NULL) {
		fprintf(stderr, "No input given");		//optional Error message, real terminal ignores empty input
		return NULL;
	}
	char **argv = malloc(DEFAULT_ARGS * sizeof(char*));
	if (argv == NULL) {
		die("malloc");
	}
	int args = DEFAULT_ARGS;
	char *command = strtok(input, " ");
	//When *command is NULL -> No space in the string that means
	//that the command has no further arguments
	if (command == NULL) {
		argv[0] = command;
		return argv;
	}
	argv[0] = command;
	for(int i = 1;;i++) {
		char *argument = strtok(NULL, " ");
		if (argument == NULL) {
			break;
		}
		saveInArgV(argv, &args, i, argument);
	}	
	return argv;
}

int main(int argc, char** argv){

	printf("%s\n", "hallo, welt");
	while(1){
		showPrompt();
		char input[MAX_INPUT_LENGTH + 1];
		getInput(input);
		printf("%s", input);
		char **argv = createArgs(input); 
	}


	exit(EXIT_SUCCESS);
}

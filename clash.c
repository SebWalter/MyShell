#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
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
			exit(EXIT_SUCCESS);				//Was ist das? Dann exited ja das programm?
		}
	}
	//remove new line character
	char *newLine = strchr(buf, '\n');
	if (newLine != NULL) {
		*newLine = '\0';
	}
}
/* creates the argv for the command execution, out of the
 * input pointer it receives
 */
static char **createArgs(char *input) {
	if (input == NULL) {
		return NULL;
	}
	int argc = DEFAULT_ARGS;
	char **args = malloc(DEFAULT_ARGS * sizeof(char*));
	if (args == NULL) {
		die("malloc");
	}
	char *command = strtok(input, " ");
	//if strtok return NULL the first time the string was empty or an error occured
	//if a string doesn't contain a token it just returns the original string the first time
	if (command == NULL) {
		free(args);
		fprintf(stderr, "Error input was empty...continue\n");
		return NULL;
	}
	args[0] = command;
	for (int i = 1;;i++) {
		char *newToken = strtok(NULL, " ");
		//if strtok return null (not the first call there arent any new tokens
		//so we write a NULL in the next position (for execvp) and return
		if (newToken == NULL) {
			args[i] = NULL;
			return args;
		}
		//argc -1 <= i because we always need a nother space for the NULL * 
		if (argc -1 <=  i) {
			argc = (int) argc *1.5;
			args = realloc(args, argc * sizeof(char*));
			if (args == NULL) {die("realloc");}
		}
		args[i] = newToken;
	}
	return args;
}

static int changeDirectory(char **args) {
	if (args == NULL || args[1] == NULL) {
		fprintf(stderr, "Error can't cd without new Path\n");
		return -1;
	}
	if  (chdir(args[1]) == -1) {
		perror("chdir");			//when the chdir fails, it's no big deal, so no exit()
		errno = 0;
		return -1;
	}
	return 0;
}
int main(int argc, char** argv){

	while(1){
		showPrompt();
		char input[MAX_INPUT_LENGTH + 1];
		getInput(input);
		char **args = createArgs(input);
		if (args == NULL || args[0] == NULL) {
			continue;
		}
		if (strcmp(args[0], "cd") == 0) {
			changeDirectory(args);
			free(args);
			continue;
		}
		pid_t processID;
		processID = fork();
		if (processID == -1) {
			die("fork");
		}
		//Child Process that executes the new command
		if (processID == 0) {
			execvp(args[0], args);
			//When execvp fails, it don't free the heap memory. 
			//Because it was cloned during fork() i have to free it in the child and parent
			free(args);
			//the exec function only returns when an error occurs -> error handling
			die("execvp");

		}
		//parent process
		int exitStatus;
		pid_t childPid = waitpid(-1,&exitStatus, 0);
		if (childPid == -1) {
			die("waitpid");
		}
		if (WIFEXITED(exitStatus)) {
				int exitMessage = WEXITSTATUS(exitStatus);
				printf("EXITSTATUS [%d]\n", exitMessage);
		}
		else {
			fprintf(stderr, "Process didn't execute properly\n");		
		}
		free(args);
	}

	exit(EXIT_SUCCESS);
}

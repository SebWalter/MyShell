#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
static int initial_cwd_capacity = 64;
static int MAX_INPUT_LENGTH = 1337;

static void die(char* message){
	perror(message);
	exit(EXIT_FAILURE);
}
static void showPrompt(){
	int capacity = initial_cwd_capacity;
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
	if(printf("%s :", buf) < 0){
		die("printf");
	}
	if(fflush(stdout) < 0 ){				//have to flush to ensure it gets written
		die("fflush");
	}
	free(buf);
}
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

int main(int argc, char** argv){

	printf("%s\n", "hallo, welt");
	while(1){
		showPrompt();
		char input[MAX_INPUT_LENGTH + 1];
		getInput(input);
		printf(input);
	}


	exit(EXIT_SUCCESS);
}

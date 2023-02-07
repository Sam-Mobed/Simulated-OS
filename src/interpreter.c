#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "shellmemory.h"
#include "shell.h"

//int MAX_ARGS_SIZE = 3;
int MAX_ARGS_SIZE = 7;

int compareStrings(const void *str1, const void *str2){
	return strcmp(*(const char**)str1, *(const char**)str2);
}

//for too many tokens in set
int tooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 6;
}

int mkdirError(){
	printf("%s\n", "Bad command: my_mkdir");
	return 7;
}

int badcommand(int args_size){
	if (args_size>MAX_ARGS_SIZE){
		return tooManyTokens();
	}else{
		printf("%s\n", "Unknown Command");
	}
	return 1;
}//had to modiy this function to match the desired output in the examples given

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int help();
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		return badcommand(args_size);
	}

	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand(args_size);
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand(args_size);
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		//to check if we have too little/too many tokens
		//if (args_size < 3) return tooFewTokens();
		//if (args_size > MAX_ARGS_SIZE) return tooManyTokens();	
		//set a decent buffer size, in case the size of the tokens are big
		char combinedTokens[900];
		char *pointer=combinedTokens;

		for(int i=2;i<args_size;i++){
			strcpy(pointer, command_args[i]);
			pointer=pointer+strlen(command_args[i]);
			if (i!=args_size-1){
				strcpy(pointer," ");
				pointer++;
			}//strcpy automatically adds a \0 at the end of the copied str
		}//so we're safe
		//now wew filter the array to get rid of non-printable characters
		int size = strlen(combinedTokens);//we write it here so it only computes once
		for(int i=0;i<size;i++){
			if(combinedTokens[i]>126 || combinedTokens[i]<32){
				combinedTokens[i]='\0';
			}
		}
		return set(command_args[1], combinedTokens); 
		
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand(args_size);
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand(args_size);
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "echo")==0){
		if (command_args[1][0]!='$'){
			printf("%s\n",command_args[1]);
			return 0;
		}else{
			char *variableName=command_args[1]+1; //to remove the $
			char *value=mem_get_value(variableName);
			if (!strcmp(value,"Variable does not exist")){ //since the return value is 1, but 1==false
				printf("\n");	//we might run into trouble if the actual value of the variable is this
				return 0; //but to avoid this we would likely have to add a function to shellmemory.
			} else{
				printf("%s\n",value);
				return 0;
			}
		}
	}else if(strcmp(command_args[0], "my_ls")==0){
		DIR *pwd;
		struct dirent *dir;
		pwd= opendir(".");
		char *arr[500]; //allot a decent buffer size
		int index=0;

		if (pwd){
			while ((dir=readdir(pwd)) != NULL){
				arr[index]=strdup(dir->d_name);
				index++;
			}
			closedir(pwd);
		}
		int arrSize = index;
		qsort(arr,arrSize,sizeof(char *),compareStrings);

		for(int j=0;j<arrSize;j++){
			printf("%s\n",arr[j]);
			free(arr[j]);
		}
		return 0;
	}else if(strcmp(command_args[0], "my_mkdir")==0){
		if (args_size != 2) return mkdirError();

		char *dname;
		int status;

		if (command_args[1][0]!='$'){
			dname=command_args[1];
			status = mkdir(dname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			return 0; //^specifying the permissions for the directory
		}else{
			char *var=command_args[1]+1; //to remove the $
			dname=mem_get_value(var);
			if (strcmp(dname,"Variable does not exist")){ 
				status = mkdir(dname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				return 0; 
			}else{
				mkdirError();
			}
		}


	}else if(strcmp(command_args[0], "my_touch")==0){
		if (args_size != 2) return mkdirError();
		//check for what they want for error codes and return that	
	}else return badcommand(args_size);
}

int help(){

	char help_string[] = "COMMAND                 DESCRIPTION\n \
help			        Displays all the commands\n \
quit			        Exits / terminates the shell with “Bye!”\n \
set VAR STRING		    Assigns a value to shell memory\n \
print VAR		        Displays the STRING assigned to VAR\n \
run SCRIPT.TXT         Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

int set(char* var, char* value){
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;

}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}

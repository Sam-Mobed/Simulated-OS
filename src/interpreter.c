#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shellmemory.h"
#include "shell.h"
#include "scheduler.h"

//int MAX_ARGS_SIZE = 3;
int MAX_ARGS_SIZE = 7;

int compareStrings(const void *str1, const void *str2){
	return strcmp(*(const char**)str1, *(const char**)str2);
}

int checkFileNames(char* arr[], int size){
	for (int i = 1; i < size-1; i++) {
        for (int j = i + 1; j < size-1; j++) {
            if (strcmp(arr[i], arr[j]) == 0) {
                return 1;
            }
        }
    }
	return 0;
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

int cdError(){
	printf("%s\n", "Bad command: my_cd");
	return 8;
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

int sameFileName(){
	printf("%s\n", "Bad command: same file name");
	return 9;
}

int help();
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();
int exec(char* script);
enum policy setPolicy(char *str);

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
				free(value); //remove if it causes problems
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

		int j;
    	int k;
    	for(j=0;j<arrSize;j++){
        	if(!strcmp(arr[j],".\0")||!strcmp(arr[j],"..\0")){
            	free(arr[j]);
            	continue;
        	}
        	for(k=0;k<strlen(arr[j]);k++){
            	if(32<=arr[j][k]<=126){
                	printf("%c",arr[j][k]);
            	}
        	}//to get rid of unprintable characters/undesirable bytes
        	printf("\n");
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
			char space=' '; //to check if the value returned is a single token
			if (strcmp(dname,"Variable does not exist")){
				if (strchr(dname,space)==NULL){
					status = mkdir(dname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					return 0;
				}else{//if there is not space, then it's a single token
					mkdirError();
				}
			}else{
				mkdirError();
			}
			free(dname); //remove if need be
		}
	}else if(strcmp(command_args[0], "my_touch")==0){
		if (args_size != 2) return badcommand(args_size);
		char *fileName = command_args[1];
		int fd = open(fileName, O_CREAT | O_RDWR, 0666);
		close(fd); //^to specify that the file can be read and written to, and then close the file
		//0666 specifies that the file will be readable and writable by owner,group and others
		//O_CREAT tells tthe open ot create he file if it doesn't alrady exist
	}else if(strcmp(command_args[0], "my_cd")==0){
		if (args_size != 2) return cdError();
		DIR *dir = opendir(command_args[1]);
		if (dir){
			closedir(dir);
			chdir(command_args[1]);
		}else{
			cdError();
		}

	}else if(strcmp(command_args[0], "exec")==0){
		if (args_size < 3) return badcommand(args_size);
		if (args_size > 5) return badcommand(args_size);
		enum policy pol = setPolicy(command_args[args_size-1]);
		int result = 0; //
		//first we have to check if two/more files of the same name have been passed
		result = checkFileNames(command_args, args_size);
		if (result!=0){
			return sameFileName();
		}

		if (pol==UNKNOWN){
			printf("Invalid policy.\n");
			return 1;
		}
		for (int i=1;i<args_size-1;i++){
			//result = exec(command_args[i]);
			result = exec(command_args[i]);
		}
		if (result==0){ //you can remove this
			execute_processes(pol);
		}else{ //incase one of the files does not exist.
			reset_queue();
			reset_tracker();
			clear_processes_data();
		}
		//execute_processes(pol);
		return result; //just return 0
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
	if(head->Content!=NULL){
		free(head->Content->pid); //if for some reason there is still something
		free(head->Content); //this might be completely useless
	}
	free(head); //we get rid of the memory alllocated to the head
	clear_memory();
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
	char *x = mem_get_value(var);
	//printf("%s\n", mem_get_value(var));
	printf("%s\n", x); 
	free(x);
	return 0;
}

int run(char* script){
	FILE *p = fopen(script,"rt");  // the program is in a file
	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	int result = memory_set_process(p);
	fclose(p);
	
	if (result==0){ //to make sure the code inside the script has been stored properly. 
		execute_processes(FCFS);
	}
	
	return result;
}

int exec(char* script){
	FILE *p = fopen(script,"rt");  // the program is in a file
	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	int result = memory_set_process(p);
	fclose(p);

	return result;
}

enum policy setPolicy(char *str){
	if (strcmp(str, "FCFS")==0){
		return FCFS;
	}else if (strcmp(str, "SJF")==0){
		return SJF;
	}else if (strcmp(str, "RR")==0){
		return RR;
	}else if (strcmp(str, "AGING")==0){
		return AGING;
	}else{
		return UNKNOWN;
	}//this will print invalid policy in the next function call
}
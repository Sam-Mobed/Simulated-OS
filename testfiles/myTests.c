#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <time.h>

int compareStrings(const void *str1, const void *str2){
	return strcmp(*(const char**)str1, *(const char**)str2);
}

enum policy{
	FCFS,
	SJF,
	RR,
	AGING,
	UNKNOWN
};

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

void initializeBackingStore(){
    char *dir = "BackingStore";
    DIR* dirPtr = opendir(dir);
    char *command;
	struct dirent *entry; //to check if directory is empty
    
    if (dirPtr){
		int empty = 1;
        while ((entry = readdir(dirPtr)) != NULL){
            if (entry->d_name[0] != '.'){
                empty = 0;
                break;
            }
        }
		
		if(!empty){
			command = (char*) calloc(1, 21);
			strncat(command, "rm -r ", 7);
			strncat(command, dir, strlen(dir));
			strncat(command, "/*", 3);
			system(command);
			free(command);
		}
    }else{
        command = (char*) calloc(1, 20); 
        strncat(command, "mkdir ", 7);
        strncat(command, dir, strlen(dir));
		system(command);
		free(command);
    }
	closedir(dirPtr);
}

int main(int argc, char *argv[]){

	initializeBackingStore();

    return 0;
}
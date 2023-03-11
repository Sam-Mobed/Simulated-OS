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


int main(int argc, char *argv[]){
    char *command_args[4] = {"exec","in.txt", "in2.txt","FCFS"};
    int args_size = 4;
    enum policy pol = setPolicy(command_args[args_size-1]);
    
    if (pol==RR){
        printf("hi\n");
    }

    for (int i=1;i<args_size-1;i++){
			printf("%s\n", command_args[i]);
	}
    return 0;
}
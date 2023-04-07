#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <dirent.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"
#include "shell.h"

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);
void initializeBackingStore();

int main(int argc, char *argv[]) {
	printf("%s\n", "Shell version 1.2 Created January 2023\n");
    char str[50];
    /////////////////////////////
    int fsize=FSIZE;
    int vsize=VSIZE;
    sprintf(str, "Frame Store Size = %d; Variable Store Size = %d",fsize, vsize);
    printf("%s\n", str);

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default
    initializeBackingStore(); //to create an empty directory for the backing store

	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();
    //initialize pages memory
    pages_init();
    //initialize accessTable
    init_accessTimeTable();

	while(1) {						
        if (isatty(fileno(stdin))) printf("%c ",prompt);

		char *str = fgets(userInput, MAX_USER_INPUT-1, stdin);
        if (feof(stdin)){
            freopen("/dev/tty", "r", stdin);
        }

		if(strlen(userInput) > 0) {
            errorCode = parseInput(userInput);
            if (errorCode == -1) exit(99);	// ignore all other errors
            memset(userInput, 0, sizeof(userInput));
		}
	}

	return 0;

}

int parseInput(char *ui) {
    char tmp[200];
    char *words[100];                            
    int a = 0;
    int b;                            
    int w=0; // wordID    
    int errorCode;
    for(a=0; ui[a]==' ' && a<1000; a++);        // skip white spaces
    
    while(ui[a] != '\n' && ui[a] != '\0' && a<1000 && a<strlen(ui)) {
        while(ui[a]==' ') a++;
        if(ui[a] == '\0') break;
        for(b=0; ui[a]!=';' && ui[a]!='\0' && ui[a]!='\n' && ui[a]!=' ' && a<1000; a++, b++) tmp[b] = ui[a];
        tmp[b] = '\0';
        if(strlen(tmp)==0) continue;
        words[w] = strdup(tmp);
        if(ui[a]==';'){
            w++;
            errorCode = interpreter(words, w);
            if(errorCode == -1) return errorCode;
            a++;
            w = 0;
            for(; ui[a]==' ' && a<1000; a++);        // skip white spaces
            continue;
        }
        w++;
        a++; 
    }
    errorCode = interpreter(words, w);
    
    for(int i=0;i<w;i++){
        free(words[i]);
    }
    
    return errorCode;
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
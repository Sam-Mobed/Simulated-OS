#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {
	printf("%s\n", "Shell version 1.2 Created January 2023");
	help();

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default

	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';

    int batchMode = 0;

	//init shell memory
    if (!isatty(STDIN_FILENO)){
        batchMode++;
    }
	mem_init();
	while(1) {	
        if (!batchMode){
            printf("%c ",prompt);
        }
    
        //here you should check the unistd library 
        //so that you can find a way to not display $ in the batch mode
        char* c = fgets(userInput, MAX_USER_INPUT-1,stdin);
        if(c[0]==EOF){
            batchMode--;
            continue;
        }
		errorCode = parseInput(userInput);
		if (errorCode == -1) exit(99);	// ignore all other errors
		memset(userInput, 0, sizeof(userInput));
	}

	return 0;

}

int parseInput(char ui[]) {
    char tmp[200];
    char *words[100];                            
    int a = 0;
    int b;                            
    int w=0; // wordID    
    int errorCode;

    const char sep[2] = ";";
    char *token;
    token = strtok(ui,sep);

    while(token!=NULL){
        for(a=0; token[a]==' ' && a<1000; a++);        // skip white spaces
        while(token[a] != '\n' && token[a] != '\0' && a<1000) {
            for(b=0; token[a]!=';' && token[a]!='\0' && token[a]!='\n' && token[a]!=' ' && a<1000; a++, b++){
                tmp[b] = token[a];                        
                // extract a word
            }
            tmp[b] = '\0';
            words[w] = strdup(tmp);
            w++;
        if(token[a] == '\0') break;
        a++; 
        }
        errorCode = interpreter(words, w);
        memset(tmp,'\0',200);
        for(int i=0;i<w;i++){
            free(words[i]);
        }
        w=0; //we need to reinitalize words as well or we might run into trouble
        memset(words, 0, sizeof(char *) * 100);

        token = strtok(NULL,sep);
    }
    return errorCode;    
}
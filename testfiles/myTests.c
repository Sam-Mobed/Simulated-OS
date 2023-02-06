#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    /*char *link = "=";
    //printf("%d\n",&link);
    //printf("%c\n",link); 
    //the above doesn't work, because &link is a pointer that poins to a pointer
    //which points to a char. pointer>pointer>char
    //the second line doesn't work either since link is a pointer to a char
    printf("%p\n",link); 
    //so int!=char *, because pointers are hexadecimal values.*/
    char *command_args[] = {"ice cream", "$cum", "coke"};
    int args_size=3;
    char combinedTokens[100];
    char *pointer = combinedTokens;
    printf("%p\n",pointer);
    /*
	for(int i=0;i<args_size;i++){
        printf("%s\n", combinedTokens);
        strcpy(pointer,command_args[i]);
        pointer=pointer+strlen(command_args[0]);
		if (i!=args_size-1){
            strcpy(pointer," ");
            pointer++;
			//combinedTokens = strcat(combinedTokens, command_args[i]);
		}
	}
    
		//for safety we add a null character at the end
		//combinedTokens = strcat(combinedTokens, "\0");
    
	//strcat(combinedTokens, " a");
    combinedTokens[strlen(combinedTokens)]='\0';
	printf("%s\n", combinedTokens);
    */ 
    
    int x = command_args[1][0]=='$';
    printf("%d\n",x);
    if (!strcmp("hi","hi")){
        printf("%d\n",x);
        printf("\n");
    }



    return 0;
}
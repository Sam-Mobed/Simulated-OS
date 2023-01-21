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
    
    char result[20] = "";
    char* command_args[]={"a\0","b\0","c\0"};
    int x = sizeof(command_args)/sizeof(command_args[0]);
    
    for (int i=0;i<x;++i){
        //printf("%s ", command_args[i] );
        strcat(result,command_args[i]);
    }
    
    
    char n = '\n';
    
    printf("%s %c", result,n);
    



    return 0;
}
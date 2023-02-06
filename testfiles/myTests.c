#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 

int compareStrings(const void *str1, const void *str2){
	return strcmp(*(const char**)str1, *(const char**)str2);
}

int main(int argc, char *argv[]){
    /*char *link = "=";
    //printf("%d\n",&link);
    //printf("%c\n",link); 
    //the above doesn't work, because &link is a pointer that poins to a pointer
    //which points to a char. pointer>pointer>char
    //the second line doesn't work either since link is a pointer to a char
    printf("%p\n",link); 
    //so int!=char *, because pointers are hexadecimal values.*/
    
    /*
    char *command_args[] = {"ice cream", "$cum", "coke"};
    int args_size=3;
    char combinedTokens[100];
    char *pointer = combinedTokens;
    printf("%p\n",pointer);
    */
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
    /*
    int x = command_args[1][0]=='$';
    printf("%d\n",x);
    if (!strcmp("hi","hi")){
        printf("%d\n",x);
        printf("\n");
    }
    */
    DIR *pwd;
    struct dirent *dir;
	pwd= opendir(".");
	char *arr[50]; //allot a decent buffer size
	int index=0;

	if (pwd){
		while ((dir=readdir(pwd)) != NULL){
			arr[index]=strdup(dir->d_name);
			index++;
		}
		closedir(pwd);
	}
	int arrSize = sizeof(arr)/sizeof(arr[0]);
	qsort(arr,arrSize,sizeof(char *),compareStrings);

	for(int j=0;j<arrSize;j++){
		printf("%s\n",arr[j]);
	}
   /*
   char prompt;
    if (!strcmp(argv[1],"<")){
        prompt='\0';
        printf("%c",prompt);
        printf("\n");
    }else{
        prompt='$';
        printf("%c\n",prompt);
    }
    */

    //printf("%c\n",prompt);



    return 0;
}
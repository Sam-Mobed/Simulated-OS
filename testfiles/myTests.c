#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 

int compareStrings(const void *str1, const void *str2){
	return strcmp(*(const char**)str1, *(const char**)str2);
}
/*
int parseInput(char ui[]) {
    char tmp[200];
    char *words[100];                            
    int a;
    int b;                       
    int w=0; // wordID    
    int errorCode;
    for(a=0; ui[a]==' ' && a<1000; a++);        // skip white spaces
    while(ui[a] != '\n' && ui[a] != '\0' && a<1000) {
        if(ui[a]==';'){
            errorCode=interpreter(words,w);
            memset(tmp,'\0',200);
            for(int i=0;i<w;i++){
                free(words[i]);
            }
            w=0; //we need to reinitalize words as well or we might run into trouble
            memset(words, 0, sizeof(char *) * 100);
            //this way we clear the pointers and the value of the strings
            a++;
        }else{
            for(b=0;ui[a]!=';' && ui[a]!='\0' && ui[a]!='\n' && ui[a]!=' ' && a<1000; a++, b++){
                tmp[b] = ui[a];                        
                // extract a word
            }
            tmp[b] = '\0';
            words[w] = strdup(tmp);
            w++;
            if(ui[a] == '\0') break;
            a++; 
        }
    }
    if (ui[a] == '\n' || ui[a] == '\0' || a==1000){
        errorCode = interpreter(words, w);
    } //to run the last command of one liners, or the only command
    //in case there are no semi colons
    
    return errorCode;
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
        token = strtok(NULL,sep);
    }
    return errorCode;    
}
*/

int main(int argc, char *argv[]){
    char tmp[200];
    char *words[100];                            
    int a = 0;
    int b;                            
    int w=0; // wordID    
    int errorCode;
    char ui[] = "set x 3; set y 7; echo hello\n";
    //char ui[] = "set x 3\0"; 

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
        //errorCode = interpreter(words, w);
        for (int z=0;z<w;z++){
            printf("%s ",words[z]);
            //printf("\n");
        }
        //errorCode=interpreter(words,w);
        memset(tmp,'\0',200);
        for(int i=0;i<w;i++){
            free(words[i]);
        }
        w=0; //we need to reinitalize words as well or we might run into trouble
        memset(words, 0, sizeof(char *) * 100);
            //this way we clear the pointers and the value of the strings
        //for(a=a; ui[a]==' ' && a<1000; a++);
        printf("\n"); 
        token = strtok(NULL,sep);
    }
    //return errorCode; 
    



    return 0;
}
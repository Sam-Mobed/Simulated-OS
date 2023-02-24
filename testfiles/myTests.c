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
    return 0;
}

int run(char* script){
        int errCode = 0;
        FILE *p = fopen(script,"rt");  // the program is in a file

        if(p == NULL){
            return badcommandFileDoesNotExist();
        }

        char line[100];
        fgets(line,99,p);
        while(1){
            //errCode = parseInput(line);	// which calls interpreter()
            memset(line, 0, sizeof(line));

            if(feof(p)){
                break;
            }
            fgets(line,999,p);
        }
        //either at the beginning or the end of the loop i gotta 
        //go in the memory and store the line

        fclose(p);

        return errCode;
}
    
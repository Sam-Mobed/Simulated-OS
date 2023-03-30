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

int load_file_backingStore(char* filename){
	FILE *sourceFile = fopen(filename, "rb");
	FILE *destFile;
    char destPath[100]; // allocate enough space
	strcpy(destPath, "./BackingStore/"); // copy initial part of the path
	strcat(destPath, filename); // append filename
    char buffer[1024];
    size_t bytesRead;

    // Open source file for reading
    if (!sourceFile) {
        printf("Unable to open source file.\n");
        return 1;
    }

    // Open destination file for writing
    destFile = fopen(destPath, "wb");
    if (!destFile) {
        printf("Unable to create destination file.\n");
        return 1;
    }

    // Read from source file and write to destination file
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            if (buffer[i]>=32 && buffer[i]<=126) {
                fwrite(&buffer[i], 1, 1, destFile);
            }
        }
    }

    // Close files
    fclose(sourceFile);
    fclose(destFile);

    return 0;
}

int contains_unprintable_bytes() {
    FILE* fp = fopen("./BackingStore/sampletestFile", "rb");
    if (!fp) {
        printf("Unable to open file %s\n", "./BackingStore/sampletestFile");
        return 1;
    }

    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c < 32 || c > 126) {
            printf("File %s contains unprintable byte: %02X\n", "./BackingStore/sampletestFile", c);
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]){

	initializeBackingStore();
	load_file_backingStore("sampletestFile");
	contains_unprintable_bytes();
    return 0;
}
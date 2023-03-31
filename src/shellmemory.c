#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include "shellmemory.h"

#define SHELL_MEM_LENGTH 1000
#define STOREPAGES_LENGTH 1000


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[SHELL_MEM_LENGTH];
struct memory_struct frameStore[STOREPAGES_LENGTH];

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init(){
	int i;
	for (i=0; i<1000; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

void pages_init(){
	int i;
	for (i=0; i<1000; i++){		
		frameStore[i].var = "none";
		frameStore[i].value = "none";
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
	int i;
	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;
	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return NULL;

}


void printShellMemory(){
	int count_empty = 0;
	for (int i = 0; i < SHELL_MEM_LENGTH; i++){
		if(strcmp(shellmemory[i].var,"none") == 0){
			count_empty++;
		}
		else{
			printf("\nline %d: key: %s\t\tvalue: %s\n", i, shellmemory[i].var, shellmemory[i].value);
		}
    }
	printf("\n\t%d lines in total, %d lines in use, %d lines free\n\n", SHELL_MEM_LENGTH, SHELL_MEM_LENGTH-count_empty, count_empty);
}


/*
 * Function:  addFileToMem 
 * 	Added in A2
 * --------------------
 * Load the source code of the file fp into the shell memory:
 * 		Loading format - var stores fileID, value stores a line
 *		Note that the first 100 lines are for set command, the rests are for run and exec command
 *
 *  pStart: This function will store the first line of the loaded file 
 * 			in shell memory in here
 *	pEnd: This function will store the last line of the loaded file 
 			in shell memory in here
 *  fileID: Input that need to provide when calling the function, 
 			stores the ID of the file
 * 
 * returns: error code, 21: no space left
 */
/*
int load_file(FILE* fp, int* pStart, int* pEnd, char* filename)
{
	char *line;
    size_t i;
    int error_code = 0;
	bool hasSpaceLeft = false;
	bool flag = true;
	i=101;
	size_t candidate;
	while(flag){
		flag = false;
		for (i; i < SHELL_MEM_LENGTH; i++){
			if(strcmp(shellmemory[i].var,"none") == 0){
				*pStart = (int)i;
				hasSpaceLeft = true;
				break;
			}
		}
		candidate = i;
		for(i; i < SHELL_MEM_LENGTH; i++){
			if(strcmp(shellmemory[i].var,"none") != 0){
				flag = true;
				break;
			}
		}
	}
	i = candidate;
	//shell memory is full
	if(hasSpaceLeft == 0){
		error_code = 21;
		return error_code;
	}
    
    for (size_t j = i; j < SHELL_MEM_LENGTH; j++){
        if(feof(fp))
        {
            *pEnd = (int)j-1;
            break;
        }else{
			line = calloc(1, SHELL_MEM_LENGTH);
            fgets(line, 999, fp);
			shellmemory[j].var = strdup(filename);
            shellmemory[j].value = strndup(line, strlen(line));
			free(line);
        }
    }

	//no space left to load the entire file into shell memory
	if(!feof(fp)){
		error_code = 21;
		//clean up the file in memory
		for(int j = 1; i <= SHELL_MEM_LENGTH; i ++){
			shellmemory[j].var = "none";
			shellmemory[j].value = "none";
    	}
		return error_code;
	}
	//printShellMemory();
    return error_code;
} //we won't need this function anymore, we won't use start, end for PCB's anymore
*/ 
//this new load file function will simply copy the scripts into the backingstore directory
//if the function is called, then we already know that fp can't be null
//after the scripts are copied, then the pages are loaded into the frame store
int load_file_backingStore(char* filename){
	FILE *sourceFile = fopen(filename, "r");
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
        fwrite(buffer, 1, bytesRead, destFile);
    }

    // Close files
    fclose(sourceFile);
    fclose(destFile);

    return 0;
}

//this function breaks down one liners
void getCommands(char *line, char *commands[]){
	int count=0;
    const char semic[2]=";";
	int index=0;//where we are inside the commands array
    char *token;
    char *line_copy = strdup(line);
    token = strtok(line_copy, semic);

    while(token!=NULL && index<10){
        commands[index]=strdup(token);
        token=strtok(NULL,semic);
        index++;
        count++;
    }
    
    free(line_copy);
}


//here we iterate through the framePage, and each time we find a 3-line hole, we add a frame
//if we reach the end of the file, we have to add a line with string END that indicates that there isn't anything left to execute.
int load_file_toFramePage(char* filename, PCB *pcb){ 
	FILE *sourceFile = fopen(filename, "r");
	int len=0;
	char buffer[100];
	memset(buffer,0,100); //clear the buffer
	int flag=0; //set to false at first, once we hit the end of the file we'll flip it to true so we can break out of the loop

	char *commandQueue[] = {"X_","X_","X_","X_","X_","X_","X_","X_","X_","X_"}; //up to ten commands in a oneliner
	int commandsLeft=0;
	int arrayIndex=0;
	
	int counter=0; //to make sure we copy 3 commands to every frame
	for (int i=0; i<1000; i++){
		if (flag){ //we've reached the end of the file, we're done
			break;
		}
		if (strcmp(frameStore[i].var, "none") == 0){ //this means we have found a hole of three lines, and we can start copying into the frame table
			pcb->pageTable[pcb->numFrames]=i/3; //in the example if frame starts at 3 or 9, then pagetable has 1 and 3, idk why, so we divide by three
			pcb->numFrames++;

			counter=0;
			while(commandsLeft && counter!=3){//are there commands left from a previous one-liner that we haven't added to the memory yet, if so copy them
				frameStore[i].var=strdup("process");
				frameStore[i].value=strdup(commandQueue[arrayIndex]);
				free(commandQueue[arrayIndex]); //since we used strdup to put a command there
				commandQueue[arrayIndex]="X_"; //to show that the slot doesnt contain a command
				len++;
				i++; //gotta make sure this works
				arrayIndex++;
				counter++;
				if(arrayIndex==10 || (strcmp(commandQueue[arrayIndex], "X_")==0)){ //if we reach the end of the array, or next element is X_then we are done with the oneliner
					arrayIndex=0;
					commandsLeft=0;
				}
			}

			while (counter!=3){ //this means there is still space inside the frame
				if(fgets(buffer,100, sourceFile)!=NULL){
					fputs(buffer, sourceFile);
					getCommands(buffer, commandQueue); //to check how many commands were in that line.
					commandsLeft=1; //we set this to true because now the array contains command(s)
					memset(buffer,0,100);
					//we check inside the array
					int tracker = 0; //to go through the array
					while(commandsLeft){ //keep in mind arrayIndex==0 here
						frameStore[i].var=strdup("process");
						frameStore[i].value=strdup(commandQueue[tracker]);
						free(commandQueue[tracker]); //since we used strdup to put a command there
						commandQueue[tracker]="X_"; //to show that the slot doesnt contain a command
						counter++;
						len++;
						i++;
						tracker++;
						arrayIndex++;
						
						if(counter==3){ //no space left inside the frame, we have to find another one
							break;
						}
						if(arrayIndex==10 || (strcmp(commandQueue[arrayIndex], "X_")==0)){ //we've cleared the array
							arrayIndex=0;
							commandsLeft=0;
						}
					}
				}else{
					frameStore[i].var="END"; //if the script has ended but the frame hasn't then we fill the remaining slots of the frame with END=END 
					frameStore[i].value="END";
					i++;
					flag=1; //so we break out of the outer loop.
				}
			}
		}
	}

	/* we assume that there will always be place for the pages of all programs, so we don't need to consider the case where we reach the end of frameStore 
	if(i==1000){ we're still not done loading a process
		fclose(sourceFile);
		return 1;
	}
	*/
	pcb->length=len;
	pcb->job_length_score=len;
	pcb->numFrames++; //since we start at 0, to get the actual number
	
	fclose(sourceFile);
	return 0;
}



char * mem_get_value_at_line(int index){
	if(index<0 || index > SHELL_MEM_LENGTH) return NULL; 
	return shellmemory[index].value;
}

char * frames_get_value_at_line(int index){
	if(index<0 || index > STOREPAGES_LENGTH) return NULL; 
	return frameStore[index].value;
}

void mem_free_lines_between(int start, int end){
	for (int i=start; i<=end && i<SHELL_MEM_LENGTH; i++){
		if(shellmemory[i].var != NULL){
			free(shellmemory[i].var);
		}	
		if(shellmemory[i].value != NULL){
			free(shellmemory[i].value);
		}	
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

void frameTable_free_frames(QueueNode *n){
	PCB *pcb = n->pcb;
	int limit = pcb->numFrames; //we will go through every frame and delete each line that is inside the frameTable
	int i;
	int frameIndex;

	while(pcb->currentFrame<limit){
		for(i=0;i<3;i++){
			frameIndex = (pcb->pageTable[pcb->currentFrame] * 3)+i;

			if(frameStore[frameIndex].var != NULL){
				free(frameStore[frameIndex].var);
			}	
			if(frameStore[frameIndex].value != NULL){
				free(frameStore[frameIndex].value);
			}	
			frameStore[frameIndex].var = "none";
			frameStore[frameIndex].value = "none";
		}
		pcb->currentFrame++;
	}
}
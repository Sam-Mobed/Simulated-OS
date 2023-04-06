#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include <unistd.h>
#include "shellmemory.h"
#include "ready_queue.h"

//#define SHELL_MEM_LENGTH 1000
//#define STOREPAGES_LENGTH 1000
#define SHELL_MEM_LENGTH VSIZE
#define STOREPAGES_LENGTH FSIZE

struct memory_struct{
	char *var;
	char *value;
};

char c = 'a';
char underscore = '_'; //in case program with the same name has to be loaded multiple times

struct memory_struct shellmemory[SHELL_MEM_LENGTH];
struct memory_struct frameStore[STOREPAGES_LENGTH];

int accessTimeTable[333]; //this will set the access time of when each frame inside the frameStore was accessed, to then evict one 

void setAccessTime(int time, int frame){
    accessTimeTable[frame]=time;
}

void init_accessTimeTable(){
    int i;
    for(i=0;i<333;i++){
        accessTimeTable[i]=-1;
    }
}

int getSmallestAccessTimeIndex(){
    int smallestTime=200;
    int index=0; //the location of the frame that was accessed longtime in the past
    int i;
    for(i=0;i<333;i++){
        if(accessTimeTable[i]==-1){ //it has either never been accessed or nothing there
            continue; 
        }else if (accessTimeTable[i]<smallestTime){
            smallestTime=accessTimeTable[i];
            index=i; //remember this is in multiple of three
        }
    }
    return index;
}

void printAccessTable(){
    int i;
    for(i=0;i<18;i++){
        printf("counter: %d\n", accessTimeTable[i]);
    }
}

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
	for (i=0; i<SHELL_MEM_LENGTH; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

void pages_init(){
	int i;
	for (i=0; i<STOREPAGES_LENGTH; i++){		
		frameStore[i].var = "none";
		frameStore[i].value = "none";
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
	int i;
	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<SHELL_MEM_LENGTH; i++){
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
	for (i=0; i<SHELL_MEM_LENGTH; i++){
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

void printFrameStore(){
	int count_empty = 0;
	for (int i = 0; i < STOREPAGES_LENGTH; i++){
		if(strcmp(frameStore[i].var,"none") == 0){
			count_empty++;
		}
		else{
			printf("\nline %d: key: %s\t\tvalue: %s\n", i, frameStore[i].var, frameStore[i].value);
		}
    }
	printf("\n\t%d lines in total, %d lines in use, %d lines free\n\n", STOREPAGES_LENGTH, (STOREPAGES_LENGTH-count_empty), count_empty);
}

////this function is only to test if the architecture is loaded properly, but it seems to work well so not needed anymore
void printArchitecure(PCB *pcb){
    for(int i=0;i<333;i++){
        for(int j=0;j<3;j++){
            printf("%s\n",pcb->fileArchitecture[i][j]);
        }
        printf("--\n");
        if (i==6){
            break;
        }
    }
}

void load_fileArchitecture(PCB *pcb){ //no need to pass the filename as the pcb already has it.
    FILE *sourceFile = fopen(pcb->pid, "r");
    char buffer[100]; //will hold the content of a line from sourceFile
	memset(buffer,0,100); //initialize the buffer
    int flag=1; //to stop when we're done

    //int counter = 0;
    int currFrame = 0;
    int currCommand = 0; //0, 1 or 2

    int fileLINE = 0;
    int lineCOMMAND = 0; //which command on that line?
    char location[4];

    const char semic[2]=";";
    char *token;

    while (flag){
        if(fgets(buffer,100, sourceFile)!=NULL){
            fputs(buffer, sourceFile);
            token = strtok(buffer, semic);
            while(token!=NULL){
                sprintf(location, "%d_%d", fileLINE, lineCOMMAND);
                pcb->fileArchitecture[currFrame][currCommand]=strdup(location);
                lineCOMMAND++;
                pcb->length++;
                
                currCommand++;
                if(currCommand==3){
                    currFrame++;
                    currCommand=0; //we move on to a new frame
                    pcb->numFrames++;
                }
                token=strtok(NULL,semic);
            }
            fileLINE++;
            lineCOMMAND=0;
            memset(buffer,0,100);

        }
        if (feof(sourceFile)){
            flag=0;
            if (currCommand!=0){ //this means that it hasn't been reset, there is space remaing in last frame
                while(currCommand!=3){
                    pcb->fileArchitecture[currFrame][currCommand]=strdup("END\0");
                    currCommand++;
                }
                pcb->numFrames++;
            }
        }
    }
    pcb->job_length_score=pcb->length;
	fclose(sourceFile);
}

int load_file_backingStore(PCB *pcb){
	FILE *sourceFile = fopen(pcb->pid, "r");
	FILE *destFile;
    char destPath[100]; // allocate enough space
	strcpy(destPath, "./BackingStore/"); // copy initial part of the path
	strcat(destPath, pcb->pid); // append filename
    char buffer[1024];
    size_t bytesRead;

	char suffix[3];

	if(access(destPath, F_OK) != -1){ //file already exists so we change the name so its not overwritten
		suffix[0]=underscore;
		suffix[1]=c;
		suffix[2]='\0';
		c++;
		strcat(destPath, suffix);
		//we need to change the name of pid too
	}
	free(pcb->pid); //free the old name
	pcb->pid=strdup(destPath); //WE need to change the name to the file in backing store
	
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
char *loadCommand(PCB *pcb, int line, int cmdnum){
    FILE *sourceFile = fopen(pcb->pid, "r");
    char buffer[100];
	memset(buffer,0,100);
    const char semic[2]=";";
    char *token;
    

    int currLine=0;
    while (currLine!=line){
        fgets(buffer,100, sourceFile); //skip line until we get to target
        currLine++;
    }

    fgets(buffer,100, sourceFile);
    fputs(buffer, sourceFile);
    token = strtok(buffer, semic);

    int currCommand=0;
    while(currCommand!=cmdnum){
        token=strtok(NULL,semic);
        currCommand++;
    }
    fclose(sourceFile);
    return strdup(token);
}

void evict_Frame(int location){
    int counter = 0;
    int trueLocation = location*3;
    printf("Page fault! Victim page contents:\n\n");
    while(counter!=3){
        printf("%s\n",frameStore[trueLocation+counter].value);
        free(frameStore[trueLocation+counter].var);
        free(frameStore[trueLocation+counter].value);
        frameStore[trueLocation+counter].var=strdup("none");
        frameStore[trueLocation+counter].value=strdup("none");
        counter++;
    }
    printf("\nEnd of victim page contents.\n");
}

void load_file_toFramePage(PCB *pcb, int FRAMES_LIMIT, int timeCounter){ //we don't always need to use the third param
    //printFrameStore();
    
    int indextracker=0;
    int frameCounter=0;
    int end = 0; //in case we reach the end
    
    //now we actually fill the pageTable
    int i;
    for (i=0; i<STOREPAGES_LENGTH; i++){
        indextracker=i;
        if (strcmp(frameStore[indextracker].var, "none") == 0){ //this means we have found a hole of three lines
            pcb->pageTable[pcb->load_frame]=i/3;
            //pcb->accessTimeTable[pcb->load_frame]=0;//this frame has never been accessed
            int k=0;
            while(k!=3){
                char *location=pcb->fileArchitecture[pcb->load_frame][k];
                if(strcmp(location,"END")==0){
                    frameStore[indextracker].var=strdup("END");
				    frameStore[indextracker].value=strdup("END");
                    indextracker++;
                    k++;
                    end=1; //we have reached the end of the file
                }else{
                    int line = atoi(&location[0]);
                    int comnumber = atoi(&location[2]);//incase there is a oneliner
                    //from location we will get the line and the exact command;
                    char *command = loadCommand(pcb, line, comnumber);
                    frameStore[indextracker].var=strdup("process");
                    frameStore[indextracker].value=command; //might need strdup
                    indextracker++;
                    k++;
                }
            }

            pcb->load_frame++;
            frameCounter++;

            if(pcb->load_frame==pcb->numFrames){
                return;
            }

            if (frameCounter==FRAMES_LIMIT || end){
                return;
            }
            i=i+2;
        }
    }

    if (i==STOREPAGES_LENGTH && frameCounter!=FRAMES_LIMIT){
        //printAccessTable();
        int evictThisFrame = getSmallestAccessTimeIndex();
        setAccessTime(timeCounter,evictThisFrame);
        //printAccessTable();
        //printf("evict:%d\n", evictThisFrame);
        evict_Frame(evictThisFrame);
        load_file_toFramePage(pcb, FRAMES_LIMIT-frameCounter, timeCounter);
    } 
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


/*
* this function was made in case the frame that was evicted wasnt done executing. but this cannot happen for FCFS, SJF or RR. the only 
* policy that might cause such an incident is AGING, but we're not testing AGING
* so best leave the function here for now. 
void reload_Currentframe(PCB *pcb){//we only need to reload if it's the current frame that was lost.
    //we go inside framestore, find a hole or remove another frame, then reload 
    //we have to update the location of the current frame inside the pagetable
    //and maybe some other info and we should be good
    int indextracker=0;
    int i;
    int done=0;//to let us know that currentframe has been reloaded
    for (i=0; i<STOREPAGES_LENGTH; i++){
        indextracker=i;
        if (strcmp(frameStore[indextracker].var, "none") == 0){
            pcb->pageTable[pcb->currentFrame]=i/3;
            pcb->accessTimeTable[pcb->currentFrame]=0;//this frame has never been accessed, 
            //but it'll be updated once we get out of the function
            int k=0;
            while(k!=3){
                char *location=pcb->fileArchitecture[pcb->currentFrame][k];
                if(strcmp(location,"END")==0){
                    frameStore[indextracker].var=strdup("END");
				    frameStore[indextracker].value=strdup("END");
                    indextracker++;
                    k++;
                    continue;
                }
                int line = atoi(&location[0]);
                int comnumber = atoi(&location[2]);//incase there is a oneliner
                //from location we will get the line and the exact command;
                char *command = loadCommand(pcb, line, comnumber);
                frameStore[indextracker].var=strdup("process");
				frameStore[indextracker].value=command; //might need strdup
                indextracker++;
                k++;
            }
            done=1;
            break; //we only load one frame
        }
    }

    if (!done){
        int evictThisFrame = get_leastRecentFrame();
        evict_Frame(evictThisFrame);
        reload_Currentframe(pcb);
    }
}



*/
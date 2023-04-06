#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <time.h>
#include <stdbool.h>

int FSIZE = 1000;
#define STOREPAGES_LENGTH FSIZE

struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct frameStore[1000];

void pages_init(){
	int i;
	for (i=0; i<STOREPAGES_LENGTH; i++){		
		frameStore[i].var = "none";
		frameStore[i].value = "none";
	}
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

typedef struct
{
    bool priority;
    //int pid; //1000/3=333 approximately its useless to leave the pid as an int. and no where in the src file is it really used. it'll be more useful
    //if the pid was a string that was the name of its sourceFile.
    char *pid;
    int pageTable[333]; //the maximum number of frames is 333: 1000/3=333. if we were to load one huge process
    int accessTimeTable[333]; //every time we access a frame, a counter will go up. the larger the counter the more recently it has been used
    char *fileArchitecture[333][3];//up to 333 frames, each frame can draw from max three different lines
    //example: fileArchitecutre[0][0]="0_0\0" the first line of the first frame comes from the first command of the first line of the sourceFile
    //thus, if we have to reload the first frame, we know exactly where to look
    int numFrames; //calculated with filearchitecture
    int length; //calculated with fileArchitecture
    int currentFrame;
    int currentLine; 
    int job_length_score;
    //char *FILE_LINENO; //"2_3\0" where we left off. the next line to be loaded is the third command from line 2 inside the sourcefile.  
    int load_frame; //the next frame to be loaded
}PCB;

typedef struct QueueNode{
    PCB *pcb;
    struct QueueNode *next;
} QueueNode;

//QueueNode head=NULL;

void pageTable_init(int arr[]){
    int i;
    for(i=0;i<333;i++){
        arr[i]=-1;
    }
}

void architecture_init(char* arr[][3]){
    int i, j;
    for(i=0;i<333;i++){ //for each frame
        for(j=0;j<3;j++){ //for each line of the frame
            arr[i][j] = strdup("000\0");
        }
    }
}

//In this implementation, Pid is the same as file ID 
PCB* makePCB(char *filename){
    PCB * newPCB = malloc(sizeof(PCB));
    //newPCB->pid = generatePID();
    newPCB->pid=strdup(filename);
    newPCB->currentFrame = 0; //because we all start at 0, and everytime a frame is executed we can increment this
    newPCB->numFrames=0; //will be incremented each time we add a frame to frameTable
    newPCB->length=0; //will be set to the right size later
    newPCB->currentLine=0; // we need this for RR and aging
    newPCB->job_length_score=0;
    newPCB->priority = false;
    pageTable_init(newPCB->pageTable);
    pageTable_init(newPCB->accessTimeTable);
    architecture_init(newPCB->fileArchitecture);
    //newPCB->FILE_LINENO=strdup("0_0\0"); //the next line to load is from the first command of the first line of the sourceFile
    newPCB->load_frame=0;//since we have to start from the very first frame. 
    return newPCB;
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
            }
        }
    }
    pcb->job_length_score=pcb->length;
}

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

    return strdup(token);
}

void evict_Frame(int location){
    int counter = 0;
    printf("Page fault! Victim page contents:\n");
    while(counter!=3){
        printf("%s\n",frameStore[location+counter].value);
        free(frameStore[location+counter].var);
        free(frameStore[location+counter].value);
        frameStore[location+counter].var=strdup("none");
        frameStore[location+counter].value=strdup("none");
        counter++;
    }
    printf("End of victim page contents.\n");
}

void load_file_toFramePage(PCB *pcb, int FRAMES_LIMIT){
    int indextracker=0;
    int frameCounter=0;
    int end = 0; //in case we reach the end
    
    //now we actually fill the pageTable
    int i;
    for (i=0; i<STOREPAGES_LENGTH; i++){
        indextracker=i;
        if (strcmp(frameStore[indextracker].var, "none") == 0){ //this means we have found a hole of three lines
            pcb->pageTable[pcb->load_frame]=i/3;
            pcb->accessTimeTable[pcb->load_frame]=0;//this frame has never been accessed
            int k=0;
            while(k!=3){
                char *location=pcb->fileArchitecture[pcb->load_frame][k];
                if(strcmp(location,"END")==0){
                    frameStore[indextracker].var=strdup("END");
				    frameStore[indextracker].value=strdup("END");
                    indextracker++;
                    k++;
                    end=1; //we have reached the end of the file
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

            pcb->load_frame++;
            frameCounter++;

            if (frameCounter==FRAMES_LIMIT || end){
                break;
            }
            i=i+2;
        }
    }
}

int main(int argc, char *argv[]){
    pages_init();

    PCB *pcb = makePCB("set.txt");

    load_fileArchitecture(pcb);

    printf("%s\n", pcb->pid);
    printf("length: %d\n",pcb->length);
    printf("score: %d\n",pcb->length);
    printf("numFrames: %d\n",pcb->numFrames);

    printf("Command:%s\n", loadCommand(pcb, 0, 1));
    printf("Command:%s\n", loadCommand(pcb, 0, 2));
    printf("Command:%s\n", loadCommand(pcb, 7, 1));
    printf("Command:%s\n", loadCommand(pcb, 7, 3));
    printf("Command:%s\n", loadCommand(pcb, 8, 0));
    
    //printf("%d\n",pcb.);
    printArchitecure(pcb);

    printShellMemory();
    return 0;
}








/*OLD LOAD FILE TO FRAMEPAGE

//here we iterate through the framePage, and each time we find a 3-line hole, we add a frame
//if we reach the end of the file, we have to add a line with string END that indicates that there isn't anything left to execute.
int load_file_toFramePage(char* filename, PCB *pcb, int FRAMES_LIMIT){
	FILE *sourceFile = fopen(filename, "r");
	int len=0;
	char buffer[100];
	memset(buffer,0,100); //clear the buffer
	int flag=0; //set to false at first, once we hit the end of the file we'll flip it to true so we can break out of the loop
	/////////
	char *commandQueue[] = {"X_","X_","X_","X_","X_","X_","X_","X_","X_","X_"}; //up to ten commands in a oneliner
	int commandsLeft=0;
	int arrayIndex=0;
    int indexTracker=0;
    int frameCounter=0;
	
	int counter=0; //to make sure we copy 3 commands to every frame
	for (int i=0; i<1000; i++){
        indexTracker=i;
        
		if (flag){ //we've reached the end of the file, we're done
			break;
		}
		if (strcmp(frameStore[indexTracker].var, "none") == 0){ //this means we have found a hole of three lines, and we can start copying into the frame table
            pcb->pageTable[frameCounter]=i/3;
            frameCounter++;

			counter=0;
			while(commandsLeft && counter!=3){//are there commands left from a previous one-liner that we haven't added to the memory yet, if so copy them
                frameStore[indexTracker].var=strdup("process");
				frameStore[indexTracker].value=strdup(commandQueue[arrayIndex]);
				free(commandQueue[arrayIndex]); //since we used strdup to put a command there
				commandQueue[arrayIndex]="X_"; //to show that the slot doesnt contain a command
				indexTracker++;
                len++;
				arrayIndex++;
				counter++;
				if(arrayIndex==10 || (strcmp(commandQueue[arrayIndex], "X_")==0)){ //if we reach the end of the array, or next element is X_then we are done with the oneliner
					arrayIndex=0;
					commandsLeft=0;
                    break;
				}
			}

			while (counter!=3 && !flag){ //this means there is still space inside the frame
				if(fgets(buffer,100, sourceFile)!=NULL){
					fputs(buffer, sourceFile);
					getCommands(buffer, commandQueue); //to check how many commands were in that line.
					commandsLeft=1; //we set this to true because now the array contains command(s)
					memset(buffer,0,100);
					//we check inside the array
					int tracker = 0; //to go through the array
					while(commandsLeft){ //keep in mind arrayIndex==0 here
						frameStore[indexTracker].var=strdup("process");
						frameStore[indexTracker].value=strdup(commandQueue[tracker]);
						free(commandQueue[tracker]); //since we used strdup to put a command there
						commandQueue[tracker]="X_"; //to show that the slot doesnt contain a command
						counter++;
						len++;
                        indexTracker++;
						tracker++;
						arrayIndex++;

                        if(arrayIndex==10 || (strcmp(commandQueue[arrayIndex], "X_")==0)){ //we've cleared the array
							arrayIndex=0;
							commandsLeft=0;
                            break;
						}
						if(counter==3){ //no space left inside the frame, we have to find another one
							break;
						}
					}
                    if (feof(sourceFile)){
                        flag=1;
                    }
				}
                if (flag && counter!=3){
                    while (counter!=3){
                        frameStore[indexTracker].var="END"; //if the script has ended but the frame hasn't then we fill the remaining slots of the frame with END=END 
                        frameStore[indexTracker].value="END";
                        indexTracker++;
                        counter++;
                    }
				}
			}
            i=i+2;
		}
	}
	pcb->length=len;
	pcb->job_length_score=len;
	pcb->numFrames=frameCounter; //since we start at 0, to get the actual number
	
	fclose(sourceFile);
	return 0;
}


*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <time.h>
#include <stdbool.h>

#define STOREPAGES_LENGTH 1000

struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct frameStore[STOREPAGES_LENGTH];

void pages_init(){
	int i;
	for (i=0; i<1000; i++){		
		frameStore[i].var = "none";
		frameStore[i].value = "none";
	}
}

void printShellMemory(){
	int count_empty = 0;
	for (int i = 0; i < STOREPAGES_LENGTH; i++){
		if(strcmp(frameStore[i].var,"none") == 0){
			count_empty++;
		}
		else{
			printf("\nline %d: key: %s\t\tvalue: %s\n", i, frameStore[i].var, frameStore[i].value);
		}
    }
	printf("\n\t%d lines in total, %d lines in use, %d lines free\n\n", STOREPAGES_LENGTH, STOREPAGES_LENGTH-count_empty, count_empty);
}

/*
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
*/
/*
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
*/

typedef struct
{
    bool priority;
    int pid;
    int pageTable[333]; //the maximum number of frames is 333: 1000/3=333. if we were to load one huge process
    int numFrames;
    int length;
    int currentFrame;
    int currentLine;
    int job_length_score;
}PCB;

int pid_counter = 1;

int generatePID(){
    return pid_counter++;
}

void pageTable_init(int arr[]){
    for(int i=0;i<334;i++){
        arr[i]=-1;
    }
}

//In this implementation, Pid is the same as file ID 
PCB* makePCB(){
    PCB * newPCB = malloc(sizeof(PCB));
    newPCB->pid = generatePID();
    newPCB->currentFrame = 0; //because we all start at 0, and everytime a frame is executed we can increment this
    newPCB->numFrames=0; //will be incremented each time we add a frame to frameTable
    newPCB->length=0; //will be set to the right size later
    newPCB->currentLine=0; // we need this for RR and aging
    newPCB->job_length_score=0;
    newPCB->priority = false;
    pageTable_init(newPCB->pageTable);
    return newPCB;
}

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

void printarr(char *commands[]){
    for (int i=0;i<10;i++){
        printf("%s\n", commands[i]);
    }
    printf("%s\n","END");
}

void printFrame(){
    for (int i=0;i<10;i++){
        printf("%s\n", frameStore[i].value);
    }
}

int load_file_toFramePage(char* filename, PCB *pcb){
	FILE *sourceFile = fopen(filename, "r");
	int len=0;
	char buffer[100];
	memset(buffer,0,100); //clear the buffer
	int flag=0; //set to false at first, once we hit the end of the file we'll flip it to true so we can break out of the loop

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

//initializeBackingStore();
	//load_file_backingStore("sampletestFile");
	//contains_unprintable_bytes();
    /*
    char *commandQueue[] = {"X_","X_","X_","X_","X_","X_","X_","X_","X_","X_"}; //up to ten commands in a oneliner
    char *line="echo hello";
    //char *line="\n";
    //char *line="";

    int c = getCommands(line, commandQueue);
    printf("%d\n",c);
    for(int i = 0; i < 10; i++){
      printf("%s\n", commandQueue[i]);
    }


    for (int i=0;i<10;i++){
        printf("%d\n", newPcb->pageTable[i]);
    }
    */

int main(int argc, char *argv[]){
    pages_init();

    frameStore[0].var="process";
    frameStore[0].value="test";
    frameStore[1].var="process";
    frameStore[1].value="test1";
    frameStore[2].var="process";
    frameStore[2].value="test2";

    frameStore[6].var="process";
    frameStore[6].value="test";
    frameStore[7].var="process";
    frameStore[7].value="test1";
    frameStore[8].var="process";
    frameStore[8].value="test2";
    frameStore[9].var="process";
    frameStore[9].value="test";
    frameStore[10].var="process";
    frameStore[10].value="test1";
    frameStore[11].var="process";
    frameStore[11].value="test2";


    PCB *newPcb = makePCB();

    load_file_toFramePage("../src/set.txt",newPcb);
    //printf("%s\n","DONE");

    printShellMemory();
    
    printf("%d\n", newPcb->pageTable[0]);
    printf("%d\n", newPcb->pageTable[1]);
    printf("%d\n", newPcb->pageTable[2]);
    printf("%d\n", newPcb->pageTable[3]);

    printf("%d\n",newPcb->length);
    printf("%d\n",newPcb->job_length_score);
    printf("%d\n",newPcb->numFrames);
    free(newPcb);
    return 0;
}
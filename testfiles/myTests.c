#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <time.h>
#include <stdbool.h>


//#define STOREPAGES_LENGTH 1000
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

//int pid_counter = 1;
/*
int generatePID(){
    return pid_counter++;
}
*/

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

typedef struct QueueNode{
    PCB *pcb;
    struct QueueNode *next;
} QueueNode;

QueueNode head;


//here we setup the PCB. we don't load anything into the memory yet, but we go through the file and 
//see how it's organized. this will determine how many frames in total we will need to fit the entire content
//this way, we will know to what line (and to which command on that line, if its a oneliner) a command on the frameStore corresponds
//this will make storing and reloading much easier.
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
                sprintf(location, "%d_%d\0", fileLINE, lineCOMMAND);
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

char *loadCommand(PCB *pcb, int line, int cmdnum){
    FILE *sourceFile = fopen(pcb->pid, "r");
    char buffer[100];
	memset(buffer,0,100);
    const char semic[2]=";";
    char *token;
    

    int currLine=0;
    while (currLine!=line+1){
        fgets(buffer,100, sourceFile); //skip line until we get to target
        currLine++;
    }

    fgets(buffer,100, sourceFile);
    fputs(buffer, sourceFile);
    char *line_copy = strdup(buffer);
    token = strtok(line_copy, semic);

    int currCommand=0;
    while(currCommand!=cmdnum+1){
        token=strtok(NULL,semic);
        currCommand++;
    }

    free(line_copy);
    return strdup(token);
}

int get_leastRecentFrame(){
    //we have to go through every single accesstable (of each pcb)
    //and find the frame that was accessed the least recent
    //-1 frame DNE, youve reached the end of the frame table
    //0: frame has never been accessed, automatically evicted
    //min_access points to the pcb->pageTable->frame accessed the least early

    QueueNode *cur = head;
    PCB *curPCB;
    int smallestAccessTime = 200;
    int frameSmallestATlocation; //the location of the frame we'll remove

    int i;
    int flag=0;
    int frameLocation;
    while(cur->next!=NULL){
        curPCB=cur->pcb;
        for(i=0;i<333;i++){//we check every frame of the pcb
            frameLocation=curPCB->pageTable[i];
            if (frameLocation==-1){
                break; //we've hit the end or the rest hasn't been loaded yet
            }else if (curPCB->accessTimeTable[i]==0){ //this one hasn't been used yet we can evict it
                curPCB->accessTimeTable[i]=-2; //it got evucted
                curPCB->pageTable[i]=-2; //it got evicted
                return frameLocation;
            }else if(curPCB->accessTimeTable[i]<smallestAccessTime){
                smallestAccessTime=curPCB->accessTimeTable[i];
                frameSmallestATlocation=frameLocation;
            }
        }
        cur = cur->next;
    }
    //the frame that does get evicted needs to have its location replaced by -2, meaning it was evicted
    return frameSmallestATlocation; //return the index of the frame that was evicted.
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

    if (i==STOREPAGES_LENGTH && frameCounter!=FRAMES_LIMIT){
        int evictThisFrame = get_leastRecentFrame();
        evict_Frame(evictThisFrame);
        load_file_toFramePage(pcb, FRAMES_LIMIT-frameCounter);
    } 
}

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





//we have to check if the frame we want to load has already been loaded once. because if it has been loaded, then 
//the length the number of frames and everything else will be repeated. 


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


    PCB *newPcb = makePCB("set.txt");

    //load_file_toFramePage("./set.txt",newPcb, 2);
    //load_file_toFramePage("./set.txt",newPcb, 1);
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
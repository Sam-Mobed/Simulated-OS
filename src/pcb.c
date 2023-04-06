#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"

/*
int pid_counter = 1;

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

/*
//In this implementation, Pid is the same as file ID 
PCB* makePCB(){
    PCB * newPCB = malloc(sizeof(PCB));
    newPCB->pid = generatePID();
    //newPCB->PC = start;
    //newPCB->start  = start;
    //newPCB->end = end; //none of these are relevant anymore
    newPCB->currentFrame = 0; //because we all start at 0, and everytime a frame is executed we can increment this
    newPCB->numFrames=0; //will be incremented each time we add a frame to frameTable
    newPCB->length=0; //will be set to the right size later
    newPCB->currentLine=0; // we need this for RR and aging
    newPCB->job_length_score=0;
    newPCB->priority = false;
    pageTable_init(newPCB->pageTable);
    return newPCB;
}
*/

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
    //pageTable_init(newPCB->accessTimeTable);
    architecture_init(newPCB->fileArchitecture);
    //newPCB->FILE_LINENO=strdup("0_0\0"); //the next line to load is from the first command of the first line of the sourceFile
    newPCB->load_frame=0;//since we have to start from the very first frame. 
    return newPCB;
}
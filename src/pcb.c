#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"

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
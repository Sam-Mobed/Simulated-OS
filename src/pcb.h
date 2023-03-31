#ifndef PCB_H
#define PCB_H
#include <stdbool.h>
/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * PC: program counter, stores the index of line that the task is executing
 * start: the first line in shell memory that belongs to this task
 * end: the last line in shell memory that belongs to this task
 * job_length_score: for EXEC AGING use only, stores the job length score
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

int generatePID();
PCB * makePCB();
#endif
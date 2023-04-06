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
/* old version
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
*/

typedef struct
{
    bool priority;
    //int pid; //1000/3=333 approximately its useless to leave the pid as an int. and no where in the src file is it really used. it'll be more useful
    //if the pid was a string that was the name of its sourceFile.
    char *pid;
    int pageTable[333]; //the maximum number of frames is 333: 1000/3=333. if we were to load one huge process
    char *fileArchitecture[333][3];//up to 333 frames, each frame can draw from max three different lines
    //example: fileArchitecutre[0][0]="0_0\0" the first line of the first frame comes from the first command of the first line of the sourceFile
    //thus, if we have to reload the first frame, we know exactly where to look
    int numFrames; //calculated with filearchitecture
    int length; //calculated with fileArchitecture
    int currentFrame;
    int currentLine; 
    int job_length_score;
    //int accessTimeTable[333]; //every time we access a frame, a counter will go up. the larger the counter the more recently it has been used
    //char *FILE_LINENO; //"2_3\0" where we left off. the next line to be loaded is the third command from line 2 inside the sourcefile.  
    int load_frame; //the next frame to be loaded
}PCB;

PCB* makePCB(char *filename);
#endif
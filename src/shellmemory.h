#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

#include "pcb.h"
#include "ready_queue.h"

void mem_init();
void pages_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
//int load_file(FILE* fp, int* pStart, int* pEnd, char* filename); //not needed anymore
int load_file_backingStore(char* filename);
int load_file_toFramePage(char* filename, PCB *pcb);
char * mem_get_value_at_line(int index);
void mem_free_lines_between(int start, int end);
char * frames_get_value_at_line(int index);
void frameTable_free_frames(QueueNode *n);
void printShellMemory();
#endif
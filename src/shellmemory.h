/*
void mem_init();
int memory_set_process(FILE *p);
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);

struct PCB;
struct node;
extern struct node *head;
*/

#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H

#include <stdio.h>

struct memory_struct{
	char *var;
	char *value;
};

void mem_init();
int memory_set_process(FILE *p);
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
void clear_slot(int slot);
struct memory_struct *get_mem_struct(int index);
void reset_tracker();

//extern struct memory_struct shellmemory[1000];

//extern struct node *head;

#endif /* SHELLMEMORY_H */

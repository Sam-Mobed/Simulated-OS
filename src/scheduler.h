

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>

enum policy{
	FCFS,
	SJF,
	RR,
	AGING,
	UNKNOWN
};

void execute_processes(enum policy pol);
void execute_FCFS ();
void execute_SJF ();
void execute_RR ();
void execute_AGING ();
//void remove_node(struct node *ptr);
//this causes an issue since it doesnt have access to node 
//definition inside this header file, but we don't need this here anyway.

#endif /* SCHEDULER_H */
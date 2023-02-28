#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "shellmemory.h"
#include "scheduler.h"
#include "shell.h"

void remove_node(struct node *ptr);

void execute_processes(enum policy pol){
	switch(pol){
		case FCFS:
			execute_FCFS();
			break;
		case SJF:
			execute_SJF();
			break;
		case RR:
			execute_RR();
			break;
		case AGING:
			execute_AGING();
			break;
		default:
			printf("Invalid policy.\n");
			break;
	}
}

void execute_FCFS (){
	struct PCB *pcb = head->Content;
	struct node *ptr = head;
	int current = pcb->start;
	int end = current+(pcb->length);
	
	while (1){
		//we need to load the PCB for each,
		//iterate through the slots and call the interpreter
		//for each line

		while(current<end){
			parseInput(get_mem_struct(current)->value);
			clear_slot(current); //each line is cleared
			current++;
		}

		free(pcb); //the pcb of the process that was executed is cleared
		struct node *oldHead = ptr;
		if (oldHead->next==NULL){
			free(oldHead); //the node that held the pcb is freed
			head->Content=NULL;
			head->next=NULL;
			break;
		}else{ //fix this
			head=ptr->next;
			ptr=ptr->next;
			free(oldHead);
		}
		pcb = ptr->Content;
		current=pcb->start;
		end=current+(pcb->length);
		//once each process is done executing, we need to 
		//delete its line from memory, deletee its pcb and remove it from queue
	}
}

void execute_SJF (){
	struct PCB *to_execute;
	int smallestLen = 200;
	struct node *ptr = head;

	while(head!=NULL){
		
		while(ptr->next!=NULL){
			if (ptr->Content->length<smallestLen){
			smallestLen = head->Content->length;
			to_execute = ptr;
			}
			ptr=ptr->next;
		}//this needs to be done in a loop outside!!!!!
		struct PCB *pcb = ptr->Content;
		char line[100];
		int current = pcb->start;
		int end = current+(pcb->length);

		while(current<end){
			fgets(line, 99, get_mem_struct(current)->value);
			parseInput(line);
			memset(line, 0, sizeof(line));
			clear_slot(current); //each line is cleared
			current++;
		}

		free(pcb); //the pcb of the process that was executed is cleared
		remove_node(ptr);
	}
}
/*
void execute_RR (){
	struct PCB *to_execute;
	int smallestLen = 200;
	struct node *ptr = head;

	while(head!=NULL){
		to_execute = ptr->Content;
		char line[100];
		int current = to_execute->current;
		int end = current+(to_execute->length);

		for(int i=0;current<end && i<2;i++){
			fgets(line, 99, get_mem_struct(current)->value);
			parseInput(line);
			memset(line, 0, sizeof(line));
			clear_slot(current);
			current++;
		}

		if(current=end){
			free(to_execute);
			remove_node(ptr);
		}
		if(head!=NULL){
			if (ptr->next==NULL){
				ptr=head;
			}else{
				ptr=ptr->next; 
			}
		}
	}
}

void execute_AGING (){
	return;
}
*/
//REEMOVE THE BOTTOM DEFS
void execute_AGING (){
	return;
}

void execute_SJF (){
	return;
}

void execute_RR (){
	return;
}

void remove_node(struct node *ptr){
	//first we need to find the node that is pointing to it.
	struct node *p = head;
	if (p->next==NULL){
		free(p);
		head=NULL;
	}
	while(p->next!=NULL){
		if (p->next==ptr){
			break;
		}
	}
	//the node after the one we want to remove
	struct node *nezt = ptr->next;
	p->next = nezt;
	free(ptr);
}
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "shellmemory.h"
#include "scheduler.h"
#include "shell.h"

//void remove_node(struct node *ptr);
void remove_pcb(struct PCB *ptr);
void reset_queue();

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
			break;
	}
}

void execute_FCFS (){
	struct PCB *pcb = head->Content;
	int current = pcb->start;
	int end = current+(pcb->length);
	struct node *execute = head;
	
	while (1){
		//we need to load the PCB for each,
		//iterate through the slots and call the interpreter
		//for each line

		while(current<end){
			parseInput(get_mem_struct(current)->value);
			clear_slot(current); //each line is cleared
			current++;
		}

		//this if statement is new
		if(execute->next==NULL){
			break;
		}else{
			execute=execute->next;
		}

		pcb = execute->Content;
		current=pcb->start;
		end=current+(pcb->length);
		//once each process is done executing, we need to 
		//delete its line from memory, deletee its pcb and remove it from queue
	}
	reset_tracker();
	reset_queue();
}

void execute_RR (){
	return;
}

void execute_SJF (){
	return;
}
/*
struct node *SJF_helper(){
	struct node *ptr = head;
	int smallestLen = 200;
	struct node *execute = ptr;

	while(ptr->next!=NULL){
		int len = ptr->Content->length;
		if (len<smallestLen){
			smallestLen = len;
			execute=ptr;
		}
		ptr = ptr->next;
	}
	return execute;
}

void execute_SJF (){

	struct node *execute = SJF_helper();
	struct PCB *exec = execute->Content;

	while(1){
		int current = exec->start;
		int end = current+(exec->length);

		while(current<end){
			parseInput(get_mem_struct(current)->value);
			clear_slot(current); //each line is cleared
			current++;
		}

		free(exec); //the pcb of the process that was executed is cleared
		remove_node(execute);
		if (head->Content==NULL){
			break;
		}
		execute=SJF_helper();
		exec=execute->Content;
	}
}

struct node *RR_findNextNode(struct node *ptr){
	if (ptr->next!=NULL){
		return ptr->next;
	}else{
		return head;
	}
}

void execute_RR (){
	struct node *execute = head;
	struct PCB *exec = execute->Content;
	int current = exec->current;
	int end = current+(exec->length);

	while(1){
		int original_current = current;
		for(int i=current;i<original_current+2 && i<end;i++){
			parseInput(get_mem_struct(current)->value);
			clear_slot(current); //each line is cleared
			current++;
			exec->length--; //to be able to accurately calculate the end
		}

		if(head->next==NULL){
			if (current!=end){
				continue;
			}else{
				free(exec);
				head->Content=NULL;
			}
		}else{
			if (current!=end){
				execute = RR_findNextNode(execute);
				exec = execute->Content;
				current = exec->current;
				end = current+(exec->length);
			}else{
				free(exec);
				struct node *remove = execute;
				execute = RR_findNextNode(execute);
				remove_node(remove);
				exec = execute->Content;
				current = exec->current;
				end = current+(exec->length);
			}
		}
	}
}
*/

void execute_AGING (){
	return;
}
/*
void remove_node(struct node *ptr){
	//first we need to find the node that is pointing to it.
	if (head->Content==NULL){
		return; //the Linked list is already empty
	}else if (head->next==NULL){ // only the head was left
		head->Content=NULL;
		return;
	}
	struct node *p = head;
	
	while(p->next!=NULL){ //we find the node before ptr
		if (p->next==ptr){
			break;
		}
		p=p->next;
	}
	//the node after the one we want to remove
	struct node *nezt = ptr->next;
	p->next = nezt;
	free(ptr);
}
*/

void remove_pcb(struct PCB *ptr){
	free(ptr->pid); //remember how we used malloc to generate PID, now we have to free that memory
	free(ptr); //we free the memory allocated for the struct itself
}

void reset_queue(){
	struct node *ptr = head->next;
	struct node *current = ptr;
	if (ptr!=NULL){
		while (ptr->next!=NULL){
		ptr=ptr->next;
		remove_pcb(current->Content);
		free(current);
		current=ptr;
		}
	}
	remove_pcb(head->Content);
	head->Content=NULL;
	head->next=NULL;
}
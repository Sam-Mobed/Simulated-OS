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
	struct node *execute = head;
	struct PCB *pcb;
	int current, end;
	
	while (1){
		pcb = execute->Content;
		current = pcb->start;
		end = current+(pcb->length);

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
		//once each process is done executing, we need to 
		//delete its line from memory, deletee its pcb and remove it from queue
	}
	reset_tracker();
	reset_queue();
}

void remove_node(struct node *ptr){
	if (head->next==NULL){
		free(head->Content->pid);
		free(head->Content);
		head->Content=NULL;
		head->next=NULL; //redundant but wtv
	}else if (head==ptr){ //shouldn't we remove this
		head=head->next;
		free(ptr->Content->pid);
		free(ptr->Content);
		free(ptr);
	}else{
		struct node *p = head;
		while(p->next!=ptr){
			p=p->next; //this will get us the node before the one we want to remvoe. 
		}
		p->next=ptr->next; //the one before ptr will point to the node after it
		free(ptr->Content->pid);
		free(ptr->Content);
		free(ptr);
	}
}

void sortedQueue() {
    if (head->Content == NULL || head->next == NULL) {
        return;
    }
    int swapped = 1;
    while (swapped) {
        swapped = 0;
        struct node *current = head;
        struct node *prev = NULL;
        while (current->next != NULL) {
            struct node *next = current->next;
            if (current->Content->length > next->Content->length) {
                if (prev == NULL) {
                    head = next;
                } else {
                    prev->next = next;
                }
                current->next = next->next;
                next->next = current;
                swapped = 1;
                break;
            }
            prev = current;
            current = next;
        }
    }
}

void execute_SJF (){
	sortedQueue();

	struct node *execute = head;
	struct PCB *pcb;
	int current, end;
	
	while (1){
		pcb = execute->Content;
		current = pcb->start;
		end = current+(pcb->length);

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
		//once each process is done executing, we need to 
		//delete its line from memory, deletee its pcb and remove it from queue
	}
	reset_tracker();
	reset_queue();
}

struct node *RR_helper(struct node *ptr){
	if (head->next==NULL){
		return head;
	}else if (ptr->next==NULL){
		return head;
	}else{
		return ptr->next;
	}
}

void execute_RR (){
	struct node *execute = head;
	struct PCB *pcb = execute->Content;
	int current = pcb->current;
	int end = current+(pcb->length);
	int delta = 2;
	int counter;
	
	while (head->Content!=NULL){
		counter = 0;

		while(current<end && counter<2){
			parseInput(get_mem_struct(current)->value);
			clear_slot(current); //each line is cleared
			current++;
			counter++;
			pcb->current++;
			pcb->length--;
		}
		
		if (current==end){
			if(head->next==NULL){
				remove_node(execute);
				break;
			}else{
				//remove_node and get the next one.
				struct node *tmp = execute;
				execute = RR_helper(execute);
				remove_node(tmp);
			}
		}else{
			execute = RR_helper(execute);
		}

		pcb = execute->Content;
		current = pcb->current;
		end = current+(pcb->length);
	}
	reset_tracker();
	//reset_queue();
}

void ageNodesExecept(struct node *p){
	struct node *ptr = head;
	if (head->next==NULL){
		return;
	}
	while(ptr->next!=NULL){
		if (ptr==p || ptr->Content->score==0){
			ptr=ptr->next;
			continue;
		}
		ptr->Content->score--;
		ptr=ptr->next;
	}
}

void ageNodes(){
	if (head->next==NULL){
		return;
	}
	struct node *ptr = head->next;
	while(ptr!=NULL){
		if (ptr->Content->score==0){
			ptr=ptr->next;
			continue;
		}
		ptr->Content->score--;
		ptr=ptr->next;
	}
}

void sortedQueuebyScore() {
    if (head->Content == NULL || head->next == NULL) {
        return;
    }
    int swapped = 1;
    while (swapped) {
        swapped = 0;
        struct node *current = head;
        struct node *prev = NULL;
        while (current->next != NULL) {
            struct node *next = current->next;
            if (current->Content->score > next->Content->score) {
                if (prev == NULL) {
                    head = next;
                } else {
                    prev->next = next;
                }
                current->next = next->next;
                next->next = current;
                swapped = 1;
                break;
            }
            prev = current;
            current = next;
        }
    }
}

void execute_AGING (){
	sortedQueuebyScore();

	struct node *execute;
	struct PCB *pcb;
	int current, end;
	
	while (head->Content!=NULL){
		execute = head;
		pcb = execute->Content;
		current = pcb->current;
		end = current+(pcb->length);

		parseInput(get_mem_struct(current)->value);
		clear_slot(current); //each line is cleared
		current++;
		pcb->current++;
		pcb->length--;
		
		
		if (current==end){
			if(head->next==NULL){
				remove_node(execute);
				break;
			}else{
				//remove_node and get the next one.
				remove_node(execute);
			}
		}
		ageNodes();
		sortedQueuebyScore();
	}
	reset_tracker();
}

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
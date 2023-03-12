#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include "shellmemory.h"
#include "shell.h"

struct memory_struct shellmemory[1000];
int tracker = 0; //when processes are done running, we need to make sure tracker is reset

//assuming each process will be at most 140 lines long, and we will have at most 3 processes
//we set aside 420 slots in the array to store the lines for the processes
// this tracker will help us know the next available slot, and to make sure we don't go above 420


// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}

//this functions generates a PID using the current time, that way its unique.
char *generatePID(){
	time_t now;
    struct tm *local_time;
    char time_string[100];

    time(&now);
    local_time = localtime(&now);

    strftime(time_string, sizeof(time_string), "%Y%m%d%H%M%S", local_time);
    return strdup(time_string);
}

struct PCB *generatePCB(int start){
	struct PCB *new_pcb = (struct PCB*) malloc(sizeof(struct PCB));
	new_pcb->pid = generatePID();
	new_pcb->start = start;
	new_pcb->current = start;
	new_pcb->length = 0;
	new_pcb->score = 0;

	return new_pcb;
}

struct node *generateNode(){
	struct node *new_node = (struct node*) malloc(sizeof(struct node));
	new_node->Content=NULL;
	new_node->next=NULL;

	return new_node;
}

// Shell memory functions

void mem_init(){
	
	int i;
	for (i=0; i<1000; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

int memory_set_process(FILE *p){
	//needs to know where to write in memory: main pointer
	// needs to know where to stop i=389.
	// File *p to get each line from // or it can be the string representing the line each time
	if (tracker==420){
		printf("No longer any space to store more processes.\n");
		return 1;
	}

	//we create a PCB for the process that will be loaded
	struct PCB *myPCB; //unitialized at first
	struct node *newNode; //this might never be used, only if we need to generate a new node. has to be declared outside if statement.
	struct node *ptr = head; //to help us iterate through the queue, needs to be declared outside.
	if (head->Content == NULL){ //if the queue is empty
		myPCB = generatePCB(tracker);
		head->Content = myPCB;
	} else {
		while (ptr->next!=NULL){
			ptr = ptr->next;
		} //once we hit NULL, it means that we're at the tail of the queue
		myPCB = generatePCB(tracker);
		newNode = generateNode();
		
		ptr->next = newNode; 
		newNode->Content = myPCB;
		newNode->next = NULL; //redundant, but just to be safe
	}

	char line[100];
	if (fgets(line, 99, p) == NULL) {
        printf("Failed to read line from file.\n");
        return 1; //sanity check
    }

	int i;
	char *to_str = malloc(sizeof(char) * 15);
	for (i=0; i<140; i++){ //each script will be at most 140 lines long
		sprintf(to_str, "%d", i);

		shellmemory[tracker].var = strdup(to_str); //each varName will be the line number
		shellmemory[tracker].value = strdup(line); //cus line will be reset
		//when the program is done running, we will come back and use free() on both
		memset(line, 0, sizeof(line));
		memset(to_str, 0, sizeof(char) * 15); // clear the memory
		
		tracker++;

		if (tracker==420){
			printf("No longer any space to store more processes.\n");
			return 1;
		}
		if(feof(p)){
			break;
		}
		fgets(line,99,p);
	} //i will count the number of lines for us, so we use it
	free(to_str);

	myPCB->length = i+1;
	myPCB->score = i+1;

	return 0;
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
	
	int i;

	for (i=420; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=420; i<1000; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=420; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){

			return strdup(shellmemory[i].value);
		} 
	}
	return "Variable does not exist";
	//for assignment2 we might change this to return NULL;

}

void clear_slot(int slot){
	free(shellmemory[slot].var);
	free(shellmemory[slot].value);
	shellmemory[slot].var = "none";
	shellmemory[slot].value = "none";
} //this is to be used after a line inside a process is executed

struct memory_struct *get_mem_struct(int index){
	return &shellmemory[index];
}

void reset_tracker(){
	tracker=0;
}

void clear_memory(){

	for (int i=420; i<1000; i++){
		if (strcmp(shellmemory[i].var, "none") != 0){
			free(shellmemory[i].var);
			free(shellmemory[i].value);
		} 
	}
}
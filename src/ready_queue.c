#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "pcb.h"
#include "kernel.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"
#include "ready_queue.h"

QueueNode *head = NULL;

void ready_queue_destory()
{
    if(!head) return;
    QueueNode *cur = head;
    QueueNode *tmp;
    //while(cur->next!=NULL){
    while(cur!=NULL){
        if(cur->pcb!=NULL){
            free_architecture(cur->pcb->fileArchitecture);
            free(cur->pcb->pid);
            frameTable_free_frames(cur);
            free(cur->pcb);
        }
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    //free(cur);
}

void ready_queue_add_to_tail(QueueNode *node)
{
    if(!head){
        head = node;
        head->next = NULL;
    }
    else{
        QueueNode *cur = head;
        while(cur->next!=NULL) cur = cur->next;
        cur->next = node;
        cur->next->next = NULL;
    }
}

void ready_queue_add_to_head(QueueNode *node)
{
    if(!head){
        head = node;
        head->next = NULL;
    }
    else{
        node->next = head;
        head = node;
    }
}

void print_ready_queue(){
    if(!head) {
        printf("ready queue is empty\n");
        return;
    }
    QueueNode *cur = head;
    printf("Ready queue: \n");
    while(cur!=NULL){
        printf("\tPID: %s, length: %d, priority: %d\n", cur->pcb->pid, cur->pcb->job_length_score, cur->pcb->priority);
        cur = cur->next;
    }
}

void terminate_process(QueueNode *node){
    //node should not be in the ready queue
    free_architecture(node->pcb->fileArchitecture);
    free(node->pcb->pid);
    //frameTable_free_frames(node); when a process terminates, we don't clean up the corresponding pages
    free(node->pcb);
    free(node);
}

bool is_ready_empty(){
    return head==NULL;
}

QueueNode *ready_queue_pop_head(){
    QueueNode *tmp = head;
    if(head!=NULL) head = head->next;
    return tmp;
}

void ready_queue_decrement_job_length_score(){
    QueueNode *cur;
    cur = head;
    while(cur!=NULL){
        if(cur->pcb->job_length_score>0) cur->pcb->job_length_score--;
        cur=cur->next;
    }
}

void ready_queue_swap_with_next(QueueNode *toSwap){
    QueueNode *next;
    QueueNode *afterNext;
    QueueNode *cur = head;
    if(head==toSwap){
        next = head->next;
        head->next = next->next;
        next->next = head;
        head = next;
    }
    while(cur!=NULL && cur->next!=toSwap) cur = cur->next;
    if(cur==NULL) return;
    next = cur->next->next;
    afterNext = next->next;
    //cur toSwap next afterNext
    cur->next = next;
    next->next = toSwap;
    toSwap->next = afterNext;
}

bool swap_needed(QueueNode *cur){
    QueueNode *next = cur->next;
    if(!next) return false;
    if(cur->pcb->priority && next->pcb->priority){
        if(cur->pcb->job_length_score > next->pcb->job_length_score){
            return true;
        }
        else return false;
    }
    else if(cur->pcb->priority && !next->pcb->priority) return false;
    else if(!cur->pcb->priority && next->pcb->priority) return true;
    else{
        if(cur->pcb->job_length_score > next->pcb->job_length_score) return true;
        else return false;
    }
}

void sort_ready_queue(){
    if(head==NULL) return;
    //bubble sort
    QueueNode *cur = head;
    bool sorted = false;
    while(!sorted){
        sorted = true;
        while(cur->next!=NULL){
            if(swap_needed(cur)){
                sorted = false;
                ready_queue_swap_with_next(cur);
            }
            else{
                cur=cur->next;
            }
        }
    }
}

QueueNode *ready_queue_pop_shortest_job(){
    sort_ready_queue();
    QueueNode *node = ready_queue_pop_head();
    return node;
}

int ready_queue_get_shortest_job_score(){
    QueueNode *cur  = head;
    int shortest = MAX_INT;
    while(cur!=NULL){
        if(cur->pcb->job_length_score<shortest) {
            shortest = cur->pcb->job_length_score;
        }
        cur = cur->next;
    }
    return shortest;
}

void ready_queue_promote(int score){
    if(head->pcb->job_length_score == score) return;
    QueueNode *cur = head;
    QueueNode *next;
    while(cur->next!=NULL){
        if(cur->next->pcb->job_length_score == score) break;
        cur = cur->next;
    }
    if(cur->next==NULL || cur->next->pcb->job_length_score!=score) return;
    next = cur->next;
    cur->next = cur->next->next;
    next->next = head;
    head = next;
}
/*
int get_leastRecentFrame(){
    //we have to go through every single accesstable (of each pcb)
    //and find the frame that was accessed the least recent
    //-1 frame DNE, youve reached the end of the frame table
    //0: frame has never been accessed, automatically evicted
    //min_access points to the pcb->pageTable->frame accessed the least early
    print_ready_queue();
    QueueNode *cur = head;
    PCB *curPCB;
    int smallestAccessTime = 200;
    int frameSmallestATlocation = 200; //the location of the frame we'll remove

    int i;
    int frameLocation;
    int whichNode = 0; //0,1 or 2.
    int whichFrame = 0;
    while(cur->next!=NULL){
        printf("%d\n",smallestAccessTime);
        curPCB=cur->pcb;
        for(i=0;i<333;i++){//we check every frame of the pcb
            frameLocation=curPCB->pageTable[i];
            printf("%d\n",frameLocation);
            if (frameLocation<=-1){
                break; //we've hit the end or the rest hasn't been loaded yet
            }else if(curPCB->accessTimeTable[i]==0){ //this one hasn't been used yet we can evict it
                curPCB->accessTimeTable[i]=-2; //it got evucted
                curPCB->pageTable[i]=-2; //it got evicted
                return frameLocation;
            }else if(curPCB->accessTimeTable[i]<smallestAccessTime){
                smallestAccessTime=curPCB->accessTimeTable[i];
                frameSmallestATlocation=frameLocation;
                whichFrame=i;
            }
        }
        cur = cur->next;
        whichNode++;
    }
    //the frame that does get evicted needs to have its location replaced by -2, meaning it was evicted
    //we need to go through the queue again and set framelocation to -2
    cur=head;
    int tracker = 0;
    while(tracker!=whichNode){
        cur = cur->next;
    }
    cur->pcb->accessTimeTable[whichFrame]=-2;
    cur->pcb->pageTable[whichFrame]=-2;
    return frameSmallestATlocation; //return the index of the frame that was evicted.
}
*/
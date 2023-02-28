#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>

struct PCB {
    char *pid;
    int start;
    int current;
    int length; 
    int score;
};

struct node {
    struct PCB *Content;
    struct node *next;
};

int parseInput(char ui[]);
extern struct node *head;

#endif /* SHELL_H */

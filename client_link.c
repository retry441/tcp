#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../head.h"
void create_head()
{
    extern Link head;
    head = (Link)malloc(sizeof(Node));
    if(head == NULL)
    {
        perror("malloc");
	    exit(1);
    }
    head->next = NULL;
}

void create_node(Link *new)
{
    extern Link head;
    *new = (Link)malloc(sizeof(Node));
    if(*new == NULL)
    {
        perror("malloc");
	    exit(1);
    }
}

void insert_node(Link new)
{
    extern Link head;
    new->next = head->next;
    head->next = new;
}

int search_node(int id)
 {
	extern Link head;
	Link p;
	p = head->next;
	while(p != NULL)
	{
		if(id == p->id)
		{
			return  p->fd;
			break;
		}
		p = p->next;
	}
 }
 
char print_all_node()
 {
	extern Link head;
	Link p;
	p = head->next;
	printf("|Name      |Id        |\n");
	while(p != NULL)
	{
		printf("|%-10s|%-10d|\n",p->name,p->id);
		p = p->next;
	}
 }

int remove_node(int id)
{
	extern Link head;
	Link p;
	Link q;
	p = head->next;
	q = p->next;
	while(1)
	{
		if(q->id == id)
		{
			p = q->next;
			break;
		}
		p = p->next;
		q = q->next;
	}
}

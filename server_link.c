#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../head.h"
void create_head()
{
    extern Link head;
    extern msg_Link msg_head;
    head = (Link)malloc(sizeof(Node));
    msg_head = (msg_Link)malloc(sizeof(msg_Node));
    msg_head->next = NULL;
    head->next = NULL;
}

void create_node(Link *new)
{
    extern Link head;
    *new = (Link)malloc(sizeof(Node));
    memset((*new)->name,0,MAX_SIZE);
}

void create_msg_node(msg_Link *new)
{
    extern msg_Link msg_head;
    *new = (msg_Link)malloc(sizeof(msg_Node));
    memset((*new)->message,0,MAX_SIZE);
}

void insert_node(Link new)
{
    extern Link head;
    new->next = head->next;
    head->next = new;
}

void insert_msg_node(msg_Link new)
{
    extern msg_Link msg_head;
    new->next = msg_head->next;
    msg_head->next = new;
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
 
 int print_all_node()
 {
	extern Link head;
	Link p;
	p = head->next;
	while(p != NULL)
	{
		printf("|name %-5s|id %-5d\n",p->name,p->id);
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

int remove_msg_node(int id)
{
	extern msg_Link msg_head;
	msg_Link p;
	msg_Link q;
	p = msg_head->next;
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


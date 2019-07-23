#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "../head.h"
#include <sqlite3.h>

int send_pk(struct send_pack *pk,int sockfd)
{
    
    if(send(sockfd,pk,sizeof(*pk),0) < 0)
    {
		perror("send");
		exit(1);
		return FAIL;
    }
    return SUCCESS;
}

int recv_pk(struct send_pack *pk,int sockfd)
{
    
    if(recv(sockfd,pk,sizeof(*pk),0) < 0)
    {
    	perror("recv");
    	exit(1);
		return FAIL;
    }
    return SUCCESS;
}

int handle_show(struct current_data temp,int cfd,int cmd,char *group)
{
	
	extern Link head;
	extern sqlite3 *db;
	extern int tp[300];
	int tpint = 0;
	int temp_flag = 0;
	extern struct current_data temp_data0;
	memset(&temp_data0,0,sizeof(temp_data0));
	int count = 0;
	int count_id = 1;
	struct send_pack msg_pack;
	memset(&msg_pack,0,sizeof(msg_pack));
	Link p;
	p = head->next;
	int len;
	char buf0[30 * MAX_SIZE] = {0};
	char buf1[MAX_SIZE] = "server";
	char buf2[MAX_SIZE] = "userdata";
	
	switch(cmd)
	{
		case 1://signle of all friend
		{
			search_by_id(db,temp.current_name,NULL,NULL,MSG);
			while(1)
			{
				memset(&temp_data0,0,sizeof(temp_data0));
				tpint = tp[count];
				if(tpint == 0)
				{
					break;
				}
				if(tpint != temp.current_id)
				{
					search_by_id(db,buf2,&tpint,NULL,INPORT);
					sprintf(buf0,"%s|%d:%s",buf0,temp_data0.current_id,temp_data0.current_name);
				}
				count++;
			}
			break;
		}
		case 2://signle of all online user
		{
			while(p != NULL)
			{
  				if(memcmp(temp.current_name,p->name,MAX_SIZE) != 0)
				{
					sprintf(buf0,"%s|%d:%s",buf0,p->id,p->name);
					sprintf(buf0,"%s|",buf0);
				}
				p = p->next;
			}
			break;
		}
		
		case 3:// single of all signed user
		{
			char table[MAX_SIZE] = "userdata";
			temp_flag = search_by_id(db,table,NULL,NULL,COUNT);
			while(1)
			{
				memset(&temp_data0,0,sizeof(temp_data0));
				if(count_id != temp.current_id)
				{
					search_by_id(db,table,&count_id,NULL,INPORT);
					sprintf(buf0,"%s|%d:%s",buf0,temp_data0.current_id,temp_data0.current_name);
				}	
				count_id++;
				if(count_id > temp_flag)
				{
					break;
				}
			}
			break;
		}
		
		case 4:// every member in group
		{
			search_by_id(db,group,NULL,NULL,MSG);
			while(1)
			{
				memset(&temp_data0,0,sizeof(temp_data0));
				tpint = tp[count];
				if(tpint == 0)
				{
					break;
				}
				if(tpint != temp.current_id)
				{
					search_by_id(db,buf2,&tpint,NULL,INPORT);
					sprintf(buf0,"%s|%d:%s",buf0,temp_data0.current_id,temp_data0.current_name);
				}
				count++;
			}
			break;
		}
		
		case 5://everyone
		{
			sprintf(buf0,"Send to everyone");
			break;
		}
		
		case 6://one of online friend
		{
			int online_friend_id[300] = {0};
			int of_count = 0;
			int curcle;
			count = search_by_id(db,temp.current_name,NULL,NULL,MSG);
			for(curcle = 0;curcle < count;curcle++)
			{
				while(p != NULL)
				{
					if(p->id == tp[curcle])
					{
						online_friend_id[of_count] = p->id;
						of_count++;
						break;
					}
					p = p->next;
				}	
			}
			for(curcle = 0;curcle < of_count;curcle++)
			{
				tpint = online_friend_id[curcle];
				memset(&temp_data0,0,sizeof(temp_data0));
				search_by_id(db,buf2,&tpint,NULL,INPORT);
				sprintf(buf0,"%s|%d:%s",buf0,temp_data0.current_id,temp_data0.current_name);
			}
			break;
		} 
	}
	
	memcpy(msg_pack.sourse_name,buf1,strlen(buf1));
	memcpy(msg_pack.send_msg0,buf0,strlen(buf0));
	msg_pack.send_cmd = MUTI_SEND;
	send_pk(&msg_pack,cfd);
	//send(cfd,&msg_pack,sizeof(msg_pack),0);
}

int press_anykey(int a,int b)
{
	if(b == 1)
	{
		printf("1 >> continue ");
		printf("2 >> stop\n");
		
	}
	if(b == 0)
	{
		printf("> Press any key to continue: ");
	}
	int buf;
	scanf("%d",&buf);
	fflush(stdin);
	if(a == 1)
	{
		system("clear");
	}
	
	return buf;
}

int handle_change_password(struct send_pack recv_pack,int cfd,struct current_data temp)
{
	extern sqlite3 *db;
	char buf0[MAX_SIZE] = "userdata";
	char buf1[MAX_SIZE] = "pswd";
	char buf2[MAX_SIZE] = "server";
	char buf3[MAX_SIZE] = {0};
	update(db,buf0,recv_pack.send_msg0,buf1,temp.current_id);
	sprintf(buf3,"Your password has been changed as %s",recv_pack.send_msg0);
	struct send_pack message;
	memset(&message,0,sizeof(message));
	memcpy(message.send_msg0,buf3,strlen(buf3));
	memcpy(message.sourse_name,buf2,strlen(buf2));
	message.send_cmd = MUTI_SEND;
	send_pk(&message,cfd);
}

int handle_signup(struct send_pack recv_pack,int cfd)
{
    extern sqlite3 *db;
	int flag;
    int ret = 0;
    char table_name[MAX_SIZE] = "userdata";
    ret = search_by_name(db,recv_pack.sourse_name,1);
    printf("%d\n",ret);
    if(ret == 0)
    {
        flag = search_by_id(db,table_name,NULL,NULL,COUNT);
        recv_pack.send_id = flag + 1;
        insert_userdata(db,recv_pack);
        printf("# Account %s signned , user id is %d\n",recv_pack.sourse_name,recv_pack.send_id);
        recv_pack.result = SUCCESS;
        send_pk(&recv_pack,cfd);
    }
    
    else
    {
     	recv_pack.result = FAIL;
     	send_pk(&recv_pack,cfd);
    }
    
}

int handle_login(struct send_pack recv_pack,int cfd,struct current_data * temp)
{
    extern sqlite3 *db;
    extern Link head;
    extern msg_Link msg_head;
    extern struct current_data temp_data0;
	memset(&temp_data0,0,sizeof(temp_data0));
    char buf[MAX_SIZE] = "name";
    search_by_name(db,recv_pack.sourse_name,1);
    
	
    if(memcmp(recv_pack.send_msg0,temp_data0.current_pswd,MAX_SIZE) == 0)
    {
        memcpy((*temp).current_name,temp_data0.current_name,strlen(temp_data0.current_name));
		(*temp).current_id = temp_data0.current_id;
		
		create_table(db,recv_pack.sourse_name);
		Link new;
	    Link p;
		create_node(&new);
		new->fd = cfd;
		new->id = temp_data0.current_id;
		memset(new->name,0,MAX_SIZE);
		memcpy(new->name,temp_data0.current_name,strlen(temp_data0.current_name));
		insert_node(new);
		printf("> %s log in , id is %d , cfd is %d\n",new->name,new->id,new->fd);
		p = head->next;
		memset(&recv_pack,0,sizeof(recv_pack));
        recv_pack.result = SUCCESS;
		recv_pack.send_id = temp_data0.current_id;
		msg_Link p1;
		p1 = msg_head->next;
		while(p1 != NULL)
		{
			if(temp_data0.current_id == p1->id)
			{
				memcpy(recv_pack.send_msg0,p1->message,strlen(p1->message));
				recv_pack.send_id1 = p1->sid;
				break;
			}
			p1 = p1->next;
		}
		send_pk(&recv_pack,cfd);
     }
     else
     {
        recv_pack.result = FAIL;
        send_pk(&recv_pack,cfd);
     }
}


int handle_refind(struct send_pack recv_pack,int cfd)
{
	extern sqlite3 *db;
	extern struct current_data temp_data0;
	struct send_pack temp;
	memset(&temp,0,sizeof(temp));
	memset(&temp_data0,0,sizeof(temp_data0));
    
	int ret = 0;
	
    ret = search_by_name(db,recv_pack.sourse_name,1);
    
	if(ret == 0)
	{
		
		temp.result = FAIL;
		send_pk(&temp,cfd);	
	}
	else if(ret == 1)
	{
		
		
		memcpy(temp.send_msg0,temp_data0.current_ques,strlen(temp_data0.current_ques));
		temp.result = SUCCESS;
		send_pk(&temp,cfd);
		memset(&temp,0,sizeof(temp));
		recv_pk(&temp,cfd);
		if(memcmp(temp_data0.current_answ,temp.send_msg0,MAX_SIZE) == 0)
		{
			memset(&temp,0,sizeof(temp));
			temp.result = SUCCESS;
			memcpy(temp.send_msg0,temp_data0.current_pswd,sizeof(temp_data0.current_pswd));
			send_pk(&temp,cfd);
		}
		else
		{
			temp.result = FAIL;
			send_pk(&temp,cfd);
		}
	}
  


}
int handle_send(struct send_pack recv_pack,int cfd,struct current_data temp)
{
	extern Link head;
	char buf[MAX_SIZE] = {0};
	int temp_id[20] = {0};
	int c;
	int count = 0;
	memcpy(buf,recv_pack.send_msg0,strlen(recv_pack.send_msg0));
	memset(&recv_pack,0,sizeof(recv_pack));
	recv_pk(&recv_pack,cfd);
	handle_show(temp,cfd,recv_pack.send_cmd,recv_pack.send_msg1);
	switch(recv_pack.send_cmd)
	{
		case 1:
		{
			if(recv(cfd,temp_id,sizeof(temp_id),0) < 0)
			{
				perror("recv");
				exit(1);
			}
			Link p;
			memcpy(recv_pack.send_msg0,buf,strlen(buf));
			memcpy(recv_pack.sourse_name,temp.current_name,strlen(temp.current_name));
			recv_pack.send_cmd = MUTI_SEND;
			for(c = 0;c < 20;c++)
			{
				if(temp_id[c] != 0)
				{
					p = head->next;
					count = 0;
					while(p != NULL)
					{
						if(p->id == temp_id[c])
						{
							send_pk(&recv_pack,p->fd);
							count++;
							break;
						}
						p = p->next;
					}
					if(count == 0)
					{
						msg_Link new;
						create_msg_node(&new);
						memcpy(new->message,buf,strlen(buf));
						new->id = temp_id[c];
						new->sid = temp.current_id;
						insert_msg_node(new);
					}	
				}
			}
			break;
			
		}
		case 2:
		{
			if(recv(cfd,temp_id,sizeof(temp_id),0) < 0)
			{
				perror("recv");
				exit(1);
			}
			Link p;
			memcpy(recv_pack.send_msg0,buf,strlen(buf));
			memcpy(recv_pack.sourse_name,temp.current_name,strlen(temp.current_name));
			recv_pack.send_cmd = MUTI_SEND;
			for(c = 0;c < 20;c++)
			{
				if(temp_id[c] != 0)
				{
					p = head->next;
					while(p != NULL)
					{
						if(p->id == temp_id[c])
						{
							send_pk(&recv_pack,p->fd);
							break;
						}
						p = p->next;
					}	
				}
			}
			break;
		}
		case 3:
		{
			if(recv(cfd,temp_id,sizeof(temp_id),0) < 0)
			{
				perror("recv");
				exit(1);
			}
			Link p;
			memcpy(recv_pack.send_msg0,buf,strlen(buf));
			memcpy(recv_pack.sourse_name,temp.current_name,strlen(temp.current_name));
			recv_pack.send_cmd = MUTI_SEND;
			for(c = 0;c < 20;c++)
			{
				if(temp_id[c] != 0)
				{
					p = head->next;
					count = 0;
					while(p != NULL)
					{
						if(p->id == temp_id[c])
						{
							send_pk(&recv_pack,p->fd);
							count++;
							break;
						}
						p = p->next;
					}
					if(count == 0)
					{
						msg_Link new;
						create_msg_node(&new);
						memcpy(new->message,buf,strlen(buf));
						new->id = temp_id[c];
						new->sid = temp.current_id;
						insert_msg_node(new);
					}	
				}
			}
			break;
		}
		case 4:
		{
			break;
		}
		case 5:
		{
			break;
		}
	}
}


int handle_accept_friend(struct send_pack recv_pk,int cfd,struct current_data temp)
{
	int fd;
	extern sqlite3 *db;
	extern struct current_data temp_data1;
	struct send_pack message;
	memset(&message,0,sizeof(message));
	char buf0[MAX_SIZE] = "server";
	if(temp_data1.current_id == recv_pk.send_id)
	{
		insert_one_column(db,temp.current_name,recv_pk.send_id);
		sprintf(message.send_msg0,"%s has been your new friend",temp_data1.current_name);
		message.send_cmd = MUTI_SEND;
		memcpy(message.sourse_name,buf0,strlen(buf0));
		send_pk(&message,cfd);
		insert_one_column(db,temp_data1.current_name,temp.current_id);	
		memset(&message,0,sizeof(message));
		sprintf(message.send_msg0,"%s has been your new friend\n",temp.current_name);
		message.send_cmd = MUTI_SEND;
		memcpy(message.sourse_name,buf0,strlen(buf0));
		send_pk(&message,temp_data1.fd);
	}
}
int handle_add_friend(struct send_pack recv_pk,int cfd,struct current_data temp)
{
	extern sqlite3 *db;
	extern Link head;
	extern struct current_data temp_data1;
	struct send_pack sdpk;
	char buf0[MAX_SIZE] = {0};
	char buf1[MAX_SIZE] = "server";
	int temp_fd;
	
	handle_show(temp,cfd,3,NULL);
	memset(&temp_data1,0,sizeof(temp_data1));
	memcpy(temp_data1.current_name,temp.current_name,strlen(temp.current_name));
	temp_data1.current_id = temp.current_id;
	temp_data1.fd = cfd;
	memset(&sdpk,0,sizeof(sdpk));
	temp_fd = search_node(recv_pk.send_id);
	sdpk.send_cmd = FRIEND_REQ;
	sdpk.send_id = temp.current_id;
	memcpy(sdpk.sourse_name,temp.current_name,strlen(temp.current_name));
	send_pk(&sdpk,temp_fd);
	memset(&sdpk,0,sizeof(sdpk));
	sprintf(buf0,"# You have recived a friend requestion from %s",temp.current_name);
	memcpy(sdpk.send_msg0,buf0,strlen(buf0));
	sdpk.send_cmd = MUTI_SEND;
	memcpy(sdpk.sourse_name,buf1,strlen(buf1));
	send_pk(&sdpk,temp_fd);
}

int handle_group(int cfd,struct current_data temp)
{
	struct send_pack recv;
	extern sqlite3 *db;
	char group_id[MAX_SIZE] = {0};
	int ret = 0;
	memset(&recv,0,sizeof(recv));
	recv_pk(&recv,cfd);
	switch(recv.send_cmd)
	{
		case 1:
		{
			printf("create\n");
			ret = insert_group(db,recv.send_msg0,group_id);
			memset(&recv,0,sizeof(recv));
			recv.result = ret;
			memcpy(recv.send_msg0,group_id,strlen(group_id));
			send_pk(&recv,cfd);
			break;
		}
		
		case 2:
		{
			break;
		}
		
		case 3:
		{
			break;
		}
		
		case 4:
		{
			break;
		}
		
		case 5:
		{
			break;
		}
		
		case 6:
		{
			break;
		}
		
		case 7:
		{
			break;
		}
	}
}
int handle_set_group(struct send_pack recv_pk,int cfd,struct current_data temp)
{

	extern sqlite3 *db;
	int temp_id[20] = {0};
	int c;
	
	create_table(db,recv_pk.send_msg0);
	insert_one_column(db,recv_pk.send_msg0,temp.current_id);
	if(recv(cfd,temp_id,sizeof(temp_id),0) < 0)
	{
		perror("recv");
		exit(1);
	}
	for(c = 0;c < 20;c++)
	{
		if(temp_id != 0)
		{
			insert_one_column(db,recv_pk.send_msg0,temp_id[c]);
		}
		else
		{
			break;
		}
	}
}



int print_all()
{
	extern Link head;
	Link p;
	p = head->next;
	char buf0[50] = "User name";
	char buf2[50] = "User id";
	char buf1[50] = "Client id";
	printf("|%-10s|%-10s|%-10s|\n",buf0,buf2,buf1);
	while(p != NULL)
	{
		printf("|%-10s|%-10d|%-10d|\n",p->name,p->id,p->fd);
		p = p->next;
	}
	printf("\n");
}

int print_online_user()
{
	int cmd;
	extern Link head;
	
	system("clear");
	printf("1 >> Print every online user\n\n");
	printf("> Please input your choice: ");
	
	scanf("%d",&cmd);
	printf("\n");
	fflush(stdin);
	
	switch(cmd)
	{
		case 1:
		{
			print_all();
		    press_anykey(1,0);
			break;
		}
	}
}

int delete_data()
{
	char table[MAX_SIZE] = "userdata";
    int id;
	extern sqlite3 *db;
	printf("> Please input the id of which user you want to delete: ");
	scanf("%d",&id);
	printf("\n");
	fflush(stdin);
	delete(db,table,id);
    printf("# The user whose id is %d has been deleted successfully !\n",id);	
}

char update_data()
{   
	int id;
	extern struct current_data temp_data0;
	memset(&temp_data0,0,sizeof(temp_data0));
    char buf[MAX_SIZE] = "userdata";
	char column[MAX_SIZE] = {0};
	char content[MAX_SIZE] = {0};
	
	extern sqlite3 *db;

	printf("> Please input id of the user you want to update: ");
	scanf("%d",&id);
	fflush(stdin);
	printf("> Please choose which column you want to update: ");
	scanf("%s",column);
	fflush(stdin);
	printf("> Please input content that you want to update: \n");
	scanf("%s",content);
	fflush(stdin);
	update(db,buf,content,column,&id);
	printf("Update successfully !\n");	
}

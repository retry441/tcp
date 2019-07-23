#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include "../head.h"
#include <pthread.h>

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

int press_anykey(int a,int b)//由用户决定是否执行下一步操作
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


int getlen(char *p)
{
    int a;
	a = strlen(p);
    return a;
}
void *pthread_recv(void *arg)//客户端主进程创建的线程，在用户登录成功以后工作，保持接收服务器的信息
{
    int fd = *((int *)arg);
    struct send_pack recv_pack;
    printf("\n");
    while(1)
    {
        memset(&recv_pack,0,sizeof(recv_pack));
		//recv(fd,&recv_pack,sizeof(recv_pack),0);
		recv_pk(&recv_pack,fd);
		switch(recv_pack.send_cmd)
		{
			case FRIEND_REQ://服务器转发过来的好友请求
			{
				handle_friend(recv_pack);
				break;
			}
			
			case MUTI_SEND://普通信息
			{
				handle_send(recv_pack);
				break;
			}
		}
    }
}
int handle_send(struct send_pack recv_pack)//pthread_recv调用，打印信息
{
	printf("\n%s:%s\n",recv_pack.sourse_name,recv_pack.send_msg0);
}

int handle_friend(struct send_pack recv_pack)//客户端有一个链表储存待处理的好友请求，服务器转发的请求会插入链表
{
	extern Link head;
	Link new;
	create_node(&new);
	new->id = recv_pack.send_id;
	memcpy(new->name,recv_pack.sourse_name,strlen(recv_pack.sourse_name));
	insert_node(new);
}

int signup(int sockfd)//向服务器发送一个结构体，包含名字，密码，问题，答案
{
    struct send_pack sign_pack;
    memset(&sign_pack,0,sizeof(sign_pack));
    while(1)
    {
        sign_pack.send_cmd = SIGNUP;
        printf("> Please input your name: ");
        scanf("%s",sign_pack.sourse_name);
        printf("> Please input your password: ");
        scanf("%s",sign_pack.send_msg0);
        printf("> Please input your refind-question: ");
        scanf("%s",sign_pack.send_msg1);
        printf("> Please input your refind-answer: ");
        scanf("%s",sign_pack.send_msg2);
        printf("\n");
        send_pk(&sign_pack,sockfd);
        memset(&sign_pack,0,sizeof(sign_pack));
        recv_pk(&sign_pack,sockfd);//接收服务器发送到结构体，获得注册结果
	    if(sign_pack.result == SUCCESS)
	    {
	    	printf("%s have successfully sign up , user id : %d\n",sign_pack.sourse_name,sign_pack.send_id);
            press_anykey(1,0);
			return FAIL;
	        break;
	    }
	    else if(sign_pack.result == FAIL)
	    {
	    	printf("Your name has already been used !\n");
	    	press_anykey(1,0);
			return FAIL;
	    }
    }
}

int login(int sockfd)
{
    extern struct current_data temp_data0;
	memset(&temp_data0,0,sizeof(temp_data0));
    struct send_pack log_pack;
    memset(&log_pack,0,sizeof(log_pack));
    while(1)
    {
        log_pack.send_cmd = LOGIN;
		printf("> Please input your name: ");
		scanf("%s",temp_data0.current_name);
		fflush(stdin);
		memcpy(log_pack.sourse_name,temp_data0.current_name,strlen(temp_data0.current_name));
		printf("> Please input your password: ");
		scanf("%s",temp_data0.current_pswd);
		fflush(stdin);
		memcpy(log_pack.send_msg0,temp_data0.current_pswd,strlen(temp_data0.current_pswd));
		send_pk(&log_pack,sockfd);
		memset(&log_pack,0,sizeof(log_pack));
		recv_pk(&log_pack,sockfd);
		//recv(sockfd,&log_pack,sizeof(log_pack),0);
		if(log_pack.result == SUCCESS)
		{
			temp_data0.current_id = log_pack.send_id;
			if(log_pack.send_id1 != 0)
			{
				system("clear");
				printf("Some message have been send to you when offline\n");
				printf("%s from id %d\n",log_pack.send_msg0,log_pack.send_id1);
				press_anykey(1,0);
        	}
        	return SUCCESS;
	    	break;
		}
		else if(log_pack.result == FAIL)
		{
	    	printf("Log in fail !\n");
	    	system("clear");
			press_anykey(1,0);
			return FAIL;
		}
    }	
}

int client_off(int sockfd)//向服务器发送客户端下线消息
{
    struct send_pack exit_pack;
    exit_pack.send_cmd = CLIENT_EXIT;
    send_pk(&exit_pack,sockfd);
}

int refind(int sockfd)
{
    struct send_pack refind_pack;
	memset(&refind_pack,0,sizeof(refind_pack));
	refind_pack.send_cmd = REFIND;
	
	while(1)
	{
	    printf("> Please input your name: ");
		scanf("%s",refind_pack.sourse_name);
		fflush(stdin);
		send_pk(&refind_pack,sockfd);
		//send(sockfd,&refind_pack,sizeof(refind_pack),0);//首先向服务器发送姓名和服务请求
		memset(&refind_pack,0,sizeof(refind_pack));
		//recv(sockfd,&refind_pack,sizeof(refind_pack),0);
		recv_pk(&refind_pack,sockfd);
		if(refind_pack.result == SUCCESS)//若姓名匹配，服务器发送问题
		{	
			printf("Your refind question is %s\n> Please input your answer: ",refind_pack.send_msg0);
			memset(&refind_pack,0,sizeof(refind_pack));
			scanf("%s",refind_pack.send_msg0);
			fflush(stdin);
			send_pk(&refind_pack,sockfd);//发送答案
			
			memset(&refind_pack,0,sizeof(refind_pack));
			//recv(sockfd,&(refind_pack),sizeof(refind_pack),0);
			recv_pk(&refind_pack,sockfd);
			if(refind_pack.result == SUCCESS)//接收结果
			{
				printf("Refind success , your password is %s\n",refind_pack.send_msg0);
				press_anykey(0,0);
				break;
			}
		
			else if(refind_pack.result == FAIL)
			{
				printf("Refind answer is wrong !\n");
				press_anykey(0,0);
			}
		}
		
		else if(refind_pack.result == FAIL)
		{
			printf("Your user name does not exist !\n");
			press_anykey(0,0);
		}
	}
}

int send_friend_req(int sockfd)//发送好友请求
{
	extern struct current_data temp_data0;
	struct send_pack frd_pk;
	memset(&frd_pk,0,sizeof(frd_pk));
	frd_pk.send_cmd = FRIEND_REQ;
	printf("> Please input id of which user you want to add friend:\n");
	scanf("%d",&frd_pk.send_id);
	fflush(stdin);
	send_pk(&frd_pk,sockfd);
	printf("Your friend requestion has been send !\n");
}

int accept_friend(int sockfd)//打印储存好友请求信息的链表，向服务器发送接收某id好友请求的消息
{
	extern Link head;
	extern struct current_data temp_data0;
	struct send_pack sdpk;
	memset(&sdpk,0,sizeof(sdpk));
	int temp_id;
	system("clear");
	printf("Acceptable friend requestion :\n");
	print_all_node();
	printf("> Please press id to accept requestion: ");
	scanf("%d",&temp_id);
	fflush(stdin);
	sdpk.send_cmd = FRIEND_ACC;
	sdpk.send_id = temp_id;
	send_pk(&sdpk,sockfd);
	//send(sockfd,&sdpk,sizeof(sdpk),0);
}
int show_message(int sockfd)//创建线程
{
    pthread_t id;
    pthread_create(&id,NULL,pthread_recv,(void *)(&sockfd));
}

int muti_send(int sockfd)
{
	int cmd;
	int ret;
    extern struct current_data temp_data0;
    struct send_pack msg_pack;
	char message[MAX_SIZE] = {0};
    memset(&msg_pack,0,sizeof(msg_pack));
	msg_pack.send_cmd = MUTI_SEND;
	printf("> Please input the message you want to send: ");
	scanf("%s",message);
	fflush(stdin);
	memcpy(msg_pack.send_msg0,message,strlen(message));
	send_pk(&msg_pack,sockfd);//发送消息
	memset(&msg_pack,0,sizeof(msg_pack));
	system("clear");
	printf("1 >> Send to single user who is your friend\n");
	printf("2 >> Send to single user who is online\n");
	printf("3 >> Send to single user who has signed up\n");
	printf("4 >> Send to everyone in the group\n");
	printf("5 >> Send to everyone who has signed up\n");
	printf("Online user will recive your message at once\nOffline user will recive message if goes online\n");
	printf("> Input your choice:\n");//选择消息接收者
	scanf("%d",&cmd);
	fflush(stdin);
	msg_pack.send_cmd = cmd;
	int temp_id[20] = {0};
	int c;
	switch(cmd)
	{
		case 1:
		{
			send_pk(&msg_pack,sockfd);
			while(1)
			{
				memset(temp_id,0,sizeof(temp_id));
				c = 0;
				printf("> Please input the id of user which you want to send message: ");
				scanf("%d",&temp_id[c]);
				c++;
				if(c == 19)
				{
					break;
				}
				fflush(stdin);
				ret = press_anykey(0,1);
				if(ret == 2)
				{
					break;
				}
				
			}
			
			if(send(sockfd,temp_id,sizeof(temp_id),0) < 0)
			{
				perror("send");
				exit(1);
			}
			break;
		}
		
		case 2:
		{
			send_pk(&msg_pack,sockfd);
			while(1)
			{
				memset(temp_id,0,sizeof(temp_id));
				c = 0;
				printf("> Please input the id of user which you want to send message: ");
				scanf("%d",&temp_id[c]);
				c++;
				if(c == 19)
				{
					break;
				}
				fflush(stdin);
				ret = press_anykey(0,1);
				if(ret == 2)
				{
					break;
				}
			}
			
			if(send(sockfd,temp_id,sizeof(temp_id),0) < 0)
			{
				perror("send");
				exit(1);
			}
			break;
		}
		case 3:
		{
			send_pk(&msg_pack,sockfd);
			while(1)
			{
				memset(temp_id,0,sizeof(temp_id));
				c = 0;
				printf("> Please input the id of user which you want to send message: ");
				scanf("%d",&temp_id[c]);
				c++;
				if(c == 19)
				{
					break;
				}
				fflush(stdin);
				ret = press_anykey(0,1);
				if(ret == 2)
				{
					break;
				}
			}
			if(send(sockfd,temp_id,sizeof(temp_id),0) < 0)
			{
				perror("send");
				exit(1);
			}
			break;
		}
		case 4:
		{
			printf("> Please input the id of group which you want to broadcast: ");
			
			scanf("%s",msg_pack.send_msg1);
			send_pk(&msg_pack,sockfd);
			break;
		}
		case 5:
		{
			break;
		}
	}
}

int change_password(int sockfd)//修改密码
{
	struct send_pack change_pack;
	memset(&change_pack,0,sizeof(change_pack));
	printf("> Please input your new password: ");
	scanf("%s",change_pack.send_msg0);
	change_pack.send_cmd = CHANGE_PASSWORD;
	send_pk(&change_pack,sockfd);
	//send(sockfd,&change_pack,sizeof(change_pack),0);
}

int mannage_friend(int sockfd)
{
	int cmd = 0;
	int buf = 0;
	system("clear");
	printf("1 >> View all friend\n");
	printf("2 >> View all online friend\n");
	printf("3 >> Add friend\n");
	printf("4 >> Check friend requestion\n");
	printf("5 >> Delete friend\n");
	printf("> Input your choice :\n");
	scanf("%d",&cmd);
	struct send_pack sdpk;
	memset(&sdpk,0,sizeof(sdpk));
	switch(cmd)
	{
		case 1:
		{
			sdpk.send_cmd = SHOW;
			sdpk.result = 1;
			send_pk(&sdpk,sockfd);
			press_anykey(0,0);
			break;
		}
		case 2:
		{
			sdpk.send_cmd = SHOW;
			sdpk.result = 6;
			send_pk(&sdpk,sockfd);
			press_anykey(0,0);
			break;
		}
		case 3:
		{
			while(1)
			{
				send_friend_req(sockfd);
				buf = press_anykey(1,1);
				if(buf == 2)
				{
					break;
				} 
			}
			break;
		}
		
		case 4:
		{
			while(1)
			{
				accept_friend(sockfd);
				buf = press_anykey(1,1);
				if(buf == 2)
				{
					break;
				} 
			}			
			break;
		}
		
		case 5:
		{
			break;
		}
	}
}	

int mannage_group(int sockfd)
{
	system("clear");
	printf("1 >> create a new group\n");
	printf("2 >> view all group\n");
	printf("3 >> join a group\n");
	printf("4 >> invite user to group\n");
	printf("5 >> chatting in a group\n");
	printf("6 >> delete user in a group\n");
	printf("7 >> keep user silent\n");
	printf("\n> Input your choice:\n");
	
	extern struct current_data temp_data0;
	struct send_pack sdpk;
	int cmd;
	memset(&sdpk,0,sizeof(sdpk));
	scanf("%d",&sdpk.send_cmd);
	switch(sdpk.send_cmd)
	{
		case 1:
		{
			printf("> Please input the name of group:\n");
			scanf("%s",sdpk.send_msg0);
			fflush(stdin);
			send_pk(&sdpk,sockfd);
			memset(&sdpk,0,sizeof(sdpk));
			
			recv(sockfd,&sdpk,sizeof(sdpk),0);
			//recv_pk(&sdpk,sockfd);
			printf("recv\n");
			if(sdpk.result == SUCCESS)
			{
				printf("# Your group has successfully been creat,group id is %s\n",sdpk.send_msg0);
				break;
			}
			
			else if(sdpk.result == FAIL)
			{
				printf("# Your group name exists,create fail !\n");
				break;
			}
			
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
int user_ui()
{
    extern int sockfd;    
	int ui_cmd;
    int ret;
	system("clear");
    printf("<<USER MENU>>\n\n");

    printf("1 >> Log in\n");
	printf("2 >> Sign up\n");
	printf("3 >> Refind password\n");
	printf("4 >> Shut down client\n");
	printf("\nYour choice is :\n");
	
    scanf("%d",&ui_cmd);
    fflush(stdin);
	
    switch(ui_cmd)
    {
        case 4:
		{
			client_off(sockfd);
			exit(1);
			break;
		}
        case 1:
		{
			ret = login(sockfd);
			if(ret == SUCCESS)
			{
				printf("# Log in successfully , turn chat menu in 2 sed !\n");
				sleep(2);
				system("clear");
				return SUCCESS;
				break;
			}
			break;
		}
	    case 2:
		{
			signup(sockfd);
			system("clear");
			break;
		}
	    case 3:
		{
			refind(sockfd);
			system("clear");
			break;
		}	
    }
}


int chat_ui()
{
    extern int sockfd;
	extern struct current_data temp_data0;
    int buf;
	int ui_cmd;
    
    system("clear");
    printf("<< CHAT MENU >>\n\n");
	printf("> current user : %s\n",temp_data0.current_name);
	printf("> current id : %d\n\n",temp_data0.current_id);
    printf("1 >> User goes offline\n");
	printf("2 >> Change password\n");
	printf("3 >> Mannage friend\n");
    printf("4 >> Send message\n");
	printf("5 >> Mannage group\n");
	printf("\n");
	printf("> Your choice is :\n");
	
	show_message(sockfd);//create a pthread,recving message from server
    scanf("%d",&ui_cmd);
    fflush(stdin);
	
    switch(ui_cmd)
    {
        case 1:
		{
			system("clear");
			printf("Do you really want to go offline?\n1 >> Yes\n2 >> No\nYour choice is : ");
			scanf("%d",&ui_cmd);
			fflush(stdin);
			if(ui_cmd == 1)
			{
				system("clear");
				return SUCCESS;
				break;
			}
			else
			{
				return FAIL;
				break;
			}
		}				 
				 
	    case 4:
		{
			while(1)
			{
				muti_send(sockfd);
				buf = press_anykey(1,1);
				if(buf == 2)
				{
					break;
				} 
			}
			
			break;
		}
		case 2:
		{
	        change_password(sockfd);
	        press_anykey(1,0);
			break;
		}
		case 3:
		{
			mannage_friend(sockfd);
			break;
		}
		case 5:
		{
			struct send_pack sdpk;
			memset(&sdpk,0,sizeof(sdpk));
			sdpk.send_cmd = GROUP;
			send_pk(&sdpk,sockfd);
			while(1)
			{
				mannage_group(sockfd);
				buf = press_anykey(1,1);
				if(buf == 2)
				{
					break;
				} 
			}
			
			break;
		}
		case 6:
		{
			break;
		}
    }
}

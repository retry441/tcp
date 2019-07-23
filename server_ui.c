#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../head.h"
#include <sqlite3.h>
int db_mannage()
{
    extern sqlite3 *db;
    
	printf("\n<< DATA BASE MANNAGER >>\n\n");
    printf("1 >> Start serving clients\n");
    printf("2 >> Shut down this server\n");
    printf("3 >> View signned user infomation\n");
    printf("4 >> Delete signned user\n");
    printf("5 >> Update signned user data\n");
	printf("\n");
    
    int ui_cmd;
	int ret;
    printf("> Input cmd to use function: ");
	scanf("%d",&ui_cmd);
	fflush(stdin);
	
    switch(ui_cmd)
    {
        case 3:
		{
			system("clear");
			printf("1 >> Search signed user\n");
			printf("2 >> Search all friends of one user\n");
			printf("3 >> Search all members of one group\n");
			int cmd = 0;
			char input[MAX_SIZE] = {0};
			printf("> Please input your choice : ");
			scanf("%d",&cmd);
			switch(cmd)
			{
				case 1:
				{
					system("clear");
					char buf[MAX_SIZE] = "userdata";
					search_by_id(db,buf,NULL,NULL,PRINT);
					press_anykey(1,0);
					break;
				}
				
				case 2:
				{
					system("clear");
					memset(input,0,MAX_SIZE);
					printf("> Please input the name of user : ");
					scanf("%s",input);
					search_by_id(db,input,NULL,NULL,ID);
					printf("\n");
					press_anykey(1,0);
					break;
				}
				
				case 3:
				{
					memset(input,0,MAX_SIZE);
					printf("> Please input the name of group : ");
					scanf("%s",input);
					search_by_id(db,input,NULL,NULL,ID);
					printf("\n");
					press_anykey(1,0);
					break;
				}
			}
			break;
		}
		
		case 4:
		{
			while(1)
			{			
				 delete_data();
		         ret = press_anykey(1,1);
				 if(ret == 2)
				 {
					 break;
				 }
			}
			break;
		}
		
		case 1:
		{
			system("clear");
			return SUCCESS;
			break;
		}
		
		case 2:
		{
			
			break;
		}
		
		case 5:
		{
			while(1)
			{			
				 update_data();
		         ret = press_anykey(1,1);
				 if(ret == 2)
				 {
					 break;
				 }
			}
			break;
		}
    }
}

int client_mannage()
{
    extern Link head;
    extern msg_Link msg_head;
	int ui_cmd;
	int ret;
	
    printf("<< ONLINE USER MANNAGER >>\n\n");
    printf("1 >> View online user\n");
    printf("2 >> Kick off online user\n");
    printf("3 >> Send message to user\n");
    printf("4 >> Check message which prepare to send offline user\n");
	printf("\n");
	printf("> Input cmd to use function:\n");
    
	
    scanf("%d",&ui_cmd);
    fflush(stdin);
    
	switch(ui_cmd)
    {
        case 1:
		{
			print_online_user();
			
			break;
		}
        	
		case 2:
		{
			break;
		}
				
		case 3:
		{
			while(1)
			{	
				int fd;
				int id;
				int ret;
				char buf[MAX_SIZE] = {0};
				struct send_pack sdpk;
				memset(&sdpk,0,sizeof(sdpk));
				printf("> Please input message you want to send: ");
				scanf("%s",buf);
				fflush(stdin);
				printf("> Please input the id: ");
				scanf("%d",&id);
				fd = search_node(id);
				sdpk.send_cmd = MUTI_SEND;
				memcpy(sdpk.send_msg0,buf,strlen(buf));
				send(fd,&sdpk,sizeof(sdpk),0);
				ret = press_anykey(1,1);
				if(ret == 2)
				{
					system("clear");
					break;
				}
			}
			break;
		}
				
		case 4:
		{
			msg_Link p;
			p = msg_head->next;
			system("clear");
			while(p != NULL)
			{
				printf("%s from id %d is ready to send to user id %d\n",p->message,p->sid,p->id);
				p = p->next;
			}
			press_anykey(1,0);
			break;
		}
    }
}

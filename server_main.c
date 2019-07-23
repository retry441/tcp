#include <stdio.h>
#include "../head.h"
#include <sqlite3.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

sqlite3 *db = NULL;
Link head = NULL;
int sockfd = 0;
int flag = 0;
int tp[300] = {0};
struct current_data temp_data0;
struct current_data temp_data1;
msg_Link msg_head = NULL;

int main()
{
	
	int ret;
	
	system("clear");
    sqlite3_init();
	
	while(1)
	{
	    ret = db_mannage();
	    if(ret == SUCCESS)
	    {
	    	system("clear");
	        break;
        }
	}
    create_head();
    server_connect();
    return 0;
}

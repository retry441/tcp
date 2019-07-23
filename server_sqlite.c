#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "../head.h"

int create_table(sqlite3 *db,char *table_name)
{
	int rc;
	char *errmsg = NULL;
	char sql[MAX_SIZE] = {0};
	sprintf(sql,"create table if not exists %s(id integer);",table_name);
	rc = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
    if(rc != SQLITE_OK)
    {
        perror("create table");
    	sqlite3_free(errmsg);
	    sqlite3_close(db);
	    exit(1);
    }
}
int insert_one_column(sqlite3 *db,char *table_name,int id)
{
	char *errmsg = NULL;
	char sql[MAX_SIZE] = {0};
	
	sprintf(sql,"insert into %s(id) values(%d);",table_name,id);
	
	if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
    {
	    return FAIL;
    }
    return SUCCESS;
	
}
int insert_userdata(sqlite3 *db,struct send_pack recv)
{
    char *errmsg = NULL;
    char sql[MAX_SIZE] = {0};
    sprintf(sql,"insert into userdata(name,pswd,ques,answ,id) values('%s','%s','%s','%s',%d);",recv.sourse_name,recv.send_msg0,recv.send_msg1,recv.send_msg2,recv.send_id);
    
	if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
    {
	    return FAIL;
    }
    return SUCCESS;
}

int insert_group(sqlite3 *db,char *group_name,char *group_id)
{
	
	char *errmsg = NULL;
    char sql[MAX_SIZE] = {0};
    char table_name[MAX_SIZE] = "grouplist";
    int count = 0;
    count = search_by_name(db,group_name,2);
    if(count == 0)
    {
    	count = 0;
    	count = search_by_id(db,table_name,NULL,NULL,COUNT);
    	sprintf(sql,"insert into grouplist(name,id) values('%s',%d);",group_name,count + 1001);
    	memset(sql,0,MAX_SIZE);
    
		if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
    	{
		    return FAIL;
    	}
    	
    	memset(table_name,0,MAX_SIZE);
    	sprintf(table_name,"%d",count + 1001);
    	sprintf(sql,"create table if not exists %s(id integer);",table_name);
    	
    	if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
    	{
		    return FAIL;
    	}
    	memcpy(group_id,table_name,strlen(table_name));
    	return SUCCESS;
    }
    else
    {
    	return FAIL;
    }
}

int print_one_column(void *para,int n_column,char **column_value,char **column_name)
{
	int temp;
	temp = atoi(column_value[0]);
	printf("%d ",temp);
	return 0;
} 

int print_userdata(void *para,int n_column,char **column_value,char **column_name)
{
	int temp;
	temp = atoi(column_value[4]);
    printf("|%-10s|%-10s|%-10s|%-10s|%-10d|\n",column_value[0],column_value[1],column_value[2],column_value[3],temp);
    return 0;
}

int write_alldata(void *para,int n_column,char **column_value,char **column_name)
{
	extern int flag;
    extern struct current_data temp_data0;
    flag++;
	memset(&temp_data0,0,sizeof(temp_data0));
    memcpy(temp_data0.current_name,column_value[0],strlen(column_value[0]));
	memcpy(temp_data0.current_pswd,column_value[1],strlen(column_value[1]));
	memcpy(temp_data0.current_ques,column_value[2],strlen(column_value[2]));
	memcpy(temp_data0.current_answ,column_value[3],strlen(column_value[3]));
	temp_data0.current_id = atoi(column_value[4]);
    return 0;
}

int count_userdata(void *para,int n_column,char **column_value,char **column_name)
{
	extern int flag;
	flag++;
	return 0;
}

int search_by_name(sqlite3 *db,char *name,int flag0)
{
	extern int flag;
	flag = 0;
	char *errmsg;
	char sql[MAX_SIZE] = {0};
	if(flag0 == 1)
	{
		sprintf(sql,"select * from userdata where name='%s';",name);
		sqlite3_exec(db,sql,write_alldata,NULL,&errmsg);
	}
	else if(flag0 == 2)
	{
		sprintf(sql,"select * from grouplist where name='%s';",name);
		sqlite3_exec(db,sql,count_userdata,NULL,&errmsg);
	}
	return flag;
}

int make_msg(void *para,int n_column,char **column_value,char **column_name)
{
	extern int flag;
	extern int tp[300];
	tp[flag] = atoi(column_value[0]);
	flag++;
	return 0;
}

int search_by_id(sqlite3 *db,char *table_name,int *search_id,char *select_column_name,int cmd)
{
	extern int flag;
	extern int tp[300];
    char *errmsg = NULL;
    flag = 0;
    char sql[MAX_SIZE] = {0};
    char buf[MAX_SIZE] = {0};
    if(select_column_name == NULL)
    {
     	sprintf(buf,"*");	
    }
    else
    {
    	sprintf(buf,"%s",select_column_name);
    }
    
    
    if(search_id == NULL)
    {
    	sprintf(sql,"select %s from %s;",buf,table_name);
    }
    else
    {
    	sprintf(sql,"select %s from %s where id=%d;",buf,table_name,*(search_id));
    }
    switch(cmd)
    {
    	case PRINT:
		{
			char a1[MAX_SIZE] = "Name";
			char a2[MAX_SIZE] = "Password";
			char a3[MAX_SIZE] = "question";
			char a4[MAX_SIZE] = "answer";
			char a5[MAX_SIZE] = "Id";
			
			printf("|%-10s|%-10s|%-10s|%-10s|%-10s|\n",a1,a2,a3,a4,a5);
			sqlite3_exec(db,sql,print_userdata,NULL,&errmsg);
			return flag;
			break;
			
		}
    	   
    	case INPORT:
		{
			sqlite3_exec(db,sql,write_alldata,NULL,&errmsg);
			return flag;
    	    break;
		}
    	case COUNT:
		{
			sqlite3_exec(db,sql,count_userdata,NULL,&errmsg);
			return flag;
    	    break;
		}
		case MSG:
		{
			sqlite3_exec(db,sql,make_msg,NULL,&errmsg);
			return flag;
    	    break;
		}
		case ID:
		{
			sqlite3_exec(db,sql,print_one_column,NULL,&errmsg);
			return flag;
    	    break;
		}
    		
    }
}

int update(sqlite3 *db,char *table_name,char *update_content,char *update_column_name,int select_id)
{
	char sql[MAX_SIZE] = {0};
	char *errmsg = NULL;
	sprintf(sql,"update %s set %s='%s' where id=%d;",table_name,update_column_name,update_content,select_id);
	sqlite3_exec(db,sql,NULL,NULL,&errmsg);
}

int delete(sqlite3 *db,char *table_name,int select_id)
{
	
	char *errmsg = NULL;
	char sql[MAX_SIZE] = {0};
	sprintf(sql,"delete from %s where id=%d;",table_name,select_id);
	sqlite3_exec(db,sql,NULL,NULL,&errmsg);	
}

int sqlite3_init()
{
    extern sqlite3 *db;
    char *errmsg = NULL;
    
    int rc = sqlite3_open("./userdata.db",&db);
	
    if(rc != SQLITE_OK)
    {
        perror("open sql");
	    sqlite3_close(db);
	    exit(1);
    }
	
	char *sql_group = "create table if not exists grouplist(name text,id integer);";
    rc = sqlite3_exec(db,sql_group,NULL,NULL,&errmsg);
	
    if(rc != SQLITE_OK)
    {
        perror("create table");
    	sqlite3_free(errmsg);
	    sqlite3_close(db);
	    exit(1);
    }
	
    char *sql_userdata = "create table if not exists userdata(name text,pswd text,ques text,answ text,id integer);";
    rc = sqlite3_exec(db,sql_userdata,NULL,NULL,&errmsg);
	
    if(rc != SQLITE_OK)
    {
        perror("create table");
    	sqlite3_free(errmsg);
	    sqlite3_close(db);
	    exit(1);
    }
}

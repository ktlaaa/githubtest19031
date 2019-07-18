#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <linux/un.h>
#include <signal.h>

struct {
	int id;		//id
	int quanxian;		//用户类型  1:管理员 0:用户
	int age;			//年龄
	float salary;		//工资
	char date[10];		//修改时间
	char phone[13];		//手机号码
	char name[20];		//用户名
	char pwd[20];		//密码
	char addr[64];	    //地址
	int flag; // 0:修改成功  1:修改失败
	int type; // 1:年龄  2:工资 3:密码 4:手机
} user;

struct
{
	int staffno;		//id
	int usertype;
	char name[20];		//姓名	
	char pwd[20];
	int login_flag;//0:正确  1:用户名不存在  2:密码错误
	int choose_flag;//选项
} login_info;

int do_login(int acceptfd, sqlite3 *db);//登录
void user_menu(int acceptfd, sqlite3 *db);//普通用户菜单
void admin_menu(int acceptfd, sqlite3 *db);//管理员用户菜单
void all_info(int acceptfd, sqlite3 *db);//查询所有的信息
int one_callback(void* arg,int f_num,char** f_value,char** f_name);
int all_callback(void* arg,int f_num,char** f_value,char** f_name);
void change_info(int acceptfd, sqlite3 *db);
void add_user(int acceptfd, sqlite3 *db);
void del_user(int acceptfd, sqlite3 *db);
#endif

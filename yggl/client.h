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
struct {
	int id;		//编号
	int quanxian;		//用户权限  1:管理员 0:用户
	int age;			//年龄
//	int level;			//权限等级
	float salary;		//工资
	char date[10];		//修改时间
	char phone[13];		//手机号码
	char name[20];		//姓名
	char pwd[20];		//密码
	char addr[64];	    //地址
		
	int flag; // 0:修改成功  1:修改失败
	int type; // 1:age  2:工资
} user;// 用户信息

struct
{
	int id;		//id
	int type; //权限
	char name[20];		//姓名	
	char pwd[20];
	int login_flag;//0:正确  1:用户名不存在  2:密码错误
	int choose_flag;//选项
} login_info; //登录信息

void login(int sockfd);
void user_menu(int sockfd);
void admin_menu(int sockfd);
void user_info(int sockfd);
void user_change(int sockfd);
void all_info(int sockfd);
void change_info(int sockfd);
void add_user(int sockfd);
void del_user(int sockfd);

#endif

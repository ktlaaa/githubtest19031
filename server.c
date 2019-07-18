#include "server.h"

int main(int argc, const char *argv[])
{
	//判断是否输入IP地址及端口号
	if(argc != 3){
		printf("Usage:%s serverip  port.\n", argv[0]);
		return -1;
	}

	//打开数据库
	sqlite3 *db;
	if(sqlite3_open("text.db", &db) != SQLITE_OK){
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else
	{
		printf("open DATABASE success.\n");
	}

	//创建套接字
	int sockfd = 0;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sockfd){
		perror("socket failed");
		exit(-1);
	}
	printf("socket ok.\n");

	//套接字填充结构体
	struct sockaddr_in  serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));

	//绑定套接字
	if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("fail to bind.\n");
		return -1;
	}
	
	printf("bind ok.\n");

	// 将套接字设为监听模式
	if(listen(sockfd, 5) < 0)
	{
		printf("fail to listen.\n");
		return -1;
	}
	printf("listen ok.\n");

	//处理僵尸进程
	signal(SIGCHLD, SIG_IGN);
	int acceptfd;
	pid_t pid;
	while(1){
		//接受套接字
		if((acceptfd = accept(sockfd, NULL, NULL)) < 0)
		{
			perror("fail to accept");
			return -1;
		}
		printf("accept ok.\n");
		if((pid = fork()) < 0)
		{
			perror("fail to fork");
			return -1;
		}
		else if(pid == 0)  // 儿子进程
		{
			//处理客户端具体的消息
			close(sockfd);
			do_login(acceptfd, db);

		}
		else  // 父亲进程,用来接受客户端的请求的
		{
			close(acceptfd);
		}
	}
	sqlite3_close(db);
	return 0;
}

// 回调函数，每找到一条记录自动执行一次回调函数。
int one_callback(void* arg,int f_num,char** f_value,char** f_name)
{
	user.id = atoi(f_value[0]);
	login_info.staffno = atoi(f_value[0]);
	user.quanxian = atoi(f_value[1]);
	strcpy(user.name,f_value[2]);
	strcpy(user.pwd,f_value[3]);
	user.age = atoi(f_value[4]);
	strcpy(user.addr,f_value[5]);
	strcpy(user.date,f_value[6]);
	user.salary = atof(f_value[7]);
	return 0;
}
//登录
int do_login(int acceptfd, sqlite3 *db){
	char sql[128];
	char *errmsg;
	while(0 < recv(acceptfd, &login_info, sizeof(login_info), 0)){
		sprintf(sql,"select * from usrinfo where name = '%s'",login_info.name);
		//执行一条sql语句，成功返回SQLITE_OK,失败返回错误码errmsg
		if(sqlite3_exec(db, sql, one_callback,(void *)&acceptfd, &errmsg)!= SQLITE_OK)
		{
			printf("%s\n", errmsg);
		}
		if(!(user.name)[0]){
			login_info.login_flag = 1;
			send(acceptfd,&login_info,sizeof(login_info),0);
			printf("无此用户1!!\n");
		}else if(strncmp(user.pwd,login_info.pwd,sizeof(login_info.pwd))){
			login_info.login_flag = 2;
			send(acceptfd,&login_info,sizeof(login_info),0);
			printf("密码错误\n");
		}else{
			login_info.login_flag = 0;
			send(acceptfd,&login_info,sizeof(login_info),0);
			printf("成功!!!\n");
			switch(login_info.usertype){
				case 1:
					admin_menu(acceptfd,db);
					break;
				case 0:
					user_menu(acceptfd,db);
					break;
			}
		}	
	}
	printf("client exit.\n");
	close(acceptfd);
	return 0;
}

void user_menu(int acceptfd, sqlite3 *db){
	while(1){
		if(0 < recv(acceptfd, &login_info, sizeof(login_info), 0)){
			switch(login_info.choose_flag){
				case 1:
					send(acceptfd,&user,sizeof(user),0);
					break;
				case 2:
					user_change(acceptfd,db);
				case 3:
					return;

			}
		}
	}
}
//修改信息
user_change(int acceptfd, sqlite3 *db){
	char sql[128];
	char *errmsg;
	if(0 < recv(acceptfd, &user, sizeof(user), 0)){
		sprintf(sql,"update usrinfo set age = %d where name = '%s'",user.age,user.name);
		if(sqlite3_exec(db, sql, NULL,NULL, &errmsg)!= SQLITE_OK)
		{
			user.flag = 1;
			send(acceptfd,&user,sizeof(user),0);
			printf("修改失败!!!\n");
			printf("%s\n", errmsg);
			return;
		}
		user.flag = 0;
		send(acceptfd,&user,sizeof(user),0);
		
		printf("修改成功!!!\n");
		return;
	}

}
//管理员用户菜单
void admin_menu(int acceptfd, sqlite3 *db){
	while(1){
		if(0 < recv(acceptfd, &login_info, sizeof(login_info), 0)){
			switch(login_info.choose_flag){
				case 1:
					all_info(acceptfd,db);
					break;
				case 2:
					change_info(acceptfd,db);
					break;
				case 3:
					add_user(acceptfd,db);
					break;
				case 4:
					del_user(acceptfd,db);
					break;
				case 5:
				case 6:
					return;

			}
		}
		
	}
}
//删除用户
void del_user(int acceptfd, sqlite3 *db){
	char sql[128];
	char *errmsg;
	if(0 < recv(acceptfd, &user, sizeof(user), 0)){
		sprintf(sql,"delete from usrinfo where name = '%s'",user.name);
		if(sqlite3_exec(db, sql, NULL,NULL, &errmsg)!= SQLITE_OK)
		{
			user.flag = 1;
			send(acceptfd,&user,sizeof(user),0);
			printf("删除失败!!!\n");
			printf("%s\n", errmsg);
			return;
		}
		user.flag = 0;
		printf("删除成功!!!\n");
		send(acceptfd,&user,sizeof(user),0);
	}
}
//添加用户
void add_user(int acceptfd, sqlite3 *db){
	char sql[128];
	char *errmsg;
	if(0 < recv(acceptfd, &user, sizeof(user), 0)){
		sprintf(sql,"insert into usrinfo values(%d,%d,'%s','%s',%d, '%s','%s','%s',%f)",
				user.id,user.quanxian,user.name,user.pwd,
				user.age,user.phone,user.addr,user.date,user.salary);
		if(sqlite3_exec(db, sql, NULL,NULL, &errmsg)!= SQLITE_OK)
		{
		    user.flag = 1;
			send(acceptfd,&user,sizeof(user),0);
			printf("添加失败!!!\n");
			printf("%s\n", errmsg);
			return;
		}
		user.flag = 0;
		printf("添加成功!!!\n");
		send(acceptfd,&user,sizeof(user),0);

	}
	
}
//修改信息
void change_info(int acceptfd, sqlite3 *db){

	char sql[128];
	char *errmsg;
	if(0 < recv(acceptfd, &user, sizeof(user), 0)){
		switch(user.type){
			case 1:
			sprintf(sql,"update usrinfo set age = %d where name = '%s'",user.age,user.name);
			break;
			case 2:
			sprintf(sql,"update usrinfo set salary = %g where name = '%s'",user.salary,user.name);
			break;
			case 3:
			sprintf(sql,"update usrinfo set pwd = %s where name = '%s'",user.pwd,user.name);
			break;
			case 4:
			sprintf(sql,"update usrinfo set phone = %s where name = '%s'",user.phone,user.name);
			break;
		}
		if(sqlite3_exec(db, sql, NULL,NULL, &errmsg)!= SQLITE_OK)
		{
			user.flag = 1;
			send(acceptfd,&user,sizeof(user),0);
			printf("修改失败!!!\n");
			printf("%s\n", errmsg);
			return;
		}
		user.flag = 0;
		send(acceptfd,&user,sizeof(user),0);
		
		printf("修改成功!!!\n");
		return;
	}
}

int all_callback(void* arg,int f_num,char** f_value,char** f_name){
	int acceptfd;
	acceptfd = *((int *)arg);
	user.id = atoi(f_value[0]);
	login_info.staffno = atoi(f_value[0]);
	user.quanxian = atoi(f_value[1]);
	strcpy(user.name,f_value[2]);
	strcpy(user.pwd,f_value[3]);
	user.age = atoi(f_value[4]);
	strcpy(user.phone,f_value[5]);
	strcpy(user.addr,f_value[6]);
	strcpy(user.date,f_value[7]);
	user.salary = atof(f_value[8]);
	send(acceptfd,&user,sizeof(user),0);
	return 0;
}

void all_info(int acceptfd, sqlite3 *db){
	char sql[128];
	char *errmsg;
	sprintf(sql,"select * from usrinfo");
	if(sqlite3_exec(db, sql, all_callback,(void *)&acceptfd, &errmsg)!= SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}else
	{
		printf("Query record done.\n");
	}
	
	user.name[0] = '\0';
	send(acceptfd,&user,sizeof(user),0);

}

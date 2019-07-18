#include "client.h"

int main(int argc, char const *argv[])
{
	//判断是否输入IP地址及端口号
	if(argc != 3){
		printf("Usage:%s serverip  port.\n", argv[0]);
		return -1;
	}
	//创建套接字
	int sockfd = 0;
	sockfd = socket(AF_INET,SOCK_STREAM,0);//创建面向网络的，tcp链接的套接字
	if(0 > sockfd){
		perror("socket failed");
		exit(-1);
	}
	printf("socket ok.\n");

	struct sockaddr_in serveraddr;
	socklen_t addrlen = sizeof(serveraddr);
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));

	connect(sockfd,(struct sockaddr *)&serveraddr,addrlen);
	int n; //用于选择
	while(1){
		printf("*************************************************************\n");
		printf("********  1：管理员模式    2：普通用户模式    3：退出********\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();
		switch(n){
			case 1:
				login_info.type = 1;
				login(sockfd);
				break;
			case 2:
				login_info.type = 0;
				login(sockfd);
				break;
			case 3:
				close(sockfd);
				exit(0);
				break;
			default:
				printf("输入错误!!!\n");
				break;

		}
	}
	return 0;
}

/*登录*/
void login(int sockfd){
	printf("请输入用户名:");
	scanf("%s",login_info.name);
	printf("请输入密码:");
	scanf("%s",login_info.pwd);
	send(sockfd,&login_info,sizeof(login_info),0);//发送数据
	recv(sockfd,&login_info,sizeof(login_info),0);//接收数据
	switch(login_info.login_flag){
		case 0:
			printf("登录成功!!!\n");
			switch(login_info.type){
				case 0:
					user_menu(sockfd);
					break;
				case 1:
					admin_menu(sockfd);
					break;
			}
			break;
		case 1:
			printf("用户不存在!!!\n");
			break;
		case 2:
			printf("密码错误!!!\n");
			break;
	}
}
//普通用户界面
void user_menu(int sockfd){
	int n;
	while(1){
		printf("*************************************************************\n");
		printf("*************  1：查询  	2：修改		3：退出	 *************\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();
		switch(n){
			case 1:
				user_info(sockfd);
				printf("查询成功\n");
				break;
			case 2:
				user_change(sockfd);
				break;
			case 3:
				close(sockfd);
				exit(0);
				break;
			default:
				printf("输入错误!!!\n");
				break;
		}
	}
}
//查询用户信息
void user_info(int sockfd){
	login_info.choose_flag = 1;
	send(sockfd,&login_info,sizeof(login_info),0);
	recv(sockfd,&user,sizeof(user),0);
	int i = 0;
	printf("编号  用户权限  用户名  密码   年龄  　手机号码　 地址 　　　　　修改时间  　工资   \n");
	printf("%5d %2d %8s %6s  %3d %s %s %s %g\n"
		,user.id,user.quanxian,user.name,user.pwd,user.age,user.phone,
		user.addr,user.date,user.salary);
}
//修改用户信息
void user_change(int sockfd){
	login_info.choose_flag = 2;
	send(sockfd,&login_info,sizeof(login_info),0);
	int n;
	printf("请输入修改后的年龄:");
	scanf("%d",&(user.age));
	strcpy(user.name,login_info.name);
	send(sockfd,&user,sizeof(user),0);
	recv(sockfd,&user,sizeof(user),0);
	switch(user.flag){
		case 0:
			printf("修改成功!!!\n");
			break;
		case 1:
			printf("修改失败!!!\n");
			break;
	}
}
//管理员用户菜单
void admin_menu(int sockfd){
	int n;
	while(1){
		printf("*************************************************************\n");
		printf("* 1：查询  2：修改 3：添加用户  4：删除用户         *\n");
		printf("* 5：退出													*\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();

		switch(n){
			case 1:
				all_info(sockfd);
				break;
			case 2:
				change_info(sockfd);
				break;
			case 3:
				add_user(sockfd);
				break;
			case 4:
				del_user(sockfd);
				break;
			case 5:
				close(sockfd);
				exit(0);
				break;
			default:
				printf("输入错误!!!\n");
				break;
		}
	}

}
//删除用户信息
void del_user(int sockfd){
	login_info.choose_flag = 4;
	send(sockfd,&login_info,sizeof(login_info),0);
	printf("请输入需要删除的用户名name\n");
	scanf("%s",user.name);
	send(sockfd,&user,sizeof(user),0);

	recv(sockfd,&user,sizeof(user),0);
	switch(user.flag){
		case 0:
			printf("删除成功!!!\n");
			break;
		case 1:
			printf("删除失败!!!\n");
			break;
	}
}
//添加用户信息
void add_user(int sockfd){
	login_info.choose_flag = 3;
	send(sockfd,&login_info,sizeof(login_info),0);
	printf("请输入编号id\n");
	scanf("%d",&(user.id));
	printf("请输入权限quanxian\n");
	scanf("%d",&(user.quanxian));
//	user.quanxian = 0;
	printf("请输入姓名name\n");
	scanf("%s",user.name);
	printf("请输入密码pwd\n");
	scanf("%s",user.pwd);
	printf("请输入年龄age\n");
	scanf("%d",&(user.age));
	printf("请输入电话\n");
	scanf("%s",user.phone);
	printf("请输入地址addr\n");
	scanf("%s",user.addr);
	printf("请输入修改时间date\n");
	scanf("%s",user.date);
	printf("请输入工资salary\n");
	scanf("%f",&(user.salary));
	send(sockfd,&user,sizeof(user),0);

	recv(sockfd,&user,sizeof(user),0);
	switch(user.flag){
		case 0:
			printf("添加成功!!!\n");
			break;
		case 1:
			printf("添加失败!!!\n");
			break;
	}
}
//改变用户信息
void change_info(int sockfd){
	login_info.choose_flag = 2;
	send(sockfd,&login_info,sizeof(login_info),0);
	int n;
	printf("请填写需要修改的用户名:");
	scanf("%s",user.name);
	printf("请输入需要改的项(1.年龄 2.工资 3.密码　4.手机号码):");
	scanf("%d",&n);
	user.type = n;
	switch(n){
		case 1:
		printf("请输入改后的年龄:");
		scanf("%d",&(user.age));
		break;
		case 2:
		printf("请输入改后的工资:");
		scanf("%f",&(user.salary));
		break;
		case 3:
		printf("请输入改后的密码:");
		scanf("%f",&(user.salary));
		break;
		case 4:
		printf("请输入改后手机号码:");
		scanf("%f",&(user.salary));
		break;
	}
	send(sockfd,&user,sizeof(user),0);
	recv(sockfd,&user,sizeof(user),0);

	switch(user.flag){
		case 0:
			printf("修改成功!!!\n");
			break;
		case 1:
			printf("修改失败!!!\n");
			break;
	}
	

}

//显示所有用户信息
void all_info(int sockfd){
	login_info.choose_flag = 1;
	send(sockfd,&login_info,sizeof(login_info),0);//将登录信息发送到服务器
	printf("编号 用户权限  用户名  密码  年龄  手机号码    地址        修改时间    工资\n");
	while(1){
		recv(sockfd,&user,sizeof(user),0);//将在服务器处理得到的用户信息接收到sockfd中
		if(!user.name[0]){
			printf("已查询!!!\n");
			break;
		}
		printf("%5d %2d %8s %6s %3d %s %s %s %7g\n"
			,user.id,user.quanxian,user.name,user.pwd,user.age,user.phone,
			user.addr,user.date,user.salary);
	}
}

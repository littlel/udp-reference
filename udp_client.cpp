#include <iostream>
#include <string>
using namespace std;

//Socket通讯需要的库
#include<Winsock2.h>
#pragma comment(lib,"WS2_32")

#define  MAX_TASK_PARAM_LEN 512
#define  MAX_LOG_LEN 512

int main()
{
	int             ret;
	SOCKET          sock;
	SOCKADDR_IN     addr;
	SOCKADDR_IN     dst_addr;
	SOCKADDR_IN     src_addr;

	char invitexml[MAX_TASK_PARAM_LEN]={0};
	char recvbuffer[MAX_TASK_PARAM_LEN]={0};
	unsigned port = 8888;

	WSADATA ws;
	WSAStartup(MAKEWORD(2,2),&ws);
	sock=socket(AF_INET,SOCK_DGRAM,NULL);       // 创建数据报sock
		
	//设置发送地址及端口
	addr.sin_family=AF_INET;								// IP协议 
	addr.sin_addr.S_un.S_addr=inet_addr("192.168.10.58");   // IP地址
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);             // 在本机的所有ip上开始监听  
	addr.sin_port=htons(port);								// 端口

	ret=bind(sock,(sockaddr*)&addr,sizeof(SOCKADDR_IN));//若没有绑定本地端口，随机端口发送
	if (ret==SOCKET_ERROR)
		cout<<"绑定发送端口失败："<<GetLastError()<<endl;

	//配置目的地址及端口
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_addr.S_un.S_addr=inet_addr("192.168.10.58");
	dst_addr.sin_port = htons(port+1);


	sprintf_s(invitexml,"\
						<xml>\r\n\
						<cmdtype>invite</cmdtype>\r\n\
						<callid>%s</callid>\r\n\
						<logicid>%d</logicid>\r\n\
						<multip>%s</multip>\r\n\
						<multport>%d</multport>\r\n\
						</xml>\r\n\r\n",
						"abc123g@invite",
						500001,
						"192.168.10.365",
						5000);

	int invitelen=strlen(invitexml);
	
	int sendlen=sendto(sock,invitexml,invitelen,NULL,(sockaddr*)&dst_addr,sizeof(SOCKADDR_IN));
	if (sendlen!=invitelen)
	{
		char log[MAX_LOG_LEN]={0};
		sprintf_s(log,MAX_LOG_LEN,"sendto 失败!%d",GetLastError());
		cout<<log<<endl;
		closesocket(sock);
		system("pause");
		return -1;
	}
	cout<<"发送完成～"<<endl;


/***********************Receive Message********************/


    timeval   ti;
	ti.tv_sec=0;
	ti.tv_usec=500;
	
//没有设置超时时间
	//memset(recvbuffer,0,sizeof(recvbuffer));
	//while (1)
	//{	
	//		ret=recv(sock,recvbuffer,MAX_TASK_PARAM_LEN,NULL);
	//		if (ret>0)
	//		{
	//			cout<<"receive message："<<recvbuffer<<endl;
	//			break;
	//		}
	//}

//设置超时时间
	fd_set  fd;          // 读描述符集
	FD_ZERO(&fd);        // 在使用之前总是要清空  
	FD_SET(sock,&fd);    // 把socka放入要测试的描述符集中  
	
	timeval   _timeout;  // 超时时间
	_timeout.tv_sec=5;   //'s'
	_timeout.tv_usec=0;  //'ms'   

	ret=select(sock+1,&fd,NULL,NULL,&_timeout);
	if (ret>0 && FD_ISSET(sock,&fd))/*FD_ISSET(sock,&fd) 可读*/
	{
//[Annotations]
		/*recv 只是获取报文内容*/
		int recvlen = recv(sock,recvbuffer,MAX_TASK_PARAM_LEN,NULL);
//[Annotations]
	/* recvfrom 多两个参数用于获取对端地址*/
		//int recvlen = recvfrom(sock, recvbuffer, sizeof(recvbuffer,), 0, (sockaddr*)&src_addr, &sizeof(SOCKADDR_IN));
		if (recvlen<=0)
		{
			closesocket(sock);
			return -1;
		}
		else
			cout<<"receive message："<<recvbuffer<<endl;	
	}	

	system("pause");
	closesocket(sock);
	return 0;
}
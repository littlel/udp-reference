#include <iostream>
#include <string>
using namespace std;

//SocketͨѶ��Ҫ�Ŀ�
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
	sock=socket(AF_INET,SOCK_DGRAM,NULL);       // �������ݱ�sock
		
	//���÷��͵�ַ���˿�
	addr.sin_family=AF_INET;								// IPЭ�� 
	addr.sin_addr.S_un.S_addr=inet_addr("192.168.10.58");   // IP��ַ
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);             // �ڱ���������ip�Ͽ�ʼ����  
	addr.sin_port=htons(port);								// �˿�

	ret=bind(sock,(sockaddr*)&addr,sizeof(SOCKADDR_IN));//��û�а󶨱��ض˿ڣ�����˿ڷ���
	if (ret==SOCKET_ERROR)
		cout<<"�󶨷��Ͷ˿�ʧ�ܣ�"<<GetLastError()<<endl;

	//����Ŀ�ĵ�ַ���˿�
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
		sprintf_s(log,MAX_LOG_LEN,"sendto ʧ��!%d",GetLastError());
		cout<<log<<endl;
		closesocket(sock);
		system("pause");
		return -1;
	}
	cout<<"������ɡ�"<<endl;


/***********************Receive Message********************/


    timeval   ti;
	ti.tv_sec=0;
	ti.tv_usec=500;
	
//û�����ó�ʱʱ��
	//memset(recvbuffer,0,sizeof(recvbuffer));
	//while (1)
	//{	
	//		ret=recv(sock,recvbuffer,MAX_TASK_PARAM_LEN,NULL);
	//		if (ret>0)
	//		{
	//			cout<<"receive message��"<<recvbuffer<<endl;
	//			break;
	//		}
	//}

//���ó�ʱʱ��
	fd_set  fd;          // ����������
	FD_ZERO(&fd);        // ��ʹ��֮ǰ����Ҫ���  
	FD_SET(sock,&fd);    // ��socka����Ҫ���Ե�����������  
	
	timeval   _timeout;  // ��ʱʱ��
	_timeout.tv_sec=5;   //'s'
	_timeout.tv_usec=0;  //'ms'   

	ret=select(sock+1,&fd,NULL,NULL,&_timeout);
	if (ret>0 && FD_ISSET(sock,&fd))/*FD_ISSET(sock,&fd) �ɶ�*/
	{
//[Annotations]
		/*recv ֻ�ǻ�ȡ��������*/
		int recvlen = recv(sock,recvbuffer,MAX_TASK_PARAM_LEN,NULL);
//[Annotations]
	/* recvfrom �������������ڻ�ȡ�Զ˵�ַ*/
		//int recvlen = recvfrom(sock, recvbuffer, sizeof(recvbuffer,), 0, (sockaddr*)&src_addr, &sizeof(SOCKADDR_IN));
		if (recvlen<=0)
		{
			closesocket(sock);
			return -1;
		}
		else
			cout<<"receive message��"<<recvbuffer<<endl;	
	}	

	system("pause");
	closesocket(sock);
	return 0;
}
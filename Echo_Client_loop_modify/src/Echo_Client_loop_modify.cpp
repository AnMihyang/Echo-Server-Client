//============================================================================
// Name        : Echo_Client_loop_modify.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//----현재 수정중인 Client 메뉴 랜덤 반복 (Echo_Serv_modify 프로젝트와 통신)----

#include <iostream>
#include <list>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "PacketDefine.h"


void output_Menu();											//Menu 출력
void message_Input_Send(int *sock, PACKET pack);			//입력받은 데이터 Server로 Send
int Print_recv_List(int *sock);				//Server에서 List에 있는 데이터 받아서 출력
void error_handling(int *sock, char *message);						//error 처리

using namespace std;

int main(int argc, char *argv[])
{
	int sock;
	char message[MAX_PACKET_SIZE];

	struct sockaddr_in serv_adr;
	int menu;
	PACKET pack;

	/*
	//IP주소와 PORT번호 입력이 안될 경우 출력, 종료 처리
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	*/

	//소켓 생성
	sock=socket(PF_INET, SOCK_STREAM, 0);

	//소켓 생성 에러 처리
	if(sock==-1)
		error_handling(&sock, "socket() error");

	//serv_adr 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr("192.168.8.37");
	serv_adr.sin_port=htons(9190);

	//server에 연결 요청 후 에러 처리
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling(&sock, "connect() error!");
	else
	{
		puts("Connected...........");
		pack.cmd = CMD_USER_LOGIN_REQ;
	}
//	cout << "Enter ID: ";
//	fgets(message, MAX_PACKET_SIZE, stdin);

	strcpy(pack.data, "login");
	pack.size = strlen(pack.data);
	// server로 패킷 보내기
	send(sock, (char*)&pack, sizeof(PACKET), 0);
//	cout << "send : " << pack.cmd << ", " << pack.data << endl;
	pack.size = 0;

	srand(time(NULL));

	while(1)
	{
		recv(sock, (char*) &pack, sizeof(PACKET), 0);
//		cout << "recv : " << pack.cmd << ", " << pack.data << endl;
		//Command 처리
		switch (pack.cmd) {
		case CMD_USER_LOGIN_RESULT:
			cout << endl << "<로그인 성공!>" << endl;
			cout << "sock: " << pack.data << endl;
			break;

		case CMD_USER_DATA_RESULT:
			cout << endl << "Message from server:" << endl;
			cout << " >> " << pack.data << endl;
			break;

		case CMD_USER_SAVE_RESULT:
			cout << endl << pack.data << endl;
			cout << endl << "---Data Structure List---" << endl;
			if(Print_recv_List(&sock))
				cout << "[ERROR] print list error" << endl;
			break;

		case CMD_USER_DELETE_RESULT:
			cout << endl << pack.data << endl;
			cout << endl << "---Data Structure List---" << endl;
			if(Print_recv_List(&sock))
				cout << "[ERROR] print list error" << endl;
			break;

		case CMD_USER_PRINT_RESULT:
			cout << endl << pack.data << endl;
			cout << endl << "---Data Structure List---" << endl;
			if(Print_recv_List(&sock))
				cout << "[ERROR] print list error" << endl;
			break;

		case CMD_USER_ERR:
			cout << "[ERROR] " << pack.data << endl;
//			close(sock);
//			return 0;
			break;

		default:
			puts("[ERROR] Result Command Error");
			cout << "[CMD] " << pack.cmd << endl;
			close(sock);
			return 0;
		}

		usleep(700000);
//		menu = rand()%4+1;

		//Server로 Packet Send
		switch(2)
		{
			case 1:	//Echo Data
				cout << endl << "Input message: " << endl;
				cout << " >> ";
				pack.cmd = CMD_USER_DATA_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 2:	//Save Data
				cout << endl << "Enter message to save: ";
				pack.cmd = CMD_USER_SAVE_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 3:	//Delete Data
				cout << endl << "Enter message to delete: ";
				pack.cmd = CMD_USER_DELETE_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 4:	//Print Data Structure List
				pack.cmd = CMD_USER_PRINT_REQ;
				strcpy(pack.data, "CMD_USER_PRINT_REQ");
				send(sock, (char*)&pack, sizeof(PACKET), 0);
				break;

			case 5:
				close(sock);
				return 0;

			default:
				puts("\n!!try again!!");
		}
	}//while end

	return 0;
}

void output_Menu()
{
	printf("\n--------Menu--------\n");
	printf(" 1. Input Echo Data\n");
	printf(" 2. Save Data\n");
	printf(" 3. Delete Data\n");
	printf(" 4. Data List\n");
	printf(" 5. exit\n");
	printf(" >> ");
}

/*
void message_Input_Send(int *sock, PACKET pack)
{
	string message;

	strcpy(pack.data, "\0");
	cin.ignore(MAX_PACKET_SIZE, '\n');		// \n전까지 입력받음
	getline(cin, message);

	strcpy(pack.data, message.c_str());
	pack.size = strlen(pack.data);

	// server로 패킷 보내기
	send(*sock, (char*)&pack, sizeof(PACKET),0);
	cout << "send : " << pack.size << ", " << pack.cmd << ", " << pack.data << endl;
}
*/

void message_Input_Send(int *sock, PACKET pack)
{
	int datalen = rand()%100+1;
	for(int i = 0; i < datalen; ++i)
	{
		pack.data[i] = rand()%94 + 33;
	}
	pack.data[datalen] = '\0';
	pack.size = strlen(pack.data);

	cout << pack.data << endl;

	// server로 패킷 보내기
	send(*sock, (char*)&pack, sizeof(PACKET),0);
	pack.size = 0;
}


int Print_recv_List(int *sock)
{
	PACKET pack;
	int cnt = 0;
	do {
		if(recv(*sock, (char*) &pack, sizeof(PACKET), 0) == -1)
			return -1;

		if (pack.cmd == 0 || !strcmp(pack.data, "\0"))
			return -1;
		cout << ' ' << ++cnt << ") " << pack.cmd << ", " << pack.data << endl;
	} while(pack.cmd != CMD_USER_ERR);
	cout << "-------------------------" << endl;

	return 0;
}

void error_handling(int *sock, char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	close(*sock);
	exit(1);
}

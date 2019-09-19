//============================================================================
// Name        : Echo_Client_0918.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <list>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "PacketDefine.h"

#define BUF_SIZE 65000

void output_Menu();											//Menu 출력
void message_Input_Send(int *sock, PACKET pack);			//입력받은 데이터 Server로 Send
void Print_recv_List(int sock, PACKET pack);				//Server에서 List에 있는 데이터 받아서 출력
void error_handling(char *message);						//error 처리

using namespace std;

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];

	struct sockaddr_in serv_adr;
	string menu;
	PACKET pack;

	//IP주소와 PORT번호 입력이 안될 경우 출력, 종료 처리
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	//소켓 생성
	sock=socket(PF_INET, SOCK_STREAM, 0);

	//소켓 생성 에러 처리
	if(sock==-1)
		error_handling("socket() error");

	//serv_adr 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	//server에 연결 요청 후 에러 처리
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
	{
		puts("Connected...........");
		pack.cmd = CMD_USER_LOGIN_REQ;
	}

	cout << "Enter ID: ";
	fgets(message, BUF_SIZE, stdin);
	strcpy(pack.data, message);

	// server로 패킷 보내기
	send(sock, (char*)&pack, sizeof(PACKET), 0);
//	cout << "send : " << pack.cmd << ", " << pack.data << endl;
	while(1)
	{
		recv(sock, (char*) &pack, sizeof(PACKET), 0);
//		cout << "recv : " << pack.cmd << ", " << pack.data << endl;
		//Command 처리
		switch (pack.cmd) {
		case CMD_USER_LOGIN_RESULT:
			cout << endl;
			cout << "<로그인 성공!>" << endl;
			break;

		case CMD_USER_DATA_RESULT:
			cout << "\nMessage from server: " << pack.data << endl;
			break;

		case CMD_USER_SAVE_RESULT:
			cout << "\n" << pack.data << endl;
			cout << "\n---Data Structure List---" << endl;
			Print_recv_List(sock, pack);
			break;

		case CMD_USER_DELETE_RESULT:
			cout << "\n" << pack.data << endl;
			cout << "\n---Data Structure List---" << endl;
			Print_recv_List(sock, pack);
			break;

		case CMD_USER_PRINT_RESULT:
			cout << "\n---Data Structure List---" << endl;
			Print_recv_List(sock, pack);
			break;

		case CMD_USER_ERR:
			puts("[ERROR] cmd: CMD_USER_ERR");
			cout << pack.data << endl;
			close(sock);
			return 0;
			break;
		default:
			puts("[ERROR] Result Command Error");
			close(sock);
			return 0;
		}

		output_Menu();
		cin >> menu;

		while(1)	// 1~5 사이가 아닌 다른 값 입력 시 다시 입력 받음
		{
			if (atoi(menu.c_str()) < 1 || atoi(menu.c_str()) > 5)
			{
				cout << endl;
				cout << "!!try again!!" << endl;
				output_Menu();
				cin >> menu;
			}
			else
				break;
		}

		//Server로 Packet Send
		switch(atoi(menu.c_str()))
		{
			case 1:	//Echo Data
				fputs("\nInput message: ", stdout);
				pack.cmd = CMD_USER_DATA_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 2:	//Save Data
				cout << "\nEnter message to save: ";
				pack.cmd = CMD_USER_SAVE_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 3:	//Delete Data
				cout << "\nEnter message to delete: ";
				pack.cmd = CMD_USER_DELETE_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 4:	//Print Data Structure List
				pack.cmd = CMD_USER_PRINT_REQ;
				strcpy(pack.data, "\0");
				send(sock, (char*)&pack, sizeof(PACKET), 0);
				break;

			case 5:
				pack.cmd = CMD_USER_ERR;
				strcpy(pack.data, "\0");
				send(sock, (char*)&pack, sizeof(PACKET),0);
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


void message_Input_Send(int *sock, PACKET pack)
{
	string message;

	strcpy(pack.data, "\0");
	cin.ignore(BUF_SIZE, '\n');		// \n전까지 입력받음
	getline(cin, message);

	strcpy(pack.data, message.c_str());

	// server로 패킷 보내기
	send(*sock, (char*)&pack, sizeof(PACKET),0);
}

void Print_recv_List(int sock, PACKET pack)
{
	while (pack.cmd != CMD_USER_ERR)
	{
		recv(sock, (char*) &pack, sizeof(PACKET), 0);
		cout << ' ' << pack.data << endl;
	}
	cout << "-------------------------" << endl;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

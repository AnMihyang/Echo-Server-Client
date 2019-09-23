//============================================================================
// Name        : Echo_Client_loop_0918.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// ----Client 명령 랜덤 반복(Echo_Server_0918 프로젝트와 통신)----

#include "PacketDefine.h"
#include <iostream>
#include <list>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <cstdio>

#define BUF_SIZE 100

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
//	string menu;
	int menu;
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

//	cout << "Enter ID: ";
//	cin >> message;
//	fgets(message, BUF_SIZE, stdin);
	strcpy(pack.data, "login");

	// server로 패킷 보내기 (login)
	send(sock, (char*)&pack, sizeof(PACKET), 0);
//	cout << "send : " << pack.cmd << ", " << pack.data << endl;

	srand(time(NULL));

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
			cout << "Message from server: " << pack.data << endl;
			break;

		case CMD_USER_SAVE_RESULT:
			cout << pack.data << endl;
			cout << "---Data Structure List---" << endl;
			Print_recv_List(sock, pack);
//			cout << endl;
			break;

		case CMD_USER_DELETE_RESULT:
			cout  << pack.data << endl;
			cout << "---Data Structure List---" << endl;
			Print_recv_List(sock, pack);
//			cout << endl;
			break;

		case CMD_USER_PRINT_RESULT:
			cout << "---Data Structure List---" << endl;
			Print_recv_List(sock, pack);
//			cout << endl;
			break;

		case CMD_USER_ERR:
			cout << "[ERROR] " << pack.cmd << ", " << pack.data << endl;
			close(sock);
			return 0;
		default:
			puts("[ERROR] Result Command Error");
			cout << "[ERROR PACKET] " << pack.cmd << ", " << pack.data << endl;
			close(sock);
			return 0;
		}

//		output_Menu();
//		cin >> menu;
/*
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
		*/

//		if(loopCnt >= 10)
//			break;
		usleep(1000000);
		menu = rand()%2+2;
//		cout << menu << endl;

		//Server로 Packet Send
		switch(menu)
		{
			case 1:	//Echo Data
				puts("\n[MENU] Echo data");
				fputs("Input message: ", stdout);
				pack.cmd = CMD_USER_DATA_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 2:	//Save Data
				puts("\n[MENU] Save data");
				cout << "Enter message to save: ";
				pack.cmd = CMD_USER_SAVE_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 3:	//Delete Data
				puts("\n[MENU] Delete data");
				cout << "Enter message to delete: ";
				pack.cmd = CMD_USER_DELETE_REQ;
				message_Input_Send(&sock, pack);
				break;

			case 4:	//Print Data Structure List
				puts("\n[MENU] Print list");
				pack.cmd = CMD_USER_PRINT_REQ;
				strcpy(pack.data, "\0");
				send(sock, (char*)&pack, sizeof(PACKET), 0);
				break;

			case 5:
				pack.cmd = CMD_USER_ERR;
				send(sock, (char*)&pack, sizeof(PACKET),0);
				close(sock);
				return 0;

			default:
				puts("\n!!try again!!");
		}
//		loopCnt++;
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
	cin.ignore(BUF_SIZE, '\n');		// \n전까지 입력받음
	getline(cin, message);
	strcpy(pack.data, message.c_str());
	// server로 패킷 보내기
	send(*sock, (char*)&pack, sizeof(PACKET),0);
}
*/

void message_Input_Send(int *sock, PACKET pack)
{
	string message;
	message = rand()%5 + 65;
	cout << message << endl;
	strcpy(pack.data, "\0");
//	cout << "packet 초기화: " << pack.data << endl;
	strcpy(pack.data, message.c_str());
//	pack.data = message;
//	strcpy(pack.data, message);
//	cout << "packet data: " << pack.data << endl;
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

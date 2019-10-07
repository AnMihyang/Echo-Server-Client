//============================================================================
// Name        : Echo_Client_modify.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//----현재 수정중인 Client (Echo_server_ver3 프로젝트와 통신)----

#include <iostream>
#include <list>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "Packet_define.h"

void Output_menu();											//Menu 출력
void Message_input_send(int *sock, PACKET pack);			//입력받은 데이터 Server로 Send
void Print_recv_list(int sock);				//Server에서 List에 있는 데이터 받아서 출력
void Error_handling(int *sock, char *message);						//error 처리

using namespace std;

int main(int argc, char *argv[])
{
	int sock;
	char message[MAX_DATA_SIZE];

	struct sockaddr_in serv_adr;
	string menu;
	PACKET pack;

	/*
	//IP주소와 PORT번호 입력이 안될 경우 출력, 종료 처리
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}*/

	//소켓 생성
	sock=socket(PF_INET, SOCK_STREAM, 0);

	//소켓 생성 에러 처리
	if(sock==-1)
		Error_handling(&sock, "socket() error");

	//serv_adr 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr("192.168.8.37");
	serv_adr.sin_port = htons(9190);

	//server에 연결 요청 후 에러 처리
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		Error_handling(&sock, "connect() error!");
	else
	{
		puts("Connected...........");
		pack.body.cmd = CMD_USER_LOGIN_REQ;
	}
	cout << "Enter ID: ";
	fgets(message, MAX_DATA_SIZE, stdin);

	strncpy(pack.body.data, message, sizeof(pack.body.data));
	pack.phead.datasize = strlen(message);

	// server로 패킷 보내기
	send(sock, (char*)&pack, sizeof(PACKET), 0);
	cout << "send : " << pack.body.cmd << ", " << pack.body.data << endl;

	while(1)
	{
		recv(sock, (char*) &pack, sizeof(PACKET), MSG_WAITALL);
		cout << "recv : " << pack.body.cmd << ", " << pack.body.data << endl;
		//Command 처리
		switch (pack.body.cmd) {
		case CMD_USER_LOGIN_RESULT:
			cout << endl;
			cout << "<로그인 성공!>" << endl;
			break;

		case CMD_USER_DATA_RESULT:
			cout << "\nMessage from server: " << pack.body.data << endl;
			break;

		case CMD_USER_SAVE_RESULT:
			cout << "\n" << pack.body.data << endl;
			Print_recv_list(sock);
			break;

		case CMD_USER_DELETE_RESULT:
			cout << "\n" << pack.body.data << endl;
			Print_recv_list(sock);
			break;

		case CMD_USER_PRINT_RESULT:
			cout << "\n" << pack.body.data << endl;
			Print_recv_list(sock);
			break;

		case CMD_USER_ERR:
			puts("[ERROR] cmd: CMD_USER_ERR");
			cout << pack.body.data << endl;
			close(sock);
			return 0;

		default:
			puts("[ERROR] Result command error");
			close(sock);
			return 0;
		}

		Output_menu();
		cin >> menu;

		while(1)	// 1~5 사이가 아닌 다른 값 입력 시 다시 입력 받음
		{
			if (atoi(menu.c_str()) < 1 || atoi(menu.c_str()) > 5)
			{
				cout << endl;
				cout << "!!try again!!" << endl;
				Output_menu();
				cin >> menu;
			}
			else
				break;
		}

		//Server로 Packet Send
		switch(atoi(menu.c_str()))
		{
			case 1:	//Echo Data
//				fputs("\nInput message: ", stdout);
				cout << "\nInput message." << endl;
				cout << ">> ";
				pack.body.cmd = CMD_USER_DATA_REQ;
				Message_input_send(&sock, pack);
				break;

			case 2:	//Save Data
				cout << "\nEnter message to save." << endl;
				cout << ">> ";
				pack.body.cmd = CMD_USER_SAVE_REQ;
				Message_input_send(&sock, pack);
				break;

			case 3:	//Delete Data
				cout << "\nEnter message to delete." << endl;
				cout << ">> ";
				pack.body.cmd = CMD_USER_DELETE_REQ;
				Message_input_send(&sock, pack);
				break;

			case 4:	//Print Data Structure List
				pack.body.cmd = CMD_USER_PRINT_REQ;
				strncpy(pack.body.data, "CMD_USER_PRINT_REQ", sizeof(pack.body.data));
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

void Output_menu()
{
	printf("\n--------Menu--------\n");
	printf(" 1. Input Echo Data\n");
	printf(" 2. Save Data\n");
	printf(" 3. Delete Data\n");
	printf(" 4. Data List\n");
	printf(" 5. exit\n");
	printf(" >> ");
}


void Message_input_send(int *sock, PACKET pack)
{
	string message;
	strncpy(pack.body.data, "\0", sizeof(pack.body.data));	//pack.body.data 초기화
	cin.ignore(MAX_PRINT_DATA_SIZE-1, '\n');		// \n전까지 입력받음
	while(1)
	{
		getline(cin, message);
		cin.clear();				//버퍼 초기화
		cout << "length: " << message.length() << endl;

		if(message.length() < MAX_PRINT_DATA_SIZE)
			break;

		cout << "Data size exceeded. Please enter again." << endl;
		cout << ">> ";
	}

	strncpy(pack.body.data, message.c_str(), sizeof(pack.body.data));
	pack.phead.datasize = strlen(pack.body.data);

	// server로 패킷 보내기
	send(*sock, (char*)&pack, sizeof(PACKET),0);
	cout << "send : " << pack.body.cmd << ", " << pack.body.data << endl;
}

void Print_recv_list(int sock)
{
	PRT_PACKET recv_pack;
	char temp[MAX_DATA_SIZE];
	int num = 0;

	memset(temp, '\0', MAX_DATA_SIZE);
	memset(recv_pack.data, '\0', sizeof(recv_pack.data));

	cout << "\n------Data Structure List------" << endl;

	do
	{
		recv(sock, (char*) &recv_pack, sizeof(PRT_PACKET), MSG_WAITALL);

		if(!recv_pack.data_num)
			cout << recv_pack.data << endl;

		for (unsigned int i = 0, index = 0; i < recv_pack.data_num; i++) {
			num++;
			strcpy(temp, &recv_pack.data[index]);
			cout << num << ") " << temp << endl;
			index += strlen(temp) + 1;
//			cout << "index: " << index << endl;
		}
	} while (recv_pack.cmd != CMD_USER_ERR);
	cout << "-------------------------------" << endl;
}

/*void Print_recv_list(int sock)
{
	PRT_PACKET recv_pack;
	char *tok;
	int data_cnt = 0;
	cout << "\n---Data Structure List---" << endl;
	do
	{
		recv(sock, (char*)&recv_pack, sizeof(PRT_PACKET), MSG_WAITALL);
		tok = strtok(recv_pack.data, "#;;");
		while(tok != NULL)
		{
			cout << ++data_cnt << ") " << tok << endl;
			tok = strtok(NULL, "#;;");
		}
	} while(recv_pack.cmd != CMD_USER_ERR);
}*/

/*void Print_recv_list(int sock, PACKET pack)
{
	int cnt = 0;
	while (pack.body.cmd != CMD_USER_ERR)
	{
		recv(sock, (char*) &pack, sizeof(PACKET), 0);
		cout << ' ' << ++cnt << ") " << pack.body.data << endl;
	}
	cout << "-------------------------" << endl;
}*/

void Error_handling(int *sock, char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	close(*sock);
	exit(1);
}

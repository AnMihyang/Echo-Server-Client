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

#define BUF_SIZE 1024000

void Output_menu();										//Menu 출력
void Message_input_send(int *sock, PACKET pack);		//입력받은 데이터 Server로 Send
void Print_recv_list(int sock);					//Server에서 List에 있는 데이터 받아서 출력
void Error_handling(int *sock, char *message);		//error 처리

using namespace std;

int main(int argc, char *argv[])
{
	int sock;
	int m_bufopt_adr = BUF_SIZE;

	struct sockaddr_in serv_adr;
	int menu;
	PACKET pack;

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

	cout << "Requesting connection to server..." << endl;

	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&m_bufopt_adr, BUF_SIZE);
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&m_bufopt_adr, BUF_SIZE);

	//server에 연결 요청 후 에러 처리
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		Error_handling(&sock, "connect() error!");
	else
	{
		puts("Connected...........");
		pack.body.cmd = CMD_USER_LOGIN_REQ;
	}
	cout << "Enter ID: ";

	strncpy(pack.body.data, "login", sizeof(pack.body.data));
	pack.phead.datasize = strlen(pack.body.data);

	// server로 패킷 보내기
	if (send(sock, (char*) &pack, sizeof(PACKET), 0) == -1)
		while(send(sock, (char*) &pack, sizeof(PACKET), 0) == -1);

	srand(time(NULL));

	while(1)
	{
		if(recv(sock, (char*) &pack, sizeof(PACKET), MSG_WAITALL) == -1)
			while(recv(sock, (char*) &pack, sizeof(PACKET), MSG_WAITALL) == -1);
		//Command 처리
		switch (pack.body.cmd) {
		case CMD_USER_LOGIN_RESULT:
			cout << endl;
			cout << "<로그인 성공!>" << endl;
			break;

		case CMD_USER_DATA_RESULT:
			cout << endl;
			cout << "Message from server. " << endl;
			cout << ">> " << pack.body.data << endl;
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
			Print_recv_list(sock);
			break;

		case CMD_USER_ERR:
			cout << "[ERROR] " << pack.body.data << endl;
			close(sock);
			return 0;

		default:
			puts("[ERROR] Result command error");
			close(sock);
			return 0;
		}
//		usleep(500000);
		Output_menu();

		menu = rand()%3+1;
		cout << menu << endl;

		//Server로 Packet Send
		switch(menu)
		{
			case 1:	//Echo Data
				cout << "\nInput message." << endl <<  ">> ";
				pack.body.cmd = CMD_USER_DATA_REQ;
				Message_input_send(&sock, pack);
				break;

			case 2:	//Save Data
				cout << "\nEnter message to save." << endl << ">> ";
				pack.body.cmd = CMD_USER_SAVE_REQ;
				Message_input_send(&sock, pack);
				break;

			case 3:	//Delete Data
				cout << "\nEnter message to delete." << endl << ">> ";
				pack.body.cmd = CMD_USER_DELETE_REQ;
				Message_input_send(&sock, pack);
				break;

			case 4:	//Print Data Structure List
				pack.body.cmd = CMD_USER_PRINT_REQ;
				strncpy(pack.body.data, "CMD_USER_PRINT_REQ", sizeof(pack.body.data));
				pack.phead.datasize = strlen(pack.body.data);
				if(send(sock, (char*)&pack, sizeof(PACKET), 0) == -1)
					while(send(sock, (char*)&pack, sizeof(PACKET), 0) == -1);
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
	int datalen;

	memset(pack.body.data, '\0', MAX_DATA_SIZE);
	pack.phead.datasize = 0;

	while(1)
	{
//		datalen = rand() % MAX_DATA_SIZE + 1;
		datalen = rand() % 100 + 1;
		if (datalen <= MAX_DATA_SIZE)
			break;
	}

	for (int i = 0; i < datalen; ++i)
		pack.body.data[i] = rand() % 94 + 33;

	pack.body.data[datalen] = '\0';
	cout << pack.body.data << endl;
	pack.phead.datasize = strlen(pack.body.data);
	// server로 패킷 보내기
	if(send(*sock, (char*)&pack, sizeof(PACKET),0) == -1)
	{
		cout << "[ERROR] send() error" << endl;
		close(*sock);
		exit(1);
	}
	usleep(700000);
//	sleep(1);
}

void Print_recv_list(int sock)
{
	PRT_PACKET recv_pack;
	char temp[MAX_DATA_SIZE];
	int num = 0;

	memset(temp, '\0', MAX_DATA_SIZE);
	memset(&recv_pack, 0x00, sizeof(PRT_PACKET));

	cout << "\n-----------Data Structure List-----------" << endl;

	do
	{
		if(recv(sock, (char*) &recv_pack, sizeof(PRT_PACKET), MSG_WAITALL) == -1)
		{
			cout << "[ERROR] recv() error" << endl;
			close(sock);
			exit(1);
		}

		if(!strcmp(recv_pack.head, "AA11"))
		{
			if(!recv_pack.data_num)	//list 비었을 경우
					cout << recv_pack.data << endl;

			for (unsigned int i = 0, index = 0; i < recv_pack.data_num; i++)
			{
				num++;
				strcpy(temp, &recv_pack.data[index]);
				cout << num << ") " << temp << endl << endl;
				index += strlen(temp) + 1;
			}
		}
		else
			puts("[ERROR] receive packet error");
	} while (recv_pack.cmd != CMD_USER_ERR);
	cout << "-----------------------------------------" << endl;
}

void Error_handling(int *sock, char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	close(*sock);
	exit(1);
}

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

	//server에 연결 요청 후 에러 처리
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		Error_handling(&sock, "connect() error!");
	else
	{
		puts("Connected...........");
		pack.body.cmd = CMD_USER_LOGIN_REQ;
	}
	cout << "Enter ID: ";
//	fgets(message, MAX_DATA_SIZE, stdin);

	strncpy(pack.body.data, "login", sizeof(pack.body.data));
	pack.phead.datasize = strlen(pack.body.data);

	// server로 패킷 보내기
	send(sock, (char*)&pack, sizeof(PACKET), 0);

	srand(time(NULL));

	while(1)
	{
		recv(sock, (char*) &pack, sizeof(PACKET), MSG_WAITALL);
//		cout << "recv : " << pack.body.cmd << ", " << pack.body.data << endl;
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
			puts("[ERROR] cmd: CMD_USER_ERR");
			cout << pack.body.data << endl;
			close(sock);
			return 0;

		default:
			puts("[ERROR] Result command error");
			close(sock);
			return 0;
		}

		usleep(500000);

		Output_menu();

		menu = rand()%4+1;
		cout << menu << endl;

		//Server로 Packet Send
		switch(menu)
		{
			case 1:	//Echo Data
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
				pack.phead.datasize = strlen(pack.body.data);
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
	int datalen;

	memset(pack.body.data, '\0', MAX_DATA_SIZE);
	pack.phead.datasize = 0;

	while(1)
	{
		datalen = rand() % 99 + 1;
		if (datalen < MAX_DATA_SIZE)
			break;
	}

	for (int i = 0; i < datalen; ++i)
		pack.body.data[i] = rand() % 94 + 33;

	pack.body.data[datalen] = '\0';
	cout << pack.body.data << endl;
	pack.phead.datasize = strlen(pack.body.data);
	// server로 패킷 보내기
	send(*sock, (char*)&pack, sizeof(PACKET),0);
//	cout << "send : " << pack.body.cmd << ", " << pack.body.data << endl;
}

void Print_recv_list(int sock)
{
	PRT_PACKET recv_pack;
	char temp[MAX_DATA_SIZE];
	int num = 0;

	memset(temp, '\0', MAX_DATA_SIZE);
	memset(recv_pack.data, '\0', sizeof(recv_pack.data));

	cout << "\n-----------Data Structure List-----------" << endl;

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
	cout << "-----------------------------------------" << endl;
}

void Error_handling(int *sock, char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	close(*sock);
	exit(1);
}

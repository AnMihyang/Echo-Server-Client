/*
 * CUser.cpp
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#include "CUser.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define ERR -1

using namespace std;

CUser::CUser()
{
	m_clnt_sock = 0;
	m_strlen = 0;
	m_clnt_connect = false;
	m_data_list = nullptr;

	for(int i = 0; i < QUEUE_SIZE; ++i)
		queue.pack[i] = (PACKET *)malloc(sizeof(PACKET));	//queue.pack 초기화

	queue.front = queue.rear = 0;		//queue 초기화
}

CUser::~CUser() {
	for(int i = 0; i < QUEUE_SIZE; ++i)
			free(queue.pack[i]);	//queue.pack 메모리 해제
}

// Client로 부터 Packet 받기
int CUser::Recv_data(int fd, int cUser_num, list<string> *data_list) {

	m_clnt_sock = fd;
	m_data_list = data_list;
	PACKET recv_pack;
	while (1)
	{
		//Client로부터 Packet 받기
		m_strlen = recv(m_clnt_sock, (char*) &recv_pack, sizeof(PACKET), 0);

		cout << endl << "recv 실행" <<endl;
		if (m_strlen == 0)
		{
			cout << "strlen: " << m_strlen << endl;
			return ERR;
		}

		else if (m_strlen < 0)
		{
			if (errno == EAGAIN)
				break;
		}
		else
		{
			cout << "[Client " << fd << "] Recv: " << recv_pack.cmd << ", " << recv_pack.data << endl;
			if(!Check_packet(recv_pack))
				m_Circularbuf.Enqueue(&queue, recv_pack);
			break;
		} //else end
	}
	return 0;
}

// 큐에 send할 패킷이 있는지 확인
int CUser::Queue_check()
{
	if(m_Circularbuf.Isempty_queue(&queue))
		return ERR;								//큐 비었으면 return -1
	return 0;
}

// Packet Parsing
int CUser::Parsing_data()
{
	PACKET pack;
	/*
	if(CheckPacket()==ERR)
	{
		puts("[ERROR] CheckPacket error");
		m_CircularBuf.Dequeue(&queue);
		pack.cmd = CMD_USER_ERR;
		strcpy(pack.data, "Packet Error");
		return SendData(pack);
	}
	else
	{*/
		memcpy(&pack, queue.pack[queue.front], sizeof(pack));		//받은 packet queue에 복사
		m_Circularbuf.Dequeue(&queue);

		switch (pack.cmd) {
			case CMD_USER_LOGIN_REQ:
				//Client에 패킷 전송
				pack.cmd = CMD_USER_LOGIN_RESULT;
				sprintf(pack.data, "%d", m_clnt_sock);
				return Send_data(pack);

				//Echo Data 처리
			case CMD_USER_DATA_REQ:
				pack.cmd = CMD_USER_DATA_RESULT;
				return Send_data(pack);

			//Data 저장 처리
			case CMD_USER_SAVE_REQ:
				pack.cmd = CMD_USER_SAVE_RESULT;
				if(! m_Data_mng.Insert_data(pack, m_data_list))
					strcpy(pack.data, "[SUCCESS] Data 저장 성공");
				else
					strcpy(pack.data, "[FAIL] 이미 저장된 데이터");

				if(Send_data(pack) == ERR)
					return ERR;
				else
				{
					//리스트 프린트
					if(m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
						return ERR;
					return 0;
				}

			//Data 삭제 처리
			case CMD_USER_DELETE_REQ:
				pack.cmd = CMD_USER_DELETE_RESULT;
				if(! m_Data_mng.Delete_data(pack, m_data_list))
					strcpy(pack.data, "[SUCCESS] Data 삭제 성공");
				else
					strcpy(pack.data, "[FAIL] list에 해당 데이터 없음");

				if(Send_data(pack) == ERR)
					return ERR;
				else
				{
					if(m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
						return ERR;
					return 0;
				}

			//Data list 출력
			case CMD_USER_PRINT_REQ:
				pack.cmd = CMD_USER_PRINT_RESULT;
				strcpy(pack.data, "CMD_USER_PRINT_RESULT");
				if(Send_data(pack) == ERR)
					return ERR;
				else
				{
					if(m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
						return ERR;
					return 0;
				}

			default:
				pack.cmd = CMD_USER_ERR;
				strcpy(pack.data, "Request ERROR");
				puts("[ERROR] request error");
				return Send_data(pack);
		} //switch
//	}

	return 0;
}

// Client로 Packet 보내기
int CUser::Send_data(PACKET pack)
{
	if(send(m_clnt_sock, (char*) &pack, sizeof(PACKET), 0) == -1)
		return ERR;
	return 0;
}

//큐에 저장된 패킷 데이터 정상인지 확인 (Packet size 초과 시 예외 처리 하기 (데이터 끊어서 받기))
int CUser::Check_packet(PACKET pack)
{
	if (strcmp(pack.head, "AA11"))	//head 확인
	{
		puts("head");
		return ERR;
	}

	/*
	 if(strcmp(queue.pack[queue.front]->tail, "11AA"))	//tail 확인
	 {
	 puts("tail");
	 return ERR;
	 }*/

	if (pack.cmd <= 0 || pack.cmd > 11)	//cmd 확인
	{
		puts("cmd");
		return ERR;
	}
	if (pack.size <= 0)			//size 확인
	{
		puts("size");
		return ERR;
	}

	/*
	if(strcmp(queue.pack[queue.front]->head, "AA11"))	//head 확인
	{
		puts("head");
		return ERR;
	}

	/*
	if(strcmp(queue.pack[queue.front]->tail, "11AA"))	//tail 확인
	{
		puts("tail");
		return ERR;
	}

	if(queue.pack[queue.front]->cmd <= 0 || queue.pack[queue.front]->cmd > 11)	//cmd 확인
	{
		puts("cmd");
		return ERR;
	}
	if(queue.pack[queue.front]->size <= 0)			//size 확인
	{
//		puts("size");
		return ERR;
	}*/
	//TODO: data 확인
//	cout << "size: " << queue.pack[queue.front]->size << endl;
	return 0;
}



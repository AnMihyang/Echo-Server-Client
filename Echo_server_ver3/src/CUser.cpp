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
	Init();
}

CUser::~CUser() {
}

//큐, 변수 초기화
void CUser::Init()
{
	queue.front = queue.rear = 0;		//queue 초기화
	m_clnt_sock = 0;
	m_clnt_connect = false;
}

void CUser::User_set(int clnt_fd)
{
	m_clnt_sock = clnt_fd;		//client fd 할당
	m_clnt_connect = true;		//connect 연결
}

// Client로 부터 Packet 받기
int CUser::Recv_data(list<string> *data_list) {
	m_data_list = data_list;
	PACKET recv_pack;
	while (1)
	{
//		Client로부터 Packet 받기
		m_strlen = recv(m_clnt_sock, (char*) &recv_pack, sizeof(PACKET), MSG_WAITALL);

		if (m_strlen == 0) return ERR;

		else if (m_strlen < 0)
		{
			if (errno == EAGAIN) break;
		}

		else
		{
//			cout << "[Client " << m_clnt_sock << "] recv: " << recv_pack.body.cmd << ", " << recv_pack.body.data << endl;
			while(m_Circular_buffer.Enqueue(&queue, recv_pack) == ERR);

			break;
		} //else end
	}
	return 0;
}

// 큐에 send할 패킷이 있는지 확인
int CUser::Queue_check()
{
	if(m_Circular_buffer.Isempty_queue(&queue))
		return ERR;								//큐 비었으면 return -1
	return 0;
}

// Packet Parsing
int CUser::Parsing_data()
{
	if(Find_packet())		//Find_packet() 에러 처리
	{
		cout << "rear: " << queue.rear << ", front: " << queue.front << endl;
		puts("[ERROR] packet not find");
		return ERR;
	}

	switch (m_parsing_pack.body.cmd)
	{
		//Login 처리
		case CMD_USER_LOGIN_REQ:
			//Client에 패킷 전송
			m_parsing_pack.body.cmd = CMD_USER_LOGIN_RESULT;
			return Send_data(m_parsing_pack);

		//Echo Data 처리
		case CMD_USER_DATA_REQ:
			m_parsing_pack.body.cmd = CMD_USER_DATA_RESULT;
			return Send_data(m_parsing_pack);

		//Data 저장 처리
		case CMD_USER_SAVE_REQ:
			m_parsing_pack.body.cmd = CMD_USER_SAVE_RESULT;
			if (!m_Data_mng.Insert_data(m_parsing_pack, m_data_list))
			{
				cout << "[Client " << m_clnt_sock << "] save: " << m_parsing_pack.body.data << endl;
				strncpy(m_parsing_pack.body.data, "[SUCCESS] Data 저장 성공", MAX_DATA_SIZE);
			}
			else
				strncpy(m_parsing_pack.body.data, "[FAIL] 이미 저장된 데이터", MAX_DATA_SIZE);
			if (Send_data(m_parsing_pack) == ERR)
				return ERR;
			else
			{   //client에 list data 전달
				if (m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
					return ERR;
				return 0;
			}

		//Data 삭제 처리
		case CMD_USER_DELETE_REQ:
			m_parsing_pack.body.cmd = CMD_USER_DELETE_RESULT;
			if (!m_Data_mng.Delete_data(m_parsing_pack, m_data_list))
			{
				cout << "[Client " << m_clnt_sock << "] delete: " << m_parsing_pack.body.data << endl;
				strncpy(m_parsing_pack.body.data, "[SUCCESS] Data 삭제 성공", MAX_DATA_SIZE);
			}
			else
				strncpy(m_parsing_pack.body.data, "[FAIL] list에 해당 데이터 없음", MAX_DATA_SIZE);

			if (Send_data(m_parsing_pack) == ERR)
				return ERR;
			else
			{
				//client에 list data 전달
				if (m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
					return ERR;
				return 0;
			}

		//Data list 출력
		case CMD_USER_PRINT_REQ:
			m_parsing_pack.body.cmd = CMD_USER_PRINT_RESULT;
			strcpy(m_parsing_pack.body.data, "CMD_USER_PRINT_RESULT");
			if (Send_data(m_parsing_pack) == ERR)
				return ERR;
			else
			{
				if (m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
				{
					puts("[ERROR] list send() error");
					return ERR;
				}
				return 0;
			}

		default:
			m_parsing_pack.body.cmd = CMD_USER_ERR;
			strcpy(m_parsing_pack.body.data, "Request error");
			puts("[ERROR] request error");
			return Send_data(m_parsing_pack);
	} //switch

	return 0;
}

// Client로 Packet 보내기
int CUser::Send_data(PACKET send_pack)
{
//	cout << "send: " << send_pack.body.cmd << ", " << send_pack.body.data << endl;
 	if(send(m_clnt_sock, (char*) &send_pack, sizeof(PACKET), 0) == -1)
		return ERR;
	return 0;
}

int CUser::Find_packet()
{
	memset(&m_parsing_pack, '\0', sizeof(m_parsing_pack));	//m_parsing_pack 초기화

	for(int i = queue.front; queue.rear != NEXT(i); ++i)
	{
		if(QUEUE_SIZE < i+sizeof(PACKET))
			memcpy(&m_parsing_pack, &queue.data[0], sizeof(PACKET));
		else
			memcpy(&m_parsing_pack, &queue.data[i], sizeof(PACKET));

		//head, tail 확인
		if(!strcmp(m_parsing_pack.phead.head, "AA11") && !strcmp(m_parsing_pack.ptail.tail, "11AA"))
			//data 확인
			if(m_parsing_pack.phead.datasize == strlen(m_parsing_pack.body.data))
			{
				m_Circular_buffer.Dequeue(&queue, i);
				return 0;
			}
	}//for
	return ERR;
}





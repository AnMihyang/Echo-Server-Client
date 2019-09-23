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

CUser::CUser() {
	m_clntSock = 0;
	m_strlen = 0;
	m_clntConnect = false;

	for(int i = 0; i < QUEUE_SIZE; ++i)
		queue.pack[i] = (PACKET *)malloc(sizeof(PACKET));	//queue.pack 초기화

	queue.front = queue.rear = 0;		//queue 초기화
}

CUser::~CUser() {
	for(int i = 0; i < QUEUE_SIZE; ++i)
			free(queue.pack[i]);	//queue.pack 메모리 해제
}

// Client로 부터 Packet 받기
int CUser::RecvData(int fd, int cUserNum, list<string> *dataList) {

	m_clntSock = fd;
	PACKET recvPack;
	while (1)
	{
		//Client로부터 Packet 받기
		m_strlen = recv(fd, (char*) &recvPack, sizeof(PACKET), 0);

		if (m_strlen == 0)
			return ERR;
		else if (m_strlen < 0)
		{
			if (errno == EAGAIN)
				break;
		}
		else
		{
			cout << "[Client " << fd << "] Recv: " << recvPack.cmd << ", " << recvPack.data << endl;
			m_CircularBuf.Enqueue(&queue, recvPack);
			cout << "Enqueue: " << queue.pack[queue.front]->head << ", " <<queue.pack[queue.front]->size
					<< ", " <<queue.pack[queue.front]->cmd << ", " << queue.pack[queue.front]->data << endl;
//			SendData(recvPack);
		} //else end
	}
	return 0;
}

// 큐에 send할 패킷이 있는지 확인
int CUser::QueueCheck()
{
	if(m_CircularBuf.IsEmpty_Queue(&queue))
		return ERR;								//큐 비었으면 return -1
	return 0;
}

// Packet Parsing
int CUser::ParsingData()
{
	PACKET pack;

	if(CheckPacket())
	{
		puts("[ERROR] CheckPacket error");
		m_CircularBuf.Dequeue(&queue);
	}
	else
	{
		memcpy(&pack, queue.pack[queue.front], sizeof(pack));		//받은 packet queue에 복사
		m_CircularBuf.Dequeue(&queue);

		switch (pack.cmd) {
		case CMD_USER_LOGIN_REQ:
			//Client에 패킷 전송
			pack.cmd = CMD_USER_LOGIN_RESULT;
			return SendData(pack);

			//Echo Data 처리
		case CMD_USER_DATA_REQ:
			pack.cmd = CMD_USER_DATA_RESULT;
			return SendData(pack);

			//Data 저장 처리
		case CMD_USER_SAVE_REQ:
			pack.cmd = CMD_USER_SAVE_RESULT;
			strcpy(pack.data, "[SUCCESS] Data 저장 성공");
			return SendData(pack);

			//Data 삭제 처리
		case CMD_USER_DELETE_REQ:
			pack.cmd = CMD_USER_DELETE_RESULT;
			strcpy(pack.data, "[SUCCESS] Data 삭제 성공");
			return SendData(pack);

			//Data list 출력
		case CMD_USER_PRINT_REQ:
			pack.cmd = CMD_USER_PRINT_RESULT;
			strcpy(pack.data, "DATA LIST 출력하기");
			return SendData(pack);

		default:
			pack.cmd = CMD_USER_ERR;
			strcpy(pack.data, "Request ERROR");
			puts("[ERROR] request error");
			return SendData(pack);
		} //switch
	}

	return 0;
}

// Client로 Packet 보내기
int CUser::SendData(PACKET pack)
{
	if(send(m_clntSock, (char*) &pack, sizeof(PACKET), 0) == ERR)
		return ERR;
	return 0;
}

//큐에 저장된 패킷 데이터 정상인지 확인
int CUser::CheckPacket()
{
	if(strcmp(queue.pack[queue.front]->head, "AA11"))	//head 확인
	{
		puts("head");
		return ERR;
	}

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
		puts("size");
		return ERR;
	}
	//TODO: data 확인

	return 0;
}



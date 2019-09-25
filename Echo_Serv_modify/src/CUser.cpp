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
	m_clntSock = 0;
	m_strlen = 0;
	m_clntConnect = false;
	m_dataList = nullptr;

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
	m_dataList = dataList;
	PACKET recvPack;
	while (1)
	{
		//Client로부터 Packet 받기
		m_strlen = recv(fd, (char*) &recvPack, sizeof(PACKET), 0);

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
			cout << "[Client " << fd << "] Recv: " << recvPack.cmd << ", " << recvPack.data << endl;
			if(!CheckPacket(recvPack))
				m_CircularBuf.Enqueue(&queue, recvPack);
			break;
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
		m_CircularBuf.Dequeue(&queue);

		switch (pack.cmd) {
			case CMD_USER_LOGIN_REQ:
				//Client에 패킷 전송
				pack.cmd = CMD_USER_LOGIN_RESULT;
				sprintf(pack.data, "%d", m_clntSock);
				return SendData(pack);

				//Echo Data 처리
			case CMD_USER_DATA_REQ:
				pack.cmd = CMD_USER_DATA_RESULT;
				return SendData(pack);

			//Data 저장 처리
			case CMD_USER_SAVE_REQ:
				pack.cmd = CMD_USER_SAVE_RESULT;
				if(! m_DataMng.InsertData(pack, m_dataList))
					strcpy(pack.data, "[SUCCESS] Data 저장 성공");
				else
					strcpy(pack.data, "[FAIL] 이미 저장된 데이터");

				if(SendData(pack) == ERR)
					return ERR;
				else
				{
					//리스트 프린트
					if(m_DataMng.SendDataList(m_clntSock, m_dataList) == ERR)
						return ERR;
					return 0;
				}

			//Data 삭제 처리
			case CMD_USER_DELETE_REQ:
				pack.cmd = CMD_USER_DELETE_RESULT;
				if(! m_DataMng.DeleteData(pack, m_dataList))
					strcpy(pack.data, "[SUCCESS] Data 삭제 성공");
				else
					strcpy(pack.data, "[FAIL] list에 해당 데이터 없음");

				if(SendData(pack) == ERR)
					return ERR;
				else
				{
					if(m_DataMng.SendDataList(m_clntSock, m_dataList) == ERR)
						return ERR;
					return 0;
				}

			//Data list 출력
			case CMD_USER_PRINT_REQ:
				pack.cmd = CMD_USER_PRINT_RESULT;
				strcpy(pack.data, "CMD_USER_PRINT_RESULT");
				if(SendData(pack) == ERR)
					return ERR;
				else
				{
					if(m_DataMng.SendDataList(m_clntSock, m_dataList) == ERR)
						return ERR;
					return 0;
				}

			default:
				pack.cmd = CMD_USER_ERR;
				strcpy(pack.data, "Request ERROR");
				puts("[ERROR] request error");
				return SendData(pack);
		} //switch
//	}

	return 0;
}

// Client로 Packet 보내기
int CUser::SendData(PACKET pack)
{
	if(send(m_clntSock, (char*) &pack, sizeof(PACKET), 0) == -1)
		return ERR;
	return 0;
}

//큐에 저장된 패킷 데이터 정상인지 확인 (Packet size 초과 시 예외 처리 하기 (데이터 끊어서 받기))
int CUser::CheckPacket(PACKET pack)
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



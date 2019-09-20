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
	// TODO Auto-generated constructor stub
	m_clntSock = 0;
	m_strlen = 0;

	queue.front = queue.rear = 0;		//queue 초기화
}

CUser::~CUser() {
	// TODO Auto-generated destructor stub
}

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
			SendData(recvPack);
		} //else end
	}
	return 0;
}

int CUser::ParsingData()
{
	if(CheckPacket() == ERR)
		return ERR;
}

void CUser::SendData(PACKET pack)
{
	switch (pack.cmd) {
		case CMD_USER_LOGIN_REQ:
			//Client에 패킷 전송
			pack.cmd = CMD_USER_LOGIN_RESULT;
			send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
			break;

		//Echo Data 처리
		case CMD_USER_DATA_REQ:
			pack.cmd = CMD_USER_DATA_RESULT;
			send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
			break;

		//Data 저장 처리
		case CMD_USER_SAVE_REQ:
			pack.cmd = CMD_USER_SAVE_RESULT;
			strcpy(pack.data, "[SUCCESS] Data 저장 성공");
			send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
			break;

		//Data 삭제 처리
		case CMD_USER_DELETE_REQ:
			pack.cmd = CMD_USER_DELETE_RESULT;
			strcpy(pack.data, "[SUCCESS] Data 삭제 성공");
			send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
			break;

		//Data list 출력
		case CMD_USER_PRINT_REQ:
			pack.cmd = CMD_USER_PRINT_RESULT;
			strcpy(pack.data, "DATA LIST 출력하기");
			send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
			break;

		default:
			pack.cmd = CMD_USER_ERR;
			strcpy(pack.data, "Request ERROR");
			send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
			cout << "[Client " << m_clntSock << "] Send: " << pack.cmd << ", " << pack.data << endl;
			puts("[ERROR] request error");
	} //switch
}

//큐에 저장된 패킷 데이터 정상인지 확인
int CUser::CheckPacket()
{
	if(strcmp(queue.pack[queue.front]->head, "AA11"))
		return ERR;

	if(strcmp(queue.pack[queue.front]->tail, "11AA"))
		return ERR;

	if(queue.pack[queue.front]->cmd <= 0 || queue.pack[queue.front]->cmd > 11)
		return ERR;

	if(queue.pack[queue.front]->size <= 0)
		return ERR;

	return 0;
}

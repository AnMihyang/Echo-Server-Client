/*
 * CUser.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "CUser.h"
#include <iostream>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <algorithm>

#define NEXT(index) ((index+1)%QUEUE_SIZE)	//다음 인덱스를 가리키는 매크로 함수
#define ERR -1

using namespace std;

CUser::CUser() {
	//멤버변수 초기화
	m_clntSock = 0;
	m_strlen = 0;
	queue.front=queue.rear=0;			//queue 초기화

	for(int i = 0; i < QUEUE_SIZE; ++i)			//queue.pack 초기화
		queue.pack[i] = (PACKET *)malloc(sizeof(PACKET));
}

CUser::~CUser() {
	for(int i = 0; i < QUEUE_SIZE; ++i)	//메모리 해제
		free(queue.pack[i]);
}

//Client로부터 Packet 받기
int CUser::RecvData(int fd, int cUserNum, list<string> *dataList)
{
	m_clntSock = fd;
	m_dataList = dataList;
	PACKET recvPack;

//	cout << "[NOTICE] " << cUserNum << "번째 CUser 실행" << endl;
	while (1)
	{
		//Client로부터 Packet 받기
		m_strlen = recv(fd, (char*) &recvPack, sizeof(PACKET), 0);

		if (m_strlen == 0)
			return ERR;
		else if (m_strlen < 0) {
			if (errno == EAGAIN)
				break;
		}
		else
		{
			cout << "[Client " << m_clntSock << "] Recv: " << recvPack.cmd << ", " << recvPack.data << endl;
			//test
			/*
			 cout << "[Client " << m_clntSock <<"] ";
			 switch (recvPack.cmd) {
				 case 1:
				 cout << "LOGIN" << endl;
				 break;
				 case 3:
				 cout << "Echo Data, " << recvPack.data << endl;
				 break;
				 case 5:
				 cout << "Save Data, " << recvPack.data << endl;
				 break;
				 case 7:
				 cout << "Delete Data, " << recvPack.data << endl;
				 break;
				 default:
				 cout << recvPack.cmd << endl;
				 cout << "Error" << endl;
			 }*/
			if(recvPack.cmd == CMD_USER_ERR)	return ERR;
			Enqueue(recvPack);
			SendData();
		} //else end
	} //while end
	return 0;
}

//Client에 Packet 보내기
void CUser::SendData()
{
	int result;

	//Client로부터 받은 Packet Command 처리
	switch (queue.pack[queue.front]->cmd) {
	//Login 처리
	case CMD_USER_LOGIN_REQ:
		//Client에 패킷 전송
		queue.pack[queue.front]->cmd = CMD_USER_LOGIN_RESULT;
		send(m_clntSock, (char*) queue.pack[queue.front], sizeof(PACKET), 0);
		Dequeue();
		break;

		//Echo Data 처리
	case CMD_USER_DATA_REQ:
		queue.pack[queue.front]->cmd = CMD_USER_DATA_RESULT;
		send(m_clntSock, (char*) queue.pack[queue.front], sizeof(PACKET), 0);
		Dequeue();
		break;

		//Data 저장 처리
	case CMD_USER_SAVE_REQ:
		queue.pack[queue.front]->cmd = CMD_USER_SAVE_RESULT;
		result = InsertData();
		if(result == ERR)
		{
			puts("[FAIL] Data 중복");
			strcpy(queue.pack[queue.front]->data, "[FAIL] Data 중복");
		}
		else
			strcpy(queue.pack[queue.front]->data, "[SUCCESS] Data 저장 성공");
		send(m_clntSock, (char*) queue.pack[queue.front], sizeof(PACKET), 0);
		SendDataList(CMD_USER_SAVE_RESULT);
		Dequeue();
		break;

		//Data 삭제 처리
	case CMD_USER_DELETE_REQ:
		queue.pack[queue.front]->cmd = CMD_USER_DELETE_RESULT;
		result = DeleteData();
		if(result == ERR)
		{
			puts("[FAIL] 해당 Data 없음");
			strcpy(queue.pack[queue.front]->data, "[FAIL] 입력한 Data 없음");
		}
		else
			strcpy(queue.pack[queue.front]->data, "[SUCCESS] Data 삭제 성공");
		send(m_clntSock, (char*) queue.pack[queue.front], sizeof(PACKET), 0);
		SendDataList(CMD_USER_DELETE_RESULT);
		Dequeue();
		break;

		//Data list 출력
	case CMD_USER_PRINT_REQ:
		queue.pack[queue.front]->cmd = CMD_USER_PRINT_RESULT;
		send(m_clntSock, (char*) queue.pack[queue.front], sizeof(PACKET), 0);
		SendDataList(CMD_USER_PRINT_RESULT);
		Dequeue();
		break;

	default:
		queue.pack[queue.front]->cmd = CMD_USER_ERR;
		send(m_clntSock, (char*) queue.pack[queue.front], sizeof(PACKET), 0);
		puts("[ERROR] request error");
	} //switch
}

//Queue가 비었는지 확인
int CUser::IsEmpty_Queue()
{
	//front와 rear가 같으면 empty
	return queue.front == queue.rear;		//큐가 비었으면 return 1
}

//Queue가 꽉 찬 상태인지 확인
void CUser::IsFull_Queue()
{
	//다음에 저장할 위치(Next(rear))가 꺼낼 위치(front)와 같으면 Full
	if(NEXT(queue.rear) == queue.front)
	{
		cout << "[NOTICE] Circular buffer full" << endl;
	}
}

//Queue에 Packet 삽입
void CUser::Enqueue(PACKET recvPack)
{
	memcpy(queue.pack[queue.rear], &recvPack, sizeof(recvPack));		//받은 packet queue에 복사

	IsFull_Queue();
	queue.rear = NEXT(queue.rear);
}

//Queue에서 Packet 삭제
int CUser::Dequeue()
{
	if(IsEmpty_Queue())
		puts("[ERROR] Empty Queue");

	queue.pack[queue.front] = (PACKET *)malloc(sizeof(PACKET));

	queue.front = NEXT(queue.front); 		//front 다음 위치로 이동

	if(!IsEmpty_Queue())
		PrintQueueData();

	return 0;
}

//Queue에 저장된 Packet 출력
void CUser::PrintQueueData()
{
	cout << endl;
	cout << "--Circular Buffer--" << endl;
	for(int i = queue.front; i < QUEUE_SIZE; i++)
	{
		if(strcmp(queue.pack[i]->data, "\0"))
			cout << i << ") " << queue.pack[i]->data << endl;
		else
			break;
	}

	for (int i = 0; i < queue.front; i++)
	{
		if(strcmp(queue.pack[i]->data, "\0"))
			cout << i << ") " << queue.pack[i]->data << endl;
	}
	cout << endl;
}

//list에 Data 삽입
int CUser::InsertData()
{
	list<string>::iterator it;

	it = find(m_dataList->begin(), m_dataList->end(), queue.pack[queue.front]->data);

	if(it != m_dataList->end())		//데이터 중복
		return -1;

	m_dataList->push_back(queue.pack[queue.front]->data);		//list에 입력받은 데이터 삽입
	PrintDataList();
	return 0;
}

//list에서 Data 삭제
int CUser::DeleteData()
{
	list<string>::iterator it;

	it = find(m_dataList->begin(), m_dataList->end(), queue.pack[queue.front]->data);

	if(it == m_dataList->end())		//삭제할 데이터가 list에 존재하지 않을 때
		return -1;

	m_dataList->remove(queue.pack[queue.front]->data);		//list에서 입력받은 데이터 삭제
	PrintDataList();
	return 0;
}

//list에 저장된 Data 출력
void CUser::PrintDataList()
{
	cout << "[Data list] ";
	for(list<string>::iterator it = m_dataList->begin(); it != m_dataList->end(); it++)
		cout << *it << ' ';
	cout << endl;
}

//Client에 list에 저장된 Data 보내기
void CUser::SendDataList(int cmd)
{
	PACKET pack;
	unsigned int cnt = 1;
	string sendMsg;
	pack.cmd = cmd;
	if(m_dataList->empty())
	{
		pack.cmd = CMD_USER_ERR;
		strcpy(pack.data, "       !!Empty!!");
		send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
	}
	for (list<string>::iterator it = m_dataList->begin(); it != m_dataList->end(); cnt++, it++)
	{
		if (cnt >= m_dataList->size())
			pack.cmd = CMD_USER_ERR;
		sendMsg = to_string(cnt) + ") " + *it;
		strcpy(pack.data, sendMsg.c_str());
		send(m_clntSock, (char*) &pack, sizeof(PACKET), 0);
	}
}

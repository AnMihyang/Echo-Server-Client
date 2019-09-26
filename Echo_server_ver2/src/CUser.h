/*
 * CUser.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef CUSER_H_
#define CUSER_H_

#include "PacketDefine.h"
#include <list>
#include <string>

using namespace std;

#define QUEUE_SIZE 20

typedef struct Queue_Info {
	PACKET *pack[50];
	int front;			// 데이터를 꺼낼 위치를 가리키는 인덱스
	int rear;			// 데이터를 저장할 위치를 가리키는 인덱스
} Queue;

class CUser {
private:
	int m_clntSock;
	int m_strlen;
	Queue queue;
	list<string> * m_dataList;
public:
	CUser();
	virtual ~CUser();
	int RecvData(int fd, int cUserNum, list<string> *dataList);		//Packet Receive
	void SendData();								//Client 에서 받은 Packet 처리
	int IsEmpty_Queue();							//Circular Buffer 비었는지 확인
	void IsFull_Queue();							//Circular Buffer 꽉 찼는지 확인
	void Enqueue(PACKET pack);					//Circular Buffer에 Packet 삽입
	int Dequeue();								//Circular Buffer에서 Packet 삭제
	void PrintQueueData();						//Circular Buffer 출력
	int InsertData();								//Client에서 받은 데이터 list에 삽입
	int DeleteData();								//Client에서 받은 데이터 list에서 삭제
	void PrintDataList();						//list 출력
	void SendDataList(int cmd);					//client에 list 전달
};
#endif /* CUSER_H_ */

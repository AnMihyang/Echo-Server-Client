/*
 * CircularBuffer.h
 *
 *  Created on: 2019. 9. 2.
 *      Author: mihyang
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <iostream>
#include <string>

#include "PacketDefine.h"
#define QUEUE_SIZE 5

using namespace std;

typedef struct Queue
{
	string data[QUEUE_SIZE];
	int front;			// 데이터를 꺼낼 위치를 가리키는 인덱스 (가장 오래된 데이터가 있는 인덱스)
	int rear;			// 데이터를 저장할 위치를 가리키는 인덱스
}Queue;

class CircularBuffer {
protected:
	Queue queue;
public:
	CircularBuffer();
	virtual ~CircularBuffer();
	void IsFull();					// queue가 꽉 찼는지 확인
	int IsEmpty();					// queue가 비었는지 확인
	void Enqueue(PACKET pack);		// queue에 데이터 저장
//	int Dequeue();
	void PrintQueueData();			// queue에 있는 모든 데이터 출력
};

#endif /* CIRCULARBUFFER_H_ */

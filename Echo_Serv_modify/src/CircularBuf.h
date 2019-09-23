/*
 * CircularBuf.h
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#ifndef CIRCULARBUF_H_
#define CIRCULARBUF_H_

#include "PacketDefine.h"

#define QUEUE_SIZE 50

typedef struct Queue_Info {
	PACKET *pack[QUEUE_SIZE];
	int front;			// 데이터를 꺼낼 위치를 가리키는 인덱스
	int rear;			// 데이터를 저장할 위치를 가리키는 인덱스
} Queue;

class CircularBuf {
public:
	CircularBuf();
	virtual ~CircularBuf();
	int IsEmpty_Queue(Queue *queue);
	bool IsFull_Queue(Queue *queue);
	int Enqueue(Queue *queue, PACKET recvPack);
	int Dequeue(Queue *queue);
};

#endif /* CIRCULARBUF_H_ */

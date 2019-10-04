/*
 * CircularBuf.h
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "Packet_define.h"

#define QUEUE_SIZE 10000
#define HEAD_SIZE sizeof(HEADER)
#define TAIL_SIZE sizeof(TAIL)
#define NEXT(index) (index%QUEUE_SIZE)
//#define NEXT(index, len) ((index+len)%QUEUE_SIZE)	//다음 인덱스를 가리키는 매크로 함수

typedef struct Queue_Info {
//	PACKET *pack[QUEUE_SIZE];
	char data[QUEUE_SIZE];
	int front;			// 데이터를 꺼낼 위치를 가리키는 인덱스
	int rear;			// 데이터를 저장할 위치를 가리키는 인덱스
} Queue;

class Circular_buffer {
public:
	Circular_buffer();
	virtual ~Circular_buffer();
	int Isempty_queue(Queue *queue);
	bool Isfull_queue(Queue *queue, int recv_size);
	int Enqueue(Queue *queue, PACKET recvPack);
	int Dequeue(Queue *queue);
};

#endif /* CIRCULAR_BUFFER_H_ */

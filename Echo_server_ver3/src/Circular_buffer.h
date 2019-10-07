/*
 * CircularBuf.h
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "Packet_define.h"

#define QUEUE_SIZE 10400
#define NEXT(index) (index%QUEUE_SIZE)			//다음 인덱스를 가리키는 매크로 함수

typedef struct Queue_Info {
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
	int Dequeue(Queue *queue, int deq_point);
};

#endif /* CIRCULAR_BUFFER_H_ */

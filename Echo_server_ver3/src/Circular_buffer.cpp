/*
 * CircularBuf.cpp
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#include "Circular_buffer.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

Circular_buffer::Circular_buffer() {
}

Circular_buffer::~Circular_buffer() {
}

int Circular_buffer::Enqueue(Queue *queue, PACKET recv_pack)
{
	if (Isfull_queue(queue, sizeof(PACKET)))
	{
		puts("[FAIL] CircularBuffer full");
		return -1;
	}
	if(QUEUE_SIZE < queue->rear + sizeof(PACKET))
	{
		memcpy(&queue->data[0], &recv_pack, sizeof(PACKET));
		queue->rear = NEXT(sizeof(PACKET));
	}
	else
	{
		memcpy(&queue->data[queue->rear], &recv_pack, sizeof(PACKET));
		queue->rear = NEXT((queue->rear+sizeof(PACKET)));
	}

	return 0;
}

//TODO:Dequeue 수정하기
int Circular_buffer::Dequeue(Queue *queue, int deq_point)
{
	if(Isempty_queue(queue))
		puts("[ERROR] Empty Queue");

	if(QUEUE_SIZE < deq_point+sizeof(PACKET))
		queue->front = 0;

	else
		queue->front = NEXT((deq_point+sizeof(PACKET)));

	return 0;
}

//Queue가 비었는지 확인
int Circular_buffer::Isempty_queue(Queue *queue)
{
	//front와 rear가 같으면 empty
	return queue->front == queue->rear;		//큐가 비었으면 return 1
}

//Queue가 꽉 찬 상태인지 확인
bool Circular_buffer::Isfull_queue(Queue *queue, int recv_size)
{
	if (queue->front < queue->rear)
	{
		if (QUEUE_SIZE - queue->rear + queue->front < recv_size)
			return 1;
	}
	else if (queue-> front > queue->rear)
		if (queue->front - queue->rear < recv_size)
			return 1;
	return 0;
}

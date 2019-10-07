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



//#define NEXT(index) ((index+1)%QUEUE_SIZE)	//다음 인덱스를 가리키는 매크로 함수


using namespace std;

Circular_buffer::Circular_buffer() {
}

Circular_buffer::~Circular_buffer() {
}

int Circular_buffer::Enqueue(Queue *queue, PACKET recv_pack)
{
//	char *ptr;
//	int body_size = strlen(recv_pack.body.data)+2;
//	int nrear = HEAD_SIZE + body_size + TAIL_SIZE;;

	if (Isfull_queue(queue, sizeof(PACKET)))
	{
		//		queue->front = NEXT(queue->front);
		puts("[FAIL] CircularBuffer full");
		return 0;
	}
	if(QUEUE_SIZE < queue->rear + sizeof(PACKET))
	{
		memcpy(&queue->data[0], &recv_pack, sizeof(PACKET));
//		memcpy(&queue->data[queue->rear], &recv_pack, QUEUE_SIZE-queue->rear);
//		memcpy(&queue->data[0], &recv_pack+(QUEUE_SIZE-queue->rear), sizeof(PACKET)-(QUEUE_SIZE-queue->rear));
		queue->rear = NEXT(sizeof(PACKET));
	}
	else
	{
		memcpy(&queue->data[queue->rear], &recv_pack, sizeof(PACKET));
		queue->rear = NEXT((queue->rear+sizeof(PACKET)));
	}

	/*memcpy(&queue->data[queue->rear], &recv_pack.head, HEAD_SIZE);
	memcpy(&queue->data[queue->rear+HEAD_SIZE], &recv_pack.body, body_size);
	memcpy(&queue->data[queue->rear+HEAD_SIZE+body_size], &recv_pack.tail, TAIL_SIZE);*/

//	memcpy(&queue->data[queue->rear], &recv_pack, sizeof(recv_pack));

//	queue->rear = NEXT(queue->rear+nrear);
//	queue->rear = NEXT((queue->rear+sizeof(PACKET)));
	cout << "front: " << queue->front << ", rear: " << queue->rear << endl;
	return 0;
}

//TODO:Dequeue 수정하기
int Circular_buffer::Dequeue(Queue *queue)
{
//	cout << "dequeue: " << dequeue_size << endl;
	if(Isempty_queue(queue))
		puts("[ERROR] Empty Queue");

	if(QUEUE_SIZE <= queue->front+sizeof(PACKET))
		queue->front = 0;

	else
		queue->front = NEXT(queue->front+sizeof(PACKET));

//	queue->pack[queue->front] = (PACKET *)malloc(sizeof(PACKET));
//	if(!IsEmpty_Queue(queue))
//		PrintQueueData();

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
/*	//다음에 저장할 위치(Next(rear))가 꺼낼 위치(front)와 같으면 Full
	if(NEXT(queue->rear) == queue->front)
	{
		cout << "[NOTICE] Circular buffer full" << endl;
		return true;
	}
	else
		return false;*/
}

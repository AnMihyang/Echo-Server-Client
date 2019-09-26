/*
 * CircularBuf.cpp
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#include "Circularbuf.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>

#define NEXT(index) ((index+1)%QUEUE_SIZE)	//다음 인덱스를 가리키는 매크로 함수

using namespace std;

Circularbuf::Circularbuf() {
}

Circularbuf::~Circularbuf() {
}

int Circularbuf::Enqueue(Queue *queue, PACKET recvPack)
{
	memcpy(queue->pack[queue->rear], &recvPack, sizeof(recvPack));
	if (Isfull_queue(queue))
		queue->front = NEXT(queue->front);
	queue->rear = NEXT(queue->rear);
	return 0;
}

int Circularbuf::Dequeue(Queue *queue)
{
	if(Isempty_queue(queue))
		puts("[ERROR] Empty Queue");

	queue->pack[queue->front] = (PACKET *)malloc(sizeof(PACKET));

	queue->front = NEXT(queue->front);

//	if(!IsEmpty_Queue(queue))
//		PrintQueueData();

	return 0;
}

//Queue가 비었는지 확인
int Circularbuf::Isempty_queue(Queue *queue)
{
	//front와 rear가 같으면 empty
	return queue->front == queue->rear;		//큐가 비었으면 return 1
}

//Queue가 꽉 찬 상태인지 확인
bool Circularbuf::Isfull_queue(Queue *queue)
{
	//다음에 저장할 위치(Next(rear))가 꺼낼 위치(front)와 같으면 Full
	if(NEXT(queue->rear) == queue->front)
	{
		cout << "[NOTICE] Circular buffer full" << endl;
		return true;
	}
	else
		return false;
}

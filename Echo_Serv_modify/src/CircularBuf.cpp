/*
 * CircularBuf.cpp
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#include "CircularBuf.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>

#define NEXT(index) ((index+1)%QUEUE_SIZE)	//다음 인덱스를 가리키는 매크로 함수

using namespace std;

CircularBuf::CircularBuf() {
}

CircularBuf::~CircularBuf() {
}

int CircularBuf::Enqueue(Queue *queue, PACKET recvPack)
{
	memcpy(queue->pack[queue->rear], &recvPack, sizeof(recvPack));
	if (IsFull_Queue(queue))
		queue->front = NEXT(queue->front);
	queue->rear = NEXT(queue->rear);
	return 0;
}

int CircularBuf::Dequeue(Queue *queue)
{
	if(IsEmpty_Queue(queue))
		puts("[ERROR] Empty Queue");

	queue->pack[queue->front] = (PACKET *)malloc(sizeof(PACKET));

	queue->front = NEXT(queue->front);

//	if(!IsEmpty_Queue(queue))
//		PrintQueueData();

	return 0;
}

//Queue가 비었는지 확인
int CircularBuf::IsEmpty_Queue(Queue *queue)
{
	//front와 rear가 같으면 empty
	return queue->front == queue->rear;		//큐가 비었으면 return 1
}

//Queue가 꽉 찬 상태인지 확인
bool CircularBuf::IsFull_Queue(Queue *queue)
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

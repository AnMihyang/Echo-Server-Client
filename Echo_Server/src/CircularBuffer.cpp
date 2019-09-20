/*
 * CircularBuffer.cpp
 *
 *  Created on: 2019. 9. 2.
 *      Author: mihyang
 */

#include "CircularBuffer.h"
#include <iostream>
#include <string.h>

#define NEXT(index) ((index+1)%QUEUE_SIZE)	//다음 인덱스를 가리키는 매크로 함수

using namespace std;

CircularBuffer::CircularBuffer() {
	// TODO Auto-generated constructor stub
	queue.front=queue.rear=0;				//queue 초기화
}

CircularBuffer::~CircularBuffer() {
	// TODO Auto-generated destructor stub
}

int CircularBuffer::IsEmpty()
{
	return queue.front == queue.rear;	//front와 rear가 같으면 빈 상태
}


void CircularBuffer::IsFull()
{
	//다음에 저장할 위치(Next rear)가 꺼낼 위치(front)와 같으면 Full Buffer
	if (NEXT(queue.rear)== queue.front)
	{
		cout << " -----Full Buffer-----" << endl;
		queue.front = NEXT(queue.front);			//front를 다음 위치로 변경
	}
}

void CircularBuffer::Enqueue(PACKET pack)
{
	queue.data[queue.rear] = pack.data;		// queue에 데이터 저장
	PrintQueueData();								// queue에 저장된 모든 데이터 출력
	IsFull();										// queue가 꽉 찼는지 확인
	cout << endl;
	queue.rear = NEXT(queue.rear);				//rear 다음 위치로 변경
}

void CircularBuffer::PrintQueueData()
{
	cout << " ---Circular Buffer---" << endl;
	//queue에 있는 데이터 출력 (오래된 데이터 순)
	for(int i = queue.front; i < QUEUE_SIZE; i++)
	{
		if(!queue.data[i].empty())
			cout << " " << i << ") " << queue.data[i] << endl;
//		else
//			break;
	}
	for(int i = 0; i < queue.front; i++)
	{
		if(!queue.data[i].empty())
				cout << " " << i << ") " << queue.data[i] << endl;
	}
}

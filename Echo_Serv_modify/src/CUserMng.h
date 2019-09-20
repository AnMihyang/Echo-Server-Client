/*
 * CUserMng.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef CUSERMNG_H_
#define CUSERMNG_H_

#include "CEpollMng.h"
#include "CUser.h"
#include "PacketDefine.h"

#define MAX_CLIENT 100

class CUserMng: public CEpollMng {
private:
	int m_clntSock;
	socklen_t m_adr_sz;
	struct epoll_event *m_events;
	int m_clntSocks[MAX_CLIENT] = { 0, };
	CUser m_CUser[MAX_CLIENT];
	PACKET m_pack;

	list<string> dataList;
public:
	CUserMng();
	virtual ~CUserMng();
	void Server_Handling();
	int Connect_Client();				// Client 연결
	void Close_Client(int *fd);			// Client 종료
	static void * WorkerThread(void *arg);				//Packet send/parsing 스레드
	static void * UserCheckThread(void *arg);		//클라이언트 연결 상태 확인 스레드

	int RecvData(int fd, int cUserNum, list<string> *dataList);
	void SendData(PACKET pack);
};


#endif /* CUSERMNG_H_ */

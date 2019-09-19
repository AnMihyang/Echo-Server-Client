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
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <list>
#include <string>

#define MAX_CLIENT 100

using namespace std;

class CUserMng : public CEpollMng {
private:
	int m_serv_sock, m_clnt_sock;
	struct sockaddr_in m_serv_adr ,m_clnt_adr;
	struct epoll_event *m_events;
	PACKET pack;
	CUser m_CUser[MAX_CLIENT];
	int clntSock[MAX_CLIENT] = {0};		//clntSock 초기화

	int m_clnt;
	pthread_t tID;
	pthread_mutex_t m_tLock;

	list<string> dataList;
	int cnt = 0;
public:
	CUserMng();
	virtual ~CUserMng();
	void ServerHandling();
	void SetSocket();									//소켓 설정
	static void * Thread_Handling(void *arg);		//스레드 실행 함수
};

#endif /* CUSERMNG_H_ */

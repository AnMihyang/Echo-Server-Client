/*
 * CUserMng.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef CUSER_MNG_H_
#define CUSER_MNG_H_

#include "CUser.h"
#include <pthread.h>

#include "CEpoll_mng.h"
#include "Packet_define.h"

#define MAX_CLIENT 100

class CUser_mng: public CEpoll_mng {
private:
	int m_clnt_sock;
	socklen_t m_adr_sz;
	int m_clnt_socks[MAX_CLIENT] = { 0, };
	CUser m_CUser[MAX_CLIENT];
	PACKET m_pack;

	list<string> m_dataList;

	pthread_t m_User_check_tID;
	pthread_t m_Worker_tID;
	pthread_mutex_t m_clntlock;

public:
	CUser_mng();
	virtual ~CUser_mng();
	void Server_handling();
	int Connect_client();				// Client 연결
	void Close_client(int *fd);			// Client 종료
	static void * Worker_thread(void *arg);				//Packet send/parsing 스레드
	static void * User_check_thread(void *arg);			//클라이언트 연결 상태 확인 스레드
};


#endif /* CUSER_MNG_H_ */

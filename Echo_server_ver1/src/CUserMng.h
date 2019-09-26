/*
 * CUserMng.h
 *
 *  Created on: 2019. 8. 20.
 *      Author: mihyang
 */

#ifndef CUSERMNG_H_
#define CUSERMNG_H_

#include "CEpollMng.h"
#include "DataMng.h"
#include "CircularBuffer.h"
#include <pthread.h>
#include "PacketDefine.h"

#define MAX_CLIENT 1000

class DataMng;
class CircularBuffer;
class CUserMng : public DataMng, public CircularBuffer{
private:
	int m_serv_sock, m_clnt_sock;
	int m_i, m_event_cnt = 0;
	int m_clnt_num = -1;
	struct sockaddr_in m_serv_adr, m_clnt_adr;
	struct epoll_event *m_ep_events = nullptr;
	socklen_t m_adr_sz;

	pthread_t tID[MAX_CLIENT];
	pthread_mutex_t m_clntLock;
	pthread_mutex_t m_dataLock;
	pthread_mutex_t m_queueLock;
public:
	class CEpollMng *m_CEpollMng;
public:
	CUserMng();
	virtual ~CUserMng();
	void Handling();
	void SetSocket();
	static void* Thread_Handling(void *arg);
	int SendDataList(int for_index, PACKET pack);		//리스트에 있는 데이터 클라이언트에 send
};

#endif /* CUSERMNG_H_ */

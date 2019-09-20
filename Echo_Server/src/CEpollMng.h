/*
 * CEpollMng.h
 *
 *  Created on: 2019. 8. 12.
 *      Author: mihyang
 */
//#include <sys/epoll.h>


#ifndef CEPOLLMNG_H_
#define CEPOLLMNG_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>


class CUserMng;
class CEpollMng {
private:
	struct epoll_event *m_ep_events = nullptr;
	struct epoll_event m_event;
	int m_epfd = 0, m_event_cnt = 0;
	int *m_serv_sock = 0;
	socklen_t m_adr_sz;

public:
	CUserMng *m_CUserMng;
public:
	CEpollMng();
	virtual ~CEpollMng();
	struct epoll_event *Epoll_Create(int *serv_sock);	// epoll 인스턴스 생성 ~ FD 등록
	int Epoll_Wait();
	void Epoll_Close();
	void Connect_Client(int *clnt_sock);					// Client 연결
	void Close_Client(int *fd);								// Client 종료
	void SetNonblocking(int *fd);							// Non-Blocking Setting
};

#endif /* CEPOLLMNG_H_ */

/*
 * CEpollMng.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef CEPOLLMNG_H_
#define CEPOLLMNG_H_

#include <sys/socket.h>
#include <sys/epoll.h>

#define ERR -1

class CEpollMng {
private:
	struct epoll_event *m_events;
	struct epoll_event m_event;
	int m_epfd;
	int *m_serv_sock;
	socklen_t m_adr_sz;
public:
	CEpollMng();
	virtual ~CEpollMng();
	struct epoll_event* EpollCreate(int *serv_sock);		// epoll 인스턴스 생성 ~ FD 등록
	int EpollWait();
	void EpollClose();
	int Connect_Client(int *clnt_sock);				// Client 연결
	void Close_Client(int *fd);							// Client 종료
	void SetNonblocking(int *fd);						// Non-Blocking Setting
};
#endif /* CEPOLLMNG_H_ */

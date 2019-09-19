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
#include <netinet/in.h>

#define PORT 9190
#define ERR -1
#define EPOLL_SIZE 20

class CEpollMng {
protected:
	int m_serv_sock;
	struct epoll_event m_event;
	int m_epfd;
private:
	struct sockaddr_in m_serv_adr;
	struct epoll_event *m_events;
public:
	CEpollMng();
	virtual ~CEpollMng();
	void SetSocket();										//소켓 설정
	struct epoll_event* EpollCreate();	// epoll 인스턴스 생성 ~ FD 등록
	int EpollWait();
	void EpollClose();
	void SetNonblocking(int *fd);						// Non-Blocking Setting
};

#endif /* CEPOLLMNG_H_ */

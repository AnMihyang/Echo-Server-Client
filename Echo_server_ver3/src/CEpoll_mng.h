/*
 * CEpollMng.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef CEPOLL_MNG_H_
#define CEPOLL_MNG_H_

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#define PORT 9190
#define ERR -1
#define EPOLL_SIZE 20

class CEpoll_mng {
protected:
	int m_serv_sock;
	struct epoll_event *m_events;
	struct epoll_event m_event;
	int m_epfd;
private:
	struct sockaddr_in m_serv_adr;

public:
	CEpoll_mng();
	virtual ~CEpoll_mng();
	void Set_socket();										//소켓 설정
	struct epoll_event* Epoll_create();	// epoll 인스턴스 생성 ~ FD 등록
	int Epoll_wait();
	void Epoll_close();
	void Set_nonblocking(int *fd);						// Non-Blocking Setting
};

#endif /* CEPOLL_MNG_H_ */

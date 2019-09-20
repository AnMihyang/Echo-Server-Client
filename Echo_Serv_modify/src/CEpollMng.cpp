/*
 * CEpollMng.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "CEpollMng.h"
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

CEpollMng::CEpollMng() {
	// TODO Auto-generated constructor stub
	m_serv_sock = 0;
	m_epfd = 0;
	m_events = nullptr;
}

CEpollMng::~CEpollMng() {
	// TODO Auto-generated destructor stub
	free(m_events);
}

//소켓 설정
void CEpollMng::SetSocket()
{
	m_serv_sock = socket(PF_INET, SOCK_STREAM, 0);		//socket 생성
	memset(&m_serv_adr, 0, sizeof(m_serv_adr));			//serv_adr 초기화
	m_serv_adr.sin_family = AF_INET;
	m_serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY:사용 가능한 랜카드 IP 사용
	m_serv_adr.sin_port = htons(PORT);					// 포트 할당

	//주소 할당
	if (bind(m_serv_sock, (struct sockaddr*) &m_serv_adr, sizeof(m_serv_adr)) == ERR)
		puts("[ERRPR] bind Error");

	//연결 가능 상태 만들기
	if (listen(m_serv_sock, 5) == ERR)
		puts("[ERROR] listen Error");
}

struct epoll_event *CEpollMng::EpollCreate()
{
	m_epfd = epoll_create1(0);		//epoll_create와 같은 방식으로 작동

	if(m_epfd == ERR)
		puts("[ERROR] epoll_create Error");

	//events 구조체 생성
	m_events = (struct epoll_event *)malloc(sizeof EPOLL_SIZE);

	SetNonblocking(&m_serv_sock);
	m_event.events = EPOLLIN;		//event 유형 설정: 수신할 데이터가 존재하는 상황
	m_event.data.fd = m_serv_sock;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_serv_sock, &m_event);

	return m_events;
}

int CEpollMng::EpollWait()
{
	int eventCnt;
	eventCnt = epoll_wait(m_epfd, m_events, EPOLL_SIZE, -1);

	if(eventCnt == ERR)
		return ERR;
	else
		return eventCnt;
}

void CEpollMng::EpollClose()
{
	close(m_epfd);
}

void CEpollMng::SetNonblocking(int *fd)
{
	int flag=fcntl(*fd, F_GETFL, 0);
	fcntl(*fd, F_SETFL, flag|O_NONBLOCK);
}

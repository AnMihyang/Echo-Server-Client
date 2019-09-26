/*
 * CEpollMng.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "CEpollMng.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#define EPOLL_SIZE 20

using namespace std;

CEpollMng::CEpollMng() {
	//멤버변수 초기화
	m_events = nullptr;
	m_epfd = 0;
	m_serv_sock = nullptr;
	m_adr_sz = 0;
}

CEpollMng::~CEpollMng() {
	free(m_events); 		//메모리 해제
}

struct epoll_event *CEpollMng::EpollCreate(int *serv_sock)
{
	m_serv_sock = serv_sock;
	m_epfd = epoll_create1(0);		//epoll_create와 같은 방식으로 작동

	if(m_epfd == ERR)
		puts("[ERROR] epoll_create Error");

	//events 구조체 생성
	m_events = (struct epoll_event *)malloc(sizeof EPOLL_SIZE);

	SetNonblocking(m_serv_sock);
	m_event.events = EPOLLIN;		//event 유형 설정: 수신할 데이터가 존재하는 상황
	m_event.data.fd = *m_serv_sock;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, *m_serv_sock, &m_event);

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

int CEpollMng::Connect_Client(int *clnt_sock)
{
	m_adr_sz = sizeof(clnt_sock);
	*clnt_sock = accept(*m_serv_sock, (struct sockaddr*) clnt_sock, &m_adr_sz);

	SetNonblocking(clnt_sock);

	//event 유형 설정: 수신할 버퍼가 존재할 때 | Edge Trigger Setting
	m_event.events = EPOLLIN | EPOLLET;
	m_event.data.fd = *clnt_sock;

	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, *clnt_sock, &m_event) == ERR)
		return ERR;

	cout << endl;
	cout << "[Connect] client: " << *clnt_sock << endl;
	return 0;
}

void CEpollMng::Close_Client(int *fd)
{
	epoll_ctl(m_epfd, EPOLL_CTL_DEL, *fd, NULL);
	close(*fd);
	cout << "[Cloese] client: " << *fd << endl;
}

void CEpollMng::SetNonblocking(int *fd)
{
	int flag=fcntl(*fd, F_GETFL, 0);
	fcntl(*fd, F_SETFL, flag|O_NONBLOCK);
}

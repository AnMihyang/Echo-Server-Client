/*
 * CEpollMng.cpp
 *
 *  Created on: 2019. 8. 12.
 *      Author: mihyang
 */

#include "CEpollMng.h"
#include "CUserMng.h"
#include <iostream>
#include "PacketDefine.h"


using namespace std;

#define EPOLL_SIZE 100
#define ERR -1

//생성자
CEpollMng::CEpollMng() {
	// TODO Auto-generated constructor stub
}

//소멸자
CEpollMng::~CEpollMng() {
	// TODO Auto-generated destructor stub
}

struct epoll_event *CEpollMng::Epoll_Create(int *serv_sock)
{
	m_serv_sock = serv_sock;
	m_epfd = epoll_create(EPOLL_SIZE);
	if(m_epfd == -1)
		puts("epoll create error()");

	//events 구조체 생성
	m_ep_events = (struct epoll_event *)malloc(sizeof EPOLL_SIZE);

	SetNonblocking(m_serv_sock); 		//Non-Blocking Setting

	m_event.events = EPOLLIN;
	m_event.data.fd = *m_serv_sock;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, *m_serv_sock, &m_event);

	return m_ep_events;
}

int CEpollMng::Epoll_Wait()
{
	m_event_cnt=epoll_wait(m_epfd, m_ep_events, EPOLL_SIZE, -1);
	if(m_event_cnt==-1)
		return -1;
	else
		return m_event_cnt;
}

void CEpollMng::Connect_Client(int *clnt_sock)
{

	m_adr_sz=sizeof(clnt_sock);
	*clnt_sock=accept(*m_serv_sock, (struct sockaddr*)clnt_sock, &m_adr_sz);

	SetNonblocking(clnt_sock);

	m_event.events = EPOLLIN|EPOLLET;		//Edge Trigger Setting
	m_event.data.fd= *clnt_sock;

	epoll_ctl(m_epfd, EPOLL_CTL_ADD, *clnt_sock, &m_event);
	cout << endl;
	cout << "connected client: " << *clnt_sock << endl;
}

void CEpollMng::Close_Client(int *fd)
{
	epoll_ctl(m_epfd, EPOLL_CTL_DEL, *fd, NULL);
	close(*fd);
	cout << "closed client: " << *fd << endl;
}

void CEpollMng::Epoll_Close()
{
	close(m_epfd);
}

void CEpollMng::SetNonblocking(int *fd)
{
	int flag=fcntl(*fd, F_GETFL, 0);
	fcntl(*fd, F_SETFL, flag|O_NONBLOCK);
}

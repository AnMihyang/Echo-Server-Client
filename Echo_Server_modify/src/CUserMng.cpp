/*
 * CUserMng.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "CUserMng.h"
#include <iostream>
#include <unistd.h>

using namespace std;

CUserMng::CUserMng() {
	// TODO Auto-generated constructor stub

}

CUserMng::~CUserMng() {
	// TODO Auto-generated destructor stub
}

int CUserMng::Connect_Client()
{
	m_adr_sz = sizeof(&m_clnt_sock);
	m_clnt_sock = accept(m_serv_sock, (struct sockaddr*) &m_clnt_sock, &m_adr_sz);

	SetNonblocking(&m_clnt_sock);

	//event 유형 설정: 수신할 버퍼가 존재할 때 | Edge Trigger Setting
	m_event.events = EPOLLIN | EPOLLET;
	m_event.data.fd = m_clnt_sock;

	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_clnt_sock, &m_event) == ERR)
		return ERR;

	cout << endl;
	cout << "[Connect] client: " << m_clnt_sock << endl;
	return 0;
}

void CUserMng::Close_Client(int *fd)
{
	epoll_ctl(m_epfd, EPOLL_CTL_DEL, *fd, NULL);
	close(*fd);
	cout << "[Cloese] client: " << *fd << endl;
}

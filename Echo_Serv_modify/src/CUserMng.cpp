/*
 * CUserMng.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "CUserMng.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

using namespace std;

CUserMng::CUserMng() {
	// TODO Auto-generated constructor stub

}

CUserMng::~CUserMng() {
	// TODO Auto-generated destructor stub
}

void CUserMng::Server_Handling()
{
	int event_cnt= 0;

	SetSocket();

	m_events = EpollCreate();

	while (1) {
		event_cnt = EpollWait();

		if (event_cnt == ERR) {
			puts("[ERROR] epoll_wait Error");
			break;
		}

		for (int i = 0; i < event_cnt; i++)
		{
//			m_clnt = m_events[i].data.fd;

			if (m_events[i].data.fd == m_serv_sock)
			{
				Connect_Client();

				for(int j = 0; j < MAX_CLIENT; j++)
				{
					//clntSock배열에 client socket 부여하기
					if(m_clntSock[j] == 0) {
						m_clntSock[j] = m_clnt_sock;
						break;
					}
					if(j == MAX_CLIENT-1) {	//접속자 수 처리
						m_pack.cmd = CMD_USER_ERR;
						strcpy(m_pack.data, "[ERROR] 접속자 수 초과\n");
						send(m_clnt_sock, (char*) &m_pack, sizeof(PACKET), 0);
						puts("[ERROR] 접속자 수 초과");
						Close_Client(&m_clnt_sock);
					}
				}
			}
			else
			{

				for (int j = 0; j < MAX_CLIENT; j++)
					if (m_clntSock[j] == m_events[i].data.fd)
//						if (m_CUser[j].RecvData(m_events[i].data.fd, j, &dataList) == ERR)
						{
							Close_Client(&m_events[i].data.fd);
							m_clntSock[j] = 0;
						}
				/*
				if(pthread_create(&tID, NULL, &CUserMng::Thread_Handling, (void*) this) != 0)
					puts("[ERROR] pthread_create Error");
				pthread_detach(tID);*/
			}//else
		}//for
	}//while
	close(m_serv_sock);
	EpollClose();
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

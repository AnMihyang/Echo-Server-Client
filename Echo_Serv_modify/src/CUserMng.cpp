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
	m_clntSock = 0;
	m_adr_sz = 0;
	m_events = nullptr;
}

CUserMng::~CUserMng() {
	// TODO Auto-generated destructor stub
}

void CUserMng::Server_Handling()
{
	int event_cnt= 0;

	SetSocket();			//소켓 설정

	m_events = EpollCreate();

	while (1)
	{
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
					if(m_clntSocks[j] == 0) {
						m_clntSocks[j] = m_clntSock;
						break;
					}
					if(j == MAX_CLIENT-1)
					{	//접속자 수 처리
						m_pack.cmd = CMD_USER_ERR;
						strcpy(m_pack.data, "[ERROR] 접속자 수 초과\n");
						send(m_clntSock, (char*) &m_pack, sizeof(PACKET), 0);
						puts("[ERROR] 접속자 수 초과");
						Close_Client(&m_clntSock);
					}
				}
			}
			else
			{
				for (int j = 0; j < MAX_CLIENT; j++)
					if (m_clntSocks[j] == m_events[i].data.fd)
						if (m_CUser[j].RecvData(m_events[i].data.fd, j, &dataList) == ERR)
						{
							Close_Client(&m_events[i].data.fd);
							m_clntSocks[j] = 0;
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
	m_adr_sz = sizeof(&m_clntSock);
	m_clntSock = accept(m_serv_sock, (struct sockaddr*) &m_clntSock, &m_adr_sz);

	SetNonblocking(&m_clntSock);

	//event 유형 설정: 수신할 버퍼가 존재할 때 | Edge Trigger Setting
	m_event.events = EPOLLIN | EPOLLET;
	m_event.data.fd = m_clntSock;

	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_clntSock, &m_event) == ERR)
		return ERR;

	cout << endl;
	cout << "[Connect] client: " << m_clntSock << endl;
	return 0;
}

void CUserMng::Close_Client(int *fd)
{
	epoll_ctl(m_epfd, EPOLL_CTL_DEL, *fd, NULL);
	close(*fd);
	cout << "[Close] client: " << *fd << endl;
}

//10분마다 현재 접속중인 유저(client) 출력
void * CUserMng::UserCheckThread(void * arg)
{
	sleep(600);
	return NULL;
}

//계속 돌면서 패킷 pasing/send 하기
void * CUserMng::WorkerThread(void *arg)
{
	CUserMng *cUserMng = (CUserMng *)arg;
	return NULL;
}

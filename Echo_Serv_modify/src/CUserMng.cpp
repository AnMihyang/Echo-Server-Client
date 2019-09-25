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

CUserMng::CUserMng(){
	m_clntSock = 0;
	m_adr_sz = 0;
	m_events = nullptr;
	m_UserCheck_tID = 0;
	m_Worker_tID = 0;

	pthread_mutex_init(&m_clntCon_mutex, NULL);
}

CUserMng::~CUserMng() {
	pthread_mutex_destroy(&m_clntCon_mutex);
}

void CUserMng::Server_Handling()
{
	int event_cnt= 0;

	SetSocket();			//소켓 설정

	m_events = EpollCreate();

	if(pthread_create(&m_UserCheck_tID, NULL, &CUserMng::UserCheckThread, (void *)this) != 0)
	{
		puts("[ERROR] pthread_create() error");
		close(m_serv_sock);
		EpollClose();
	}
	pthread_detach(m_UserCheck_tID);

	//Worker스레드 생성
	if(pthread_create(&m_Worker_tID, NULL, &CUserMng::WorkerThread, (void *)this) != 0)
	{
		puts("[ERROR] pthread_create() error");
		close(m_serv_sock);
		EpollClose();
	}
	pthread_detach(m_Worker_tID);

	while (1)
	{
		event_cnt = EpollWait();

		if (event_cnt == ERR) {
			puts("[ERROR] epoll_wait Error");
			break;
		}

		for (int i = 0; i < event_cnt; i++)
		{
			m_clntSock = m_events[i].data.fd;

			if (m_events[i].data.fd == m_serv_sock)
			{
				Connect_Client();
				pthread_mutex_lock(&m_clntCon_mutex);
				for(int j = 0; j < MAX_CLIENT; j++)
				{

					//clntSocks배열에 client socket 부여하기
					if(m_clntSocks[j] == 0)
					{
						m_clntSocks[j] = m_clntSock;
						m_CUser[j].m_clntSock = m_clntSock;
						m_CUser[j].m_clntConnect = true;
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
				pthread_mutex_unlock(&m_clntCon_mutex);
			}
			else
			{

				for (int j = 0; j < MAX_CLIENT; j++)
					if (m_clntSocks[j] == m_events[i].data.fd)
					{
						pthread_mutex_lock(&m_clntCon_mutex);
						if (m_CUser[j].RecvData(m_events[i].data.fd, j, &m_dataList) == ERR)
						{
							m_CUser[j].m_clntConnect = false;
							m_CUser[j].m_clntSock = 0;
							Close_Client(&m_events[i].data.fd);
							m_clntSocks[j] = 0;
						}
						pthread_mutex_unlock(&m_clntCon_mutex);
						break;
					}
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
	m_event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
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
	cout << "[Closed] client: " << *fd << endl;

//	int index = 0;
//	m_CUser[index].m_clntSock = 0;
//	m_CUser[index].m_clntConnect = false;
}

//10분마다 현재 접속중인 유저(client) 출력
void * CUserMng::UserCheckThread(void * arg)
{
	CUserMng *cUserMng = (CUserMng *)arg;
	int userCnt;

	while(1)
	{
		userCnt = 0;
		sleep(10);
		cout << endl;
		cout << "-----------User Check-----------" << endl;
		pthread_mutex_lock(&cUserMng->m_clntCon_mutex);


		if (cUserMng->m_event.events == EPOLLRDHUP)
		{
			for (int i = 0; i < MAX_CLIENT; ++i)
			{
				//클라이언트 연결 끊기
				if (cUserMng->m_CUser[i].m_clntSock == cUserMng->m_event.data.fd)
				{
					cout << "[Disconnected] CUser" << i << " - Client " << cUserMng->m_CUser[i].m_clntSock << endl;
					cUserMng->Close_Client(&cUserMng->m_event.data.fd);
					cUserMng->m_CUser[i].m_clntSock = 0;
					cUserMng->m_CUser[i].m_clntConnect = false;
				}
			}
		}

		for(int i = 0; i < MAX_CLIENT; ++i)
		{

			/*if (cUserMng->m_event.events == EPOLLRDHUP)
			{
				//클라이언트 연결 끊기
				if (cUserMng->m_CUser[i].m_clntSock == cUserMng->m_event.data.fd)
				{
					cout << "[Disconnected] CUser" << i << " - Client " << cUserMng->m_CUser[i].m_clntSock << endl;
					cUserMng->Close_Client(&cUserMng->m_event.data.fd);
					cUserMng->m_CUser[i].m_clntSock = 0;
					cUserMng->m_CUser[i].m_clntConnect = false;
				}
			}*/

			if(cUserMng->m_CUser[i].m_clntConnect == true)
			{
				if(cUserMng->m_CUser[i].m_clntSock == 0)	 //|| cUserMng->m_event.events != EPOLLRDHUP
				{
					//클라이언트 연결 끊기
					cout << "[Disconnected] CUser" << i << " - Client " << cUserMng->m_CUser[i].m_clntSock << endl;
					cUserMng->Close_Client(&cUserMng->m_CUser[i].m_clntSock);
					cUserMng->m_CUser[i].m_clntSock = 0;
					cUserMng->m_CUser[i].m_clntConnect = false;
				}
				else
				{
					cout << " [Connected] CUser" << i << " - Client " << cUserMng->m_CUser[i].m_clntSock << endl;
					++userCnt;
				}
			}
		}//for

		pthread_mutex_unlock(&cUserMng->m_clntCon_mutex);
		cout << endl;
		cout << "-----현재 접속자 수: " << userCnt << endl << endl;
	}//while
}

//계속 돌면서 패킷 parsing/send 하기
void * CUserMng::WorkerThread(void *arg)
{
	CUserMng *cUserMng = (CUserMng *)arg;
	pthread_mutex_t tmutex;

	pthread_mutex_init(&tmutex, NULL);

	while(1)
	{
		for(int i = 0; i < MAX_CLIENT; ++i)
		{
			if(cUserMng->m_CUser[i].m_clntConnect == true)
			{
				pthread_mutex_lock(&tmutex);
				if(cUserMng->m_CUser[i].QueueCheck() != ERR)		//큐에 처리할 패킷 있는지 확인
					if(cUserMng->m_CUser[i].ParsingData() == ERR)
						puts("[ERROR] send() error");
				pthread_mutex_unlock(&tmutex);
			}
			usleep(100);
		}
		usleep(300000);
	}

	pthread_mutex_destroy(&tmutex);
}

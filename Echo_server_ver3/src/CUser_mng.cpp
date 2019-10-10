/*
 * CUserMng.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "CUser_mng.h"

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <iomanip>

using namespace std;

CUser_mng::CUser_mng(){
	m_clnt_sock = 0;
	m_adr_sz = 0;
	m_events = nullptr;
	m_User_check_tID = 0;
	m_Worker_tID = 0;

	pthread_mutex_init(&m_clntlock, NULL);
}

CUser_mng::~CUser_mng() {
	pthread_mutex_destroy(&m_clntlock);
}

void CUser_mng::Server_handling()
{
	int event_cnt= 0;

	Set_socket();			//소켓 설정

	m_events = Epoll_create();

	//User_check 스레드 생성
	if(pthread_create(&m_User_check_tID, NULL, &CUser_mng::User_check_thread, (void *)this) != 0)
	{
		puts("[ERROR] pthread_create() error");
		close(m_serv_sock);
		Epoll_close();
	}
	pthread_detach(m_User_check_tID);

	//Worker스레드 생성
	if(pthread_create(&m_Worker_tID, NULL, &CUser_mng::Worker_thread, (void *)this) != 0)
	{
		puts("[ERROR] pthread_create() error");
		close(m_serv_sock);
		Epoll_close();
	}
	pthread_detach(m_Worker_tID);


	while (1)
	{
		event_cnt = Epoll_wait();

		if (event_cnt == ERR) {
			puts("[ERROR] epoll_wait Error");
			break;
		}

		for (int i = 0; i < event_cnt; i++)
		{
			m_clnt_sock = m_events[i].data.fd;

			if (m_events[i].data.fd == m_serv_sock)
			{
				Connect_client();		//클라이언트 연결


				for(int j = 0; j < MAX_CLIENT; j++)
				{

					//clntSocks배열에 client socket 부여하기
					if(m_clnt_socks[j] == 0)
					{
						pthread_mutex_lock(&m_clntlock);
						m_clnt_socks[j] = m_clnt_sock;
						m_CUser[j].User_set(m_clnt_sock);
//						m_CUser[j].m_clnt_sock = m_clnt_sock;
//						m_CUser[j].m_clnt_connect = true;
						pthread_mutex_unlock(&m_clntlock);
						break;
					}
					if(j == MAX_CLIENT-1)
					{	//접속자 수 처리
						m_pack.body.cmd = CMD_USER_ERR;
						strcpy(m_pack.body.data, "[ERROR] 접속자 수 초과\n");
						if(send(m_clnt_sock, (char*) &m_pack, sizeof(PACKET), 0) == -1)
							while(send(m_clnt_sock, (char*) &m_pack, sizeof(PACKET), 0) == -1);
						puts("[ERROR] 접속자 수 초과");
						Close_client(&m_clnt_sock);
					}

				}

			}
			else
			{
//				pthread_mutex_lock(&m_clntlock);
				for (int j = 0; j < MAX_CLIENT; j++)
				{
					if (m_clnt_socks[j] == m_events[i].data.fd)
					{
						//CUser클래스의 Recv_data 호출
						if (m_CUser[j].Recv_data(&m_dataList) == ERR)
						{
//							pthread_mutex_lock(&m_clntlock);
							//클라이언트 연결 끊기
							m_CUser[j].Init();		//CUser 초기화
							Close_client(&m_events[i].data.fd);
							m_clnt_socks[j] = 0;
//							pthread_mutex_unlock(&m_clntlock);
						}
						break;
					}
				}
//				pthread_mutex_unlock(&m_clntlock);
			}//else
		}//for
	}//while
	cout << "[NOTICE] close server" << endl;
	close(m_serv_sock);
	Epoll_close();
}

int CUser_mng::Connect_client()
{
	m_adr_sz = sizeof(&m_clnt_sock);
	m_clnt_sock = accept(m_serv_sock, (struct sockaddr*) &m_clnt_sock, &m_adr_sz);

	Set_nonblocking(&m_clnt_sock);

	//event 유형 설정: 수신할 버퍼가 존재할 때 | Edge Trigger Setting
	m_event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
	m_event.data.fd = m_clnt_sock;

	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_clnt_sock, &m_event) == ERR)
		return ERR;

	cout << "[Connect] client: " << m_clnt_sock << endl;
	return 0;
}

void CUser_mng::Close_client(int *fd)
{
	epoll_ctl(m_epfd, EPOLL_CTL_DEL, *fd, NULL);
	close(*fd);
	cout << "[Closed] client: " << *fd << endl;
}

//1분마다 현재 접속중인 user(client)목록 출력
void * CUser_mng::User_check_thread(void * arg)
{
	CUser_mng *cUserMng = (CUser_mng *)arg;
	int userCnt;

	while(1)
	{
		userCnt = 0;
		sleep(300);
		cout << endl;
		cout << "-----------------User Check-----------------" << endl;
		pthread_mutex_lock(&cUserMng->m_clntlock);

		for(int i = 0; i < MAX_CLIENT; ++i)
		{
			if(cUserMng->m_CUser[i].m_clnt_connect == true)
			{
				if(cUserMng->m_CUser[i].m_clnt_sock == 0)
				{
					//클라이언트 연결 끊기
					cout << "   [Disconnected] CUser " << i << " - Client " << cUserMng->m_CUser[i].m_clnt_sock << endl;
					cUserMng->Close_client(&cUserMng->m_CUser[i].m_clnt_sock);
					cUserMng->m_CUser[i].Init();
//					cUserMng->m_CUser[i].m_clnt_sock = 0;
//					cUserMng->m_CUser[i].m_clnt_connect = false;
				}
				else
				{
					cout << "   [Connected] CUser " << i << " - Client " << cUserMng->m_CUser[i].m_clnt_sock << endl;
					++userCnt;
				}
			}
		}//for

		pthread_mutex_unlock(&cUserMng->m_clntlock);
		cout << endl;
		cout << "----------- 현재 접속자 수 : " << setw(3)  << userCnt <<" -----------"<< endl << endl;
	}//while
}

//스레드 돌면서 패킷 parsing/send 하기
void * CUser_mng::Worker_thread(void *arg)
{
	CUser_mng *cUserMng = (CUser_mng *)arg;

	while(1)
	{
		for(int i = 0; i < MAX_CLIENT; ++i)
		{
			pthread_mutex_lock(&cUserMng->m_clntlock);
			if(cUserMng->m_CUser[i].m_clnt_connect == true)
				if(cUserMng->m_CUser[i].Queue_check() != ERR)		//큐에 처리할 패킷 있는지 확인
				{
					if(cUserMng->m_CUser[i].Parsing_data() == ERR)
						cout << "[ERROR] client " << cUserMng->m_CUser[i].m_clnt_sock << ": send() error" << endl;
				}
			usleep(100);
			pthread_mutex_unlock(&cUserMng->m_clntlock);
		}

		usleep(100000);
	}

}

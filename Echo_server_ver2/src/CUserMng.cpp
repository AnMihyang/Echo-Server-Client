/*
 * CUserMng.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "CUserMng.h"
#include <string.h>
#include <unistd.h>

#define PORT 9190

using namespace std;

CUserMng::CUserMng() {
	//지역변수 초기화
	m_serv_sock = 0;
	m_clnt_sock = 0;
	m_events = nullptr;
	m_clnt = 0;
	pthread_mutex_init(&m_tLock, NULL);
}

CUserMng::~CUserMng() {
	pthread_mutex_destroy(&m_tLock);
}

void CUserMng::ServerHandling()
{
	int event_cnt= 0;

	SetSocket();

	m_events = EpollCreate(&m_serv_sock);

	while (1) {
		event_cnt = EpollWait();

		if (event_cnt == ERR) {
			puts("[ERROR] epoll_wait Error");
			break;
		}

		for (int i = 0; i < event_cnt; i++)
		{
			m_clnt = m_events[i].data.fd;

			if (m_events[i].data.fd == m_serv_sock)
			{
				Connect_Client(&m_clnt_sock);

				for(int j = 0; j < MAX_CLIENT; j++)
				{
					//clntSock배열에 client socket 부여하기
					if(clntSock[j] == 0) {
						clntSock[j] = m_clnt_sock;
						break;
					}
					if(j == MAX_CLIENT-1) {	//접속자 수 처리
						pack.cmd = CMD_USER_ERR;
						strcpy(pack.data, "[ERROR] 접속자 수 초과\n");
						send(m_clnt_sock, (char*) &pack, sizeof(PACKET), 0);
						puts("[ERROR] 접속자 수 초과");
						Close_Client(&m_clnt_sock);
					}
				}
			}
			else
			{

				for (int j = 0; j < MAX_CLIENT; j++)
					if (clntSock[j] == m_events[i].data.fd)
						if (m_CUser[j].RecvData(m_events[i].data.fd, j, &dataList) == ERR)
						{
							Close_Client(&m_events[i].data.fd);
							clntSock[j] = 0;
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

//소켓 설정
void CUserMng::SetSocket()
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

//스레드 실행 함수
void* CUserMng::Thread_Handling(void *arg)
{
	CUserMng *cUserMng = (CUserMng *)arg;
	pthread_mutex_lock(&cUserMng->m_tLock);
//	cout << ++cUserMng -> cnt << "번 실행" << endl;
	for (int j = 0; j < MAX_CLIENT; j++)
	{
		if (cUserMng->clntSock[j] == cUserMng->m_clnt)
		{
			if (cUserMng->m_CUser[j].RecvData(cUserMng->m_clnt, j, &cUserMng->dataList) == ERR)
			{
				cUserMng->Close_Client(&cUserMng->m_clnt);
				cUserMng->clntSock[j] = 0;		// client 연결 끊기면 해당 clientSock 원소 초기화
			}
			break;
		}
	}
	pthread_mutex_unlock(&cUserMng->m_tLock);
	return NULL;
}

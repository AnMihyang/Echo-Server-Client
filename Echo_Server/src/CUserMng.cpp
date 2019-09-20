/*
 * CUserMng.cpp
 *
 *  Created on: 2019. 8. 20.
 *      Author: mihyang
 */

#include "CUserMng.h"
#include <iostream>
#include <string.h>

#define ERR -1

using namespace std;

CUserMng::CUserMng() {
	// TODO Auto-generated constructor stub
	m_CEpollMng = new CEpollMng();
}

CUserMng::~CUserMng() {
	// TODO Auto-generated destructor stub
}

void CUserMng::Handling()
{
	SetSocket();
	m_ep_events = m_CEpollMng -> Epoll_Create(&m_serv_sock);

	pthread_mutex_init(&m_clntLock, NULL);
	pthread_mutex_init(&m_dataLock, NULL);
	pthread_mutex_init(&m_queueLock, NULL);

	while (1)
	{
		m_event_cnt = m_CEpollMng->Epoll_Wait();

		if (m_event_cnt == ERR)
		{
			puts("epoll_wait() error");
			break;
		}
		for (int i = 0; i < m_event_cnt; i++)
		{
			if (m_ep_events[i].data.fd == m_serv_sock)
			{
				m_CEpollMng -> Connect_Client(&m_clnt_sock);
				m_i = i;
				if(m_clnt_num+1 >= 1000)
				{
					m_CEpollMng->Close_Client(&m_ep_events[i].data.fd);
					puts("접속자 수 초과");
					break;
				}
				else
					m_clnt_num++;

				//thread 생성 에러 처리
				if (pthread_create(&tID[m_clnt_num], NULL, &CUserMng::Thread_Handling, (void*) this) != 0)
				{
					puts("pthread_create Error()");
					m_CEpollMng->Epoll_Close();
				}
				cout << "---현재 접속자 : " << m_clnt_num + 1 << "명" << endl << endl;
				pthread_detach(tID[m_clnt_num]);
			} // if
		} // for
	} // while
	pthread_mutex_destroy(&m_queueLock);
	pthread_mutex_destroy(&m_dataLock);
	pthread_mutex_destroy(&m_clntLock);
	close(m_serv_sock);
	m_CEpollMng->Epoll_Close();
}


void CUserMng::SetSocket()
{
	m_serv_sock=socket(PF_INET, SOCK_STREAM, 0);		// socket 생성
	memset(&m_serv_adr, 0, sizeof(m_serv_adr));		// serv_adr 초기화
	m_serv_adr.sin_family=AF_INET;
	m_serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);		// INADDR_ANY:사용 가능한 랜카드 IP 사용
	m_serv_adr.sin_port=htons(9190);					// 포트번호 9190

	//주소 할당
	if(bind(m_serv_sock, (struct sockaddr*)&m_serv_adr, sizeof(m_serv_adr)) == -1)
		puts("bind() error");

	//연결 가능 상태 만들기
	if(listen(m_serv_sock, 5) == -1)
		puts("listen() error");
}

void* CUserMng::Thread_Handling(void *arg) {

	CUserMng *cUserMng = (CUserMng*) arg;
	int i = cUserMng->m_i;
	int str_len;
	PACKET pack;

	while (1)
	{
		cUserMng->m_CEpollMng->Epoll_Wait();
		while (1)
		{
			//Client로부터 Packet 받기
			str_len = recv(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);

			if (str_len == 0)
			{
				pthread_mutex_lock(&cUserMng->m_clntLock);
				cUserMng->m_clnt_num--;
				if (cUserMng->m_clnt_num < -1)
				{
					puts("Client Num Err");
					return NULL;
				}
				cUserMng->m_CEpollMng->Close_Client(&cUserMng->m_ep_events[i].data.fd);
				cout << "close 현재 접속자 : " << cUserMng->m_clnt_num + 1 << "명" << endl << endl;
				pthread_mutex_unlock(&cUserMng->m_clntLock);
				return NULL;
			}
			else if (str_len < 0)
			{
				if (errno == EAGAIN) break;
			}
			else
			{
				cout << "Recv: " << pack.cmd  << ", "<< pack.data << endl;

				//Client로부터 받은 Packet Command 처리
				switch (pack.cmd)
				{
					//Login 처리
					case CMD_USER_LOGIN_REQ:
						//Client에 패킷 전송
						pack.cmd = CMD_USER_LOGIN_RESULT;
						send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
						break;

					//Echo Data 처리
					case CMD_USER_DATA_REQ:
						pack.cmd = CMD_USER_DATA_RESULT;
						pthread_mutex_lock(&cUserMng->m_queueLock);
						cUserMng->Enqueue(pack);
						pthread_mutex_unlock(&cUserMng->m_queueLock);
						send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
						break;

					//Data 저장 처리
					case CMD_USER_SAVE_REQ:
						pack.cmd = CMD_USER_SAVE_RESULT;
						pthread_mutex_lock(&cUserMng->m_dataLock);
						if (cUserMng->InsertData(pack) == ERR) {
							//입력한 데이터가 이미 자료구조에 존재할 때
							cUserMng->PrintDataList();
							strcpy(pack.data, " !!Data already exists!!");
						} else {
							cUserMng->PrintDataList();
							strcpy(pack.data, " !!Success Insert!!");
						}
						send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
						cUserMng->SendDataList(i, pack);
						pthread_mutex_unlock(&cUserMng->m_dataLock);
						break;

					//Data 삭제 처리
					case CMD_USER_DELETE_REQ:
						pack.cmd = CMD_USER_DELETE_RESULT;
						pthread_mutex_lock(&cUserMng->m_dataLock);
						int err;
						err = cUserMng->DeleteData(pack);	//DataMng의 DeleteData() 함수 호출
						if (err == ERR)		//자료구조가 비어있을 때 pack.data에 안내 메세지 저장
						{
							strcpy(pack.data, " !!Empty Data Structure!!");
							send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
							cUserMng->SendDataList(i, pack);
						}
						//Client에서 입력한 데이터가 자료구조에 존재하지 않을 때 pack.data에 안내 메세지 저장
						else if (err == -2)
						{
							strcpy(pack.data, " !!The data does not exist!!\n");
							send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
							cUserMng->SendDataList(i, pack);
						}
						else	// Success Delete
						{
							cUserMng->PrintDataList();		//DataMng의 PrintDataList() 함수 호출
							strcpy(pack.data, " !!Success Delete!!");
							send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
							cUserMng->SendDataList(i, pack);
						}
						pthread_mutex_unlock(&cUserMng->m_dataLock);
						break;

						//Data list 출력
					case CMD_USER_PRINT_REQ:
						pack.cmd = CMD_USER_PRINT_RESULT;
						send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
						cUserMng->SendDataList(i, pack);
						break;

					default:
						pack.cmd = CMD_USER_ERR;
						strcpy(pack.data, " !!Request Error!!");
						send(cUserMng->m_ep_events[i].data.fd, (char*) &pack, sizeof(PACKET), 0);
						cUserMng->m_CEpollMng->Close_Client(&cUserMng->m_ep_events[i].data.fd);
						puts("Request Error");
				} //switch
			} //else end
		} //while end
	} //while end
}

int CUserMng::SendDataList(int for_index, PACKET pack)
{
	unsigned int cnt = 1;
	string sendMsg;
	if(dataList.empty())
	{
		pack.cmd = CMD_USER_ERR;
		strcpy(pack.data, "\n    !!Empty!!\n");
		send(m_ep_events[for_index].data.fd, (char*) &pack, sizeof(PACKET), 0);
		return 0;
	}
	for (list<string>::iterator it = dataList.begin(); it != dataList.end(); cnt++, it++)
	{
		if (cnt >= dataList.size())
			pack.cmd = CMD_USER_ERR;
		sendMsg = to_string(cnt) + ") " + *it;
		strcpy(pack.data, sendMsg.c_str());
		send(m_ep_events[for_index].data.fd, (char*) &pack, sizeof(PACKET), 0);
	}

	return 0;
}


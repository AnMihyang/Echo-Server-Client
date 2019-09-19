/*
 * Server.cpp
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#include "Server.h"


Server::Server() {
	// TODO Auto-generated constructor stub
	m_CUserMng = CUserMng();
}

Server::~Server() {
	// TODO Auto-generated destructor stub
}

void Server::Server_Handling()
{
	int event_cnt= 0;

	m_CUserMng.SetSocket();

	m_events = m_CUserMng.EpollCreate();

	while (1) {
		event_cnt = EpollWait();

		if (event_cnt == ERR) {
			puts("[ERROR] epoll_wait Error");
			break;
		}

		for (int i = 0; i < event_cnt; i++) {
			m_clnt = m_events[i].data.fd;

			if (m_events[i].data.fd == m_serv_sock) {
				Connect_Client (&m_clnt_sock);

				for (int j = 0; j < MAX_CLIENT; j++) {
					//clntSock배열에 client socket 부여하기
					if (clntSock[j] == 0) {
						clntSock[j] = m_clnt_sock;
						break;
					}
					if (j == MAX_CLIENT - 1) {	//접속자 수 처리
						pack.cmd = CMD_USER_ERR;
						strcpy(pack.data, "[ERROR] 접속자 수 초과\n");
						send(m_clnt_sock, (char*) &pack, sizeof(PACKET), 0);
						puts("[ERROR] 접속자 수 초과");
						Close_Client(&m_clnt_sock);
					}
				}
			} else {

				for (int j = 0; j < MAX_CLIENT; j++)
					if (clntSock[j] == m_events[i].data.fd)
						if (m_CUser[j].RecvData(m_events[i].data.fd, j,
								&dataList) == ERR) {
							Close_Client(&m_events[i].data.fd);
							clntSock[j] = 0;
						}
				/*
				 if(pthread_create(&tID, NULL, &CUserMng::Thread_Handling, (void*) this) != 0)
				 puts("[ERROR] pthread_create Error");
				 pthread_detach(tID);*/
			}	//else
		}	//for
	}	//while
	close (m_serv_sock);
	EpollClose();
}

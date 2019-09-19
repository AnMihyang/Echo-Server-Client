/*
 * Server.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "CUserMng.h"

class Server {
private:
	CUserMng m_CUserMng;
	struct epoll_event *m_events;
public:
	Server();
	virtual ~Server();
	void Server_Handling();
};

#endif /* SERVER_H_ */

/*
 * CUserMng.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef CUSERMNG_H_
#define CUSERMNG_H_

#include "CEpollMng.h"
#include "CUser.h"
#include "PacketDefine.h"

#define MAX_CLIENT 100

class CUserMng: public CEpollMng {
private:
	int m_clnt_sock;
	socklen_t m_adr_sz;
	int m_clntSock[MAX_CLIENT] = {0,};
	PACKET m_pack;
	CUser m_CUser[MAX_CLIENT];
public:
	CUserMng();
	virtual ~CUserMng();
	void Server_Handling();
	int Connect_Client();				// Client 연결
	void Close_Client(int *fd);							// Client 종료
};


#endif /* CUSERMNG_H_ */

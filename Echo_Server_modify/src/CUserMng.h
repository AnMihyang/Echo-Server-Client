/*
 * CUserMng.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: mihyang
 */

#ifndef CUSERMNG_H_
#define CUSERMNG_H_

#include "CEpollMng.h"


class CUserMng: public CEpollMng {
private:
	int m_clnt_sock;
	socklen_t m_adr_sz;
public:
	CUserMng();
	virtual ~CUserMng();
	int Connect_Client();				// Client 연결
	void Close_Client(int *fd);							// Client 종료
};


#endif /* CUSERMNG_H_ */

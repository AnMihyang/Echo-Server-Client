/*
 * CUser.h
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#ifndef CUSER_H_
#define CUSER_H_

#include "PacketDefine.h"
#include "CircularBuf.h"
#include <list>
#include <string>

using namespace std;

class CUser {
private:
	Queue queue;
	int m_strlen;
	int m_clntSock;
	CircularBuf m_CircularBuf;
public:
	CUser();
	virtual ~CUser();
	int RecvData(int fd, int cUserNum, list<string> *dataList);
	int ParsingData();
	void SendData(PACKET pack);
	int CheckPacket();
};

#endif /* CUSER_H_ */

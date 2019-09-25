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
#include "DataMng.h"

using namespace std;

class CUser {
private:
	Queue queue;
	int m_strlen;
	list<string> *m_dataList;
	CircularBuf m_CircularBuf;
	DataMng m_DataMng;
public:
	int m_clntSock;
	bool m_clntConnect;

public:
	CUser();
	virtual ~CUser();
	int RecvData(int fd, int cUserNum, list<string> *dataList);
	int QueueCheck();
	int ParsingData();
	int SendData(PACKET pack);
	int CheckPacket(PACKET pack);
};

#endif /* CUSER_H_ */

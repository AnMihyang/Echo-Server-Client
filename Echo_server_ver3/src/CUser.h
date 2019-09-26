/*
 * CUser.h
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#ifndef CUSER_H_
#define CUSER_H_

#include "Circularbuf.h"
#include "Data_mng.h"
#include "Packet_define.h"

using namespace std;

class CUser {
private:
	Queue queue;
	int m_strlen;
	list<string> *m_data_list;
	Circularbuf m_Circularbuf;
	Data_mng m_Data_mng;
public:
	int m_clnt_sock;
	bool m_clnt_connect;

public:
	CUser();
	virtual ~CUser();
	int Recv_data(int fd, int cUser_num, list<string> *data_list);
	int Queue_check();
	int Parsing_data();
	int Send_data(PACKET pack);
	int Check_packet(PACKET pack);
};

#endif /* CUSER_H_ */

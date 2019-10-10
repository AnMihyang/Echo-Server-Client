/*
 * CUser.h
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#ifndef CUSER_H_
#define CUSER_H_

#include "Circular_buffer.h"
#include "Data_mng.h"
#include "Packet_define.h"

using namespace std;

class CUser {
private:
	Queue queue;
	int m_strlen = 0;
//	char m_recvbuff[RECV_BUFF];
	list<string> *m_data_list = nullptr;
	Circular_buffer m_Circular_buffer;
	Data_mng m_Data_mng;
	PACKET m_parsing_pack;
public:
	int m_clnt_sock = 0;
	bool m_clnt_connect = false;

public:
	CUser();
	virtual ~CUser();
	void Init();
	void User_set(int clnt_fd);
	int Recvn(int s, char *buf, int len, int flags);
	int Recv_data(list<string> *data_list);
	int Queue_check();
	int Parsing_data();
	int Send_data(PACKET pack);
	int Find_packet();
};

#endif /* CUSER_H_ */

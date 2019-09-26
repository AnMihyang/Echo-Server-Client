/*
 * DataMng.cpp
 *
 *  Created on: 2019. 9. 24.
 *      Author: mihyang
 */

#include "Data_mng.h"

#include <iostream>
#include <algorithm>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

Data_mng::Data_mng() {

}

Data_mng::~Data_mng() {
}

int Data_mng::Insert_data(PACKET in_pack, list<string> *data_list) {
	list<string>::iterator it;

	it = find(data_list->begin(), data_list->end(), in_pack.data);

	if (it != data_list->end())		//데이터 중복
		return -1;

	data_list->push_back(in_pack.data);	//list에 입력받은 데이터 삽입
	Print_data_list(data_list);
	return 0;
}

int Data_mng::Delete_data(PACKET del_pack, list<string> *data_list) {
	list<string>::iterator it;

	it = find(data_list->begin(), data_list->end(), del_pack.data);

	if (it == data_list->end())		//삭제할 데이터가 list에 존재하지 않을 때
		return -1;

	data_list->remove(del_pack.data);		//list에서 입력받은 데이터 삭제
	Print_data_list(data_list);
	return 0;
}

void Data_mng::Print_data_list(list<string> *data_list)
{
	int cnt = 0;

	cout << endl;
	cout << "[Data list] " << endl;
		for(list<string>::iterator it = data_list->begin(); it != data_list->end(); it++)
			cout << ++cnt << ") " << *it << endl;
		cout << "------------------------------------------------" <<endl;
}

//TODO: Test 해보기 (packet size 초과 시 예외처리 하기)
int Data_mng::Send_data_list(int clnt_sock, list<string> *data_list)
{
	PACKET pack;
	unsigned int cnt = 1;
	string send_msg;
	int send_res = 0;
	pack.cmd = CMD_USER_PRINT_RESULT;

	if (data_list->empty())
	{
		pack.cmd = CMD_USER_ERR;
		strcpy(pack.data, "       !!Empty!!");
		send(clnt_sock, (char*) &pack, sizeof(PACKET), 0);
	}
	for (list<string>::iterator it = data_list->begin(); it != data_list->end(); ++cnt, ++it)
	{
		if (cnt >= data_list->size())
			pack.cmd = CMD_USER_ERR;

		send_msg = *it + '\0';
		strcpy(pack.data, send_msg.c_str());
//		cout << "send: " << pack.cmd << ", " << pack.data << endl;
		send_res = send(clnt_sock, (char*) &pack, sizeof(PACKET), 0);
		if(send_res == -1)
			return -1;
//		else
//			cout << "[sendResult] clntSock: " << clntSock << ", " << sendR << endl;
	}
	return 0;
}

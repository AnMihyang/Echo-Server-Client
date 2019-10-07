
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

	it = find(data_list->begin(), data_list->end(), in_pack.body.data);

	if (it != data_list->end())		//데이터 중복
		return -1;

	data_list->push_back(in_pack.body.data);	//list에 입력받은 데이터 삽입
	Print_data_list(data_list);
	return 0;
}

int Data_mng::Delete_data(PACKET del_pack, list<string> *data_list) {
	list<string>::iterator it;

	it = find(data_list->begin(), data_list->end(), del_pack.body.data);

	if (it == data_list->end())		//삭제할 데이터가 list에 존재하지 않을 때
		return -1;

	data_list->remove(del_pack.body.data);		//list에서 입력받은 데이터 삭제
	Print_data_list(data_list);
	return 0;
}

void Data_mng::Print_data_list(list<string> *data_list)
{
	int cnt = 0;

	cout << endl;
	cout << "[Data list] " << endl;
	for (list<string>::iterator it = data_list->begin(); it != data_list->end();
			it++)
		cout << ++cnt << ") " << *it << endl;
	cout << "-------------------------------------------------------------" <<endl;
}

//TODO: Test 해보기 (packet size 초과 시 예외처리 하기)
int Data_mng::Send_data_list(int clnt_sock, list<string> *data_list)
{
	int r;
	PRT_PACKET print_pack;
	unsigned int data_cnt = 1;
	unsigned int pindex = 0;
	string send_msg;
	int cnt = 0;
	print_pack.cmd = CMD_USER_PRINT_RESULT;
	memset(&print_pack.data, '\0', sizeof(print_pack.data));

	//list 비어있을 경우
	if (data_list->empty())
	{
		print_pack.cmd = CMD_USER_ERR;
		strncpy(print_pack.data, "          !!Empty!!\0", sizeof(print_pack.data));
		print_pack.data_num = 0;
		if (send(clnt_sock, (char*) &print_pack, sizeof(PRT_PACKET), 0) == -1)
			return -1;
	}

	for (list<string>::iterator it = data_list->begin(); it != data_list->end(); ++data_cnt, ++it)
	{

		send_msg = *it+'\0';

		if(sizeof(print_pack.data) < pindex+send_msg.length())
		{
			print_pack.data_num = cnt;
			if(send(clnt_sock, (char*) &print_pack, sizeof(PRT_PACKET), 0) == -1)
				return -1;
			pindex = 0;
			memset(&print_pack.data, '\0', sizeof(print_pack.data));
			cnt = 0;
		}
		cnt++;
		strncpy(&print_pack.data[pindex], send_msg.c_str(), send_msg.length());
		pindex += send_msg.length();

		if (data_cnt >= data_list->size())
					print_pack.cmd = CMD_USER_ERR;


		/*if( sizeof(print_pack.data) >= pindex+send_msg.length())
		{
			//print_pack.data에 list 데이터 저장
			strncpy(&print_pack.data[pindex], send_msg.c_str(), send_msg.length());
			pindex += send_msg.length();
		}
		else
		{
			print_pack.data_num = cnt;
			cout << "cnt: " <<cnt <<endl;
			if (send(clnt_sock, (char*) &print_pack, sizeof(PACKET), 0) == -1)
				return -1;
			memset(&print_pack, '\0', sizeof(PRT_PACKET));
			pindex = 0;
			cnt = 0;
			strncpy(&print_pack.data[pindex], send_msg.c_str(), send_msg.length());
		}*/
		if(print_pack.cmd == CMD_USER_ERR)
		{
			print_pack.data_num = cnt;
			r = send(clnt_sock, (char*) &print_pack, sizeof(PRT_PACKET), 0);
			if (r == -1)
				return -1;
//			else
//				cout << "send: " << r << endl;
		}
	}
	return 0;
}

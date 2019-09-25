/*
 * DataMng.cpp
 *
 *  Created on: 2019. 9. 24.
 *      Author: mihyang
 */

#include "DataMng.h"
#include <iostream>
#include <algorithm>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

DataMng::DataMng() {

}

DataMng::~DataMng() {
}

int DataMng::InsertData(PACKET inPack, list<string> *dataList) {
	list<string>::iterator it;

	it = find(dataList->begin(), dataList->end(), inPack.data);

	if (it != dataList->end())		//데이터 중복
		return -1;

	dataList->push_back(inPack.data);	//list에 입력받은 데이터 삽입
	PrintDataList(dataList);
	return 0;
}

int DataMng::DeleteData(PACKET delPack, list<string> *dataList) {
	list<string>::iterator it;

	it = find(dataList->begin(), dataList->end(), delPack.data);

	if (it == dataList->end())		//삭제할 데이터가 list에 존재하지 않을 때
		return -1;

	dataList->remove(delPack.data);		//list에서 입력받은 데이터 삭제
	PrintDataList(dataList);
	return 0;
}

void DataMng::PrintDataList(list<string> *dataList)
{
	int cnt = 0;

	cout << endl;
	cout << "[Data list] " << endl;
		for(list<string>::iterator it = dataList->begin(); it != dataList->end(); it++)
			cout << ++cnt << ") " << *it << endl;
		cout << "------------------------------------------------" <<endl;
}

//TODO: Test 해보기 (packet size 초과 시 예외처리 하기)
int DataMng::SendDataList(int clntSock, list<string> *dataList)
{
	PACKET pack;
	unsigned int cnt = 1;
	string sendMsg;
	pack.cmd = CMD_USER_PRINT_RESULT;

	if (dataList->empty())
	{
		pack.cmd = CMD_USER_ERR;
		strcpy(pack.data, "       !!Empty!!");
		send(clntSock, (char*) &pack, sizeof(PACKET), 0);
	}
	for (list<string>::iterator it = dataList->begin(); it != dataList->end(); ++cnt, ++it)
	{
		if (cnt >= dataList->size())
			pack.cmd = CMD_USER_ERR;

		sendMsg = *it;
		strcpy(pack.data, sendMsg.c_str());
//		cout << "send: " << pack.cmd << ", " << pack.data << endl;
		if(send(clntSock, (char*) &pack, sizeof(PACKET), 0) == -1)
			return -1;
	}
	return 0;
}

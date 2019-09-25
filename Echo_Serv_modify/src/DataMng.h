/*
 * DataMng.h
 *
 *  Created on: 2019. 9. 24.
 *      Author: mihyang
 */

// Client로부터 받은 데이터 list에 Insert/Delete/Print

#ifndef DATAMNG_H_
#define DATAMNG_H_

#include "PacketDefine.h"
#include <list>
#include <string>

using namespace std;

class DataMng {
public:
	DataMng();
	virtual ~DataMng();
	int InsertData(PACKET inPack, list<string> *dataList);		//Client로부터 받은 데이터 list에 삽입
	int DeleteData(PACKET delPack, list<string> *dataList);		//Client로부터 받은 데이터 list에서 삭제
	void PrintDataList(list<string> *dataList);					//server에 list 데이터 출력
	int SendDataList(int clntSock, list<string> *dataList);					//client에 list 데이터 전달
};

#endif /* DATAMNG_H_ */

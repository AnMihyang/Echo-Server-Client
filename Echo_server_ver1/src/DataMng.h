/*
 * CUser.h
 *
 *  Created on: 2019. 8. 21.
 *      Author: mihyang
 */

#ifndef DATAMNG_H_
#define DATAMNG_H_

#include <list>
#include <string>
#include <iostream>

#include "PacketDefine.h"

using namespace std;

class DataMng{
protected:
	list<string> dataList;

public:
	DataMng();
	virtual ~DataMng();
	int InsertData(PACKET pack);
	int DeleteData(PACKET pack);
	int CheckData(string data);
	void PrintDataList();
};

#endif /* DATAMNG_H_ */

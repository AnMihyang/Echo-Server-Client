/*
 * CUser.cpp
 *
 *  Created on: 2019. 8. 21.
 *      Author: mihyang
 */

#include "DataMng.h"
#include <algorithm>


DataMng::DataMng() {
	// TODO Auto-generated constructor stub

}

DataMng::~DataMng() {
	// TODO Auto-generated destructor stub
}

//list에 Client에서 받은 데이터 삽입
int DataMng::InsertData(PACKET pack)
{
	if (CheckData(pack.data) == -1)
		return -1;
	else
	{
		dataList.push_back(pack.data);
		return 0;
	}
}

//list에서 Client에서 받은 데이터 삭제
int DataMng::DeleteData(PACKET pack)
{
	int checkData_result = CheckData(pack.data);

	if (checkData_result == 0)	// List가 비어있을 때 return -1
		return -1;
	else if(checkData_result == 1)	// 입력받은 데이터가 List에 존재하지 않을 때 return -2
		return -2;
	else	// 그 외 입력받은 데이터 삭제 처리
	{
		dataList.remove(pack.data);	//erase
		return 0;
	}
}

//list가 비어있거나, Client에서 입력받은 데이터가 존재하는지 확인
int DataMng::CheckData(string data)
{
	list<string>::iterator it;

	it = find(dataList.begin(), dataList.end(), data);

	if(dataList.empty())			// List가 비어있을 경우 return 0
		return 0;

	if(it == dataList.end())		// 입력받은 데이터가 List에 존재하지 않을 경우 return 1
		return 1;

	return -1;
}

//list에 있는 모든 데이터 출력
void DataMng::PrintDataList()
{
	cout << "list 출력: ";
	for(list<string>:: iterator it = dataList.begin(); it != dataList.end(); it++)
		cout << *it << ' ';
	cout << endl << endl;
}

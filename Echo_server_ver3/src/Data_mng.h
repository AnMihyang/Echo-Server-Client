/*
 * DataMng.h
 *
 *  Created on: 2019. 9. 24.
 *      Author: mihyang
 */

// Client로부터 받은 데이터 list에 Insert/Delete/Print

#ifndef DATA_MNG_H_
#define DATA_MNG_H_

#include <list>
#include <string>
#include "Packet_define.h"

using namespace std;

class Data_mng {
public:
	Data_mng();
	virtual ~Data_mng();
	int Insert_data(PACKET in_pack, list<string> *data_list);		//Client로부터 받은 데이터 list에 삽입
	int Delete_data(PACKET del_pack, list<string> *data_list);		//Client로부터 받은 데이터 list에서 삭제
	void Print_data_list(list<string> *data_list);					//server에 list 데이터 출력
	int Send_data_list(int clnt_sock, list<string> *data_list);					//client에 list 데이터 전달
};

#endif /* DATA_MNG_H_ */

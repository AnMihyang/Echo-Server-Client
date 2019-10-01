/*
 * CUser.cpp
 *
 *  Created on: 2019. 9. 20.
 *      Author: mihyang
 */

#include "CUser.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define ERR -1

using namespace std;

CUser::CUser()
{
//	for(int i = 0; i < QUEUE_SIZE; ++i)
//		queue.pack[i] = (PACKET *)malloc(sizeof(PACKET));	//queue.pack 초기화

	queue.front = queue.rear = 0;		//queue 초기화
}

CUser::~CUser() {
//	for(int i = 0; i < QUEUE_SIZE; ++i)

//			free(queue.pack[i]);	//queue.pack 메모리 해제
}

// Client로 부터 Packet 받기
int CUser::Recv_data(int fd, int cUser_num, list<string> *data_list) {

	m_clnt_sock = fd;
	m_data_list = data_list;
	PACKET recv_pack;
	while (1)
	{
//		Client로부터 Packet 받기
		m_strlen = recv(m_clnt_sock, (char*) &recv_pack, sizeof(PACKET), MSG_WAITALL);;

		cout << endl << "recv 실행" << endl;
		if (m_strlen == 0)
		{
			cout << "strlen: " << m_strlen << endl;
			return ERR;
		}

		else if (m_strlen < 0)
		{
			if (errno == EAGAIN)
				break;
		}
		else
		{
			cout << "[Client " << fd << "] recv: " << recv_pack.body.cmd << ", " << recv_pack.body.data << endl;
//			if(!Check_packet(recv_pack))
			m_Circular_buffer.Enqueue(&queue, recv_pack);
			cout << "[Client " << fd << "] Buffer: " << queue.data << endl;
			break;
		} //else end
	}
	return 0;
}

// 큐에 send할 패킷이 있는지 확인
int CUser::Queue_check()
{
	if(m_Circular_buffer.Isempty_queue(&queue))
		return ERR;								//큐 비었으면 return -1
	return 0;
}

// Packet Parsing
int CUser::Parsing_data()
{
	if(Find_packet())
	{
		puts("[ERROR] packet not find");
		return ERR;
	}
	/*
	if(CheckPacket()==ERR)
	{
		puts("[ERROR] CheckPacket error");
		m_CircularBuf.Dequeue(&queue);
		pack.cmd = CMD_USER_ERR;
		strcpy(pack.data, "Packet Error");
		return SendData(pack);
	}
	else
	{*/
//		memcpy(&pack, queue.pack[queue.front], sizeof(pack));		//처리할 패킷 큐에서 꺼내서 pack에 복사
//		m_Circular_buffer.Dequeue(&queue);

		switch (m_parsing_pack.body.cmd) {
			case CMD_USER_LOGIN_REQ:
				//Client에 패킷 전송
				m_parsing_pack.body.cmd = CMD_USER_LOGIN_RESULT;
				sprintf(m_parsing_pack.body.data, "%d", m_clnt_sock);
				return Send_data(m_parsing_pack);

				//Echo Data 처리
			case CMD_USER_DATA_REQ:
				m_parsing_pack.body.cmd = CMD_USER_DATA_RESULT;
				return Send_data(m_parsing_pack);

			//Data 저장 처리
			case CMD_USER_SAVE_REQ:
				m_parsing_pack.body.cmd = CMD_USER_SAVE_RESULT;
				/*if(! m_Data_mng.Insert_data(pack, m_data_list))
					strcpy(pack.data, "[SUCCESS] Data 저장 성공");
				else
					strcpy(pack.data, "[FAIL] 이미 저장된 데이터");

				if(Send_data(pack) == ERR)
					return ERR;
				else
				{
					//리스트 프린트
					if(m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
						return ERR;
					return 0;
				}*/
				return 0;

			//Data 삭제 처리
			case CMD_USER_DELETE_REQ:
				m_parsing_pack.body.cmd = CMD_USER_DELETE_RESULT;
				/*if(! m_Data_mng.Delete_data(m_parsing_pack, m_data_list))
					strcpy(m_parsing_pack.body.data, "[SUCCESS] Data 삭제 성공");
				else
					strcpy(m_parsing_pack.body.data, "[FAIL] list에 해당 데이터 없음");

				if(Send_data(m_parsing_pack) == ERR)
					return ERR;
				else
				{
					if(m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
						return ERR;
					return 0;
				}*/
				return 0;

			//Data list 출력
			case CMD_USER_PRINT_REQ:
				m_parsing_pack.body.cmd = CMD_USER_PRINT_RESULT;
				strcpy(m_parsing_pack.body.data, "CMD_USER_PRINT_RESULT");
				/*if(Send_data(pack) == ERR)
					return ERR;
				else
				{
					if(m_Data_mng.Send_data_list(m_clnt_sock, m_data_list) == ERR)
						return ERR;
					return 0;
				}*/
				return 0;

			default:
				m_parsing_pack.body.cmd = CMD_USER_ERR;
				strcpy(m_parsing_pack.body.data, "Request ERROR");
				puts("[ERROR] request error");
				return Send_data(m_parsing_pack);
		} //switch
//	}

	return 0;
}

// Client로 Packet 보내기
int CUser::Send_data(PACKET send_pack)
{
	cout << send_pack.body.data << endl;
	if(send(m_clnt_sock, (char*) &send_pack, sizeof(PACKET), 0) == -1)
		return ERR;
	return 0;
}

//큐에 저장된 패킷 데이터 정상인지 확인 (Packet size 초과 시 예외 처리 하기 (데이터 끊어서 받기))
int CUser::Find_packet()
{
	memset(&m_parsing_pack, '\0', sizeof(m_parsing_pack));
	HEADER hcheck;
	TAIL tcheck;
	int tindex;

	cout << "front: " << queue.front << ", rear: " << queue.rear << endl;
	for (int i = queue.front; queue.rear != NEXT(i); ++i)
	{
		memcpy(&hcheck, &queue.data[i], HEAD_SIZE);
		//큐에서 head 찾기
		if (!strcmp(hcheck.head, "AA11"))
		{
			tindex = HEAD_SIZE + 2 + hcheck.datasize;
			memcpy(&tcheck, &queue.data[tindex + i], TAIL_SIZE);
			//tail 확인 되면 큐에있는 데이터 패킷 변수(m_parsing_pack)에 넣기
			if (!strcmp(tcheck.tail, "11AA"))
			{
				memcpy(&m_parsing_pack, &queue.data[i], tindex);
				memcpy(&m_parsing_pack.tail, &queue.data[tindex + i], TAIL_SIZE);
				m_Circular_buffer.Dequeue(&queue, tindex + TAIL_SIZE);
				return 0;
			}
		}
	}//for

	return -1;
}

/*
int CUser::Recvn(int len)
{
	int recvd;
	char *ptr = m_recvbuff;
	int left = len;
	while(left > 0)
	{
		recvd = recv(m_clnt_sock, ptr, left, 0);
		if(recvd == -1)
			return -1;
		else if(recvd == 0)
			break;

		left -= recvd;
		ptr += recvd;
	}
	return (len-left);
}
*/



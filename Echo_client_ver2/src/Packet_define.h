/*
 * packetdefine.h
 *
 *  Created on: 2019. 8. 12.
 *      Author: mihyang
 */


#ifndef PACKET_DEFINE_H_
#define PACKET_DEFINE_H_

#define CMD_USER_LOGIN_REQ 0x01
#define CMD_USER_LOGIN_RESULT 0x02
#define CMD_USER_DATA_REQ 0x03
#define CMD_USER_DATA_RESULT 0x04
#define CMD_USER_SAVE_REQ 0x05
#define CMD_USER_SAVE_RESULT 0x06
#define CMD_USER_DELETE_REQ 0x07
#define CMD_USER_DELETE_RESULT 0x08
#define CMD_USER_PRINT_REQ 0x09
#define CMD_USER_PRINT_RESULT 0x0A
#define CMD_USER_ERR 0x0B

#define MAX_DATA_SIZE 1023
#define MAX_PRINT_DATA_SIZE 5000

#pragma pack(push, 1)	//1byte 정렬

typedef struct PACKET_HEADER
{
	char head[5] = {"AA11"};
	unsigned int datasize;
}HEADER;

typedef struct PACKET_BODY
{
	unsigned short cmd;
	char data[MAX_DATA_SIZE+1];
}BODY;

typedef struct PACKET_TAIL
{
	char tail[5] = {"11AA"};
}TAIL;

typedef struct MILESTONE_PACKET
{
	HEADER phead;
	BODY body;
//	unsigned short cmd;
//	char data[MAX_DATA_SIZE];
	TAIL ptail;
}PACKET;

typedef struct PRINT_PACKET
{
	unsigned short cmd;
	unsigned int data_num;
	char data[10];
}PRT_PACKET;

#pragma pack(pop)	//정렬 설정을 이전 상태(기본값)로 되돌림





#endif /* PACKET_DEFINE_H_ */

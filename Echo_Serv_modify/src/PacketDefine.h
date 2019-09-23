/*
 * packetdefine.h
 *
 *  Created on: 2019. 8. 12.
 *      Author: mihyang
 */


#ifndef PACKETDEFINE_H_
#define PACKETDEFINE_H_

#include <stdio.h>

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

#define MAX_PACKET_SIZE 8096

#pragma pack(push, 1)	//1byte 정렬

typedef struct Milestone_Packet
{
	char head[5] = {"AA11"};
	unsigned short cmd;
	int size;
	char data[MAX_PACKET_SIZE];
	char tail[5]={"11AA"};
}PACKET;

#pragma pack(pop);	//정렬 설정을 이전 상태(기본값)로 되돌림





#endif /* PACKETDEFINE_H_ */

//============================================================================
// Name        : EchoServerVer3.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// ----현재 수정중인 Server (2차 개발 발표 이후)----

#include <iostream>

#include "CUser_mng.h"
#include "Packet_define.h"

using namespace std;

int main() {
	cout << "< Server Open! >" << endl;
	CUser_mng cUser_mng;
	cUser_mng.Server_handling();

	return 0;
}

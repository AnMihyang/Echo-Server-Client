//============================================================================
// Name        : Echo_Server.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// ----1차 개발 발표 코드----

#include "CUserMng.h"
#include "PacketDefine.h"
#include <iostream>

using namespace std;

int main() {
	cout << "Server Open" << endl;
	CUserMng cUserMng;
	cUserMng.Handling();

	return 0;
}

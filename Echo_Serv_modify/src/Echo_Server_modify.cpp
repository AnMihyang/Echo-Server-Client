//============================================================================
// Name        : Echo_Server_modify.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "CUserMng.h"
#include "PacketDefine.h"
#include <iostream>

using namespace std;

int main() {
	cout << "< Server Open! >" << endl;
	CUserMng cUserMng;
	cUserMng.Server_Handling();

	return 0;
}

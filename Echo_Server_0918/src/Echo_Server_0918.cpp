//============================================================================
// Name        : Echo_Server_0918.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "CUserMng.h"
#include <iostream>

using namespace std;

int main() {
	cout << "Server Open" << endl;
	CUserMng cUserMng;
	cUserMng.ServerHandling();

	return 0;
}

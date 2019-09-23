//============================================================================
// Name        : Echo_Server_0918.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// ----1차 개발 발표 이후 구조 수정 (2차 개발 발표 코드)----
#include "CUserMng.h"
#include <iostream>

using namespace std;

int main() {
	cout << "Server Open" << endl;
	CUserMng cUserMng;
	cUserMng.ServerHandling();

	return 0;
}

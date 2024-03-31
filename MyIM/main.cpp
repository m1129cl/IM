#include<iostream>
#include"CMySql.h"
#include"TcpServerMediator.h"
using namespace std;
int main()
{
	CMySql mysql;
	INetMediator* p = new TcpServerMediator;
	return 0;
}
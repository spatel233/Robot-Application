#include <iostream>
#include "PKT_DEF.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

void main()
{
	//Local Variables
	SOCKET ListenSocket;
	SOCKET ConnectionSocket;
	PKT TxPKT;
	int check;

	//Configues ListenSocket
	if (TxPKT.Connect(ListenSocket) == 1)
	{
		while (1)
		{
			//Initialize Connect
			check = TxPKT.Accept(ConnectionSocket,ListenSocket);

			if (check == 5)
			{
				cout << "Connected To Client..." << endl;

				check = TxPKT.SetUpID(ConnectionSocket);

				//If SetUpID returns a 5 then the application will access the recieve function 
				if (check == 5)
				{
					TxPKT.recvPkt(ConnectionSocket);
				}
			}
		}
	}
	else
		cout << "Connection Failure" << endl;
}
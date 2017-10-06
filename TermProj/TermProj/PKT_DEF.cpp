#include <iostream>
#include "PkT_DEF.h"
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

PKT::PKT()
{
	//Initialize Values to 0
	Head.PacketID = 0;
	Head.CmdListSize = 0;
	body.Body = new char[Head.CmdListSize];
	Trailer.Parity = 0;
}

PKT::~PKT()
{
	//Deallocates body.Body
	delete[] body.Body;
}

int PKT::Connect(SOCKET &Listen)
{
	//Local Variable
	WSADATA wsaData;

	//Startup WSA
	if (WSAStartup((MAKEWORD(2,2)), &wsaData) != 0)
	{
		cout << "WSA did not start... ERROR WSAStartup" << endl;
		system("pause");
		return 0;
	}

	//SOCKADDR
	SOCKADDR_IN SvrAddr;
	int SvrAddrLen = sizeof(SvrAddr);
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(5000);


	//BIND AND LISTEN
	Listen = socket(AF_INET, SOCK_STREAM, NULL);
	if (Listen == INVALID_SOCKET)
	{
		cout << "ListenSocket is an invalid socket ... ERROR Listen " << endl;
		system("pause");
		return 0;
	}

	if ((bind(Listen, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
	{
		cout << "ERROR Bind Failure" << endl;
		system("pause");
		return 0;
	}

	if ((listen(Listen, 1)) == SOCKET_ERROR)
	{
		cout << "ERROR Listen Failure" << endl;
		system("pause");
		return 0;
	}

	//If all goes good it will return the passing value of 1
	return 1;
}

int PKT::SetUpID(SOCKET &Connect)
{
	//Local Variables
	int ans;
	bool pass = false;

	//Getting ID Info
	while (!pass)
	{
		cout << endl;
		cout << "Drive (15)" << endl;
		cout << "Status (5)" << endl;
		cout << "Sleep (0)" << endl;
		cout << "Value : "; cin >> ans;
		cout << endl;

		if (ans == 15 || ans == 5 || ans == 0)
			pass = true;
		else
			cout << "Wrong Entry, Please Try again " << endl;
	}

	//PacketID is being initialized
	Head.PacketID = ans;


	//Depending on the ID the swith will call functions
	switch (ans)
	{
	case 0:
	case 5:
		//Reinitialize
		Trailer.Parity = 0;
		Head.CmdListSize = 0;
		sendPkt(Connect);
		break;
	case 15:
		//Reinitialize
		Trailer.Parity = 0;
		Head.CmdListSize = 0;
		SetInfo();
		sendPkt(Connect);
		break;
	default:
		break;
	}
	if (ans == 5)
		return 5;
	else
		return 0;
}

void PKT::SetInfo()
{
	//local variables
	int Direction;
	int Duration;
	int ans;
	int i = 1;
	int j = 0;

	//Getting amount of commands to allocate
	cout << "Mars Rover Simulation" << endl;
	cout << "How many commands do you plan on entering : ";	cin >> ans;

	//Initialize ListSize and allocate body.Boyd using CmdListSize times 2 for Direction and Duration
	Head.CmdListSize = ans;
	body.Body = new char[Head.CmdListSize * 2];

	//Populating Commands into New allocated array
	//Displays commands for user to choose from
	cout << "Drive Command Directives" << endl;
	cout << "Forward (1)" << endl;
	cout << "Backward (2)" << endl;
	cout << "Left(3)" << endl;
	cout << "Right(4)" << endl;
	cout << endl;
	
	do
	{
		//Getting Direction Value
		cout << "Direction : " << i << " ";
		cin >> Direction;

		//Checking Direction for appropriate values
		while (Direction <=0 || Direction > 4)
		{
			cout << "Inappropriate Direction Value. Your value has to be one of the following showed above " << endl;
			cout << "Re-enter Direction : " << i << " "; cin >> Direction;
		}

		//Putting Direction into array
		body.Body[j++] = Direction;
		
		//Getting Duration Value
		cout << "Duration : " << i << " "; cin >> Duration;

		//Checking Duration for appropriate values
		while (Duration <= 0 || Duration > 127)
		{
			cout << "ERROR Duration..." << endl;
			cout << "Inappropriate Duration Value. Your value must be greater than 0 and less than 128... " << endl;
			cout << "Re-enter Duration : " << i << " "; cin >> Duration;
		}

		//Putting Duration into array
		body.Body[j++] = Duration;

		//Calculating the Parity for the input numbers
		getParity_Dir_Dur(Direction, Duration);

		i++;
	} while (i <= Head.CmdListSize);
}

void PKT::sendPkt(SOCKET &Connect)
{
	//Local Variables
	int i;

	//Initialize size two 3 bytes for PacketID CmdListSize and 
	//Parity values and multipy by two to get size of body.Body
	int size = 3;
	size += Head.CmdListSize * 2;

	//Allocate new Buffer
	char * TxBuffer = new char[size];

	//Fill up Buffer with values
	TxBuffer[0] = (int)Head.PacketID;
	TxBuffer[1] = (int)Head.CmdListSize;

	for (i = 0; i <= (Head.CmdListSize * 2); i++)
		TxBuffer[i + 2] = body.Body[i];

	//Calculate Parity Values For the header
	getParity();

	i++;

	TxBuffer[i] = Trailer.Parity;

	send(Connect, (char *)TxBuffer, size, 0);
}

void PKT::getParity()
{
	//Local Variable
	int remainder = 0;

	//Hard coded parity Calculator
	if (Head.PacketID == 15)
		Trailer.Parity += 4;
	else if (Head.PacketID == 5)
		Trailer.Parity += 2;
	else
		Trailer.Parity += 0;

	if (Head.CmdListSize == 10 || Head.CmdListSize == 9 || Head.CmdListSize == 6 || Head.CmdListSize == 5 || Head.CmdListSize == 3)
		Trailer.Parity += 2;
	else if (Head.CmdListSize == 1 || Head.CmdListSize == 2 || Head.CmdListSize == 4 || Head.CmdListSize == 8)
		Trailer.Parity += 1;
	else if (Head.CmdListSize == 7)
		Trailer.Parity += 3;
	else
		Trailer.Parity += 0;
}

void PKT::getParity_Dir_Dur(int Direction, int Duration)
{
	//Local Variables
	int temp_Dir = Direction;
	int temp_Dur = Duration;
	int remainder = 0;


	//Calculating Parity for Direction
	while (temp_Dir != 0)
	{
		remainder = temp_Dir % 2;
		temp_Dir /= 2;
		if (remainder == 1)
			Trailer.Parity += 1;
	}
	
	//Calculating Parity for Duration
	while (temp_Dur != 0)
	{
		remainder = temp_Dur % 2;
		temp_Dur /= 2;
		if (remainder == 1)
			Trailer.Parity += 1;
	}

}

void PKT::recvPkt(SOCKET &Connect)
{
	//Local Variables
	int tempParity;
	bool pass;
	int size;
	char RxBuffer[128];
	recv(Connect, (char *)RxBuffer, sizeof(RxBuffer), 0);

	//Assigning Head Variables
	Head.PacketID = RxBuffer[0];
	Head.CmdListSize = RxBuffer[1];

	//Deallocate old body.Body and Allocate new body.Body depending on Head.CmdListSize
	delete[] body.Body;
	body.Body = new char[Head.CmdListSize];

	//Building Body
	for (int i = 0; i < Head.CmdListSize; i++)
		body.Body[i] = RxBuffer[i + 2];
	
	//Calculating Parity
	size = (Head.CmdListSize + 2);
	tempParity = RxBuffer[size];
	pass = getParity(tempParity);

	if (pass)
		display();
	else
		cout << "Package error" << endl;
}

void PKT::display() const
{
	cout << "Packet ID : " << Head.PacketID << endl;
	cout << "List Size : " << Head.CmdListSize << endl;

	cout << "Body : ";
	for (int i = 0; i < Head.CmdListSize; i++)
		cout << (int)body.Body[i] << ", ";

	cout << endl;
	cout << "Parity : " << Trailer.Parity << endl;
	cout << endl;
}

int PKT::Accept(SOCKET &ConnectionSocket, SOCKET &ListenSocket)
{
	//Initialize the ConnectionSocket to SOCKET_ERROR
	ConnectionSocket = SOCKET_ERROR;

	cout << "Waiting For Client Connection..." << endl;

	//Waiting for a connection
	if ((ConnectionSocket = accept(ListenSocket, NULL, NULL)) == SOCKET_ERROR)
	{
		cout << "ERROR ACCEPT FUNCTION FAIL" << endl;
		system("pause");
		closesocket(ConnectionSocket);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	//If Everything Passes it will return the passing value of 5
	return 5;
}

bool PKT::getParity(int tempParity)
{

	//Local Variable
	int remainder = 0;
	int tempBody;
	Trailer.Parity = 0;

	//Hard coded parity Calculator
	if (Head.PacketID == 15)
		Trailer.Parity += 4;
	else if (Head.PacketID == 5)
		Trailer.Parity += 2;
	else
		Trailer.Parity += 0;

	if (Head.CmdListSize == 10 || Head.CmdListSize == 9 || Head.CmdListSize == 6 || Head.CmdListSize == 5 || Head.CmdListSize == 3)
		Trailer.Parity += 2;
	else if (Head.CmdListSize == 1 || Head.CmdListSize == 2 || Head.CmdListSize == 4 || Head.CmdListSize == 8)
		Trailer.Parity += 1;
	else if (Head.CmdListSize == 7)
		Trailer.Parity += 3;
	else
		Trailer.Parity += 0;

	//Calculates Parity on body.Body Values
	for (int i = 0; i < Head.CmdListSize; i++)
	{
		tempBody = (int)body.Body[i];
		
		while (tempBody != 0)
		{
			remainder = tempBody % 2;
			tempBody /= 2;
			if (remainder == 1)
				Trailer.Parity += 1;
		}
	}

	//Checks to see if they are not the same values as the one sent
	if (tempParity != Trailer.Parity)
		return false;
	else
		return true;
}
#include <Windows.networking.sockets.h>
class PKT
{
	struct PKT_Header
	{
		unsigned int PacketID : 8;
		unsigned int CmdListSize : 8;
	}Head;
	struct PKT_Body
	{
		char *Body;
	}body;
	struct PKT_Trailer
	{
		unsigned int Parity : 8;
	}Trailer;
public:
	PKT();	                            //PKT Constructor
	~PKT();	                            //PKT Destructor
	int Connect(SOCKET &);	            //Sets up Server and configures ListeningSocket socket
	int Accept(SOCKET &, SOCKET &);	    //Configures ConnectionSocket and waits for client
	void SetInfo();	                    //Sets up info for body
	int SetValue(char);                 //sets values for direction and duration
	int SetUpID(SOCKET &);	            //Sets up the ID of the Packet
	void sendPkt(SOCKET &);	            //Sends the packet over the network
	void recvPkt(SOCKET &);	            //Recieves packet from client 
	void display() const;	            //Displays packet
	bool getParity(int);	            //Validates Recieving Packet 
	void getParity();	                //Calculates the parity for the Header Only
	void getParity_Dir_Dur(int, int);	//Calculates the parity for the body
};
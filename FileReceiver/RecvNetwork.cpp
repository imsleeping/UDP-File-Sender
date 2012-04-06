
#include <limits.h>
#include <WinSock2.h>
#include <stdio.h>

#include "RecvNetwork.h"
#include "Definitions.h"

SOCKET SendSocket = INVALID_SOCKET,
		RecvSocket = INVALID_SOCKET;
sockaddr_in RecvAddr, 
			SendAddr;

int startConnection(int port)
{

	if (SendSocket != INVALID_SOCKET || RecvSocket != INVALID_SOCKET)
	{
		return 0;
	}

	WSADATA wsaData;
	int iResult;

	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 0;
	}

	//---------------------------------------------
	// Create a socket for sending data
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		closeConnection();
		return 0;
	}

	//---------------------------------------------
	// Create a socket for recving data
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		closeConnection();
		return 0;
	}
	//-----------------------------------------------
    // Bind the socket to any address and the specified port.
    SendAddr.sin_family = AF_INET;
    SendAddr.sin_port = htons(port);
    SendAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    iResult = bind(RecvSocket, (SOCKADDR *) & SendAddr, sizeof (SendAddr));
    if (iResult != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
		closeConnection();
        return 0;
    }

	//---------------------------------------------
	// Set up the RecvAddr structure with the IP address of
	// the receiver
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port+1);

	if (recvHandshake() != MODE_READY)
	{

		printf("Sender not ready");
		closeConnection();
		return 0;

	}

	sendHandshake(MODE_READY);

	// set receive timeout to 1
	if (!setTimeout(RecvSocket,-1))
	{

		closeConnection();
		return 0;

	}

	return 1;

}

int setTimeout(SOCKET s, int timeout)
{

    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(int)) == SOCKET_ERROR)
	{
		return 0;
	}

	return 1;

}

int connectionOpen()
{

	if (SendSocket != INVALID_SOCKET && RecvSocket != INVALID_SOCKET)
		return 1;
	
	return 0;

}

int sendHandshake(byte mode)
{

	PacketHandshake handshake;

	if (SendSocket == INVALID_SOCKET)
		return 0;

	handshake.mode = mode;

	sendto(SendSocket, (char*)&handshake, sizeof(PacketHandshake), 0, (sockaddr*) &RecvAddr, sizeof(RecvAddr));

	return 1;

}

int recvHandshake()
{

	sockaddr_in SenderAddr;
	int iResult, SenderAddrSize = sizeof (SenderAddr);
	PacketHandshake handshake;

	if (RecvSocket == INVALID_SOCKET)
		return 0;

	iResult = recvfrom(RecvSocket, (char *)&handshake, sizeof(handshake), 0, (sockaddr*) &SenderAddr, &SenderAddrSize);
	
	if (iResult <= 0)
	{

		iResult = WSAGetLastError();
		if (iResult != WSAETIMEDOUT)
		{
			printf("Socket error: %i", iResult);
		}

		return 0;

	}

	// send sender address 
	RecvAddr.sin_addr = SenderAddr.sin_addr;

	return handshake.mode;

}

int sendPacket(PacketACK* packet)
{

	if (SendSocket == INVALID_SOCKET)
		return 0;

	sendto(SendSocket, (char*)packet, sizeof(PacketData), 0, (sockaddr*) &RecvAddr, sizeof(RecvAddr));

	return 1;

}

void handleCloseHandshake()
{

	if (recvHandshake() == MODE_COMPLETE)
		sendHandshake(MODE_COMPLETE);
}

int recvPacket(PacketData* packet)
{

	sockaddr_in SenderAddr;
	int iResult, SenderAddrSize = sizeof (SenderAddr);

	if (RecvSocket == INVALID_SOCKET)
		return 0;

	iResult = recvfrom(RecvSocket, (char *)packet, sizeof(PacketData), 0, (sockaddr*) &SenderAddr, &SenderAddrSize);
	
	if (iResult <= 0)
	{

		iResult = WSAGetLastError();
		if (iResult != WSAETIMEDOUT)
		{
			printf("Socket error: %i", iResult);
		}

		return 0;

	}

	return 1;

}

seqnum increment(seqnum i)
{

	int j = i + 1;
	return j % USHRT_MAX;

}

void closeConnection()
{

	if (SendSocket != INVALID_SOCKET)
	{
		closesocket(SendSocket);
		SendSocket = INVALID_SOCKET;
	}

	if (RecvSocket != INVALID_SOCKET)
	{
		closesocket(RecvSocket);
		RecvSocket = INVALID_SOCKET;
	}

	WSACleanup();

}

int isCorrupt(PacketData* packet)
{

	unsigned short checksum = calculateChecksum(packet);
	if (checksum != packet->checksum)
		return 1;

	return 0;

}

unsigned short calculateChecksum(PacketData* packet)
{

	int i, tmpChecksum = 0;

	for (i = 0; i < packet->length; i++)
	{
		
		tmpChecksum = tmpChecksum + packet->payload[i];
		
	}

	return tmpChecksum % USHRT_MAX;

}

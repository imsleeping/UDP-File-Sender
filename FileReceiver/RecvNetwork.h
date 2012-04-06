
#include "Definitions.h"

#include <WinSock2.h>

const int WINDOW_SIZE = 32;

int startConnection(int port);
int setTimeout(SOCKET s, int timeout);
int connectionOpen();

int sendHandshake(byte mode);
int recvHandshake();
int sendPacket(PacketACK* packet);
void handleCloseHandshake();
int recvPacket(PacketData* packet);

seqnum increment(seqnum i);

void closeConnection();

int isCorrupt(PacketData* packet);
unsigned short calculateChecksum(PacketData* packet);

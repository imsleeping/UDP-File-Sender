
#include "Definitions.h"

#include <WinSock2.h>

const int WINDOW_SIZE = 32;
const int TIMEOUT = 1000;

int startConnection(char* ip, int port);
int connectionOpen();
void closeConnection();

seqnum increment(seqnum i);

int setTimeout(SOCKET s, int timeout);
int recvHandshake();
int sendHandshake(byte mode);
void handleCloseHandshake();
int sendPacket(PacketData* packet);
int recvPacket(PacketACK* packet);

int isCorrupt(PacketData* packet);
void insertChecksum(PacketData* packet);
unsigned short calculateChecksum(PacketData* packet);

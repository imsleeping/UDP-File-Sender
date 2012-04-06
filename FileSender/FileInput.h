
const int CACHE_SIZE = 1024; // ~1mb cache

int loadFile(char* filename);
int getFileError();
int getPacket(PacketData* packet);
int countPackets();
void closeFile();

UINT PipeProcedure( LPVOID pParam );
void WritePipe(void* buffer, unsigned char bytesToWrite);

#define BUFFERSIZE 32

union COMMANDBUFFER {
	unsigned char command[BUFFERSIZE];
	struct {
		BYTE type;
		unsigned char body[BUFFERSIZE-1];
	};
};

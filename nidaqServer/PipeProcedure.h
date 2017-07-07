#pragma once

UINT PipeProcedure( LPVOID pParam );
//void WritePipe(void* buffer, unsigned char bytesToWrite);

#define BUFFERSIZE 32
#define EVEN(N) (((N) & 1) == 0)

union COMMANDBUFFER {
	unsigned char command[BUFFERSIZE];
	struct {
		BYTE type;
		unsigned char body[BUFFERSIZE-1];
	};
};

#include "stdafx.h"
#include "PipeProcedure.h"
#include "nidaqServer.h"
#include "nidaqServerDoc.h"
#include "Reward.h"
#include "Log.h"

HANDLE hPipe;

UINT PipeProcedure( LPVOID pParam ) {
	
	extern CnidaqServerApp theApp;
	HRESULT hr = S_OK;
//	short key;
	COMMANDBUFFER commandBuffer;
	DWORD messageLength;
	hr = CoInitializeEx(
		NULL,
		COINIT_MULTITHREADED);
	ASSERT(hr == S_OK);

	POSITION pos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* temp = theApp.GetNextDocTemplate(pos);
	pos = temp->GetFirstDocPosition();
	CnidaqServerDoc* pDoc = (CnidaqServerDoc*) temp->GetNextDoc(pos);

	hPipe = CreateNamedPipe(
		_T("\\\\.\\pipe\\NidaqServerPipe"),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1,			// max. number of instances
		8,			// size of output buffer
		BUFFERSIZE,	// size of input buffer
		0,			// default time out 50ms
		NULL);
	ASSERT(hPipe != INVALID_HANDLE_VALUE);

	// Documentation of "CreateNamedPipe Function" states:
	// The pipe server should not perform a blocking read operation until the pipe client has started.
	// Otherwise, a race condition can occur. This typically occurs when initialization code, such as
	// the C run-time, needs to lock and examine inherited handles.
	while (true) {
		DWORD error = 0;
		VERIFY(ConnectNamedPipe(hPipe, NULL));
		TRACE("Client connected to Pipe.\n");
		while (true) {
//			TRACE("Waiting on Pipe.\n");
			if (error = !ReadFile(hPipe, &commandBuffer, BUFFERSIZE, &messageLength, NULL)) {
				error = GetLastError();
				switch (error) {
				case ERROR_BROKEN_PIPE:
					TRACE("The pipe has been ended.\n");
					break;
				default:
					TRACE("Error: %x\n", error);
					ASSERT(FALSE);
				}
			}
			if (error == ERROR_BROKEN_PIPE) break;
			ASSERT(error == 0);
//			TRACE("Message received. Length:%u; Key: %u\n", messageLength, commandBuffer.key);
			//key = pDoc->Command(commandBuffer.key, &commandBuffer.body[0], messageLength-2);
			//if (key != -1) {
			//	DWORD bytesWritten;
			//	VERIFY(WriteFile(hPipe, &key, 2, &bytesWritten, NULL));
			//	ASSERT(bytesWritten == 2);
			//}
			switch (commandBuffer.type)
			{
			case 1:	// add pulse-line to change detection
				if (CChangeDetection::Running())
				{
					CLog::AddToLog(CString("Will not add line to running task."));
				}
				else
				{
					char* pulseName = (char*) &commandBuffer.body[1];
					TRACE("PulseEventName: %s\n", pulseName);
					CChangeDetection::AddLine(commandBuffer.body[0], pulseName);
					//				theApp.m_pDevice->AddInputLine(commandBuffer.body[0], onName, offName);
					//				theApp.m_pChangeDetection->AddLine(commandBuffer.body[0], onName, offName);
				}
				break;
			case 2:	// add on/off-line to change detection
				if (CChangeDetection::Running())
				{
					CLog::AddToLog(CString("Will not add line to running task."));
				}
				else
				{
					char* onName = (char*) &commandBuffer.body[1];
					char* offName = (char*) &commandBuffer.body[2+strlen(onName)];
					TRACE("EventNames: %s %s\n", onName, offName);
					CChangeDetection::AddLine(commandBuffer.body[0], onName, offName);
					//				theApp.m_pDevice->AddInputLine(commandBuffer.body[0], onName, offName);
					//				theApp.m_pChangeDetection->AddLine(commandBuffer.body[0], onName, offName);
				}
				break;
			case 3: // start change detection
				if (CChangeDetection::Running())
				{
					CLog::AddToLog(CString("Will not start running task."));
				}
				else
				{
					CChangeDetection::Start();
					//				theApp.m_pDevice->StartChangeDetection();
					//				theApp.m_pChangeDetection->Start();
				}
				break;
			case 4: // set reward pulse duration
				if (messageLength != 3)
				{
					CLog::AddToLog(CString("Command length error in set reward pulse duration."));
				}
				else
				{
					pDoc->m_rewardTime = *((short*) &commandBuffer.body[0]);
				}
				break;
			case 5: // start reward sequence
				// if messageLength is even, it's caught in the error check to follow
				// nParams is invalid in this case
				BYTE nParams = (messageLength-1) / 2;
				if (EVEN(messageLength) || EVEN(nParams))
				{
					CLog::AddToLog(CString("Command length error in start reward sequence."));
				}
				else
				{
					CReward::StartSequence(nParams, (short*) &commandBuffer.body[0]);
				}
				break;
			}
		}
		VERIFY(DisconnectNamedPipe(hPipe));	// neccessary for re-connect
	}
	CloseHandle(hPipe);
	TRACE("Destruct Pipe.\n");
	CoUninitialize();
	return 0;	// pretend success
}

//void WritePipe(void* buffer, unsigned char bytesToWrite)
//{
//	DWORD bytesWritten;
//
//	VERIFY(WriteFile(hPipe, buffer, bytesToWrite, &bytesWritten, NULL));
//	ASSERT(bytesWritten == bytesToWrite);
//}
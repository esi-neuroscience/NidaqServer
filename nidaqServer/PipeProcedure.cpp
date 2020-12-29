#include "stdafx.h"
#include "PipeProcedure.h"
#include "nidaqServer.h"
#include "nidaqServerDoc.h"
//#include "Reward.h"
#include "DAQmx.h"
#include "Log.h"

extern HANDLE hDaqServerDone;
HANDLE hPipe;
HANDLE hPipeDone;

bool IsAddChangeDetectionLineValid(BYTE lineNumber)
{
	if (CChangeDetection::Running())
	{
		CLog::AddToLog(CString("Will not add line to running task."));
		return false;
	}
	return true;
}


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
	VERIFY(SetEvent(hPipeDone));
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
			BYTE lineNumber = 255;
			short pulseTime = 0;
			switch (commandBuffer.type)
			{
			case 1:	// add pulse-line to change detection
				if (IsAddChangeDetectionLineValid(commandBuffer.body[0]))
				{
					char* pulseName = (char*) &commandBuffer.body[1];
					TRACE("PulseEventName: %s\n", pulseName);
					CChangeDetection::AddLine(commandBuffer.body[0], pulseName);
				}
				break;
			case 2:	// add on/off-line to change detection
				if (IsAddChangeDetectionLineValid(commandBuffer.body[0]))
				{
					char* onName = (char*) &commandBuffer.body[1];
					char* offName = (char*) &commandBuffer.body[2+strlen(onName)];
					TRACE("EventNames: %s %s\n", onName, offName);
					CChangeDetection::AddLine(commandBuffer.body[0], onName, offName);
				}
				break;
			case 3: // start change detection
				if (CChangeDetection::Running())
				{
					CLog::AddToLog(CString("Will not start running task."));
					VERIFY(SetEvent(hDaqServerDone));
				}
				else
				{
					CChangeDetection::Start();
				}
				break;
			case 4: // set output pulse duration
				lineNumber = 255;
				pulseTime = 0;
				switch (messageLength)
				{
				case 3:
					lineNumber = 3;	// reward (is default)
					pulseTime = *((short*)&commandBuffer.body[0]);
					break;
				case 4:
					lineNumber = commandBuffer.body[0];
					pulseTime = *((short*)&commandBuffer.body[1]);
					break;
				default:
					CLog::AddToLog(CString("Command length error in set output pulse duration."));
					break;
				}
				if (lineNumber != 255)
				{
					((CReward*)pDoc->m_pauxLines[lineNumber])->m_rewardTime = pulseTime;
				}
				break;
			case 5: // start reward sequence
				{
					// if messageLength is even, it's caught in the error check to follow
					// nParams is invalid in this case
					BYTE nParams = (BYTE) ((messageLength-1) / 2);
					if (EVEN(messageLength) || EVEN(nParams))
					{
						CLog::AddToLog(CString("Command length error in start reward sequence."));
					}
					else
					{
						((CReward*)pDoc->m_pauxLines[3])->StartSequence(nParams, (short*)&commandBuffer.body[0]);
				//		CReward::StartSequence(nParams, (short*) &commandBuffer.body[0]);
					}
				}
				break;
			case 6: // write event marker
				ASSERT(messageLength == 3);
				if (messageLength != 3)
				{
					CLog::AddToLog(CString("Command length error in write event marker."));
				}
				else
				{
					CDAQmx::WriteEventMarker((short*) &commandBuffer.body[0]);
				}
				break;
			case 7: // set eventmarker code for manual reward
				if (messageLength != 3)
				{
					CLog::AddToLog(CString("Command length error in set manual reward marker code."));
				}
				else
				{
					((CReward*)pDoc->m_pauxLines[3])->m_rewardCode = *((short*)&commandBuffer.body[0]);
				}
				break;
			case 8:	// query total reward time
				DWORD totalTime;
				totalTime = ((CReward*)pDoc->m_pauxLines[3])->TotalTime();
				WritePipe(&totalTime, sizeof(DWORD));
				break;
			case 9:	// add output pulse line
				lineNumber = commandBuffer.body[0];
				if (pDoc->m_pauxLines[lineNumber])	// check if line is valid (still unused)
				{
					CString errstr;
					errstr.Format("Can't use line %u for pulse output", lineNumber);	//TODO:: is already used for "name"
					CLog::AddToLog(errstr);
				}
				else
				{
					char* pulseName = (char*)&commandBuffer.body[1];
					TRACE("Pulse Line Name: %s\n", pulseName);
					pDoc->m_pauxLines[lineNumber] = new CPulseLine(lineNumber, pulseName);
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

void WritePipe(void* buffer, unsigned char bytesToWrite)
{
	DWORD bytesWritten;

	VERIFY(WriteFile(hPipe, buffer, bytesToWrite, &bytesWritten, NULL));
	ASSERT(bytesWritten == bytesToWrite);
}

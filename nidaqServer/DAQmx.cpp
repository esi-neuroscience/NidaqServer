#include "StdAfx.h"
#include "DAQmx.h"
#include "Log.h"

#pragma comment(lib, "NIDAQmx.lib")

uInt32 leverLineMask;

static void DAQCheckStatus(void)
{
	if (DAQstatus == 0) return;
	CString errMsg;
	uInt32 msgSize;
	char* msg;
	int32 status;
	errMsg = (DAQstatus < 0 ? _T("Error: ") : _T("Warning: "));
	msgSize = DAQmxGetErrorString(DAQstatus, NULL, 0);
	msg = new char[msgSize];
	status = DAQmxGetErrorString(DAQstatus, msg, msgSize);
	ASSERT(status == 0);
	errMsg.Append(CString(msg));
	TRACE("%S\n", errMsg);
	delete msg;
	msgSize = DAQmxGetExtendedErrorInfo(NULL, 0);
	msg = new char[msgSize];
	status = DAQmxGetExtendedErrorInfo(msg, msgSize);
	ASSERT(status == 0);
	TRACE("%s\n", msg);
	delete msg;
//	ASSERT(false);
}

CDAQmxTask::CDAQmxTask(void)
{
	TRACE("DAQmxTask Konstruktor\n");
	DAQstatus = DAQmxCreateTask("DAQmxTask", &m_taskHandle);
	DAQCheckStatus();
}


CDAQmxTask::~CDAQmxTask(void)
{
	DAQstatus = DAQmxClearTask(m_taskHandle);
	DAQCheckStatus();
	TRACE("DAQmxTask Destruktor\n");
}


void CDAQmxTask::CreateDOChan(char* lines, char* name)
{
	DAQstatus = DAQmxCreateDOChan(m_taskHandle, lines, name, DAQmx_Val_ChanPerLine);
	DAQCheckStatus();
	TRACE("DAQmxTask Create Channel\n");
}


void CDAQmxTask::CreateCOPulseChanTime(char* counter, char* name, float64 highTime)
{
	DAQstatus = DAQmxCreateCOPulseChanTime(m_taskHandle, counter, name, DAQmx_Val_Seconds, DAQmx_Val_Low, 0.0, 0.00002, highTime);
	DAQCheckStatus();
	TRACE("DAQmxTask Pulse Channel\n");
	DAQstatus = DAQmxSetCOCtrTimebaseRate(m_taskHandle, name, 100000.0);
	DAQCheckStatus();
}


CESI_Reward::CESI_Reward(void)
{
	DAQstatus = DAQmxConnectTerms("/Dev1/Ctr1InternalOutput", "/Dev1/PFI11", DAQmx_Val_DoNotInvertPolarity);
	DAQCheckStatus();
	TRACE("Reward Konstruktor\n");
}


CESI_Reward::~CESI_Reward(void)
{
	DAQstatus = DAQmxDisconnectTerms("/Dev1/Ctr1InternalOutput", "/Dev1/PFI11");
	DAQCheckStatus();
	TRACE("Reward Destruktor\n");
}


void CESI_Reward::Start(void)
{
	DAQstatus = DAQmxStartTask(m_taskHandle);
	DAQCheckStatus();
	DAQstatus = DAQmxWaitUntilTaskDone(m_taskHandle, -1);
	DAQCheckStatus();
	DAQstatus = DAQmxStopTask(m_taskHandle);
	DAQCheckStatus();
}



int32 CVICALLBACK LeverCallback(TaskHandle taskHandle, int32 signalID, void *callbackData)
{
	uInt32 value = 0;
	int32 sampsRead;
	int32 numBytesPerSamp;
	DAQstatus = DAQmxReadDigitalScalarU32(taskHandle, 0.0, &value, NULL);
	DAQCheckStatus();
	value &= leverLineMask;
	CString temp;
	if (value)
	{
		VERIFY(ResetEvent(((CESI_Lever*)callbackData)->m_hReleaseEvent));
		VERIFY(SetEvent(((CESI_Lever*)callbackData)->m_hPressEvent));
		CLog::AddToLog(_T("Lever press"));
	}
	else
	{
		VERIFY(ResetEvent(((CESI_Lever*)callbackData)->m_hPressEvent));
		VERIFY(SetEvent(((CESI_Lever*)callbackData)->m_hReleaseEvent));
		CLog::AddToLog(_T("Lever release"));
	}
	return 0;
}


CESI_Lever::CESI_Lever(void)
{
	VERIFY(m_hPressEvent = CreateEvent(NULL, FALSE, FALSE, _T("LeverPressEvent")));
	VERIFY(m_hReleaseEvent = CreateEvent(NULL, FALSE, FALSE, _T("LeverReleaseEvent")));
// change detection works on port 0 only! The LEVER_LINE is defined in the header file.
	char leverLine[17] = "Dev1/port0/lineX";
	leverLine[15] = '0'+LEVER_LINE;
	DAQstatus = DAQmxCreateDIChan(m_taskHandle, leverLine, "LeverLine", DAQmx_Val_ChanPerLine);
	DAQCheckStatus();
	DAQstatus = DAQmxCfgChangeDetectionTiming(m_taskHandle, leverLine, leverLine, DAQmx_Val_HWTimedSinglePoint, 0);
	DAQCheckStatus();
	DAQstatus = DAQmxRegisterSignalEvent(m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, LeverCallback, this);
	DAQCheckStatus();
	leverLineMask = 1 << LEVER_LINE;
	TRACE("Lever Konstruktor for: %s, Mask: %u\n", leverLine, leverLineMask);
	DAQstatus = DAQmxStartTask(m_taskHandle);
	DAQCheckStatus();
}


CESI_Lever::~CESI_Lever(void)
{
	DAQstatus = DAQmxStopTask(m_taskHandle);
	DAQCheckStatus();
	// unregister the callback
	DAQstatus = DAQmxRegisterSignalEvent(m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, NULL, NULL);
	DAQCheckStatus();
	VERIFY(CloseHandle(m_hPressEvent));
	VERIFY(CloseHandle(m_hReleaseEvent));
	TRACE("Lever Destruktor\n");
}

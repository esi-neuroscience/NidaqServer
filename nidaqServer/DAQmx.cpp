#include "StdAfx.h"
#include "DAQmx.h"
#include "Log.h"
#include <strsafe.h>

#pragma comment(lib, "NIDAQmx.lib")

TaskHandle CChangeDetection::m_taskHandle;
char CChangeDetection::m_lines[136] = {0};
BYTE CChangeDetection::m_nLines = 0;
BYTE CChangeDetection::m_lineMask = 0;
uInt32 CChangeDetection::m_value;
CChangeDetectionLine* CChangeDetection::m_pLines[8];

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
	ASSERT(false);
}

CDAQmxTask::CDAQmxTask(void)
{
	TRACE("DAQmxTask Konstruktor\n");
//	DAQstatus = DAQmxCreateTask("DAQmxTask", &m_taskHandle);
//	DAQCheckStatus();
}


CDAQmxTask::~CDAQmxTask(void)
{
//	DAQstatus = DAQmxClearTask(m_taskHandle);
//	DAQCheckStatus();
//	TRACE("DAQmxTask Destruktor\n");
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



int32 CVICALLBACK CChangeDetection::ChangeDetectionCallback(TaskHandle taskHandle, int32 signalID, void *callbackData)
{
	uInt32 value = 0;
	DAQstatus = DAQmxReadDigitalScalarU32(taskHandle, 0.0, &value, NULL);
	DAQCheckStatus();
	value &= m_lineMask;
	for (BYTE i=0; i<m_nLines; i++) m_pLines[i]->SignalEvent(value, value ^ m_value);
	m_value = value;
	return 0;
}


CChangeDetection::CChangeDetection(void)
{
	DAQstatus = DAQmxCreateTask("ChangeDetectionTask", &m_taskHandle);
	DAQCheckStatus();
}


CChangeDetection::~CChangeDetection(void)
{
	DAQstatus = DAQmxStopTask(m_taskHandle);
	DAQCheckStatus();
	// unregister the callback
	DAQstatus = DAQmxRegisterSignalEvent(m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, NULL, NULL);
	DAQCheckStatus();
	for (BYTE i=0; i<m_nLines; i++) delete m_pLines[i];
}


void CChangeDetection::Start(void)
{
	DAQstatus = DAQmxCreateDIChan(m_taskHandle, m_lines, "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
	DAQCheckStatus();
	DAQstatus = DAQmxReadDigitalScalarU32(m_taskHandle, 0.0, &m_value, NULL);
	DAQCheckStatus();
	m_value &= m_lineMask;
	DAQstatus = DAQmxCfgChangeDetectionTiming(m_taskHandle, m_lines, m_lines, DAQmx_Val_HWTimedSinglePoint, 0);
	DAQCheckStatus();
	DAQstatus = DAQmxRegisterSignalEvent(m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, ChangeDetectionCallback, NULL);
	DAQCheckStatus();
	DAQstatus = DAQmxStartTask(m_taskHandle);
	DAQCheckStatus();
}



void CChangeDetection::AddLine(BYTE lineNumber, char* onName, char* offName)
{
	char lineName[17] = "Dev1/port0/lineX";
	lineName[15] = '0'+lineNumber;
	if (m_lines[0] != 0) StringCbCatA(m_lines, 136, ",");
	StringCbCatA(m_lines, 136, lineName);
	TRACE("Channel: %s\n", m_lines);
	m_pLines[m_nLines] = new CChangeDetectionLine(lineNumber, onName, offName);
	m_lineMask |= m_pLines[m_nLines++]->m_mask;
	TRACE("Maske: %u\n", m_lineMask);
}


CChangeDetectionLine::CChangeDetectionLine(BYTE lineNumber, char* onName, char* offName)
{
	TRACE("ChangeDetectionLine Konstruktor\n");
	m_mask = (1 << lineNumber);
	VERIFY(m_onEvent = CreateEventA(NULL, FALSE, FALSE, onName));
	VERIFY(m_offEvent = CreateEventA(NULL, FALSE, FALSE, offName));
	m_onName = onName;
	m_offName = offName;
}

CChangeDetectionLine::~CChangeDetectionLine(void)
{
	TRACE("ChangeDetectionLine Destruktor\n");
	VERIFY(CloseHandle(m_onEvent));
	VERIFY(CloseHandle(m_offEvent));
}

void CChangeDetectionLine::SignalEvent(uInt32 value, uInt32 changedBits)
{
	if (m_mask & changedBits)
	{
		if (value & m_mask)
		{
			VERIFY(ResetEvent(m_offEvent));
			VERIFY(SetEvent(m_onEvent));
			CLog::AddToLog(CString(m_onName));
		}
		else
		{
			VERIFY(ResetEvent(m_onEvent));
			VERIFY(SetEvent(m_offEvent));
			CLog::AddToLog(CString(m_offName));
		}
	}
}

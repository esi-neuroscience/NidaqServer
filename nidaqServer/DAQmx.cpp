#include "StdAfx.h"
#include "DAQmx.h"

#pragma comment(lib, "NIDAQmx.lib")

static void DAQCheckStatus(void)
{
	if (DAQstatus == 0) return;
	CString errMsg;
	uInt32 msgSize;
	char* msg;
	int32 status;
	errMsg = (DAQstatus < 0 ? "Error: " : "Warning: ");
	msgSize = DAQmxGetErrorString(DAQstatus, NULL, 0);
	msg = new char[msgSize];
	status = DAQmxGetErrorString(DAQstatus, msg, msgSize);
	ASSERT(status == 0);
	errMsg.Append(CString(msg));
	TRACE("%s\n", errMsg);
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



CESI_Lever::CESI_Lever(void)
{
	DAQstatus = DAQmxCreateDIChan(m_taskHandle, "/Dev1/port2/line5", "LeverLine", DAQmx_Val_ChanPerLine);
	DAQCheckStatus();
	TRACE("Lever Konstruktor\n");
}


CESI_Lever::~CESI_Lever(void)
{
//	DAQstatus = DAQmxDisconnectTerms("/Dev1/Ctr1InternalOutput", "/Dev1/PFI11");
//	DAQCheckStatus();
	TRACE("Lever Destruktor\n");
}


void CESI_Lever::Start(void)
{
	DAQstatus = DAQmxStartTask(m_taskHandle);
	DAQCheckStatus();
	DAQstatus = DAQmxWaitUntilTaskDone(m_taskHandle, -1);
	DAQCheckStatus();
	DAQstatus = DAQmxStopTask(m_taskHandle);
	DAQCheckStatus();
}

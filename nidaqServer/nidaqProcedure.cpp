#include "stdafx.h"
#include "nidaqProcedure.h"
//#include "nidaqServer.h"
//#include "Log.h"

//HANDLE hPipe;
TaskHandle taskHandle;
static int32 DAQstatus;

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

UINT nidaqProcedure( LPVOID pParam ) {
	
	uInt32 value = 0;
	DAQstatus = DAQmxReadDigitalScalarU32(m_taskHandle, 0.0, &value, NULL);
	DAQCheckStatus();
	value &= m_lineMask;
	if (value == m_value) return;
	TRACE("Change detected: %u->%u\n", m_value, value);
	for (BYTE i=0; i<m_nLines; i++) m_pLines[i]->SignalEvent(value, value ^ m_value);
	m_value = value;
//	extern CnidaqServerApp theApp;
	HRESULT hr = S_OK;

	//POSITION pos = theApp.GetFirstDocTemplatePosition();
	//CDocTemplate* temp = theApp.GetNextDocTemplate(pos);
	//pos = temp->GetFirstDocPosition();
	//CStimServerDoc* pDoc = (CStimServerDoc*) temp->GetNextDoc(pos);
	bool32 changeDetectSupport;
	DAQstatus = DAQmxGetPhysicalChanDIChangeDetectSupported("Dev1/port0", &changeDetectSupport);
	DAQCheckStatus();
	TRACE("Support: %u\n", changeDetectSupport);
	DAQstatus = DAQmxCreateTask("ChangeDetectionTask", &taskHandle);
	DAQCheckStatus();
//	DAQstatus = DAQmxCreateDIChan(taskHandle, "Dev2/port0/line5", "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
//	DAQstatus = DAQmxCreateDIChan(taskHandle, "Dev2/port0/line5", "ChangeDetectionLines", DAQmx_Val_ChanPerLine);
	DAQstatus = DAQmxCreateDIChan(taskHandle, "Dev2/port2/line4:7", "ChangeDetectionLines", DAQmx_Val_ChanPerLine);
	DAQCheckStatus();
	DAQstatus = DAQmxSetSampTimingType(taskHandle, DAQmx_Val_ChangeDetection);
	DAQCheckStatus();
//	DAQstatus = DAQmxCfgDigPatternStartTrig(taskHandle, "ChangeDetectionLines", "E", DAQmx_Val_PatternMatches);
//	DAQstatus = DAQmxStartTask(taskHandle);
//	DAQCheckStatus();
//	DAQstatus = DAQmxCfgDigPatternStartTrig(taskHandle, "Dev2/port2/line4:7", "XEXX", DAQmx_Val_PatternMatches);
//	DAQstatus = DAQmxCfgDigEdgeStartTrig(taskHandle, "Dev2/P2.5", DAQmx_Val_Rising);
//	DAQCheckStatus();
	while (false)
	{
		DAQstatus = DAQmxWaitUntilTaskDone(taskHandle, -1);
		DAQCheckStatus();
		TRACE("Task Done\n");
	}
//	CoUninitialize();
	return 0;	// pretend success
}

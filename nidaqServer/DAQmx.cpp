#include "StdAfx.h"
#include "DAQmx.h"
#include "nidaqServer.h"
#include "Log.h"
#include <strsafe.h>
//#include "nidaqProcedure.h"

#pragma comment(lib, "NIDAQmx.lib")

extern CnidaqServerApp theApp;

//CDAQmxDevice* CDAQmx::m_pDevice;
//CDAQmxDevice* CChangeDetection::m_pDevice;
CDAQmxDevice* CDAQmx::m_pDevice;
TaskHandle CChangeDetection::m_taskHandle;
//char CChangeDetection::m_lines[136] = {0};
char CDAQmxM_Series::m_lines[136] = {0};
BYTE CChangeDetection::m_nLines = 0;
BYTE CChangeDetection::m_lineMask = 0;
uInt32 CChangeDetection::m_value;
CChangeDetectionLine* CChangeDetection::m_pLines[8];
BYTE CDAQmxDigitalIO::m_nLines = 0;
BYTE CDAQmxDigitalIO::m_lineMask = 0;
CChangeDetectionLine* CDAQmxDigitalIO::m_pLines[4];

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


CDAQmxDevice::~CDAQmxDevice(void)
{
//	delete m_pChangeDetection;
//	DAQstatus = DAQmxClearTask(m_taskHandle);
//	DAQCheckStatus();
	TRACE("DAQmxDevice Destruktor\n");
}


CDAQmxM_Series::CDAQmxM_Series(void)
{
	TRACE("DAQmxM_Series Konstruktor\n");
	m_pChangeDetection = new CChangeDetection();
//	m_pChangeDetection = new CChangeDetection(this);
//	DAQstatus = DAQmxCreateTask("DAQmxTask", &m_taskHandle);
//	DAQCheckStatus();
}

CDAQmxM_Series::~CDAQmxM_Series(void)
{
	delete m_pChangeDetection;
//	DAQstatus = DAQmxClearTask(m_taskHandle);
//	DAQCheckStatus();
	TRACE("DAQmxM_Series Destruktor\n");
}

//void CDAQmxM_Series::AddLine(BYTE lineNumber, char* onName, char* offName)
//{
//	char lineName[17] = "Dev1/port0/lineX";
//	lineName[15] = '0'+lineNumber;
//	if (m_lines[0] != 0) StringCbCatA(m_lines, 136, ",");
//	StringCbCatA(m_lines, 136, lineName);
//	TRACE("Channel: %s\n", m_lines);
//	//m_pLines[m_nLines] = new CChangeDetectionLine(lineNumber, onName, offName);
//	//m_lineMask |= m_pLines[m_nLines++]->m_mask;
//	//TRACE("Maske: %u\n", m_lineMask);
//}

void CDAQmxM_Series::StartChangeDetection()
{
	DAQstatus = DAQmxCreateDIChan(m_pChangeDetection->m_taskHandle, m_lines, "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
	DAQCheckStatus();
	m_pChangeDetection->Init();
	//DAQstatus = DAQmxReadDigitalScalarU32(m_pChangeDetection->m_taskHandle, 0.0, &m_value, NULL);
	//DAQCheckStatus();
	//m_value &= m_lineMask;
	DAQstatus = DAQmxCfgChangeDetectionTiming(m_pChangeDetection->m_taskHandle, m_lines, m_lines, DAQmx_Val_HWTimedSinglePoint, 0);
	DAQCheckStatus();
	DAQstatus = DAQmxRegisterSignalEvent(m_pChangeDetection->m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, CChangeDetection::ChangeDetectionCallback, NULL);
	DAQCheckStatus();
	//DAQstatus = DAQmxStartTask(m_pChangeDetection->m_taskHandle);
	//DAQCheckStatus();
}

CDAQmxDigitalIO::CDAQmxDigitalIO(void)
{
	TRACE("DAQmxDigitalIO Konstruktor\n");
	m_pChangeDetection = new CChangeDetection();
//	m_pChangeDetection = new CChangeDetection(this);
	//DAQstatus = DAQmxCreateTask("ChangeDetectionTask", &m_taskHandle);
	//DAQCheckStatus();
	DAQstatus = DAQmxCreateDIChan(m_pChangeDetection->m_taskHandle, "Dev1/port0/line4:7", "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
	DAQCheckStatus();
}

CDAQmxDigitalIO::~CDAQmxDigitalIO(void)
{
	delete m_pChangeDetection;
//	DAQstatus = DAQmxClearTask(m_taskHandle);
//	DAQCheckStatus();
	TRACE("DAQmxDigitalIO Destruktor\n");
}

//void CDAQmxDigitalIO::AddInputLine(BYTE lineNumber, char* onName, char* offName)
//{
//	m_pLines[m_nLines] = new CChangeDetectionLine(lineNumber, onName, offName);
//	m_lineMask |= m_pLines[m_nLines++]->m_mask;
//	TRACE("Maske: %u\n", m_lineMask);
//}

//void CDAQmxDigitalIO::AddLine(BYTE lineNumber, char* onName, char* offName)
//{
//	TRACE("TODO: DigitalIO::AddLine\n");
//}

//TP_CALLBACK_INSTANCE* instance;
//TP_TIMER* timer;
//
//VOID CALLBACK CChangeDetection::TimerCallback(PTP_CALLBACK_INSTANCE instance, PVOID pv, PTP_TIMER timer)
//{
////	TRACE("Timer Callback\n");
//	uInt32 value = 0;
//	DAQstatus = DAQmxReadDigitalScalarU32(m_taskHandle, 0.0, &value, NULL);
//	DAQCheckStatus();
//	value &= m_lineMask;
//	if (value == m_value) return;
//	TRACE("Change detected: %u->%u\n", m_value, value);
//	for (BYTE i=0; i<m_nLines; i++) m_pLines[i]->SignalEvent(value, value ^ m_value);
//	m_value = value;
//}

UINT CChangeDetection::nidaqProcedure( LPVOID pParam ) {
	
	uInt32 value = 0;
	while (true)
	{
		DAQstatus = DAQmxReadDigitalScalarU32(m_taskHandle, 0.0, &value, NULL);
		DAQCheckStatus();
		value &= m_lineMask;
		if (value != m_value)
		{
			TRACE("Change detected: %u->%u\n", m_value, value);
			for (BYTE i=0; i<m_nLines; i++) m_pLines[i]->SignalEvent(value, value ^ m_value);
			m_value = value;
		}
		Sleep(1);
	}
	return 0;	// pretend success
}

void CDAQmxDigitalIO::StartChangeDetection()
{
	TRACE("DigitalIO::StartChangeDetection\n");
	AfxBeginThread(CChangeDetection::nidaqProcedure, NULL);
	//PTP_TIMER tpTimer = CreateThreadpoolTimer((PTP_TIMER_CALLBACK) CChangeDetection::TimerCallback, NULL, NULL);
	//ASSERT(tpTimer);
 //     FILETIME FileDueTime;
 //     ULARGE_INTEGER ulDueTime;
 //     ulDueTime.QuadPart = (ULONGLONG) -(1 * 10 * 1000 * 1000);
 //     FileDueTime.dwHighDateTime = ulDueTime.HighPart;
 //     FileDueTime.dwLowDateTime  = ulDueTime.LowPart;
	//SetThreadpoolTimer(tpTimer, &FileDueTime, 6, 0);
	//ASSERT(tpTimer);
}


void UnsupportedDevice(char* category, char* type)
{
	CString temp;
	temp.Format(_T("Unsupported %s device: %s"), CString(category), CString(type));
	CLog::AddToLog(temp);
}

void CDAQmx::Init(void)
{
	char productType[16];
	DAQstatus = DAQmxGetDevProductType("Dev1", &productType[0], 16);
	DAQCheckStatus();
	TRACE("Product Type: %s\n", productType);
	int32 data;
	DAQstatus = DAQmxGetDevProductCategory("Dev1", &data);
	DAQCheckStatus();
	TRACE("Product Category: %d\n", data);
	switch (data)
	{
	case DAQmx_Val_MSeriesDAQ:
		if (StrCmpA(productType, "PCI-6221") != 0)
		{
			UnsupportedDevice("M-Series", productType);
		}
//		new CDAQmxM_Series();
//		new CDAQmxDigitalIO();
		m_pDevice = new CDAQmxDigitalIO();
//		theApp.m_pDevice = new CDAQmxM_Series();
		break;
	case DAQmx_Val_DigitalIO:
		if (StrCmpA(productType, "PCI-6503") != 0)
		{
			UnsupportedDevice("Digital I/O", productType);
		}
//		new CDAQmxDigitalIO();
		m_pDevice = new CDAQmxDigitalIO();
		break;
	default:
		CString temp;
		temp.Format(_T("Unsupported Product Category: %u"), data);
		CLog::AddToLog(temp);
		break;
	}
	//bool32 changeDetectSupported;
	//DAQstatus = DAQmxGetPhysicalChanDIChangeDetectSupported("Dev2/port0", &changeDetectSupported);
	//DAQCheckStatus();
	//TRACE("Change Detection Supported: %d\n", changeDetectSupported);
}


void CDAQmx::Cleanup(void)
{
	m_pDevice->~CDAQmxDevice();
//	delete m_pDevice;
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


//CChangeDetection::CChangeDetection(CDAQmxDevice* pDevice)
//{
//	DAQstatus = DAQmxCreateTask("ChangeDetectionTask", &m_taskHandle);
//	DAQCheckStatus();
//	m_pDevice = pDevice;
//}


CChangeDetection::~CChangeDetection(void)
{
	TRACE("ChangeDetection Destruktor\n");
	DAQstatus = DAQmxStopTask(m_taskHandle);
	DAQCheckStatus();
	// unregister the callback
	DAQstatus = DAQmxRegisterSignalEvent(m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, NULL, NULL);
	DAQCheckStatus();
	for (BYTE i=0; i<m_nLines; i++) delete m_pLines[i];
} 

void CChangeDetection::Init(void)
{
	DAQstatus = DAQmxReadDigitalScalarU32(m_taskHandle, 0.0, &m_value, NULL);
	DAQCheckStatus();
	m_value &= m_lineMask;
}

void CChangeDetection::Start(void)
{
	CDAQmx::m_pDevice->StartChangeDetection();
	//DAQstatus = DAQmxCreateDIChan(m_taskHandle, m_lines, "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
	//DAQCheckStatus();
	//DAQstatus = DAQmxReadDigitalScalarU32(m_taskHandle, 0.0, &m_value, NULL);
	//DAQCheckStatus();
	//m_value &= m_lineMask;
	//DAQstatus = DAQmxCfgChangeDetectionTiming(m_taskHandle, m_lines, m_lines, DAQmx_Val_HWTimedSinglePoint, 0);
	//DAQCheckStatus();
	//DAQstatus = DAQmxRegisterSignalEvent(m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, ChangeDetectionCallback, NULL);
	//DAQCheckStatus();
	DAQstatus = DAQmxStartTask(m_taskHandle);
	DAQCheckStatus();
}



void CChangeDetection::AddLine(BYTE lineNumber, char* pulseName)
{
//	CDAQmx::m_pDevice->AddLine(lineNumber, pulseName);
	//char lineName[17] = "Dev1/port0/lineX";
	//lineName[15] = '0'+lineNumber;
	//if (m_lines[0] != 0) StringCbCatA(m_lines, 136, ",");
	//StringCbCatA(m_lines, 136, lineName);
	//TRACE("Channel: %s\n", m_lines);
	m_pLines[m_nLines] = new CPulseDetectionLine(lineNumber, pulseName);
	m_lineMask |= m_pLines[m_nLines++]->m_mask;
	TRACE("Maske: %u\n", m_lineMask);
}


void CChangeDetection::AddLine(BYTE lineNumber, char* onName, char* offName)
{
//	CDAQmx::m_pDevice->AddLine(lineNumber, onName, offName);
	//char lineName[17] = "Dev1/port0/lineX";
	//lineName[15] = '0'+lineNumber;
	//if (m_lines[0] != 0) StringCbCatA(m_lines, 136, ",");
	//StringCbCatA(m_lines, 136, lineName);
	//TRACE("Channel: %s\n", m_lines);
	m_pLines[m_nLines] = new COnOffDetectionLine(lineNumber, onName, offName);
	m_lineMask |= m_pLines[m_nLines++]->m_mask;
	TRACE("Maske: %u\n", m_lineMask);
}


//CChangeDetectionLine::CChangeDetectionLine(BYTE lineNumber, char* onName, char* offName)
//{
//	size_t bufferSize;
//	TRACE("ChangeDetectionLine Konstruktor\n");
//	m_mask = (1 << lineNumber);
//	// we use manual reset events (for on/off lines)
//	VERIFY(m_onEvent = CreateEventA(NULL, TRUE, FALSE, onName));
//	VERIFY(m_offEvent = CreateEventA(NULL, TRUE, FALSE, offName));
//	bufferSize = strlen(onName)+1;
//	m_onName = new char[bufferSize];
//	StringCchCopyA(m_onName, bufferSize, onName);
//	bufferSize = strlen(offName)+1;
//	m_offName = new char[strlen(offName)+1];
//	StringCchCopyA(m_offName, bufferSize, offName);
//	//m_onName = onName;
//	//m_offName = offName;
//}

//CChangeDetectionLine::~CChangeDetectionLine(void)
//{
//	TRACE("ChangeDetectionLine Destruktor\n");
//	VERIFY(CloseHandle(m_onEvent));
//	VERIFY(CloseHandle(m_offEvent));
//	delete m_onName;
//	delete m_offName;
//}

//void CChangeDetectionLine::SignalEvent(uInt32 value, uInt32 changedBits)
//{
//	if (m_mask & changedBits)
//	{
//		if (value & m_mask)
//		{
//			VERIFY(ResetEvent(m_offEvent));
//			VERIFY(SetEvent(m_onEvent));
//			CLog::AddToLog(CString(m_onName));
//		}
//		else
//		{
//			VERIFY(ResetEvent(m_onEvent));
//			VERIFY(SetEvent(m_offEvent));
//			CLog::AddToLog(CString(m_offName));
//		}
//	}
//}


bool CChangeDetection::Running(void)
{
	bool32 isTaskDone;
	DAQstatus = DAQmxIsTaskDone(m_taskHandle, &isTaskDone);
	DAQCheckStatus();
	return !isTaskDone;
}


CPulseDetectionLine::CPulseDetectionLine(BYTE lineNumber, char* pulseName)
{
	size_t bufferSize;
	TRACE("PulseDetectionLine Konstruktor\n");
	m_mask = (1 << lineNumber);
	// we use auto reset events (for pulse lines)
	VERIFY(m_pulseEvent = CreateEventA(NULL, FALSE, FALSE, pulseName));
	bufferSize = strlen(pulseName)+1;
	m_pulseName = new char[bufferSize];
	StringCchCopyA(m_pulseName, bufferSize, pulseName);
}

CPulseDetectionLine::~CPulseDetectionLine(void)
{
	TRACE("CPulseDetectionLine Destruktor\n");
	VERIFY(CloseHandle(m_pulseEvent));
	delete m_pulseName;
}

void CPulseDetectionLine::SignalEvent(uInt32 value, uInt32 changedBits)
{
	if (m_mask & changedBits)
	{
		if (value & m_mask)
		{
			VERIFY(SetEvent(m_pulseEvent));
			CLog::AddToLog(CString(m_pulseName));
		}
	}
}


COnOffDetectionLine::COnOffDetectionLine(BYTE lineNumber, char* onName, char* offName)
{
	size_t bufferSize;
	TRACE("ChangeDetectionLine Konstruktor\n");
	m_mask = (1 << lineNumber);
	// we use manual reset events (for on/off lines)
	VERIFY(m_onEvent = CreateEventA(NULL, TRUE, FALSE, onName));
	VERIFY(m_offEvent = CreateEventA(NULL, TRUE, FALSE, offName));
	bufferSize = strlen(onName)+1;
	m_onName = new char[bufferSize];
	StringCchCopyA(m_onName, bufferSize, onName);
	bufferSize = strlen(offName)+1;
	m_offName = new char[strlen(offName)+1];
	StringCchCopyA(m_offName, bufferSize, offName);
	//m_onName = onName;
	//m_offName = offName;
}

COnOffDetectionLine::~COnOffDetectionLine(void)
{
	TRACE("COnOffDetectionLine Destruktor\n");
	VERIFY(CloseHandle(m_onEvent));
	VERIFY(CloseHandle(m_offEvent));
	delete m_onName;
	delete m_offName;
}

void COnOffDetectionLine::SignalEvent(uInt32 value, uInt32 changedBits)
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

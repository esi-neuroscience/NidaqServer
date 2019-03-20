#include "StdAfx.h"
#include "DAQmx.h"
#include "nidaqServer.h"
#include "Reward.h"
#include "Log.h"
#include <strsafe.h>
//#include "nidaqProcedure.h"

#pragma comment(lib, "NIDAQmx.lib")

extern HANDLE hDaqServerDone;
int32 DAQstatus;

// extern CnidaqServerApp theApp;

CDAQmxDevice* CDAQmx::m_pDevice;
TaskHandle CDAQmx::m_eventMarkerTask;
TaskHandle CDAQmx::m_eventMarkerStrobeTask;
TaskHandle CChangeDetection::m_taskHandle;
//char CDAQmxM_Series::m_lines[136] = {0};
char CDAQmxDevice::m_lines[136] = {0};
BYTE CChangeDetection::m_nLines = 0;
BYTE CChangeDetection::m_lineMask = 0;
uInt32 CChangeDetection::m_value;
CChangeDetectionLine* CChangeDetection::m_pLines[8];
CChangeDetection CDAQmx::m_changeDetection;
CRITICAL_SECTION CDAQmx::m_eventMarkerSection;
LARGE_INTEGER CDAQmx::m_41us;
LARGE_INTEGER CDAQmx::m_startTime = {0};

void DAQCheckStatus(void)
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
	CLog::AddToLog(CString(msg));
	delete msg;
	ASSERT(false);
}


void CDAQmxDevice::AddLine(BYTE lineNumber)
{
		char lineName[17] = "Dev1/port2/lineX";
		lineName[15] = '0'+lineNumber;
		if (m_lines[0] != 0) StringCbCatA(m_lines, 136, ",");
		StringCbCatA(m_lines, 136, lineName);
		TRACE("Channel: %s\n", m_lines);
}

bool CDAQmxDevice::StartChangeDetection()
{
	if (m_lines[0] == 0)
	{
		CLog::AddToLog(CString("No lines defined for change detection --> will not start."));
		return false;
	}
	DAQstatus = DAQmxCreateDIChan(CChangeDetection::m_taskHandle, m_lines, "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
	DAQCheckStatus();
	CChangeDetection::Init();
	TRACE("Device::StartChangeDetection\n");
	AfxBeginThread(CChangeDetection::nidaqProcedure, NULL);
	return true;
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
//	m_pChangeDetection = new CChangeDetection();
}

CDAQmxM_Series::~CDAQmxM_Series(void)
{
//	delete m_pChangeDetection;
	TRACE("DAQmxM_Series Destruktor\n");
}

bool CDAQmxM_Series::IsChangeDetectionLineValid(BYTE lineNumber)
{
	bool result = (lineNumber != 3) && (lineNumber != 7);
	if (!result)
	{
		CString temp;
		temp.Format(_T("port2/line%u is used as the %s output signal."), lineNumber, lineNumber == 3 ? _T("reward") : _T("event marker strobe"));
		CLog::AddToLog(temp);
		temp.Format(_T(" It will not be added as an input line."));
		CLog::AddToLog(temp);
	}
	return result;
}

CString CDAQmxM_Series::ValidChangeDetectionLines(void)
{
	return CString(_T("0-2 and 4-6"));
}

//void CDAQmxM_Series::StartChangeDetection()
//{
////	DAQstatus = DAQmxCreateDIChan(m_pChangeDetection->m_taskHandle, m_lines, "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
//	DAQstatus = DAQmxCreateDIChan(CChangeDetection::m_taskHandle, m_lines, "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
//	DAQCheckStatus();
////	m_pChangeDetection->Init();
//	TRACE("m_Series::StartChangeDetection\n");
////	AfxBeginThread(CChangeDetection::nidaqProcedure, NULL);
//	CDAQmxDevice::StartChangeDetection();
//	//DAQstatus = DAQmxReadDigitalScalarU32(m_pChangeDetection->m_taskHandle, 0.0, &m_value, NULL);
//	//DAQCheckStatus();
//	//m_value &= m_lineMask;
//	//DAQstatus = DAQmxCfgChangeDetectionTiming(m_pChangeDetection->m_taskHandle, m_lines, m_lines, DAQmx_Val_HWTimedSinglePoint, 0);
//	//DAQCheckStatus();
//	//DAQstatus = DAQmxRegisterSignalEvent(m_pChangeDetection->m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, CChangeDetection::ChangeDetectionCallback, NULL);
//	//DAQCheckStatus();
//	//DAQstatus = DAQmxStartTask(m_pChangeDetection->m_taskHandle);
//	//DAQCheckStatus();
//}

CDAQmxDigitalIO::CDAQmxDigitalIO(void)
{
	TRACE("DAQmxDigitalIO Konstruktor\n");
//	m_pChangeDetection = new CChangeDetection();
//	DAQstatus = DAQmxCreateDIChan(m_pChangeDetection->m_taskHandle, "Dev1/port2/line4:7", "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
//	DAQstatus = DAQmxCreateDIChan(m_pChangeDetection->m_taskHandle, IOINPUTLINES , "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
//	DAQstatus = DAQmxCreateDIChan(CChangeDetection::m_taskHandle, IOINPUTLINES , "ChangeDetectionLines", DAQmx_Val_ChanForAllLines);
//	DAQCheckStatus();
}

CDAQmxDigitalIO::~CDAQmxDigitalIO(void)
{
//	delete m_pChangeDetection;
	TRACE("DAQmxDigitalIO Destruktor\n");
}

bool CDAQmxDigitalIO::IsChangeDetectionLineValid(BYTE lineNumber)
{
	bool result = lineNumber > 3;
	if (!result)
	{
		CString temp;
		temp.Format(_T("port2/line%u is configured as an output signal."), lineNumber);
		CLog::AddToLog(temp);
		temp.Format(_T(" It will not be added as an input line."));
		CLog::AddToLog(temp);
	}
	return result;
}

CString CDAQmxDigitalIO::ValidChangeDetectionLines(void)
{
	return CString(_T("4-7"));
}


UINT CChangeDetection::nidaqProcedure( LPVOID pParam ) {
	
	uInt32 value = 0;
	VERIFY(SetEvent(hDaqServerDone));
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

//void CDAQmxDigitalIO::StartChangeDetection()
//{
//	TRACE("DigitalIO::StartChangeDetection\n");
//	AfxBeginThread(CChangeDetection::nidaqProcedure, NULL);
//}
//

void UnsupportedDevice(char* category, char* type)
{
	CString temp;
	temp.Format(_T("Unsupported %s device: %s"), CString(category), CString(type));
	CLog::AddToLog(temp);
}

void CDAQmx::Init(void)
{
//	CChangeDetection::CChangeDetection();
	char productType[16];
	DAQstatus = DAQmxGetDevProductType(DEVICE, &productType[0], 16);
	DAQCheckStatus();
	TRACE("Product Type: %s\n", productType);
	int32 data;
	DAQstatus = DAQmxGetDevProductCategory(DEVICE, &data);
	DAQCheckStatus();
	TRACE("Product Category: %d\n", data);
	char* eventMarkerLines;
	char strobeLine[17] = "Dev1/port2/line7";
	switch (data)
	{
	case DAQmx_Val_XSeriesDAQ:
		if ((StrCmpA(productType, "USB-6353") != 0) &&
			(StrCmpA(productType, "PCIe-6321") != 0))
		{
			UnsupportedDevice("X-Series", productType);
		}
		m_pDevice = new CDAQmxM_Series();
		if (StrCmpA(productType, "USB-6353") == 0)
			eventMarkerLines = "Dev1/port0/line0:15";
		else
			eventMarkerLines = "Dev1/port1:0";
		break;
	case DAQmx_Val_MSeriesDAQ:
		if ((StrCmpA(productType, "PCI-6221") != 0) &&
			(StrCmpA(productType, "PCIe-6251") != 0))
		{
			UnsupportedDevice("M-Series", productType);
		}
		m_pDevice = new CDAQmxM_Series();
		eventMarkerLines = "Dev1/port1:0";
		break;
	case DAQmx_Val_DigitalIO:
		if (StrCmpA(productType, "PCI-6503") != 0)
		{
			UnsupportedDevice("Digital I/O", productType);
		}
		m_pDevice = new CDAQmxDigitalIO();
		eventMarkerLines = "Dev1/port1:0";
		strobeLine[15] = '0';
		break;
	default:
		CString temp;
		temp.Format(_T("Unsupported Product Category: %u"), data);
		CLog::AddToLog(temp);
		eventMarkerLines = "Dev1/port1:0";
		break;
	}
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	m_41us.QuadPart = frequency.QuadPart / 24414;
	DAQstatus = DAQmxCreateTask("EventMarkerTask", &m_eventMarkerTask);
	DAQCheckStatus();
	DAQstatus = DAQmxCreateDOChan(m_eventMarkerTask, eventMarkerLines, "eventMarkerChannel", DAQmx_Val_ChanForAllLines);
//	DAQstatus = DAQmxCreateDOChan(m_eventMarkerTask, "Dev1/port1:0", "eventMarkerChannel", DAQmx_Val_ChanForAllLines);
//	DAQstatus = DAQmxCreateDOChan(m_eventMarkerTask, "Dev4/port1/line0:7, Dev4/port0/line0:7", "", DAQmx_Val_ChanForAllLines);
	DAQCheckStatus();
	DAQstatus = DAQmxStartTask(m_eventMarkerTask);
	DAQCheckStatus();
	DAQstatus = DAQmxCreateTask("EventMarkerStrobeTask", &m_eventMarkerStrobeTask);
	DAQCheckStatus();
	DAQstatus = DAQmxCreateDOChan(m_eventMarkerStrobeTask, strobeLine, "eventMarkerStrobeChannel", DAQmx_Val_ChanForAllLines);
	DAQCheckStatus();
	DAQstatus = DAQmxStartTask(m_eventMarkerStrobeTask);
	DAQCheckStatus();
	CReward::Init();
	InitializeCriticalSection(&m_eventMarkerSection);
}

bool CDAQmx::IsChangeDetectionLineValid(BYTE lineNumber)
{
	if (lineNumber > 7)
	{
		CString temp;
		temp.Format(_T("Config error: line %u is not a valid input line."), lineNumber);
		CLog::AddToLog(temp);
		temp.Format(_T(" Valid line numbers are %s."), m_pDevice->ValidChangeDetectionLines());
		CLog::AddToLog(temp);
		return false;
	}
	return m_pDevice->IsChangeDetectionLineValid(lineNumber);
}

void CDAQmx::Delay41us(void)
{
	LARGE_INTEGER currentTime;
	do
	{
		QueryPerformanceCounter(&currentTime);
	} while ((currentTime.QuadPart - m_startTime.QuadPart) < m_41us.QuadPart);
}

void CDAQmx::WriteEventMarker(short* pMarker)
{
//	int32 sampsWritten = 0;
	Delay41us();
	EnterCriticalSection(&m_eventMarkerSection);
	DAQstatus = DAQmxWriteDigitalScalarU32(m_eventMarkerTask, false, 0, *pMarker, NULL);
//	DAQstatus = DAQmxWriteDigitalLines(m_eventMarkerTask, 1, false, 0, DAQmx_Val_GroupByChannel, (uInt8*) pMarker, &sampsWritten, NULL);
//	DAQstatus = DAQmxWriteDigitalU16(m_eventMarkerTask,1, false, 0, DAQmx_Val_GroupByChannel, (uInt16*) pMarker, &sampsWritten,NULL);
	DAQCheckStatus();
//	ASSERT(sampsWritten == 1);
	DAQstatus = DAQmxWriteDigitalScalarU32(m_eventMarkerStrobeTask, false, 0, 255, NULL);
	DAQCheckStatus();
	QueryPerformanceCounter(&m_startTime);
//	Sleep(1);
	Delay41us();
	DAQstatus = DAQmxWriteDigitalScalarU32(m_eventMarkerStrobeTask, false, 0, 0, NULL);
	DAQCheckStatus();
	QueryPerformanceCounter(&m_startTime);
//	Delay41us();
	LeaveCriticalSection(&m_eventMarkerSection);
}


void CDAQmx::Cleanup(void)
{
	DeleteCriticalSection(&m_eventMarkerSection);
	DAQstatus = DAQmxStopTask(m_eventMarkerTask);
	DAQCheckStatus();
	DAQstatus = DAQmxClearTask(m_eventMarkerTask);
	DAQCheckStatus();
	DAQstatus = DAQmxStopTask(m_eventMarkerStrobeTask);
	DAQCheckStatus();
	DAQstatus = DAQmxClearTask(m_eventMarkerStrobeTask);
	DAQCheckStatus();
	CReward::Cleanup();
	if (m_pDevice) m_pDevice->~CDAQmxDevice();
//	delete m_pDevice;
}

	
CDAQmxTask::CDAQmxTask(void)
{
	TRACE("DAQmxTask Konstruktor\n");
}


CDAQmxTask::~CDAQmxTask(void)
{
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



//int32 CVICALLBACK CChangeDetection::ChangeDetectionCallback(TaskHandle taskHandle, int32 signalID, void *callbackData)
//{
//	uInt32 value = 0;
//	DAQstatus = DAQmxReadDigitalScalarU32(taskHandle, 0.0, &value, NULL);
//	DAQCheckStatus();
//	value &= m_lineMask;
//	for (BYTE i=0; i<m_nLines; i++) m_pLines[i]->SignalEvent(value, value ^ m_value);
//	m_value = value;
//	return 0;
//}
//

CChangeDetection::CChangeDetection(void)
{
	TRACE("ChangeDetection Konstruktor\n");
	DAQstatus = DAQmxCreateTask("ChangeDetectionTask", &m_taskHandle);
	DAQCheckStatus();
}

CChangeDetection::~CChangeDetection(void)
{
	TRACE("ChangeDetection Destruktor\n");
	DAQstatus = DAQmxStopTask(m_taskHandle);
	DAQCheckStatus();
	// unregister the callback
//	DAQstatus = DAQmxRegisterSignalEvent(m_taskHandle, DAQmx_Val_ChangeDetectionEvent, 0, NULL, NULL);
//	DAQCheckStatus();
	for (BYTE i=0; i<m_nLines; i++)	delete m_pLines[i];
} 

void CChangeDetection::Init(void)
{
	DAQstatus = DAQmxReadDigitalScalarU32(m_taskHandle, 0.0, &m_value, NULL);
	DAQCheckStatus();
	m_value &= m_lineMask;
}

void CChangeDetection::Start(void)
{
	if (!CDAQmx::m_pDevice->StartChangeDetection())
	{
		VERIFY(SetEvent(hDaqServerDone));
		return;
	}
	DAQstatus = DAQmxStartTask(m_taskHandle);
	DAQCheckStatus();
}


void CChangeDetection::UpdateLines(BYTE lineNumber)
{
	CDAQmx::m_pDevice->AddLine(lineNumber);
	m_lineMask |= m_pLines[m_nLines++]->m_mask;
	TRACE("Maske: %u\n", m_lineMask);
}


void CChangeDetection::AddLine(BYTE lineNumber, char* pulseName)
{
	m_pLines[m_nLines] = new CPulseDetectionLine(lineNumber, pulseName);
	UpdateLines(lineNumber);
}


void CChangeDetection::AddLine(BYTE lineNumber, char* onName, char* offName)
{
	m_pLines[m_nLines] = new COnOffDetectionLine(lineNumber, onName, offName);
	UpdateLines(lineNumber);
}

bool CChangeDetection::Running(void)
{
	bool32 isTaskDone;
	DAQstatus = DAQmxIsTaskDone(m_taskHandle, &isTaskDone);
	DAQCheckStatus();
	return !isTaskDone;
}

char* EventNameCopy(char* eventName)
{
	size_t bufferSize = strlen(eventName)+1;
	char* temp = new char[bufferSize];
	StringCchCopyA(temp, bufferSize, eventName);
	return temp;
}

CPulseDetectionLine::CPulseDetectionLine(BYTE lineNumber, char* pulseName)
{
	TRACE("PulseDetectionLine Konstruktor\n");
	m_mask = (1 << lineNumber);
	// we use auto reset events (for pulse lines)
	VERIFY(m_pulseEvent = CreateEventA(NULL, FALSE, FALSE, pulseName));
	m_pulseName = EventNameCopy(pulseName);
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
			//CString temp;
			//temp.Format(_T(" %u %u %u"), value, changedBits, m_mask);
			//CLog::AddToLog(CString(m_pulseName) + temp);
			CLog::AddToLog(CString(m_pulseName));
		}
	}
}


COnOffDetectionLine::COnOffDetectionLine(BYTE lineNumber, char* onName, char* offName)
{
	TRACE("COnOffDetectionLine Konstruktor\n");
	m_mask = (1 << lineNumber);
	// we use manual reset events (for on/off lines)
	VERIFY(m_onEvent = CreateEventA(NULL, TRUE, FALSE, onName));
	VERIFY(m_offEvent = CreateEventA(NULL, TRUE, FALSE, offName));
	m_onName = EventNameCopy(onName);
	m_offName = EventNameCopy(offName);
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

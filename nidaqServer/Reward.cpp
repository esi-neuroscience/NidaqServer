#include "StdAfx.h"
#include "Reward.h"
#include "Log.h"
#include "nidaqServerDoc.h"
#include "DAQmx.h"

//extern CnidaqServerApp theApp;
extern int32 DAQstatus;
extern void DAQCheckStatus(void);

CauxLine::CauxLine()
//: m_lineNumber(127)
//, m_mask(0)
//, m_name(nullptr)
{

}

CauxLine::CauxLine(BYTE lineNumber, char name[])
	: m_lineNumber(lineNumber)
	, m_mask(1 << lineNumber)
	, m_name(EventNameCopy(name))
{

}

CauxLine::~CauxLine(void)
{
	TRACE("auxLine Destruktor\n");
}

CPulseLine::CPulseLine()
/*
	: m_rewardTime(100)
	, m_pulseDoneEvent(INVALID_HANDLE_VALUE)
	, m_pulseEvent(INVALID_HANDLE_VALUE)
	, m_pulseTask()
	, m_totalTime(0)
	, m_pulseActive(false)
	, m_rewardCode(0)
	, m_logPulseTriggers(false)
	*/
{
	TRACE("Pulse Line default Konstruktor\n");

}

CPulseLine::CPulseLine(BYTE lineNumber, char name[])
	//	: m_rewardTime(100)
	//	, m_rewardCode(0)
	: CauxLine(lineNumber, name)
{
	TRACE("Pulse Line Konstruktor 1\n");
	m_name = EventNameCopy(name);
	m_lineNumber = lineNumber;
	m_mask = (1 << lineNumber);
	char eventName[11] = "PulseDoneX";	// X is replaced by the line number (0-7)
	eventName[9] = '0' + lineNumber;
	VERIFY(m_pulseDoneEvent = CreateEvent(NULL, FALSE, FALSE, eventName));
	char taskName[11] = "PulseTaskX";
	taskName[9] = '0' + lineNumber;
	DAQstatus = DAQmxCreateTask(taskName, &m_pulseTask);
	DAQCheckStatus();
	char channelName[17] = "Dev1/port2/lineX";
	channelName[15] = '0' + lineNumber;
	char pulseChannel[14] = "PulseChannelX";
	pulseChannel[12] = '0' + lineNumber;
	DAQstatus = DAQmxCreateDOChan(m_pulseTask, channelName, pulseChannel, DAQmx_Val_ChanPerLine);
	DAQCheckStatus();
	VERIFY(AfxBeginThread(CPulseLine::EventProcedure, this));
//	m_pwk = CreateThreadpoolWork(&Sequence, NULL, NULL);
	// m_totalTime = 0;
}

CPulseLine::~CPulseLine(void)
{
	TRACE("PulseLine Destruktor\n");
}

TaskHandle CReward::m_rewardTask;
short CReward::m_times[];
bool CReward::m_sequenceRunning;
BYTE CReward::m_nTimes;
HANDLE CReward::m_rewardDoneEvent;
PTP_WORK CReward::m_pwk;
//CnidaqServerDoc* CReward::m_pDoc;
//DWORD CReward::m_totalTime;


UINT CPulseLine::EventProcedure(LPVOID pParam)
{
	CPulseLine* that = (CPulseLine*)pParam;
	TRACE("Pre Pulse Event Procedure\n");
	that->DoPulse();
	TRACE("Post Pulse Event Procedure\n");
	return 0;
}
void CPulseLine::DoPulse()
{
//	char eventName[11] = "PulseX";
//	taskName[9] = '0' + lineNumber;
//	VERIFY(m_pulseEvent = CreateEvent(NULL, FALSE, FALSE, _T("Reward")));
	VERIFY(m_pulseEvent = CreateEvent(NULL, FALSE, FALSE, _T(m_name)));
	TRACE("Create Pulse Event %s\n", m_name);
	CString LogEntry;
	while (true)
	{
		TRACE("Pre Pulse Event\n");
		VERIFY(0 == WaitForSingleObject(m_pulseEvent, INFINITE));
		TRACE("Post Pulse Event\n");
//		m_totalTime += m_rewardTime;
		if (m_logPulseTriggers) LogEntry.Format(_T("Pulse trigger (%u ms)."), m_rewardTime);
		if (m_rewardCode != 0)
		{
			CDAQmx::WriteEventMarker(&m_rewardCode);
			if (m_logPulseTriggers) LogEntry.AppendFormat(_T(" Marker code %u."), m_rewardCode);
		}
		DAQstatus = DAQmxStartTask(m_pulseTask);
		DAQCheckStatus();
		DAQstatus = DAQmxWriteDigitalScalarU32(m_pulseTask, false, 0, m_mask, NULL);
		DAQCheckStatus();
		TRACE("Pulse Time: %u, Mask: %u\n", m_rewardTime, m_mask);
		Sleep(m_rewardTime);
		DAQstatus = DAQmxWriteDigitalScalarU32(m_pulseTask, false, 0, 0, NULL);
		DAQCheckStatus();
		DAQstatus = DAQmxStopTask(m_pulseTask);
		DAQCheckStatus();
		VERIFY(SetEvent(m_pulseDoneEvent));
		if (m_logPulseTriggers) CLog::AddToLog(LogEntry);
	}
}


UINT CReward::EventProcedure(LPVOID pParam)
//UINT CReward::EventProcedure(CReward* that)
{
	CReward* that = (CReward*)pParam;
	TRACE("Pre Reward Event Procedure\n");
	that->DoPulse();
	TRACE("Post Reward Event Procedure\n");
	return 0;
}
void CReward::DoPulse()
{
	HANDLE rewardEvent;
	VERIFY(rewardEvent = CreateEvent(NULL, FALSE, FALSE, _T("Reward")));
	CString LogEntry;
	while (true)
	{
		TRACE("Pre Reward Event\n");
		VERIFY(0 == WaitForSingleObject(rewardEvent, INFINITE));
		TRACE("Post Reward Event\n");
		//	m_totalTime += m_pDoc->m_rewardTime;
	//	m_totalTime += m_pDoc->m_pauxLines[3]->m_rewardTime;
		m_totalTime += m_rewardTime;
		//	if (theApp.m_logRewardTriggers) LogEntry.Format(_T("Reward trigger (%u ms)."), m_pDoc->m_rewardTime);
		//	if (theApp.m_logRewardTriggers) LogEntry.Format(_T("Reward trigger (%u ms)."), m_rewardTime);
		if (m_logPulseTriggers) LogEntry.Format(_T("Reward trigger (%u ms)."), m_rewardTime);
		//if (m_pDoc->m_rewardCode != 0)
		if (m_rewardCode != 0)
		{
			//			CDAQmx::WriteEventMarker(&m_pDoc->m_rewardCode);
			CDAQmx::WriteEventMarker(&m_rewardCode);
			//			if (theApp.m_logRewardTriggers) LogEntry.AppendFormat(_T(" Marker code %u."), m_pDoc->m_rewardCode);
			//			if (theApp.m_logRewardTriggers) LogEntry.AppendFormat(_T(" Marker code %u."), m_rewardCode);
			if (m_logPulseTriggers) LogEntry.AppendFormat(_T(" Marker code %u."), m_rewardCode);
		}
		DAQstatus = DAQmxStartTask(m_rewardTask);
		DAQCheckStatus();
		m_pulseActive = true;
		DAQstatus = DAQmxWriteDigitalScalarU32(m_rewardTask, false, 0, 8, NULL);
		DAQCheckStatus();
		Sleep(m_rewardTime);
		DAQstatus = DAQmxWriteDigitalScalarU32(m_rewardTask, false, 0, 0, NULL);
		DAQCheckStatus();
		DAQstatus = DAQmxStopTask(m_rewardTask);
		m_pulseActive = false;
		DAQCheckStatus();
		VERIFY(SetEvent(m_rewardDoneEvent));
		//		if (theApp.m_logRewardTriggers) CLog::AddToLog(LogEntry);
		if (m_logPulseTriggers) CLog::AddToLog(LogEntry);
	}
}


VOID CALLBACK CReward::Sequence(
      PTP_CALLBACK_INSTANCE Instance,
      PVOID                 Parameter,
      PTP_WORK              Work
      )
{
	short* pTime = &m_times[0];
	DAQstatus = DAQmxStartTask(m_rewardTask);
	DAQCheckStatus();
	do
	{
		DAQstatus = DAQmxWriteDigitalScalarU32(m_rewardTask, false, 0, 8, NULL);
		DAQCheckStatus();
		Sleep(*pTime++);
		DAQstatus = DAQmxWriteDigitalScalarU32(m_rewardTask, false, 0, 0, NULL);
		DAQCheckStatus();
		if (--m_nTimes != 0)
		{
			Sleep(*pTime++);
			--m_nTimes;
		}
	} while (m_nTimes != 0);
	DAQstatus = DAQmxStopTask(m_rewardTask);
	DAQCheckStatus();
	m_sequenceRunning = false;
	VERIFY(SetEvent(m_rewardDoneEvent));
//	TRACE("Reward Callback\n");
}

//void CReward::Init(void)
CReward::CReward()
{
	TRACE("Reward Konstruktor\n");
	//	POSITION pos = theApp.GetFirstDocTemplatePosition();
//	CDocTemplate* temp = theApp.GetNextDocTemplate(pos);
//	pos = temp->GetFirstDocPosition();
//	m_pDoc = (CnidaqServerDoc*) temp->GetNextDoc(pos);

	VERIFY(m_rewardDoneEvent = CreateEvent(NULL, FALSE, FALSE, _T("RewardDone")));
	DAQstatus = DAQmxCreateTask("RewardTask", &m_rewardTask);
	DAQCheckStatus();
	DAQstatus =  DAQmxCreateDOChan (m_rewardTask, "Dev1/port2/line3", "RewardChannel", DAQmx_Val_ChanPerLine);
	DAQCheckStatus();
//	VERIFY(AfxBeginThread(CReward::EventProcedure, NULL));
	VERIFY(AfxBeginThread(CReward::EventProcedure, this));
	m_pwk = CreateThreadpoolWork(&Sequence, NULL, NULL);
	// m_totalTime = 0;
}

//void CReward::Cleanup(void)
CReward::~CReward(void)
{
	TRACE("Reward Cleanup\n");

//	VERIFY(m_syncEvent = CreateEvent(NULL, FALSE, FALSE, NULL));
	CloseThreadpoolWork(m_pwk);
//	if (m_sequenceRunning) WaitForSingleObject(m_syncEvent, INFINITE);

	//TRACE("Reward Cleanup 1\n");
	//m_eventThreadRunning = false;
	//WaitForSingleObject(m_syncEvent, INFINITE);
	//TRACE("Reward Cleanup 2\n");

//	CloseHandle(m_syncEvent);
	if (!(m_pulseActive || m_sequenceRunning))
	{
		DAQstatus = DAQmxStartTask(m_rewardTask);
		DAQCheckStatus();
}
	DAQstatus = DAQmxWriteDigitalScalarU32(m_rewardTask, false, 0, 0, NULL);
	DAQCheckStatus();
	DAQstatus = DAQmxStopTask(m_rewardTask);
	DAQCheckStatus();
	DAQstatus = DAQmxClearTask(m_rewardTask);
	DAQCheckStatus();
}

void CReward::StartSequence(BYTE nParams, short* params)
{
	if (m_sequenceRunning)
	{
		CLog::AddToLog(_T("Will not run multiple reward sequences in parallel."));
		return;
	}
	m_sequenceRunning = true;
	for (BYTE i=0; i<nParams; i++)
	{
		m_times[i] = *params++;
		if ((i & 1) == 0) m_totalTime += m_times[i];
	}
	m_nTimes = nParams;
	SubmitThreadpoolWork(m_pwk);
}

DWORD CReward::TotalTime()
{
	DWORD totalTime = m_totalTime;
	m_totalTime = 0;
	return totalTime;
}

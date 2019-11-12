#include "StdAfx.h"
#include "Reward.h"
#include "Log.h"

extern int32 DAQstatus;

TaskHandle CReward::m_rewardTask;
short CReward::m_times[];
bool CReward::m_sequenceRunning;
BYTE CReward::m_nTimes;
HANDLE CReward::m_rewardDoneEvent;
PTP_WORK CReward::m_pwk;
CnidaqServerDoc* CReward::m_pDoc;
DWORD CReward::m_totalTime;

extern void DAQCheckStatus(void);

UINT CReward::EventProcedure( LPVOID pParam )
{
	HANDLE rewardEvent;
	VERIFY(rewardEvent = CreateEvent(NULL, FALSE, FALSE, _T("Reward")));
	CString LogEntry;
	while (true)
	{
		VERIFY(0 == WaitForSingleObject(rewardEvent,INFINITE));
		m_totalTime += m_pDoc->m_rewardTime;
		if (theApp.m_logRewardTriggers) LogEntry.Format(_T("Reward trigger (%u ms)."), m_pDoc->m_rewardTime);
		if (m_pDoc->m_rewardCode != 0)
		{
			CDAQmx::WriteEventMarker(&m_pDoc->m_rewardCode);
			if (theApp.m_logRewardTriggers) LogEntry.AppendFormat(_T(" Marker code %u."), m_pDoc->m_rewardCode);
		}
		DAQstatus = DAQmxStartTask(m_rewardTask);
		DAQCheckStatus();
		DAQstatus = DAQmxWriteDigitalScalarU32(m_rewardTask, false, 0, 8, NULL);
		DAQCheckStatus();
		Sleep(m_pDoc->m_rewardTime);
		DAQstatus = DAQmxWriteDigitalScalarU32(m_rewardTask, false, 0, 0, NULL);
		DAQCheckStatus();
		DAQstatus = DAQmxStopTask(m_rewardTask);
		DAQCheckStatus();
		VERIFY(SetEvent(m_rewardDoneEvent));
		if (theApp.m_logRewardTriggers) CLog::AddToLog(LogEntry);
	}
	return 0;
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

void CReward::Init(void)
{
	POSITION pos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* temp = theApp.GetNextDocTemplate(pos);
	pos = temp->GetFirstDocPosition();
	m_pDoc = (CnidaqServerDoc*) temp->GetNextDoc(pos);

	VERIFY(m_rewardDoneEvent = CreateEvent(NULL, FALSE, FALSE, _T("RewardDone")));
	DAQstatus = DAQmxCreateTask("RewardTask", &m_rewardTask);
	DAQCheckStatus();
	DAQstatus =  DAQmxCreateDOChan (m_rewardTask, "Dev1/port2/line3", "RewardChannel", DAQmx_Val_ChanPerLine);
	DAQCheckStatus();
	VERIFY(AfxBeginThread(CReward::EventProcedure, NULL));
	m_pwk = CreateThreadpoolWork(&Sequence, NULL, NULL);
	m_totalTime = 0;
}

void CReward::Cleanup(void)
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
	DAQstatus = DAQmxStartTask(m_rewardTask);
	DAQCheckStatus();
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

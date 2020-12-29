#pragma once
#include <NIDAQmx.h>
//#include "nidaqServer.h"
//#include "nidaqServerDoc.h"
//#include <afx.h>


class CauxLine
{
public:
	CauxLine();
//	CauxLine(char name[]);
	CauxLine(BYTE lineNumber, char name[]);
	virtual ~CauxLine();
protected:
	BYTE m_lineNumber;
	BYTE m_mask;
	char* m_name;
};

class CPulseLine : public CauxLine
{
public:
	CPulseLine();
	CPulseLine(BYTE lineNumber, char name[]);
	virtual ~CPulseLine();
	short m_rewardTime{ 100 };
	short m_rewardCode{ 0 };
	bool m_logPulseTriggers{ false };
	//	static UINT EventProcedure(LPVOID pParam);
protected:
	HANDLE m_pulseDoneEvent;
	TaskHandle m_pulseTask;
	DWORD m_totalTime;
	bool m_pulseActive{ false };
	static UINT EventProcedure(LPVOID pParam);
	void DoPulse(void);
	HANDLE m_pulseEvent;
};

class CReward : public CPulseLine
{
public:
	CReward();
	virtual ~CReward();
	//static void Init(void);
	//static void Cleanup(void);
//	static void StartSequence(BYTE nParams, short* params);
//	static DWORD TotalTime(void);
	void StartSequence(BYTE nParams, short* params);
	DWORD TotalTime(void);
private:
	static UINT EventProcedure(LPVOID pParam);
//	static UINT EventProcedure(CReward* pParam);
//	UINT EventProcedure( LPVOID pParam );
	void DoPulse(void);
	static VOID CALLBACK Sequence(
		PTP_CALLBACK_INSTANCE Instance,
		PVOID                 Parameter,
		PTP_WORK              Work
		);
	static TaskHandle m_rewardTask;
	static short m_times[15];
	static PTP_WORK m_pwk;
//	static CnidaqServerDoc* m_pDoc;
	static BYTE m_nTimes;
	static bool m_sequenceRunning;
	static HANDLE m_rewardDoneEvent;
//	static DWORD m_totalTime;
};

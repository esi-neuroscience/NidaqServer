#pragma once
#include "nidaqServer.h"
#include "nidaqServerDoc.h"

class CReward
{
public:
	static void Init(void);
	static void Cleanup(void);
	static void StartSequence(BYTE nParams, short* params);
	static DWORD TotalTime(void);
private:
	static UINT EventProcedure( LPVOID pParam );
	static VOID CALLBACK Sequence(
		PTP_CALLBACK_INSTANCE Instance,
		PVOID                 Parameter,
		PTP_WORK              Work
		);
	static TaskHandle m_rewardTask;
	static short m_times[15];
	static PTP_WORK m_pwk;
	static CnidaqServerDoc* m_pDoc;
	static BYTE m_nTimes;
	static bool m_sequenceRunning;
	static HANDLE m_rewardDoneEvent;
	static DWORD m_totalTime;
};


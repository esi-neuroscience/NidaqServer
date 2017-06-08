#pragma once

#include "NIDAQmx.h"

// change detection works on port 0 only!
const BYTE LEVER_LINE = 5;
const BYTE PHOTODIODE_LINE = 6;

static int32 DAQstatus;
static void DAQCheckStatus(void);

class CDAQmxTask
{
public:
	CDAQmxTask(void);
	~CDAQmxTask(void);
protected:
	TaskHandle m_taskHandle;
public:
	void CreateDOChan(char* lines, char* name);
	void CreateCOPulseChanTime(char* counter, char* name, float64 highTime);
};

class CESI_Reward :
	public CDAQmxTask
{
public:
	CESI_Reward(void);
	~CESI_Reward(void);
	void Start(void);
};


class CChangeDetectionLine
{
public:
	CChangeDetectionLine(BYTE lineNumber, char* onName, char* offName);
	~CChangeDetectionLine(void);
	BYTE m_lineNumber;
	BYTE m_mask;
	HANDLE m_onEvent;
	HANDLE m_offEvent;
	char* m_onName;
	char* m_offName;
	void SignalEvent(uInt32 value, uInt32 changedBits);
};


class CChangeDetection
{
public:
	CChangeDetection(void);
	~CChangeDetection(void);
	static void AddLine(BYTE lineNumber, char* onName, char* offName);
	static void Start(void);
	static int32 CVICALLBACK ChangeDetectionCallback(TaskHandle taskHandle, int32 signalID, void *callbackData);
private:
	static TaskHandle m_taskHandle;
	static char m_lines[];
	static BYTE m_nLines;
	static BYTE m_lineMask;
	static uInt32 m_value;
	static CChangeDetectionLine* m_pLines[8];
public:
	static bool Running(void);
};

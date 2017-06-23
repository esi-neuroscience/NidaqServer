#pragma once

#include "NIDAQmx.h"

// change detection works on port 0 only!
//const BYTE LEVER_LINE = 5;
//const BYTE PHOTODIODE_LINE = 6;

static int32 DAQstatus;
static void DAQCheckStatus(void);

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

class CChangeDetection;

class CDAQmxDevice
{
public:
//	virtual void AddInputLine(BYTE lineNumber, char* onName, char* offName) = 0;
	virtual void AddLine(BYTE lineNumber, char* onName, char* offName) = 0;
	virtual void StartChangeDetection(void) = 0;
//	CDAQmxDevice(void);
//	~CDAQmxDevice(void);
protected:
	CChangeDetection* m_pChangeDetection;
};

class CChangeDetection
{
public:
//	CChangeDetection(void);
	CChangeDetection(CDAQmxDevice* pDevice);
	~CChangeDetection(void);
	static void AddLine(BYTE lineNumber, char* onName, char* offName);
	static void Start(void);
	static void Init(void);
	static int32 CVICALLBACK ChangeDetectionCallback(TaskHandle taskHandle, int32 signalID, void *callbackData);
	static VOID CALLBACK TimerCallback(PTP_CALLBACK_INSTANCE instance, PVOID pv, PTP_TIMER timer);
	static TaskHandle m_taskHandle;
private:
	static BYTE m_nLines;
	static BYTE m_lineMask;
	static uInt32 m_value;
	static CChangeDetectionLine* m_pLines[8];
	static CDAQmxDevice* m_pDevice;
public:
	static bool Running(void);
};


class CDAQmxM_Series :
	virtual public CDAQmxDevice
{
public:
	CDAQmxM_Series(void);
	~CDAQmxM_Series(void);
//	void AddInputLine(BYTE lineNumber, char* onName, char* offName) {m_pChangeDetection->AddLine(lineNumber, onName, offName);};
	void AddLine(BYTE lineNumber, char* onName, char* offName);
//	void StartChangeDetection() {m_pChangeDetection->Start();};
	void StartChangeDetection();
private:
	static char m_lines[];
};

class CDAQmxDigitalIO :
	virtual public CDAQmxDevice
{
public:
	CDAQmxDigitalIO(void);
	~CDAQmxDigitalIO(void);
//	void AddInputLine(BYTE lineNumber, char* onName, char* offName);
	void AddLine(BYTE lineNumber, char* onName, char* offName);
	void StartChangeDetection();
private:
	static BYTE m_nLines;
	static BYTE m_lineMask;
//	static uInt32 m_value;
	static CChangeDetectionLine* m_pLines[4];
};

class CDAQmx
{
public:
	static void Init(void);
//	static CDAQmxDevice* m_pDevice;
};

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



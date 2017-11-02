#pragma once

#include "NIDAQmx.h"

// change detection works on port 0 only!
//const BYTE LEVER_LINE = 5;
//const BYTE PHOTODIODE_LINE = 6;

//static int32 DAQstatus;
//static void DAQCheckStatus(void);

class CChangeDetectionLine
{
public:
	CChangeDetectionLine(void) {};
	virtual ~CChangeDetectionLine(void) {};
	virtual void SignalEvent(uInt32 value, uInt32 changedBits) = 0;
	BYTE m_mask;
};

class CChangeDetection
{
public:
	CChangeDetection(void);
	~CChangeDetection(void);
	static void UpdateLines(BYTE lineNumber);
	static void AddLine(BYTE lineNumber, char* pulseName);
	static void AddLine(BYTE lineNumber, char* onName, char* offName);
	static void Start(void);
	static void Init(void);
	static bool Running(void);
//	static int32 CVICALLBACK ChangeDetectionCallback(TaskHandle taskHandle, int32 signalID, void *callbackData);
//	static VOID CALLBACK TimerCallback(PTP_CALLBACK_INSTANCE instance, PVOID pv, PTP_TIMER timer);
	static UINT nidaqProcedure( LPVOID pParam );
	static TaskHandle m_taskHandle;
private:
	static BYTE m_nLines;
	static BYTE m_lineMask;
	static uInt32 m_value;
	static CChangeDetectionLine* m_pLines[8];
};


class CDAQmxDevice
{
public:
	virtual bool IsChangeDetectionLineValid(BYTE) = 0;
	virtual CString ValidChangeDetectionLines(void) = 0;
//	virtual void AddLine(BYTE lineNumber) = 0;
	void AddLine(BYTE lineNumber);
//	virtual void StartChangeDetection(void);
	bool StartChangeDetection(void);
	virtual ~CDAQmxDevice(void) = 0;
private:
	static char m_lines[];
};

class CDAQmxM_Series :
	virtual public CDAQmxDevice
{
public:
	CDAQmxM_Series(void);
	~CDAQmxM_Series(void);
	bool IsChangeDetectionLineValid(BYTE);
	CString ValidChangeDetectionLines(void);
//	void AddLine(BYTE lineNumber);
//	void StartChangeDetection();
};

class CDAQmxDigitalIO :
	virtual public CDAQmxDevice
{
public:
	CDAQmxDigitalIO(void);
	~CDAQmxDigitalIO(void);
	bool IsChangeDetectionLineValid(BYTE);
	CString ValidChangeDetectionLines(void);
//	void AddLine(BYTE lineNumber) {};
};

class CDAQmx
{
public:
	static void Init(void);
	static void Cleanup(void);
	static bool IsChangeDetectionLineValid(BYTE lineNumber);
	static void WriteEventMarker(short*);
	static CDAQmxDevice* m_pDevice;
private:
	static void Delay41us(void);
	static TaskHandle m_eventMarkerTask;
	static TaskHandle m_eventMarkerStrobeTask;
	static CChangeDetection m_changeDetection;
	static CRITICAL_SECTION m_eventMarkerSection;
	static LARGE_INTEGER m_41us;
	static LARGE_INTEGER m_startTime;
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

class CPulseDetectionLine :
	virtual public CChangeDetectionLine
{
public:
//	CPulseDetectionLine(void);
	CPulseDetectionLine(BYTE lineNumber, char* pulseName);
	~CPulseDetectionLine(void);
	void SignalEvent(uInt32 value, uInt32 changedBits);
	HANDLE m_pulseEvent;
	char* m_pulseName;
};

class COnOffDetectionLine :
	virtual public CChangeDetectionLine
{
public:
//	COnOffDetectionLine(void);
	COnOffDetectionLine(BYTE lineNumber, char* onName, char* offName);
	~COnOffDetectionLine(void);
	void SignalEvent(uInt32 value, uInt32 changedBits);
	HANDLE m_onEvent;
	HANDLE m_offEvent;
	char* m_onName;
	char* m_offName;
};

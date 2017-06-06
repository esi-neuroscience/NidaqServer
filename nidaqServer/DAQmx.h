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


class CESI_Lever :
	public CDAQmxTask
{
public:
	CESI_Lever(void);
	~CESI_Lever(void);
//	static int32 CVICALLBACK LeverCallback(TaskHandle taskHandle, int32 signalID, void* callbackData);
	HANDLE m_hPressEvent;
	HANDLE m_hReleaseEvent;
};


class CESI_Photodiode :
	public CDAQmxTask
{
public:
	CESI_Photodiode(void);
	~CESI_Photodiode(void);
//	static int32 CVICALLBACK LeverCallback(TaskHandle taskHandle, int32 signalID, void* callbackData);
	HANDLE m_hOnEvent;
	HANDLE m_hOffEvent;
};

class CChangeDetection :
	public CDAQmxTask
{
public:
	CChangeDetection(void);
	~CChangeDetection(void);
};


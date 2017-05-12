#pragma once

#include "NIDAQmx.h"

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
	void Start(void);
};


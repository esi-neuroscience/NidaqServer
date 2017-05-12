#pragma once
#include "DAQmx.h"
class CLever
{
public:
	CLever(void);
	~CLever(void);
protected:
	HANDLE m_hPressEvent;
	HANDLE m_hReleaseEvent;
private:
	CESI_Lever m_lever;
};


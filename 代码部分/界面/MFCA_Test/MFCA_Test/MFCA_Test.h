
// MFCA_Test.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCA_TestApp: 
// �йش����ʵ�֣������ MFCA_Test.cpp
//

class CMFCA_TestApp : public CWinApp
{
public:
	CMFCA_TestApp();

// ��д
public:
	virtual BOOL InitInstance();
	int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCA_TestApp theApp;
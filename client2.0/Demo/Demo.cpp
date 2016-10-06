
// Demo.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Demo.h"
#include "DemoDlg.h"
#include <dbghelp.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDemoApp

BEGIN_MESSAGE_MAP(CDemoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDemoApp ����

CDemoApp::CDemoApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDemoApp ����
LONG WINAPI  MyUnhandleExc(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	DWORD dwTime = GetTickCount();
	WCHAR dumpFileName[MAX_PATH] = { 0 };
	wsprintf(dumpFileName, L"%d dumpFile.dmp", dwTime);
	HANDLE lhDumpFile = CreateFile(dumpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (lhDumpFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;

	loExceptionInfo.ExceptionPointers = ExceptionInfo;

	loExceptionInfo.ThreadId = GetCurrentThreadId();

	loExceptionInfo.ClientPointers = TRUE;
	//��dump �ļ�д���ļ�
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpWithFullMemory, &loExceptionInfo, NULL, NULL);

	CloseHandle(lhDumpFile);
	return EXCEPTION_EXECUTE_HANDLER;
}
CDemoApp theApp;


// CDemoApp ��ʼ��

void CDemoApp::TryPopDialog()
{
	//__try
	{
		PopDialog();
	}
	//__except (MyUnhandleExc(GetExceptionInformation()))
	{

	}
}

void CDemoApp::PopDialog()
{
	CDemoDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}
}

BOOL CDemoApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	AfxInitRichEdit2();
	
	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	//SetUnhandledExceptionFilter(MyUnhandleExc);
	
	


	
	
	TryPopDialog();
	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


#include "stdafx.h"
#include "Demo.h"
#include "BatchLogin.h"
#include "afxdialogex.h"
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include <thread>
#include "DemoDlg.h"


#define LOGIN_NUM 200

IMPLEMENT_DYNAMIC(BatchLogin, CDialogEx)

BatchLogin::BatchLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(BatchLogin::IDD, pParent)
{

}

BatchLogin::~BatchLogin()
{

}

void BatchLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(BatchLogin, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_Login, &BatchLogin::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_Logout, &BatchLogin::OnBnClickedBtnLogout)
END_MESSAGE_MAP()


void BatchLogin::OnBnClickedBtnLogin()
{
	CString csStartID, csUserNum, csDelayTime;
	GetDlgItemText(IDC_EDIT_StartID, csStartID);
	GetDlgItemText(IDC_EDIT_UserNum, csUserNum);
	GetDlgItemText(IDC_EDIT_DelayTime, csDelayTime);
	if (csStartID.IsEmpty() || csUserNum.IsEmpty())
	{
		::MessageBox(NULL, L"请输入起始用户ID和用户数", L"提示", MB_OK);
		return;
	}
	int nStartID = _wtoi(csStartID.GetString());
	int nUserNum = _wtoi(csUserNum.GetString());
	if (nStartID == 0 || nUserNum == 0)
	{
		::MessageBox(NULL, L"请输入起始用户ID和用户数", L"提示", MB_OK);
		return;
	}
	int nDelayTime = 0;
	if (!csDelayTime.IsEmpty())
	{
		nDelayTime = _wtoi(csDelayTime.GetString());
	}

	m_bLoginTest = true;
	//m_nRecordLength = 0;

	std::thread t(&BatchLogin::LoginTestThreadProc, this, nStartID, nUserNum > LOGIN_NUM ? LOGIN_NUM : nUserNum, nDelayTime);
	t.detach();
	//m_loginTestWait.SetSignal();


	/*if (nUserNum > 300)
	{
	wchar_t wszPath[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, wszPath, MAX_PATH) <= 0)
	{
	::MessageBox(NULL, L"GetModuleFileName error", L"提示", MB_OK);
	return;
	}

	int nProcessNum = nUserNum / 300;
	if (nUserNum % 300 == 0)
	{
	--nProcessNum;
	}

	int nStart = nStartID + 300;
	for (int i = 1; i <= nProcessNum; ++i)
	{
	wchar_t wszParameter[64] = {0};
	if (i == nProcessNum)
	{
	wnsprintf(wszParameter, 64, L"logintest %d %d", nStart, nUserNum % 300);
	}
	else
	{
	wnsprintf(wszParameter, 64, L"logintest %d 300", nStart);
	}
	ShellExecute(NULL, NULL, wszPath, wszParameter, NULL, SW_SHOWNORMAL);

	nStart += 300;
	}
	}*/

	if (nUserNum > LOGIN_NUM)
	{
		std::thread loginDispatchThread(&BatchLogin::LoginTestDispatchThreadProc, this, nStartID, nUserNum, nDelayTime);
		loginDispatchThread.detach();
	}

	GetDlgItem(IDC_BTN_Login)->EnableWindow(FALSE);
}


void BatchLogin::OnBnClickedBtnLogout()
{
	for (std::map<int, IYouMeIMManager*>::iterator itr = m_loginIMManagerList.begin(); itr != m_loginIMManagerList.end(); ++itr)
	{
		YouMeIMErrorcode errorcode = itr->second->Logout();
		if (errorcode != YouMeIMErrorcode_Success)
		{
			CString csTest;
			csTest.Format(__XT("登出状态：%d"), errorcode);
			//AddRecord(csTest);
		}
	}

	/*for (std::list<IYouMeIMManager*>::iterator itr = m_loginIMManagerList.begin(); itr != m_loginIMManagerList.end(); ++itr)
	{
	IYouMeIMManager* pIMManager = *itr;
	pIMManager->UnInit();
	IYouMeIMManager::DestroyInstance(pIMManager);
	pIMManager = NULL;
	}
	m_loginIMManagerList.clear();*/
}

void BatchLogin::LoginTestThreadProc(int startID, int userNum, int delayTime)
{
	Sleep(1000);
	CDemoDlg* pDemoDlg = dynamic_cast<CDemoDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDemoDlg == NULL)
	{
		return;
	}

	int userId = startID;
	XString password(L"123456");
	for (int i = 0; i < userNum; ++i)
	{
		//m_loginTestWait.WaitTime(100);
		Sleep(100);

		wchar_t wszUserId[16] = { 0 };
		wsprintf(wszUserId, L"%d", userId);

		IYouMeIMManager* pIMManager = IYouMeIMManager::CreateInstance();
		if (pIMManager == NULL)
		{
			continue;
		}
		m_loginIMManagerList.insert(std::make_pair(userId, pIMManager));

		pIMManager->Init(__XT("YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0"),
			__XT("r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE="),
			__XT("com.youme.sdfa.test"));
		pIMManager->SetLoginCallback(pDemoDlg);

		YouMeIMErrorcode errorcode = pIMManager->Login(wszUserId, password, __XT(""));
		if (errorcode != YouMeIMErrorcode_Success)
		{
			CString csLog;
			csLog.Format(__XT("登陆失败:%d"), errorcode);
			pDemoDlg->AddRecord(csLog);
		}
		++userId;
	}
}

void BatchLogin::LoginTestDispatchThreadProc(int startID, int userNum, int delayTime)
{
	wchar_t wszPath[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, wszPath, MAX_PATH) <= 0)
	{
		return;
	}

	int nProcessNum = userNum / LOGIN_NUM;
	if (userNum % LOGIN_NUM == 0)
	{
		--nProcessNum;
	}

	int nStart = startID + LOGIN_NUM;
	for (int i = 1; i <= nProcessNum; ++i)
	{
		Sleep(LOGIN_NUM * delayTime);

		wchar_t wszParameter[64] = { 0 };
		if (i == nProcessNum && userNum % LOGIN_NUM != 0)
		{
			wnsprintf(wszParameter, 64, L"logintest %d %d %d", nStart, userNum % LOGIN_NUM, delayTime);
		}
		else
		{
			wnsprintf(wszParameter, 64, L"logintest %d %d %d", nStart, LOGIN_NUM, delayTime);
		}
		ShellExecute(NULL, NULL, wszPath, wszParameter, NULL, SW_SHOWNORMAL);

		nStart += LOGIN_NUM;
	}
}

void BatchLogin::BatchLoginTest(int startID, int userNum, int delayTime)
{
	/*CString csStartID, csUserNum, csDelayTime;
	csStartID.Format(L"%d", startID);
	csUserNum.Format(L"%d", userNum);
	csDelayTime.Format(L"%d", delayTime);
	GetDlgItem(IDC_EDIT_StartID)->SetWindowText(csStartID);
	GetDlgItem(IDC_EDIT_UserNum)->SetWindowText(csUserNum);
	GetDlgItem(IDC_EDIT_DelayTime)->SetWindowText(csDelayTime);*/

	m_bLoginTest = true;
	std::thread t(&BatchLogin::LoginTestThreadProc, this, startID, userNum > LOGIN_NUM ? LOGIN_NUM : userNum, delayTime);
	t.detach();

	//GetDlgItem(IDC_BTN_Login)->EnableWindow(FALSE);
}
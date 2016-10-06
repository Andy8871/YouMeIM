
// DemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Demo.h"
#include "DemoDlg.h"
#include "afxdialogex.h"
#include "ChatDlg.h"
#include "ContactsDialog.h"
#include "BatchLogin.h"
#include "BatchMessage.h"
#include "SettingDialog.h"
#include "CommonDlg.h"

#include <iostream>
#include <chrono>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)	: CDialogEx(CDemoDlg::IDD, pParent)
,m_pIMManager(NULL),
m_pBatchLogin(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

#if WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
#endif
	
	// 环境参数
	m_envSetting.environment = 2;
	m_envSetting.zone = ServerZone_China;
	m_envSetting.appKey = L"YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0";
	m_envSetting.appSecurity = L"r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE=";
	m_envSetting.packageName = L"com.youme.sdfa.test";

	m_nRecordLength = 0;
	m_bLoginTest = false;
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_RICHEDIT_Record, m_richEditRecord);
	DDX_Control(pDX, IDC_TAB_Contacts, m_contactsTab);
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CDemoDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CDemoDlg::OnBnClickedButtonLogout)
	ON_BN_CLICKED(IDC_BUTTON_ADDCONTACT, &CDemoDlg::OnBnClickedButtonAddcontact)
	ON_BN_CLICKED(IDC_BUTTON_UNINT, &CDemoDlg::OnBnClickedButtonUnint)
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CDemoDlg::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_CONTACTLIST, &CDemoDlg::OnBnClickedButtonContactlist)
	ON_BN_CLICKED(IDC_BUTTON_DELETECONTACT, &CDemoDlg::OnBnClickedButtonDeletecontact)
	ON_BN_CLICKED(IDC_BUTTON_GETOFFLINEMSG, &CDemoDlg::OnBnClickedButtonGetofflinemsg)
	ON_BN_CLICKED(IDC_BUTTON_ADDCHATROOM, &CDemoDlg::OnBnClickedButtonAddchatroom)
	ON_BN_CLICKED(IDC_BUTTON_LEAVECHATROOM, &CDemoDlg::OnBnClickedButtonLeavechatroom)

	ON_MESSAGE(WM_RECVINVITEDMSG, OnReceiveContactInvitedMessage)
	ON_MESSAGE(WM_RECV_CHAT_MESSAGE, OnReceiveContactChatMessage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_Contacts, &CDemoDlg::OnTcnSelchangeTabContacts)
	ON_BN_CLICKED(IDC_BUTTON_BatchLogin, &CDemoDlg::OnBnClickedButtonBatchlogin)
	ON_BN_CLICKED(IDC_BUTTON_BatchMessage, &CDemoDlg::OnBnClickedButtonBatchmessage)
END_MESSAGE_MAP()

void CDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CDemoDlg::OnInitDialog(){
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	IYouMeIMManager::Startup();
	InitIMEngine();


	m_contactsTab.InsertItem(0, L"联系人");
	ContactsDialog* pFriend = new ContactsDialog;
	pFriend->Create(IDD_CONTACTSDIALOG, GetDlgItem(IDC_TAB_Contacts));
	m_contactsMap.insert(std::make_pair(CONTACTSTYPE_FRIEND, pFriend));

	m_contactsTab.InsertItem(1, L"群组");
	ContactsDialog* pGroup = new ContactsDialog;
	pGroup->Create(IDD_CONTACTSDIALOG, GetDlgItem(IDC_TAB_Contacts));
	m_contactsMap.insert(std::make_pair(CONTACTSTYPE_GROUP, pGroup));

	m_contactsTab.InsertItem(2, L"聊天室");
	ContactsDialog* pChatRoom = new ContactsDialog;
	pChatRoom->Create(IDD_CONTACTSDIALOG, GetDlgItem(IDC_TAB_Contacts));
	m_contactsMap.insert(std::make_pair(CONTACTSTYPE_CHATROOM, pChatRoom));


	CSize titleSize = CSize(80, 28);
	m_contactsTab.SetItemSize(titleSize);
	CRect rect;
	m_contactsTab.GetClientRect(&rect);
	rect.top += titleSize.cy;
	for (std::map<ContactsType, ContactsDialog*>::const_iterator itr = m_contactsMap.begin(); itr != m_contactsMap.end(); ++itr)
	{
		itr->second->MoveWindow(&rect);
	}
	m_contactsMap[CONTACTSTYPE_FRIEND]->ShowWindow(true);
	m_contactsTab.SetCurSel(0);
	m_iCurContactsTabIndex = CONTACTSTYPE_FRIEND;



	// 批量登录测试
	LPWSTR *szArglist = NULL;
	int nArgs = 0;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (nArgs == 5 && wcscmp(szArglist[1], L"logintest") == 0)
	{
		int nStartID = _wtoi(szArglist[2]);
		int nUserNum = _wtoi(szArglist[3]);
		int nDelayTime = _wtoi(szArglist[4]);

		m_bLoginTest = true;

		CString csLog;
		csLog.Format(L"起始用户ID：%d 用户数：%d 延时：%d", nStartID, nUserNum, nDelayTime);
		AddRecord(csLog);

		m_pBatchLogin = new BatchLogin(this);
		//m_pBatchLogin->Create(IDD_DLG_BatchLogin);
		m_pBatchLogin->BatchLoginTest(nStartID, nUserNum, nDelayTime);

		SetForegroundWindow();
	}
	LocalFree(szArglist);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDemoDlg::OnCancel()
{
	UnInit();

	for (std::map<ContactsType, ContactsDialog*>::iterator itr = m_contactsMap.begin(); itr != m_contactsMap.end();)
	{
		delete itr->second;
		itr = m_contactsMap.erase(itr);
	}

	CDialogEx::OnCancel();
}

void CDemoDlg::OnBnClickedButtonLogin()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	UpdateData();
	CCommonDlg dlg;
	dlg.SetTitle(__XT("登录"));
	dlg.SetLabel(__XT("ID:"), __XT("密码:"));
	int iRet = dlg.DoModal();
	if (iRet != 1)
	{
		return;
	}
	m_pIMManager->SetLoginCallback(this);
	m_pIMManager->SetContactCallback(this);
	m_pIMManager->SetGroupCallback(this);

	m_bLoginTest = false;

	CString csLog;
	YouMeIMErrorcode errorcode = m_pIMManager->Login(dlg.m_strUserName.GetString(), dlg.m_strPasswd.GetString(),__XT(""));
	if (errorcode != YouMeIMErrorcode_Success)
	{
		csLog.Format(__XT("登陆失败:%d"), errorcode);
	}
	else
	{
		csLog = __XT("正在登陆......");
	}

	AddRecord(csLog);
}

void CDemoDlg::OnLogin(YouMeIMErrorcode errorcode, IYouMeIMContactInfo* pUserInfo)
{
	CString csTest;
	if ((errorcode == YouMeIMErrorcode_Success) && (NULL != pUserInfo))
	{
		m_chatManager.Init(m_pIMManager);

		csTest.Format(__XT("登陆成功：%s"), pUserInfo->GeYouMeID().c_str());
		m_strCurUserID = pUserInfo->GeYouMeID();
		SetWindowText(m_strCurUserID.c_str());
		AddRecord(csTest);
	}
	else
	{
		csTest.Format(__XT("登陆失败：%d"), errorcode);
		AddRecord(csTest);
		return;
	}

	if (!m_bLoginTest)
	{
		YouMeIMErrorcode errorcode = m_pIMManager->GetContactManager()->GetContactList();
		if (errorcode != YouMeIMErrorcode_Success)
		{
			CString csText;
			csText.Format(__XT("获取联系人列表失败：%d"), errorcode);
			AddRecord(csText);
		}

		/*errorcode = m_pIMManager->GetGroupManager()->GetAllGroup();
		if (errorcode != YouMeIMErrorcode_Success)
		{
			CString csText;
			csText.Format(__XT("获取群组列表失败：%d"), errorcode);
			AddRecord(csText);
		}*/
	}
}

void CDemoDlg::OnLogout(YouMeIMErrorcode errorcode)
{
	CString csTest;
	if (YouMeIMErrorcode_Success == errorcode)
	{
		csTest.Format(__XT("登出成功"));

		m_strCurUserID = __XT("");
		for (std::map<ContactsType, ContactsDialog*>::iterator itr = m_contactsMap.begin(); itr != m_contactsMap.end(); ++itr)
		{
			itr->second->ClearContacts();
		}
	}
	else
	{
		csTest.Format(__XT("登出失败：%d"), errorcode);
	}
	AddRecord(csTest);
}

#include <mmsystem.h>
void CDemoDlg::OnBnClickedButtonLogout()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	YouMeIMErrorcode errorcode = m_pIMManager->Logout();
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csLog;
		csLog.Format(__XT("登出状态：%d"), errorcode);
		AddRecord(csLog);
	}
}

void CDemoDlg::OnBnClickedButtonAddcontact()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	UpdateData();
	CCommonDlg dlg;
	dlg.SetTitle(__XT("添加好友"));
	dlg.SetLabel(__XT("好友ID:"), __XT("添加理由:"));
	if (1 != dlg.DoModal())
	{
		return;
	}
	// TODO:  在此添加控件通知处理程序代码
	IYouMeIMContactManager* pContactManager = m_pIMManager->GetContactManager();
	YouMeIMErrorcode errorcode = pContactManager->AddContact(dlg.m_strUserName.GetString(),dlg.m_strPasswd.GetString());
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csLog;
		csLog.Format(__XT("添加联系人错误码：%d"), errorcode);
		AddRecord(csLog);
	}
}

void CDemoDlg::OnGetContactInfo(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnGetContactList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue)
{
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csLog;
		csLog.Format(__XT("添加获取联系人列表失败了 错误码：%d"), errorcode);
		AddRecord(csLog);
	}
	else
	{
		std::list<ContactsInfo> contactsList;
		for (std::list<IYouMeIMContactInfo*>::const_iterator itr = pValue->begin(); itr != pValue->end(); ++itr)
		{
			ContactsInfo contactsInfo;
			contactsInfo.ID = (*itr)->GeYouMeID();
			contactsInfo.type = CONTACTSTYPE_FRIEND;
			contactsList.push_back(contactsInfo);
		}
		
		std::map<ContactsType, ContactsDialog*>::const_iterator itr = m_contactsMap.find(CONTACTSTYPE_FRIEND);
		if (itr == m_contactsMap.end())
		{
			return;
		}
		itr->second->AddContacts(contactsList);

		SetCurContactsTab(CONTACTSTYPE_FRIEND);
	}
}
 
void CDemoDlg::OnGetBlacklistList(YouMeIMErrorcode errorcde, const std::list<IYouMeIMContactInfo*>* pValue)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnAddContactRequest(YouMeIMErrorcode errorcode, const XString& strUserID)
{
	CString csLog;
	csLog.Format(__XT("添加好友的请求回掉:%d  %s"), errorcode, strUserID.c_str());
	AddRecord(csLog);
}

void CDemoDlg::OnAddContactResponse(YouMeIMContactEvent evtType, IYouMeIMContactInfo* pContactInfo)
{
	CString csLog;
	if (evtType == AddContactEvent_Agree)
	{
		csLog.Format(__XT("添加好友成功:%s"), pContactInfo->GeYouMeID().c_str());	
	}
	else
	{
		csLog.Format(__XT("好友请求被拒绝"));
	}
	AddRecord(csLog);
}

void CDemoDlg::OnReceiveContactInvited(IYouMeIMContactInfo*strUserID, const XString& strReason)
{
	XString* pUserName = new XString;
	*pUserName = strUserID->GeYouMeID();

	XString* pReason = new XString;
	*pReason = strReason;
	PostMessage(WM_RECVINVITEDMSG,(WPARAM) pUserName,(LPARAM) pReason);
}

void CDemoDlg::OnContactInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMContactEvent evtType, const XString&strUserID)
{
	CString csLog;
	if (errorcode == YouMeIMErrorcode_Success)
	{
		if (evtType == AddContactEvent_Agree)
		{
			csLog.Format(__XT("成功处理添加好友请求:%s"),strUserID.c_str());
		}
		else
		{
			csLog.Format(__XT("成功拒绝添加好友请求:%s"),strUserID.c_str());
		}
	}
	else
	{
		csLog.Format(__XT("处理好友请求失败了：%d"),errorcode);
	}
	AddRecord(csLog);
}

void CDemoDlg::OnContactBeDeleted(const XString& strUserID)
{
	CString csLog;
	csLog.Format(__XT("不好意思，被好友:%s 删除了"), strUserID.c_str());
	AddRecord(csLog);
}

void CDemoDlg::OnContactAddedBlacklist(const XString& strUserID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnKickoutedBlacklist(const XString& strUserID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnDeleteContactResponse(YouMeIMErrorcode errorcode, const XString& strUserID)
{
	CString csLog;
	if (errorcode == YouMeIMErrorcode_Success)
	{		
		csLog.Format(__XT("删除好友成功:%s"), strUserID.c_str());
	}
	else
	{
		csLog.Format(__XT("删除好友失败了：%d :%s"), errorcode, strUserID.c_str());
	}
	AddRecord(csLog);
}

void CDemoDlg::OnAddBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnKickoutBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

LRESULT CDemoDlg::OnReceiveContactInvitedMessage(WPARAM w, LPARAM l)
{
	XString* pUserName = (XString*)w;
	XString* pReason = (XString*)l;
	CCommonDlg dlg;
	dlg.SetTitle(__XT("接收到好友请求"));
	dlg.SetLabel(__XT("ID:"), __XT("原因"));
	dlg.SetButtonText(__XT("同意"), __XT("拒绝"));
	dlg.SetEdit(pUserName->c_str(), pReason->c_str());
	int iRet = dlg.DoModal();
	if (iRet == 1)
	{
		//同意
		m_pIMManager->GetContactManager()->AgreeContactInvited(*pUserName);
	}
	else if (iRet == 3)
	{
		//拒绝
		m_pIMManager->GetContactManager()->RefuseContactInvited(*pUserName,__XT("没有理由"));
	}

	delete pUserName;
	delete pReason;
	return 0;
}


void CDemoDlg::OnBnClickedButtonUnint()
{
	//UnInit();
}


void CDemoDlg::OnBnClickedButtonInit()
{
	SettingDialog settingDialog;
	settingDialog.SetEnrionment(m_envSetting);
	int iRet = settingDialog.DoModal();
	if (iRet != 1)
	{
		return;
	}

	EnvSetting envSetting = settingDialog.GetEnrionment();

	if (m_envSetting == envSetting || m_pIMManager == NULL)
	{
		return;
	}

	UnInit();

	InitIMEngine();
}

void CDemoDlg::OnBnClickedButtonContactlist()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	YouMeIMErrorcode errorcode = m_pIMManager->GetContactManager()->GetContactList();
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csTest;
		csTest.Format(__XT("获取联系人列表错误码：%d"), errorcode);
		AddRecord(csTest);
	}
}

void CDemoDlg::OnBnClickedButtonDeletecontact()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	ContactsInfo* pContactsInfo = NULL;
	std::map<ContactsType, ContactsDialog*>::const_iterator itr = m_contactsMap.find(m_iCurContactsTabIndex);
	if (itr != m_contactsMap.end())
	{
		pContactsInfo = itr->second->GetSelectedItem();
	}
	if (pContactsInfo == NULL)
	{
		::MessageBox(NULL, L"未选择联系人", L"提示", MB_OK);
		return;
	}

	YouMeIMErrorcode errorcode = m_pIMManager->GetContactManager()->DeleteContact(pContactsInfo->ID);
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csLog;
		csLog.Format(__XT("删除联系人错误码：%d"), errorcode);
		AddRecord(csLog);
	}
}

void CDemoDlg::OnContactOnline(const XString& strYouMeID)
{
	CString csLog;
	csLog.Format(__XT("好友:%s 登陆了"), strYouMeID.c_str());
	AddRecord(csLog);
}

void CDemoDlg::OnContactOffline(const XString& strYouMeID)
{
	CString csLog;
	csLog.Format(__XT("好友:%s 退出了"), strYouMeID.c_str());
	AddRecord(csLog);
}


void CDemoDlg::OnBnClickedButtonGetofflinemsg()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	m_pIMManager->GetMessageManager()->GetContactOfflineMessage();
}


void CDemoDlg::OnBnClickedButtonAddchatroom()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	//m_pIMManager->GetGroupManager()->JoinChatRoom(__XT("test3d"));
	m_pIMManager->GetGroupManager()->JoinChatRoom(__XT("YoumeTestChat"));
	return;

	if (m_pIMManager == NULL || m_pIMManager->GetGroupManager() == NULL)
	{
		return;
	}
	CCommonDlg dlg;
	dlg.SetTitle(__XT("加入群组"));
	dlg.SetLabel(__XT("群组名称："), __XT("备注："));
	if (1 != dlg.DoModal())
	{
		return;
	}
	
	if (dlg.m_strUserName.GetLength() == 0)
	{
		return;
	}

	YouMeIMErrorcode errorcode = m_pIMManager->GetGroupManager()->JoinGroup(dlg.m_strUserName.GetString(), dlg.m_strPasswd.GetString());
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csLog;
		csLog.Format(__XT("加入群组失败：%d"), errorcode);
		AddRecord(csLog);
	}
}

void CDemoDlg::OnBnClickedButtonLeavechatroom()
{
	if (m_pIMManager == NULL)
	{
		::MessageBox(NULL, L"未初始化", L"提示", MB_OK);
		return;
	}

	// TODO:  在此添加控件通知处理程序代码
	m_pIMManager->GetGroupManager()->LeaveChatRoom(__XT("test3d"));
}

void CDemoDlg::OnGetAllGroup(YouMeIMErrorcode errorcode, const std::list<IYouMeIMGroupInfo*>* pGroupInfoList)
{
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csLog;
		csLog.Format(__XT("获取群组列表失败：%d"), errorcode);
		AddRecord(csLog);
	}
	else
	{
		std::list<ContactsInfo> contactsList;
		for (std::list<IYouMeIMGroupInfo*>::const_iterator itr = pGroupInfoList->begin(); itr != pGroupInfoList->end(); ++itr)
		{
			ContactsInfo contactsInfo;
			contactsInfo.ID = (*itr)->GetGroupID();
			contactsInfo.name = (*itr)->GetGroupName();
			contactsInfo.type = CONTACTSTYPE_GROUP;
			contactsList.push_back(contactsInfo);
		}

		std::map<ContactsType, ContactsDialog*>::const_iterator itr = m_contactsMap.find(CONTACTSTYPE_GROUP);
		if (itr == m_contactsMap.end())
		{
			return;
		}
		itr->second->AddContacts(contactsList);

		SetCurContactsTab(CONTACTSTYPE_GROUP);
	}
}

void CDemoDlg::OnCreateGroup(YouMeIMErrorcode errorcode, XINT64 iRequestID, IYouMeIMGroupInfo* pGroupInfo)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnDestroyGroup(YouMeIMErrorcode errorcode, const XString& strGroupID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnGroupDestroyed(const XString& strGroupID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnGetGroupMembers(YouMeIMErrorcode errorcode, const XString& strGroupID, const std::list<IYouMeIMGroupMemberInfo*>* pGroupMembers)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnJoinGroupRequest(YouMeIMErrorcode errorcode, const XString& strGroupID)
{
	
}

void CDemoDlg::OnJoinGroupResponse(YouMeIMErrorcode errorcode, YouMeIMGroupEvent evtType, IYouMeIMGroupInfo* pGroupInfo)
{
	if (errorcode != YouMeIMErrorcode_Success)
	{
		CString csLog;
		csLog.Format(__XT("加入群组失败：%d"), errorcode);
		AddRecord(csLog);
		return;
	}
	
	std::map<ContactsType, ContactsDialog*>::const_iterator itr = m_contactsMap.find(CONTACTSTYPE_CHATROOM);
	if (itr != m_contactsMap.end())
	{
		ContactsInfo contactsInfo;
		contactsInfo.ID = pGroupInfo->GetGroupID();
		contactsInfo.name = pGroupInfo->GetGroupName();
		contactsInfo.type = CONTACTSTYPE_CHATROOM;// (ContactsType)pGroupInfo->GetGroupType();

		itr->second->AddContacts(contactsInfo);

		m_contactsTab.SetCurSel(CONTACTSTYPE_CHATROOM);
		SetCurContactsTab(CONTACTSTYPE_CHATROOM);
	}
}

void CDemoDlg::OnReceiveGroupInvited(const XString&strGroupID, IYouMeIMContactInfo* pContact, const XString& strReason)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnGroupInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMGroupEvent evtType, const XString&strGroupID, const XString& strYouMeID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnOtherJoinGroup(const XString&strGroupID, IYouMeIMGroupMemberInfo* pMemberInfo)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnKickMemberResponse(YouMeIMErrorcode errorcode, const XString&strGroupID, const XString&strYouMeID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnOtherKickedGroup(const XString&strGroupID, const XString& strYouMeID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnLeaveGroupResponse(YouMeIMErrorcode errorcode, const XString&strGroupID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::OnOtherLeaveGroup(const XString&strGroupID, const XString& strYouMeID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CDemoDlg::AddRecord(CString& text)
{
	text.Append(L"\r\n");
	m_richEditRecord.SetSel(m_nRecordLength, m_nRecordLength);
	m_richEditRecord.ReplaceSel(text);
	m_nRecordLength += text.GetLength();
}

void CDemoDlg::UnInit()
{
	if (m_pIMManager == NULL)
	{
		return;
	}

	if (m_pBatchLogin != NULL)
	{
		m_pBatchLogin->CloseWindow();
		delete m_pBatchLogin;
		m_pBatchLogin = NULL;
	}
	if (m_pBatchMessage != NULL)
	{
		m_pBatchMessage->CloseWindow();
		delete m_pBatchMessage;
		m_pBatchMessage = NULL;
	}

	for (std::map<ContactsType, ContactsDialog*>::iterator itr = m_contactsMap.begin(); itr != m_contactsMap.end(); ++itr)
	{
		itr->second->ClearContacts();
	}

	if (m_pIMManager != NULL)
	{
		m_pIMManager->UnInit();

		//Sleep(2000);
		//IYouMeIMManager::DestroyInstance(m_pIMManager);
		//m_pIMManager = NULL;
	}
}

void CDemoDlg::InitIMEngine()
{
	if (m_pIMManager == NULL)
	{
		m_pIMManager = IYouMeIMManager::CreateInstance();
	}
	if (m_pIMManager != NULL)
	{
		IYouMeIMManager::SetMode(m_envSetting.environment);
		IYouMeIMManager::SetServerZone(m_envSetting.zone);

		m_pIMManager->Init(m_envSetting.appKey, m_envSetting.appSecurity, m_envSetting.packageName);
		m_pIMManager->SetLoginCallback(this);
	}
}

XString CDemoDlg::GetCurrentUserID()
{
	return m_strCurUserID;
}

ChatManager* CDemoDlg::GetChatManager()
{
	return &m_chatManager;
}

LRESULT CDemoDlg::OnReceiveContactChatMessage(WPARAM w, LPARAM l)
{
	m_chatManager.ShowMessage((MessageInfo*) w);
	return 0;
}


void CDemoDlg::OnTcnSelchangeTabContacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	ContactsType index = (ContactsType)m_contactsTab.GetCurSel();
	SetCurContactsTab(index);
}

void CDemoDlg::SetCurContactsTab(ContactsType index)
{
	std::map<ContactsType, ContactsDialog*>::const_iterator itr = m_contactsMap.find(m_iCurContactsTabIndex);
	if (itr != m_contactsMap.end())
	{
		itr->second->ShowWindow(false);
	}
	itr = m_contactsMap.find(index);
	if (itr != m_contactsMap.end())
	{
		m_iCurContactsTabIndex = index;
		itr->second->ShowWindow(true);
	}
}

void CDemoDlg::OnBnClickedButtonBatchlogin()
{
	/*if (m_pBatchLogin == NULL)
	{
		m_pBatchLogin = new BatchLogin(this);
		m_pBatchLogin->Create(IDD_DLG_BatchLogin);
	}

	m_pBatchLogin->ShowWindow(true);

	m_bLoginTest = true;*/
}


void CDemoDlg::OnBnClickedButtonBatchmessage()
{
	if (m_pIMManager == NULL || m_strCurUserID.empty())
	{
		::MessageBox(NULL, L"未初始化或未登录", L"提示", MB_OK);
		return;
	}

	if (m_pBatchMessage == NULL)
	{
		m_pBatchMessage = new BatchMessage(this);
		m_pBatchMessage->Create(IDD_DLG_Batch_Message);
	}

	m_pBatchMessage->ShowWindow(true);
}

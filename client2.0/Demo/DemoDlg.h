#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include "afxwin.h"
#include "resource.h"
#include "ChatManager.h"
#include "ContactsDialog.h"
#include "SettingDialog.h"


#define WM_RECVINVITEDMSG (WM_USER + 100)
#define WM_RECV_CHAT_MESSAGE (WM_USER + 101)

class BatchLogin;
class BatchMessage;

class CDemoDlg : public CDialogEx,
	public IYouMeIMLoginCallback,
	public IYouMeIMContactCallback,
	public IYouMeIMGroupCallback
{
public:
	CDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

	enum { IDD = IDD_DEMO_DIALOG };

	XString GetCurrentUserID();
	ChatManager* GetChatManager();
	IYouMeIMManager* GetYouMeIMManager(){ return m_pIMManager; };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual void OnCancel();


private:
	//全部的自定义消息
	afx_msg LRESULT OnReceiveContactInvitedMessage(WPARAM w, LPARAM l);
	afx_msg LRESULT OnReceiveContactChatMessage(WPARAM w, LPARAM l);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonLogout();
	afx_msg void OnBnClickedButtonAddcontact();

	afx_msg void OnBnClickedButtonUnint();
	afx_msg void OnBnClickedButtonInit();
	afx_msg void OnBnClickedButtonContactlist();

	afx_msg void OnBnClickedButtonDeletecontact();

	afx_msg void OnBnClickedButtonGetofflinemsg();
	afx_msg void OnBnClickedButtonAddchatroom();

	afx_msg void OnBnClickedButtonLeavechatroom();
	afx_msg void OnBnClickedButtonBatchlogin();
	afx_msg void OnBnClickedButtonBatchmessage();
	afx_msg void OnTcnSelchangeTabContacts(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnLogin(YouMeIMErrorcode errorcode, IYouMeIMContactInfo* pUserInfo) override;
	virtual void OnLogout(YouMeIMErrorcode errorcode) override;

	// IYouMeIMContactCallback
	virtual void OnGetContactInfo(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override;
	virtual void OnGetContactList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override;
	virtual void OnGetBlacklistList(YouMeIMErrorcode errorcde, const std::list<IYouMeIMContactInfo*>* pValue) override;
	virtual void OnAddContactRequest(YouMeIMErrorcode errorcode, const XString& strUserID) override;
	virtual void OnAddContactResponse(YouMeIMContactEvent evtType, IYouMeIMContactInfo* pContactInfo) override;
	virtual void OnReceiveContactInvited(IYouMeIMContactInfo*strUserID, const XString& strReason) override;
	virtual void OnContactInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMContactEvent evtType, const XString&strUserID) override;
	virtual void OnContactBeDeleted(const XString& strUserID) override;
	virtual void OnContactAddedBlacklist(const XString& strUserID) override;
	virtual void OnKickoutedBlacklist(const XString& strUserID) override;
	virtual void OnDeleteContactResponse(YouMeIMErrorcode errorcode, const XString& strUserID) override;
	virtual void OnAddBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID) override;
	virtual void OnKickoutBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID) override;
	virtual void OnContactOnline(const XString& strYouMeID) override;
	virtual void OnContactOffline(const XString& strYouMeID) override;

	// IYouMeIMGroupCallback
	virtual void OnGetAllGroup(YouMeIMErrorcode errorcode, const std::list<IYouMeIMGroupInfo*>* pGroupInfoList) override;
	virtual void OnCreateGroup(YouMeIMErrorcode errorcode, XINT64 iRequestID, IYouMeIMGroupInfo* pGroupInfo) override;
	virtual void OnDestroyGroup(YouMeIMErrorcode errorcode, const XString& strGroupID) override;
	virtual void OnGroupDestroyed(const XString& strGroupID) override;
	virtual void OnGetGroupMembers(YouMeIMErrorcode errorcode, const XString& strGroupID, const std::list<IYouMeIMGroupMemberInfo*>* pGroupMembers) override;
	virtual void OnJoinGroupRequest(YouMeIMErrorcode errorcode, const XString& strGroupID) override;
	virtual void OnJoinGroupResponse(YouMeIMErrorcode errorcode, YouMeIMGroupEvent evtType, IYouMeIMGroupInfo* pGroupInfo) override;
	virtual void OnReceiveGroupInvited(const XString&strGroupID, IYouMeIMContactInfo* pContact, const XString& strReason) override;
	virtual void OnGroupInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMGroupEvent evtType, const XString&strGroupID, const XString& strYouMeID) override;
	virtual void OnOtherJoinGroup(const XString&strGroupID, IYouMeIMGroupMemberInfo* pMemberInfo) override;
	virtual void OnKickMemberResponse(YouMeIMErrorcode errorcode, const XString&strGroupID, const XString&strYouMeID) override;
	virtual void OnOtherKickedGroup(const XString&strGroupID, const XString& strYouMeID) override;
	virtual void OnLeaveGroupResponse(YouMeIMErrorcode errorcode, const XString&strGroupID) override;
	virtual void OnOtherLeaveGroup(const XString&strGroupID, const XString& strYouMeID) override;
	
	void AddRecord(CString& text);

private:
	void InitIMEngine();
	void SetCurContactsTab(ContactsType index);
	void UnInit();

	IYouMeIMManager* m_pIMManager;
	BatchLogin* m_pBatchLogin;
	BatchMessage* m_pBatchMessage;
	ChatManager m_chatManager;

	CRichEditCtrl m_richEditRecord;
	CTabCtrl m_contactsTab;
	ContactsType m_iCurContactsTabIndex;
	std::map<ContactsType, ContactsDialog*> m_contactsMap;
	
	long m_nRecordLength;
	bool m_bLoginTest;
	EnvSetting m_envSetting;

	XString m_strCurUserID;	
};

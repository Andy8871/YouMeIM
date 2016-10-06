#pragma once

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>


enum ContactsType
{
	CONTACTSTYPE_FRIEND,
	CONTACTSTYPE_GROUP,
	CONTACTSTYPE_CHATROOM
};

struct ContactsInfo
{
	XString ID;
	XString name;
	ContactsType type;
};


class ContactsDialog : public CDialog
{
	DECLARE_DYNAMIC(ContactsDialog)

public:
	ContactsDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ContactsDialog();

	enum { IDD = IDD_CONTACTSDIALOG };


	void AddContacts(const std::list<ContactsInfo>& contactsList, bool reset = true);
	void AddContacts(ContactsInfo& contactsInfo);
	void DeleteContacts(XString userID);
	void ClearContacts();
	ContactsInfo* GetSelectedItem();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	

	DECLARE_MESSAGE_MAP()

private:
	CListBox m_contactsList;

public:
	afx_msg void OnLbnDblclkListContacts();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

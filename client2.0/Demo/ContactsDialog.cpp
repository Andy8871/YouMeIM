#include "stdafx.h"
#include "Demo.h"
#include "ContactsDialog.h"
#include "afxdialogex.h"
#include <YouMeIMEngine/YouMeInterface.h>
#include "DemoDlg.h"



#define	ITEM_HEIGHT 30

IMPLEMENT_DYNAMIC(ContactsDialog, CDialog)

ContactsDialog::ContactsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ContactsDialog::IDD, pParent)
{

}

ContactsDialog::~ContactsDialog()
{
	ClearContacts();
}

void ContactsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_Contacts, m_contactsList);
}

BEGIN_MESSAGE_MAP(ContactsDialog, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_Contacts, &ContactsDialog::OnLbnDblclkListContacts)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL ContactsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void ContactsDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_contactsList.m_hWnd == NULL)
	{
		return;
	}
	
	CRect rect;
	GetClientRect(&rect);
	m_contactsList.MoveWindow(rect);
}

void ContactsDialog::AddContacts(const std::list<ContactsInfo>& contactsList, bool reset)
{
	if (reset)
	{
		m_contactsList.ResetContent();
	}

	for (std::list<ContactsInfo>::const_iterator itr = contactsList.begin(); itr != contactsList.end(); ++itr)
	{
		ContactsInfo* pContactsInfo = new ContactsInfo;
		pContactsInfo->ID = itr->ID;
		pContactsInfo->type = itr->type;
		pContactsInfo->name = itr->ID;
		if (!itr->name.empty())
		{
			pContactsInfo->name.append(__XT("_"));
			pContactsInfo->name.append(itr->name);
		}
		m_contactsList.AddString(pContactsInfo->name.c_str());
		m_contactsList.SetItemDataPtr(m_contactsList.GetCount() - 1, pContactsInfo);
		m_contactsList.SetItemHeight(m_contactsList.GetCount() - 1, ITEM_HEIGHT);
	}
}

void ContactsDialog::AddContacts(ContactsInfo& contactsInfo)
{
	ContactsInfo* pContactsInfo = new ContactsInfo;
	pContactsInfo->ID = contactsInfo.ID;
	pContactsInfo->type = contactsInfo.type;
	pContactsInfo->name = contactsInfo.ID;
	if (!contactsInfo.name.empty())
	{
		pContactsInfo->name.append(__XT("_"));
		pContactsInfo->name.append(contactsInfo.name);
	}
	m_contactsList.AddString(pContactsInfo->name.c_str());
	m_contactsList.SetItemDataPtr(m_contactsList.GetCount() - 1, pContactsInfo);
	m_contactsList.SetItemHeight(m_contactsList.GetCount() - 1, ITEM_HEIGHT);
}


void ContactsDialog::OnLbnDblclkListContacts()
{
	int iSel = m_contactsList.GetCurSel();
	if (-1 == iSel)
	{
		return;
	}
	CString csUserID;
	ContactsInfo* pContactInfo = (ContactsInfo*)m_contactsList.GetItemDataPtr(iSel);
	if (pContactInfo == NULL)
	{
		return;
	}

	YouMeIMChatType chatType;
	if (pContactInfo->type == CONTACTSTYPE_FRIEND)
	{
		chatType = ChatType_PrivateChat;
	}
	else if (pContactInfo->type == CONTACTSTYPE_GROUP)
	{
		chatType = ChatType_GroupChat;
	}
	else if (pContactInfo->type == CONTACTSTYPE_CHATROOM)
	{
		chatType = ChatType_RoomChat;
	}

	CDemoDlg* pDemoDlg = dynamic_cast<CDemoDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDemoDlg && pDemoDlg->GetChatManager() != NULL)
	{
		pDemoDlg->GetChatManager()->ShowChatDialog(pContactInfo->ID, chatType);
	}
}

void ContactsDialog::DeleteContacts(XString userID)
{
	int iIndex = -1;
	for (int i = 0; i < m_contactsList.GetCount(); ++i)
	{
		ContactsInfo* pContactInfo = (ContactsInfo*)m_contactsList.GetItemDataPtr(i);
		if (pContactInfo != NULL && pContactInfo->ID == userID)
		{
			iIndex = i;
			break;
		}
	}
	if (iIndex != -1)
	{
		ContactsInfo* pContactInfo = (ContactsInfo*)m_contactsList.GetItemDataPtr(iIndex);
		if (pContactInfo != NULL)
		{
			delete pContactInfo;
		}
		m_contactsList.DeleteString(iIndex);
	}
}

void ContactsDialog::ClearContacts()
{
	while (m_contactsList.GetCount() > 0)
	{
		ContactsInfo* pContactInfo = (ContactsInfo*)m_contactsList.GetItemDataPtr(0);
		if (pContactInfo != NULL)
		{
			delete pContactInfo;
		}
		m_contactsList.DeleteString(0);
	}
}

ContactsInfo* ContactsDialog::GetSelectedItem()
{
	ContactsInfo contactsInfo;
	int iSel = m_contactsList.GetCurSel();
	if (-1 == iSel)
	{
		return NULL;
	}
	CString csUserID;
	ContactsInfo* pContactInfo = (ContactsInfo*)m_contactsList.GetItemDataPtr(iSel);
	if (pContactInfo == NULL)
	{
		return NULL;
	}

	return pContactInfo;
}
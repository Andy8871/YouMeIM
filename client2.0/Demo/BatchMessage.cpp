#include "stdafx.h"
#include "Demo.h"
#include "BatchMessage.h"
#include "afxdialogex.h"
#include <thread>
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include "DemoDlg.h"


IMPLEMENT_DYNAMIC(BatchMessage, CDialogEx)

BatchMessage::BatchMessage(CWnd* pParent /*=NULL*/)
	: CDialogEx(BatchMessage::IDD, pParent)
{

}

BatchMessage::~BatchMessage()
{
}

void BatchMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, BatchMsg_EDT_HistoryMsg, m_edtHistory);
	DDX_Control(pDX, BatchMsg_EDT_SendMsg, m_edtSend);
	DDX_Control(pDX, BatchMsg_Comb_ChatType, m_combType);
}


BEGIN_MESSAGE_MAP(BatchMessage, CDialogEx)
	ON_BN_CLICKED(ChatDlg_BTN_BatchSend, &BatchMessage::OnBnClickedBtnBatchsend)
	ON_BN_CLICKED(BatchMsg_BTN_BatchSend, &BatchMessage::OnBnClickedBtnBatchsend)
END_MESSAGE_MAP()


BOOL BatchMessage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CDemoDlg* pDemoDlg = dynamic_cast<CDemoDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDemoDlg)
	{
		m_pIMManager = pDemoDlg->GetYouMeIMManager();
		m_strCurrentUserID = pDemoDlg->GetCurrentUserID();
	}
	GetDlgItem(BatchMsg_EDT_MsgNum)->SetWindowText(L"1");

	m_combType.InsertString(m_combType.GetCount(), L"˽��");
	m_combType.InsertString(m_combType.GetCount(), L"Ⱥ��");
	m_combType.InsertString(m_combType.GetCount(), L"������");
	m_combType.SetCurSel(0);
	
	return TRUE;
}

void BatchMessage::BatchSendThreadProc(XString msg, int msgNum, XString receiverID, YouMeIMChatType type)
{
	if (m_pIMManager == NULL)
	{
		return;
	}
	wchar_t wszCount[16] = { 0 };
	XString strMsg;
	for (int i = 0; i < msgNum; ++i)
	{
		_itow_s(i, wszCount, 10);
		strMsg = msg + L" " + wszCount;

		m_pIMManager->GetMessageManager()->SendTextMessage(receiverID, type, msg, NULL);

		wchar_t wszNick[128] = { 0 };
		wnsprintf(wszNick, 128, L"%s ˵��", m_strCurrentUserID.c_str());
		AddRecord(XString(wszNick), 1);
		AddRecord(strMsg, 2);

		Sleep(5);
	}
}

void BatchMessage::AddRecord(XString& text, int style)
{
	text.append(L"\r\n");

	CHARFORMAT cf;
	m_edtHistory.GetDefaultCharFormat(cf);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	if (style == 1)
	{
		cf.crTextColor = RGB(104, 104, 104);
	}
	else if (style == 2)
	{
		cf.crTextColor = RGB(130, 130, 130);
	}
	else
	{
		cf.crTextColor = RGB(0, 0, 0);
	}

	m_edtHistory.SetSelectionCharFormat(cf);
	m_edtHistory.SetSel(m_nRecordLength, m_nRecordLength);
	m_edtHistory.ReplaceSel(text.c_str());
	m_nRecordLength += text.length();

	//m_edtHistory.LineScroll(m_edtHistory.GetLineCount());
}

void BatchMessage::OnBnClickedBtnBatchsend()
{
	CString csSend;
	m_edtSend.GetWindowText(csSend);
	if (csSend.IsEmpty())
	{
		return;
	}

	CString csMsgNum, csReceiverID;
	GetDlgItemText(BatchMsg_EDT_MsgNum, csMsgNum);
	GetDlgItemText(BatchMsg_EDT_ReceiverID, csReceiverID);
	if (csMsgNum.IsEmpty() || csReceiverID.IsEmpty())
	{
		::MessageBox(NULL, L"������Է�ID����Ϣ����", L"��ʾ", MB_OK);
		return;
	}

	wchar_t* pwszMsgNum = csMsgNum.GetBuffer(csMsgNum.GetLength());
	csMsgNum.ReleaseBuffer();
	int nMsgNum = _wtoi(pwszMsgNum);
	if (nMsgNum == 0)
	{
		::MessageBox(NULL, L"��������Ϣ����", L"��ʾ", MB_OK);
		return;
	}


	int nSelIndex = (ServerZone) m_combType.GetCurSel();
	YouMeIMChatType chatType = ChatType_PrivateChat;
	if (0 == nSelIndex)
	{
		chatType = ChatType_PrivateChat;
	}
	else if (1 == nSelIndex)
	{
		chatType = ChatType_GroupChat;
	}
	else if (2 == nSelIndex)
	{
		chatType = ChatType_RoomChat;
	}

	m_edtSend.SetWindowText(L"");

	std::thread t(&BatchMessage::BatchSendThreadProc, this, csSend.GetString(), nMsgNum, csReceiverID.GetString(), chatType);
	t.detach();
}

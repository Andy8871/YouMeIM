#include "stdafx.h"
#include "Demo.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>
#include <thread>
#include "DemoDlg.h"
#include "ChatManager.h"


#define	ITEM_HEIGHT 30

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg() : m_nRecordLength(0)
, m_pChatManager(NULL)
, m_nReceiveCount(0)
, m_bSpeeching(false)
, m_chatType(ChatType_PrivateChat)
{

}

CChatDlg::~CChatDlg()
{

}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ChatDlg_EDT_HistoryMsg, m_edtHistory);
	DDX_Control(pDX, ChatDlg_EDT_SendMsg, m_edtSend);
	DDX_Control(pDX, ChatDlg_BTN_SendSpeech, m_btnSendSpeech);
	DDX_Control(pDX, ChatDlg_LST_Member, m_lstMember);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SENDCUSTOM, &CChatDlg::OnBnClickedButtonSendcustom)
	ON_BN_CLICKED(ChatDlg_BTN_Send, &CChatDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(ChatDlg_BTN_BatchSend, &CChatDlg::OnBnClickedBtnBatchsend)
	ON_BN_CLICKED(ChatDlg_BTN_SendSpeech, &CChatDlg::OnBnClickedBtnSendspeech)
	ON_BN_CLICKED(ChatDlg_BTN_SendFile, &CChatDlg::OnBnClickedBtnSendfile)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


BOOL CChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CDemoDlg* pDemoDlg = dynamic_cast<CDemoDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDemoDlg)
	{
		m_pChatManager = pDemoDlg->GetChatManager();
		m_pIMManager = pDemoDlg->GetYouMeIMManager();
		m_strCurrentUserID = pDemoDlg->GetCurrentUserID();
	}

	return TRUE;
}

void CChatDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (ChatType_PrivateChat == m_chatType)
	{
		CRect rectMemberList;
		m_lstMember.GetClientRect(rectMemberList);
		CRect rect;
		GetWindowRect(&rect);
		rect.right -= rectMemberList.Width() + 20;
		MoveWindow(rect);
	}

	if (ChatType_GroupChat == m_chatType || ChatType_RoomChat == m_chatType)
	{
		m_lstMember.AddString(m_strCurrentUserID.c_str());
		m_lstMember.SetItemHeight(m_lstMember.GetCount() - 1, ITEM_HEIGHT);
	}
}

XString Test_str_from_hex(const char *hex, int size)
{
	XCHAR * pBufer = new XCHAR[(2 * (size + 1))];
	pBufer[2 * size] = 0;
	static const char *TSK_HEXA_VALUES = { "0123456789abcdef" };
	for (int i = 0; i < size; i++){
		pBufer[2 * i] = TSK_HEXA_VALUES[(*(hex + i) & 0xf0) >> 4];
		pBufer[(2 * i) + 1] = TSK_HEXA_VALUES[(*(hex + i) & 0x0f)];
	}
	XString strResult;
	strResult.append(pBufer, 2 * (size + 1));
	delete pBufer;
	return  strResult;
}

void CChatDlg::OnBnClickedButtonSendcustom()
{
	char buffer[4000] = { 0 };
	buffer[10] = 1;
	buffer[15] = 2;
	buffer[510] = 3;
	buffer[1023] = 4;
	//buffer[4090] = 5;
	std::string strMessage;
	strMessage.append(buffer, sizeof(buffer));//strMessage 内容长度20 . 包含若干0
	m_pIMManager->GetMessageManager()->SendCustomMessage(m_strReceiverID, ChatType_PrivateChat, strMessage, NULL);
}

void CChatDlg::SetChatProperty(XString& receiverID, YouMeIMChatType chatType)
{
	m_strReceiverID = receiverID;
	m_chatType = chatType;

	XString strTitle;
	if (ChatType_PrivateChat == m_chatType)
	{
		strTitle = __XT("私聊 ");
	}
	else if (ChatType_GroupChat == m_chatType)
	{
		strTitle = __XT("群聊 ");
	}
	else if (ChatType_RoomChat == m_chatType)
	{
		strTitle = __XT("聊天室 ");
	}
	strTitle.append(m_strReceiverID);

	SetWindowText(strTitle.c_str());
}

void CChatDlg::ShowTextMessage(const XString& message)
{
	wchar_t wszNick[128] = { 0 };
	wnsprintf(wszNick, 128, L"%s 说：", m_strReceiverID.c_str());

	wchar_t wszMessageCount[16] = { 0 };
	wnsprintf(wszMessageCount, 16, L"%d", m_nReceiveCount++);
	XString strMessage = message;
	strMessage.append(__XT(" "));
	strMessage.append(wszMessageCount);

	AddRecord(XString(wszNick), 1);
	AddRecord(strMessage, 0);
}

void CChatDlg::ReceiveAudio(XINT64 messageID, XString text, unsigned int fileSize, unsigned int time)
{
	DownloadFile(messageID, FileType_Audio, __XT("temp.wav"));

	wchar_t wszNick[128] = { 0 };
	wnsprintf(wszNick, 128, L"%s 说：", m_strReceiverID.c_str());

	wchar_t wszMessage[64] = { 0 };
	wnsprintf(wszMessage, 64, L"语音 size：%d time:%ds", fileSize, time);
	XString strMessage = text;
	if (!strMessage.empty())
	{
		strMessage.append(__XT("\r\n"));
	}
	strMessage.append(wszMessage);
	AddRecord(XString(wszNick), 1);
	AddRecord(strMessage, 0);
}

void CChatDlg::ReceiveFile(XINT64 messageID, const XString& fileName, unsigned int fileSize)
{
	DownloadFile(messageID, FileType_Other, fileName);

	wchar_t wszNick[128] = { 0 };
	wnsprintf(wszNick, 128, L"%s 说：", m_strReceiverID.c_str());

	wchar_t wszMessage[64] = { 0 };
	wnsprintf(wszMessage, 64, L"文件 orginFileName:%s size：%d", fileName.c_str(), fileSize);
	XString strMessage(wszMessage);
	AddRecord(XString(wszNick), 1);
	AddRecord(strMessage, 0);
}

void CChatDlg::SendMessageStatus(XINT64 msgSerial, XString& text)
{
	std::vector<XINT64>::const_iterator itr = std::find(m_msgSerial.begin(), m_msgSerial.end(), msgSerial);
	if (itr == m_msgSerial.end())
	{
		return;
	}
	m_msgSerial.erase(itr);
	if (text.empty())
	{
		return;
	}
	wchar_t wszNick[128] = { 0 };
	wnsprintf(wszNick, 128, L"%s 说：", m_strCurrentUserID.c_str());
	AddRecord(XString(wszNick), 1);
	AddRecord(text, 0);
}

void CChatDlg::OnCancel()
{
	CDialogEx::OnCancel();

	if (m_pChatManager != NULL)
	{
		m_pChatManager->CloseChatDailog(m_strReceiverID);
	}
}

void CChatDlg::OnBnClickedBtnSend()
{
	CString csSend;
	m_edtSend.GetWindowText(csSend);
	if (csSend.IsEmpty())
	{
		return;
	}

	XINT64 msgSerial = 0;
	YouMeIMErrorcode errorCode = m_pIMManager->GetMessageManager()->SendTextMessage(m_strReceiverID, m_chatType, csSend.GetString(), &msgSerial);

	wchar_t wszNick[128] = { 0 };
	wnsprintf(wszNick, 128, L"%s 说：", m_strCurrentUserID.c_str());
	AddRecord(XString(wszNick), 1);
	AddRecord(XString(csSend.GetBuffer()), 2);

	if (errorCode == YouMeIMErrorcode_Success)
	{
		m_msgSerial.push_back(msgSerial);
	}
	else
	{
		CString csLog;
		csLog.Format(L"发送失败（d）", errorCode);
		AddRecord(XString(csLog.GetBuffer()), 2);
	}

	m_edtSend.SetWindowText(__XT(""));
}

void CChatDlg::AddRecord(XString& text, int style)
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


void CChatDlg::OnBnClickedBtnBatchsend()
{
	CString csSend;
	m_edtSend.GetWindowText(csSend);
	if (csSend.IsEmpty())
	{
		return;
	}

	CString csMsgNum;
	GetDlgItemText(ChatDlg_EDT_MsgNum, csMsgNum);
	if (csMsgNum.IsEmpty())
	{
		::MessageBox(NULL, L"请输入消息数量", L"提示", MB_OK);
		return;
	}

	wchar_t* pwszMsgNum = csMsgNum.GetBuffer(csMsgNum.GetLength());
	csMsgNum.ReleaseBuffer();
	int nMsgNum = _wtoi(pwszMsgNum);
	if (nMsgNum == 0)
	{
		::MessageBox(NULL, L"请输入消息数量", L"提示", MB_OK);
		return;
	}

	m_edtSend.SetWindowText(__XT(""));

	std::thread t(&CChatDlg::BatchSendThreadProc, this, csSend.GetString(), nMsgNum);
	t.detach();
}

void CChatDlg::BatchSendThreadProc(XString msg, int msgNum)
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

		m_pIMManager->GetMessageManager()->SendTextMessage(m_strReceiverID, m_chatType, strMsg, NULL);

		wchar_t wszNick[128] = { 0 };
		wnsprintf(wszNick, 128, L"%s 说：", m_strCurrentUserID.c_str());
		AddRecord(XString(wszNick), 1);
		AddRecord(strMsg, 2);

		Sleep(5);
	}
}

void CChatDlg::OnBnClickedBtnSendspeech()
{
	if (m_pIMManager == NULL || m_pIMManager->GetMessageManager() == NULL)
	{
		return;
	}
	if (m_bSpeeching)
	{
		m_pIMManager->GetMessageManager()->StopAudioMessage(__XT(""));
		m_btnSendSpeech.SetWindowText(_T("开始语音"));
	}
	else
	{
		XINT64 msgSerial;
		YouMeIMErrorcode errorCode = m_pIMManager->GetMessageManager()->SendAudioMessage(m_strReceiverID, m_chatType, &msgSerial);
		m_btnSendSpeech.SetWindowText(_T("停止语音"));

		if (errorCode == YouMeIMErrorcode_Success)
		{
			m_msgSerial.push_back(msgSerial);
		}
		else
		{
			CString csLog;
			csLog.Format(L"发送失败（d）", errorCode);
			AddRecord(XString(csLog.GetBuffer()), 2);
		}
	}
	m_bSpeeching = !m_bSpeeching;
}


void CChatDlg::OnBnClickedBtnSendfile()
{
	if (m_pIMManager == NULL || m_pIMManager->GetMessageManager() == NULL)
	{
		return;
	}
	/*std::wstring strPath = L"E:\\201681617955.wav";
	m_pIMManager->GetMessageManager()->SendFile(m_strReceiverID, m_chatType, strPath, NULL, __XT(""));
	return;
	*/


	wchar_t* szFilters = L"所有文件(*.*)|*.*";
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_HIDEREADONLY, szFilters);
	fileDlg.m_ofn.nMaxFile = 5 * MAX_PATH;	//最多可打开文件 5
	TCHAR* pPathBuffer = new TCHAR[fileDlg.m_ofn.nMaxFile];
	fileDlg.m_ofn.lpstrFile = pPathBuffer;
	ZeroMemory(fileDlg.m_ofn.lpstrFile, sizeof(TCHAR) * fileDlg.m_ofn.nMaxFile);

	std::vector<std::wstring> files;
	if (fileDlg.DoModal() == IDOK)
	{
		POSITION pos = fileDlg.GetStartPosition();
		while (pos != NULL)
		{
			CString pathName = fileDlg.GetNextPathName(pos);
			files.push_back(pathName.GetString());
		}
	}
	delete[] pPathBuffer;

	for (std::vector<std::wstring>::const_iterator itr = files.begin(); itr != files.end(); ++itr)
	{
		XINT64 msgSerial = 0;
		YouMeIMErrorcode errorCode = m_pIMManager->GetMessageManager()->SendFile(m_strReceiverID, m_chatType, *itr, &msgSerial, __XT(""));
		wchar_t wszNick[128] = { 0 };
		wnsprintf(wszNick, 128, L"%s 说：", m_strCurrentUserID.c_str());
		AddRecord(XString(wszNick), 1);
		AddRecord(XString(*itr), 2);

		if (errorCode == YouMeIMErrorcode_Success)
		{
			CString csLog;
			csLog.Format(L"发送失败（d）", errorCode);
			AddRecord(XString(csLog.GetBuffer()), 2);
		}
		else
		{
			m_msgSerial.push_back(msgSerial);
		}
	}
}


void CChatDlg::DownloadFinish(YouMeIMErrorcode errorcode, XINT64 messageID, const XString& localPath)
{
	std::map<XINT64, DownloadInfo>::const_iterator itr = m_downloadMap.find(messageID);
	if (itr == m_downloadMap.end())
	{
		return;
	}

	if (itr->second.fileType == FileType_Audio)
	{
		::PlaySound(localPath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
	else
	{
		XString strMessage(__XT("文件下载"));
		if (errorcode == YouMeIMErrorcode_Success)
		{
			strMessage.append(__XT("成功，路径："));
			strMessage.append(localPath);
		}
		else
		{
			wchar_t szCode[16] = { 0 };
			_itow_s(errorcode, szCode, 10);
			strMessage.append(__XT("失败 "));
			strMessage.append(szCode);
		}
		
		AddRecord(strMessage, 0);
	}
}

void CChatDlg::DownloadFile(XINT64 messageID, FileType type, const XString& fileName)
{
	if (m_pChatManager == NULL)
	{
		return;
	}

	std::map<XINT64, DownloadInfo>::const_iterator itr = m_downloadMap.find(messageID);
	if (itr != m_downloadMap.end())
	{
		return;
	}

	std::wstring wstrPath;
	wchar_t wszPath[MAX_PATH] = { 0 };
	if (GetTempPathW(MAX_PATH, wszPath) > 0)
	{
		wstrPath = std::wstring(wszPath);
	}

	if (type == FileType_Audio)
	{
		wstrPath.append(L"temp.wav");
	}
	else
	{
		wstrPath.append(L"FileRecv\\");
		if (GetFileAttributesW(wstrPath.c_str()) != FILE_ATTRIBUTE_DIRECTORY)
		{
			CreateDirectoryW(wstrPath.c_str(), NULL);
		}
		if (fileName.size() == 0)
		{
			wstrPath.append(__XT("temp"));
		}
		else
		{
			wstrPath.append(fileName);
		}
		
	}

	DownloadInfo downloadInfo;
	downloadInfo.fileType = type;
	downloadInfo.localPath = wstrPath;
	m_downloadMap[messageID] = downloadInfo;

	m_pChatManager->DownloadFile(messageID, wstrPath);
}
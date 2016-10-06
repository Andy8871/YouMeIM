#pragma once
#include "afxwin.h"
#include "resource.h"

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>

#include <mmsystem.h>
#include <string>


struct DownloadInfo
{
	FileType fileType;
	XString localPath;
};

class ChatManager;

class CChatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	//CChatDlg(IYouMeIMManager* pIManager,CWnd* pParent = NULL);   // 标准构造函数
	CChatDlg();   // 标准构造函数
	virtual ~CChatDlg();

	virtual void OnCancel();

	enum { IDD = IDD_DIALOG_CHATDLG };
	BOOL OnInitDialog();

	void SetChatProperty(XString& receiverID, YouMeIMChatType chatType);
	void ShowTextMessage(const XString& message);
	void ReceiveAudio(XINT64 messageID, XString text, unsigned int fileSize = 0, unsigned int time = 0);
	void ReceiveFile(XINT64 messageID, const XString& fileName, unsigned int fileSize = 0);
	void DownloadFinish(YouMeIMErrorcode errorcode, XINT64 messageID, const XString& localPath);
	void SendMessageStatus(XINT64 msgSerial, XString& text);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedButtonSendcustom();
	afx_msg void OnBnClickedBtnBatchsend();
	afx_msg void OnBnClickedBtnSendspeech();
	afx_msg void OnBnClickedBtnSendfile();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);


private:
	void AddRecord(XString& text, int = 0);
	void BatchSendThreadProc(XString msg, int msgNum);
	void DownloadFile(XINT64 messageID, FileType type, const XString& fileName);

	IYouMeIMManager* m_pIMManager;
	ChatManager* m_pChatManager;
	CRichEditCtrl m_edtHistory;
	CEdit m_edtSend;
	CButton m_btnSendSpeech;
	CListBox m_lstMember;
	YouMeIMChatType m_chatType;
	XString m_strReceiverID;
	XString m_strCurrentUserID;
	long m_nRecordLength;
	long m_nReceiveCount;
	bool m_bSpeeching;
	std::map<XINT64, DownloadInfo> m_downloadMap;
	std::vector<XINT64> m_msgSerial;
};

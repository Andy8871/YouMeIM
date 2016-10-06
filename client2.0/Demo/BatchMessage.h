#pragma once


#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>


class BatchMessage : public CDialogEx
{
	DECLARE_DYNAMIC(BatchMessage)

public:
	BatchMessage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~BatchMessage();

// �Ի�������
	enum { IDD = IDD_DLG_Batch_Message };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnBatchsend();

private:
	void BatchSendThreadProc(XString msg, int msgNum, XString receiverID, YouMeIMChatType type);
	void AddRecord(XString& text, int style);

	IYouMeIMManager* m_pIMManager;
	CRichEditCtrl m_edtHistory;
	CComboBox m_combType;
	CEdit m_edtSend;
	XString m_strCurrentUserID;
	long m_nRecordLength;
};

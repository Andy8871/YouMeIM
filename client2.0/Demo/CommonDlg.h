#pragma once


// CLogin �Ի���

class CCommonDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCommonDlg)

public:
	CCommonDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCommonDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_COMMON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
public:
	void SetLabel(const CString& strLabel1, const CString& strLabel2);
	void SetTitle(const CString& strTitle);
	void SetButtonText(const CString& strOkText, const CString& strCancleText);
	void SetEdit(const CString& strLabel1, const CString& strLabel2);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strUserName;
	CString m_strPasswd;

	CString m_strTitle;
	CString m_strBtnOK;
	CString m_strBtnCancle;
	CString m_strLabel1;
	CString m_strLabel2;
	CString m_strEdit1;
	CString m_strEdit2;
	afx_msg void OnBnClickedCancel();
};

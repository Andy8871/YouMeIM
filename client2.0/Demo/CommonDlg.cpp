// Login.cpp : 实现文件
//

#include "stdafx.h"
#include "Demo.h"
#include "CommonDlg.h"
#include "afxdialogex.h"


// CLogin 对话框

IMPLEMENT_DYNAMIC(CCommonDlg, CDialogEx)

CCommonDlg::CCommonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCommonDlg::IDD, pParent)
	, m_strUserName(_T(""))
	, m_strPasswd(_T(""))
{

}

CCommonDlg::~CCommonDlg()
{
}

void CCommonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PASSWND, m_strPasswd);
}


BEGIN_MESSAGE_MAP(CCommonDlg, CDialogEx)
	ON_BN_CLICKED(ID_OK, &CCommonDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_CANCEL, &CCommonDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLogin 消息处理程序


void CCommonDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	EndDialog(1);
}

void CCommonDlg::SetButtonText(const CString& strOkText, const CString& strCancleText)
{
	m_strBtnOK = strOkText;
	m_strBtnCancle = strCancleText;
	
}

void CCommonDlg::SetTitle(const CString& strTitle)
{
	m_strTitle = strTitle;
}

void CCommonDlg::SetLabel(const CString& strLabel1, const CString& strLabel2)
{
	m_strLabel1 = strLabel1;
	m_strLabel2 = strLabel2;

}

BOOL CCommonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText(IDC_STATIC_LABLE1, m_strLabel1);
	SetDlgItemText(IDC_STATIC_LABEL2, m_strLabel2);

	SetWindowTextW(m_strTitle);

	if (!m_strBtnOK.IsEmpty())
	{
		SetDlgItemText(ID_OK, m_strBtnOK);
	}
	if (!m_strBtnCancle.IsEmpty())
	{
		SetDlgItemText(ID_CANCEL, m_strBtnCancle);
	}
	if (!m_strEdit1.IsEmpty())
	{
		SetDlgItemText(IDC_EDIT_USERNAME, m_strEdit1);
	}
	if (!m_strEdit2.IsEmpty())
	{
		SetDlgItemText(IDC_EDIT_PASSWND, m_strEdit2);
	}

	GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();

	return FALSE;
}


void CCommonDlg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	EndDialog(3);
}

void CCommonDlg::SetEdit(const CString& strLabel1, const CString& strLabel2)
{
	m_strEdit1 = strLabel1;
	m_strEdit2 = strLabel2;
}

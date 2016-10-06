#include "stdafx.h"
#include "Demo.h"
#include "SettingDialog.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(SettingDialog, CDialogEx)

SettingDialog::SettingDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SettingDialog::IDD, pParent)
{
	m_zoneMap[ServerZone_China] = L"中国";
	m_zoneMap[ServerZone_Singapore] = L"新加坡";
	m_zoneMap[ServerZone_America] = L"美国";
	m_zoneMap[ServerZone_HongKong] = L"香港";
	m_zoneMap[ServerZone_Korea] = L"韩国";
	m_zoneMap[ServerZone_Australia] = L"澳洲";
	m_zoneMap[ServerZone_Deutschland] = L"德国";
	m_zoneMap[ServerZone_Brazil] = L"巴西";
	m_zoneMap[ServerZone_India] = L"印度";
	m_zoneMap[ServerZone_Japan] = L"日本";
	m_zoneMap[ServerZone_Ireland] = L"爱尔兰";
}

SettingDialog::~SettingDialog()
{

}

void SettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_Zone, m_combZone);
}


BEGIN_MESSAGE_MAP(SettingDialog, CDialogEx)
	ON_BN_CLICKED(Setting_Btn_Confirm, &SettingDialog::OnBnClickedBtnConfirm)
	ON_BN_CLICKED(Setting_Btn_Close, &SettingDialog::OnBnClickedBtnClose)
END_MESSAGE_MAP()

BOOL SettingDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (std::map<int, std::wstring>::const_iterator itr = m_zoneMap.begin(); itr != m_zoneMap.end(); ++itr)
	{
		m_combZone.InsertString(m_combZone.GetCount(), itr->second.c_str());
	}


	if (m_envSetting.environment == 0)
	{
		((CButton*)GetDlgItem(IDC_RAD_Formal))->SetCheck(TRUE);
	}
	else if (m_envSetting.environment == 1)
	{
		((CButton*)GetDlgItem(IDC_RAD_Develop))->SetCheck(TRUE);
	}
	else if (m_envSetting.environment == 2)
	{
		((CButton*)GetDlgItem(IDC_RAD_Test))->SetCheck(TRUE);
	}
	else if (m_envSetting.environment == 3)
	{
		((CButton*)GetDlgItem(IDC_RAD_Business))->SetCheck(TRUE);
	}
	m_combZone.SetCurSel(m_envSetting.zone);
	((CEdit*)GetDlgItem(Setting_Edt_appKey))->SetWindowText(m_envSetting.appKey.c_str());
	((CEdit*)GetDlgItem(Setting_Edt_appSecurity))->SetWindowText(m_envSetting.appSecurity.c_str());
	((CEdit*)GetDlgItem(Setting_Edt_packageName))->SetWindowText(m_envSetting.packageName.c_str());

	return TRUE;
}

void SettingDialog::SetEnrionment(EnvSetting setting)
{
	m_envSetting = setting;
}

EnvSetting SettingDialog::GetEnrionment()
{
	return m_envSetting;
}

void SettingDialog::OnBnClickedBtnConfirm()
{
	m_envSetting.zone = (ServerZone) m_combZone.GetCurSel();
	int iEnvironment = 2;
	if (((CButton*)GetDlgItem(IDC_RAD_Formal))->GetCheck())
	{
		m_envSetting.environment = 0;
	}
	else if (((CButton*)GetDlgItem(IDC_RAD_Business))->GetCheck())
	{
		m_envSetting.environment = 3;
	}
	else if (((CButton*)GetDlgItem(IDC_RAD_Test))->GetCheck())
	{
		m_envSetting.environment = 2;
	}
	else if (((CButton*)GetDlgItem(IDC_RAD_Develop))->GetCheck())
	{
		m_envSetting.environment = 1;
	}

	CString csAppKey, csAppSecurity, csPackageName;
	GetDlgItem(Setting_Edt_appKey)->GetWindowText(csAppKey);
	GetDlgItem(Setting_Edt_appSecurity)->GetWindowText(csAppSecurity);
	GetDlgItem(Setting_Edt_packageName)->GetWindowText(csPackageName);
	if (csAppKey.IsEmpty() || csAppSecurity.IsEmpty() || csPackageName.IsEmpty())
	{
		::MessageBox(NULL, L"请输入配置信息", L"提示", MB_OK);
		return;
	}

	m_envSetting.appKey = csAppKey.GetString();
	m_envSetting.appSecurity = csAppSecurity.GetString();
	m_envSetting.packageName = csPackageName.GetString();

	EndDialog(1);
}

void SettingDialog::OnBnClickedBtnClose()
{
	EndDialog(0);
}

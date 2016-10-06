#pragma once

#include <map>
#include <string>

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>

struct EnvSetting 
{
	int environment;
	ServerZone zone;
	std::wstring appKey;
	std::wstring appSecurity;
	std::wstring packageName;

	EnvSetting() : environment(2)
		, zone(ServerZone_China)
		, appKey(L"YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0")
		, appSecurity(L"r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE=")
		, packageName(L"com.youme.sdfa.test")
	{
	}

	bool operator==(const EnvSetting& rhs)
	{
		return environment == rhs.environment && zone == rhs.zone && appKey == rhs.appKey && appSecurity == appSecurity && packageName == rhs.packageName;
	}
};

class SettingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(SettingDialog)

public:
	SettingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SettingDialog();

	enum { IDD = IDD_DLG_Setting };

	void SetEnrionment(EnvSetting setting);
	EnvSetting GetEnrionment();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnConfirm();
	afx_msg void OnBnClickedBtnClose();

private:
	CComboBox m_combZone;
	std::map<int, std::wstring> m_zoneMap;
	EnvSetting m_envSetting;
};

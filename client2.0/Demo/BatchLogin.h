#pragma once

#include <map>


class IYouMeIMManager;

class BatchLogin : public CDialogEx
{
	DECLARE_DYNAMIC(BatchLogin)

public:
	BatchLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~BatchLogin();

	void BatchLoginTest(int startID, int userNum, int delayTime);

	enum { IDD = IDD_DLG_BatchLogin };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnLogout();

private:
	void LoginTestThreadProc(int startID, int userNum, int delayTime);
	void LoginTestDispatchThreadProc(int startID, int userNum, int delayTime);

	std::map<int, IYouMeIMManager*> m_loginIMManagerList;
	bool m_bLoginTest;
	//youmecommon::CXCondWait m_loginTestWait;
};

#pragma  once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>
#include "IIMNotifier.h"

class ReceiveContactInviteNotifier : public cocos2d::LayerColor
{
public:
	ReceiveContactInviteNotifier();
    
    virtual bool init();
	CREATE_FUNC(ReceiveContactInviteNotifier);

	void InitPara(const std::string& strID, const std::string& strReason);
	void SetCallBack(IIMReceiveInviteNotifyCallback* pCallBack);
protected:
    void initDialog();

	void OnAgreeCallEvent(Ref* pSender);

	void OnRefuseCallEvent(Ref* pSender);

private:

	cocos2d::ui::TextField* m_textContactID;
	cocos2d::ui::TextField* m_textContactReason;

	IIMReceiveInviteNotifyCallback* m_pCallBack = NULL;

	std::string m_strContactID;
};

#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "MyIMManager.h"
//#include <common/CrossPlatformDefine/PlatformDef.h>
//#include <YouMeIMEngine/YouMeInterface.h>
//#include "IIMNotifier.h"
#include "CursorTextField.h"

typedef struct LocalMessageBase : MyMessageBase
{

	std::string strLocalBody;

} LocalMessageBase;

class SoloChatLayer : public cocos2d::LayerColor,
	public IMyIMMessageResponseCallback
{
public:
	SoloChatLayer();
    
    virtual bool init();
	CREATE_FUNC(SoloChatLayer);

public:
	void InitPara(const std::string& strID);

public:
	void OnMessageResponse(const std::string& strID) override;

protected:
    void initDialog();

	void OnSendMsgCallEvent(Ref* pSender);

	void OnReturnCallEvent(Ref* pSender);

    
    void OnStartAudio(Ref* pSender);
    void OnStopAudio(Ref* pSender);
    void OnCancleAudio(Ref* pSender);
protected:
	void update(float dt);

private:
	
	cocos2d::ui::Text* m_textLabel;

	cocos2d::ui::ListView* m_chatMessageListView;

    CursorTextField* m_msgTextField;

	std::string m_strUserID;

	MyMessageList m_msgList;

	bool m_bResponseMessage = false;
};
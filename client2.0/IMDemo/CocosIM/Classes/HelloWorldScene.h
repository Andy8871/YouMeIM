#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include "YouMeNotifyCenter.h"
#include "CursorTextField.h"

class HelloWorld : public cocos2d::Layer,
	public IYouMeIMLoginCallback,
	public IYouMeNotifyObserver
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	~HelloWorld();
private:
	void OnInitCallEvent(Ref* pSender);
	
	void OnUnInitCallEvent(Ref* pSender);

	void OnLoginCallEvent(Ref* pSender);

    void OnExitCallEvent(Ref* pSender);
public:

	void OnLogin(YouMeIMErrorcode errorcode, IYouMeIMContactInfo* pUserInfo) override;

	void OnLogout(YouMeIMErrorcode errorcode) override;

	void OnMessageNotify(YouMeMSGTYPE type) override;

protected:
	void update(float dt) override;

private:
	std::string m_strTip;

private:

	CursorTextField* m_textID;
    CursorTextField* m_textPwd;
	CursorTextField* m_textNewPwd;
};

#endif // __HELLOWORLD_SCENE_H__

//
//  VolumeDialogLayer.cpp
//  CocosDemo
//
//  Created by zxxing on 16/1/9.
//
//

#include "YouMeMsgBox.h"
#include "cocostudio/CocoStudio.h"
#include "utils.h"
#include "YouMeMessage.h"

USING_NS_CC;

using namespace cocos2d::ui;

YouMeMsgBox::YouMeMsgBox()
{

}

bool YouMeMsgBox::init()
{
	bool bRet = false;

	do {
		CC_BREAK_IF(!LayerColor::initWithColor(Color4B(0, 0, 0, 200)));

		this->initDialog();

		bRet = true;
	} while (0);

	return bRet;
}

void YouMeMsgBox::initDialog()
{
	auto rootNode = CSLoader::createNode("MsgBoxLayer.csb");
	addChild(rootNode);

	cocos2d::ui::Button* btnOK = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_OK"));
	btnOK->addClickEventListener(CC_CALLBACK_1(YouMeMsgBox::OnOKCallEvent, this));

	m_TextTips = dynamic_cast<cocos2d::ui::Text*>(rootNode->getChildByName("Text_Tips"));
}

YouMeMsgBox::~YouMeMsgBox()
{
	if (2 == m_nType)
	{
		YouMeNotifyCenter::GetInstance().UnRegisterMsgObserver(this);
	}
}

void YouMeMsgBox::InitPara(const std::string& strTips, int nType)
{
	m_TextTips->setString(strTips);

	m_nType = nType;
	switch (nType)
	{
	case 1:
		this->scheduleOnce(schedule_selector(YouMeMsgBox::Collapse), 5.0f);
		break;
	case  2:
		YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_LOGINTFAIL, this);
		YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_LOGINSUCCESS, this);
		break;
	}
}

void YouMeMsgBox::OnOKCallEvent(Ref* pSender)
{
	removeFromParentAndCleanup(true);
}

void YouMeMsgBox::Collapse(float dt)
{
	removeFromParentAndCleanup(true);
}

void YouMeMsgBox::OnMessageNotify(YouMeMSGTYPE type)
{
	switch (type)
	{
	case YOUME_MSG_LOGINTFAIL:
	case YOUME_MSG_LOGINSUCCESS:
	{
		removeFromParentAndCleanup(true);
		break;
	}
	default:
		break;
	}
}
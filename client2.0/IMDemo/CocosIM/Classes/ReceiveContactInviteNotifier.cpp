//
//  VolumeDialogLayer.cpp
//  CocosDemo
//
//  Created by zxxing on 16/1/9.
//
//

#include "ReceiveContactInviteNotifier.h"
#include "cocostudio/CocoStudio.h"
#include "utils.h"

USING_NS_CC;

using namespace cocos2d::ui;

ReceiveContactInviteNotifier::ReceiveContactInviteNotifier()
{

}

bool ReceiveContactInviteNotifier::init()
{
	bool bRet = false;

	do {
		CC_BREAK_IF(!LayerColor::initWithColor(Color4B(0, 0, 0, 200)));

		this->initDialog();

		bRet = true;
	} while (0);

	return bRet;
}

void ReceiveContactInviteNotifier::initDialog()
{
	auto rootNode = CSLoader::createNode("receiveContactInviteLayer.csb");
	addChild(rootNode);

	cocos2d::ui::Button* btnAgree = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_Agree"));
	btnAgree->addClickEventListener(CC_CALLBACK_1(ReceiveContactInviteNotifier::OnAgreeCallEvent, this));

	cocos2d::ui::Button* btnRefuse = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_Refuse"));
	btnRefuse->addClickEventListener(CC_CALLBACK_1(ReceiveContactInviteNotifier::OnRefuseCallEvent, this));

	m_textContactID = dynamic_cast<cocos2d::ui::TextField*>(rootNode->getChildByName("TextField_ContactID"));
	m_textContactReason = dynamic_cast<cocos2d::ui::TextField*>(rootNode->getChildByName("TextField_ContactReason"));

}

void ReceiveContactInviteNotifier::InitPara(const std::string& strID, const std::string& strReason)
{
	m_strContactID = strID;

	m_textContactID->setString(strID);
	m_textContactReason->setString(strReason);
}

void  ReceiveContactInviteNotifier::SetCallBack(IIMReceiveInviteNotifyCallback* pCallBack)
{
	m_pCallBack = pCallBack;
}

void ReceiveContactInviteNotifier::OnAgreeCallEvent(Ref* pSender)
{
	if (NULL != m_pCallBack)
	{
		m_pCallBack->OnInviteReturnNotify(m_strContactID, true);
	}

	removeFromParentAndCleanup(true);
}

void ReceiveContactInviteNotifier::OnRefuseCallEvent(Ref* pSender)
{
	if (NULL != m_pCallBack)
	{
		m_pCallBack->OnInviteReturnNotify(m_strContactID, false);
	}

	removeFromParentAndCleanup(true);
}
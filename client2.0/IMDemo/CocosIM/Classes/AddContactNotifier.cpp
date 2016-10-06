//
//  VolumeDialogLayer.cpp
//  CocosDemo
//
//  Created by zxxing on 16/1/9.
//
//

#include "AddContactNotifier.h"
#include "cocostudio/CocoStudio.h"
#include "utils.h"

USING_NS_CC;

using namespace cocos2d::ui;

AddContactNotifier::AddContactNotifier()
{

}

bool AddContactNotifier::init()
{
	bool bRet = false;

	do {
		CC_BREAK_IF(!LayerColor::initWithColor(Color4B(0, 0, 0, 200)));

		this->initDialog();

		bRet = true;
	} while (0);

	return bRet;
}

void AddContactNotifier::initDialog()
{
	auto rootNode = CSLoader::createNode("addNotifier.csb");
	addChild(rootNode);

	cocos2d::ui::Button* btnOK = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_OK"));
	btnOK->addClickEventListener(CC_CALLBACK_1(AddContactNotifier::OnOKCallEvent, this));

	cocos2d::ui::Button* btnCancle = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_CANCLE"));
	btnCancle->addClickEventListener(CC_CALLBACK_1(AddContactNotifier::OnCancleCallEvent, this));

    m_textContactID = CursorTextField::textFieldWithPlaceHolder(this, "click and input", "fonts/Marker Felt.ttf", 22);
    m_textContactID->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_textContactID->setInputWidth(200);
    m_textContactID->setPosition(Vec2(150, 185));
    m_textContactID->setTextColor(Color4B(26, 26, 26, 255));
    this->addChild(m_textContactID, 1);
    
    m_textContactReason = CursorTextField::textFieldWithPlaceHolder(this, "click and input", "fonts/Marker Felt.ttf", 22);
    m_textContactReason->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_textContactReason->setInputWidth(200);
    m_textContactReason->setPosition(Vec2(150, 125));
    m_textContactReason->setTextColor(Color4B(26, 26, 26, 255));
    this->addChild(m_textContactReason, 1);
}

void  AddContactNotifier::SetCallBack(IIMAddContactNotifyCallback* pCallBack)
{
	m_pCallBack = pCallBack;
}

void AddContactNotifier::OnOKCallEvent(Ref* pSender)
{
	std::string strContactID = m_textContactID->getInputText();
	std::string strContactReason = m_textContactReason->getInputText();

	if (NULL != m_pCallBack)
	{
		m_pCallBack->OnAddReturnNotify(strContactID, strContactReason);
	}

	removeFromParentAndCleanup(true);
}

void AddContactNotifier::OnCancleCallEvent(Ref* pSender)
{
	removeFromParentAndCleanup(true);
}
//
//  VolumeDialogLayer.cpp
//  CocosDemo
//
//  Created by zxxing on 16/1/9.
//
//

#include "SoloChatLayer.h"
#include "cocostudio/CocoStudio.h"
#include "utils.h"
#include <wchar.h>

USING_NS_CC;

using namespace cocos2d::ui;

SoloChatLayer::SoloChatLayer()
{

}

bool SoloChatLayer::init()
{
	bool bRet = false;

	do {
		CC_BREAK_IF(!LayerColor::initWithColor(Color4B(0, 0, 0, 200)));

		this->initDialog();

		bRet = true;
	} while (0);

	return bRet;
}

void SoloChatLayer::initDialog()
{
	auto rootNode = CSLoader::createNode("SoloChatLayer.csb");
	addChild(rootNode);


	cocos2d::ui::Button* btnSend = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_Send"));
	btnSend->addClickEventListener(CC_CALLBACK_1(SoloChatLayer::OnSendMsgCallEvent, this));

	cocos2d::ui::Button* btnReturn = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_Return"));
	btnReturn->addClickEventListener(CC_CALLBACK_1(SoloChatLayer::OnReturnCallEvent, this));

//	m_msgTextField = dynamic_cast<cocos2d::ui::TextField*>(rootNode->getChildByName("TextField_MsgBox"));
    m_msgTextField = CursorTextField::textFieldWithPlaceHolder(this, "click and input", "fonts/Marker Felt.ttf", 22);
    m_msgTextField->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_msgTextField->setInputWidth(480);
    m_msgTextField->setPosition(Vec2(20, 840));
    m_msgTextField->setTextColor(Color4B(26, 26, 26, 255));
    this->addChild(m_msgTextField, 1);
	
	m_textLabel = dynamic_cast<cocos2d::ui::Text*>(rootNode->getChildByName("Text_NAME"));

	m_chatMessageListView = dynamic_cast<cocos2d::ui::ListView*>(rootNode->getChildByName("ListView_Message"));
	m_chatMessageListView->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
	m_chatMessageListView->setTouchEnabled(true);
	m_chatMessageListView->setBounceEnabled(true);
	m_chatMessageListView->setGravity(cocos2d::ui::ListView::Gravity::CENTER_HORIZONTAL);
    m_chatMessageListView->setClippingEnabled(true);
  
    //录音部分
    cocos2d::ui::Button* btnSendAudio = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_SendAudio"));
    btnSendAudio->addClickEventListener(CC_CALLBACK_1(SoloChatLayer::OnStartAudio, this));

    cocos2d::ui::Button* btnStopAudio = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_StopAudio"));
    btnStopAudio->addClickEventListener(CC_CALLBACK_1(SoloChatLayer::OnStopAudio, this));

    
    cocos2d::ui::Button* btnCancleAudio = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_CancleAudio"));
    btnCancleAudio->addClickEventListener(CC_CALLBACK_1(SoloChatLayer::OnCancleAudio, this));

    
    
	MyIMMessageContainer::Instance().SetResponseCallback(this);
}
void SoloChatLayer::OnStartAudio(Ref* pSender)
{
    MyIMManager::Instance().GetYouMeIMEngine()->GetMessageManager()->SendAudioMessage(UTF8TOXString(m_strUserID), ChatType_PrivateChat, NULL);
    PMyMessageBase pmymsg = new MyMessageBase();
    pmymsg->strBody = "正在录音....";
    pmymsg->ymType = MessageBodyType_TXT;
#ifdef WIN32
    pmymsg->strSendID = AnsiToUtf8("我");
#else
    pmymsg->strSendID = "我";
#endif
    m_msgList.push_back(pmymsg);
    m_bResponseMessage = true;
    this->scheduleUpdate();

}
void SoloChatLayer::OnStopAudio(Ref* pSender)
{
    MyIMManager::Instance().GetYouMeIMEngine()->GetMessageManager()->StopAudioMessage();
    PMyMessageBase pmymsg = new MyMessageBase();
    pmymsg->strBody = "录音结束";
    pmymsg->ymType = MessageBodyType_TXT;
#ifdef WIN32
    pmymsg->strSendID = AnsiToUtf8("我");
#else
    pmymsg->strSendID = "我";
#endif
    m_msgList.push_back(pmymsg);
    m_bResponseMessage = true;
    this->scheduleUpdate();

}
void SoloChatLayer::OnCancleAudio(Ref* pSender)
{
    MyIMManager::Instance().GetYouMeIMEngine()->GetMessageManager()->CancleAudioMessage();
    PMyMessageBase pmymsg = new MyMessageBase();
    pmymsg->strBody = "取消录音";
    pmymsg->ymType = MessageBodyType_TXT;
#ifdef WIN32
    pmymsg->strSendID = AnsiToUtf8("我");
#else
    pmymsg->strSendID = "我";
#endif
    m_msgList.push_back(pmymsg);
    m_bResponseMessage = true;
    this->scheduleUpdate();

}
void SoloChatLayer::InitPara(const std::string& strID)
{
	m_strUserID = strID;

#ifdef WIN32
	wchar_t sTips[256] = { 0 };
	swprintf(sTips, L"与 %s 的会话", UTF8TOXString(strID).c_str());
	std::string strTip = XStringToUTF8(std::wstring(sTips));
#else
    char sTips[256] = { 0 };
    sprintf(sTips, "与 %s 的会话", UTF8TOXString(strID).c_str());
    std::string strTip = XStringToUTF8(std::string(sTips));
#endif
	m_textLabel->setString(strTip);

	MyIMMessageContainer::Instance().GetMessageList(strID, m_msgList);
	if (m_msgList.size() > 0)
	{
		m_bResponseMessage = true;
		this->scheduleUpdate();
	}
}

void SoloChatLayer::OnSendMsgCallEvent(Ref* pSender)
{
	std::string strContent = m_msgTextField->getInputText();
	if (strContent.size() == 0)
		return;

	PMyMessageBase pmymsg = new MyMessageBase();
	pmymsg->strBody = strContent;
	pmymsg->ymType = MessageBodyType_TXT;
#ifdef WIN32
	pmymsg->strSendID = AnsiToUtf8("我");
#else
    pmymsg->strSendID = "我";
#endif
	m_msgList.push_back(pmymsg);

	MyIMManager::Instance().GetYouMeIMEngine()->GetMessageManager()->SendTextMessage(UTF8TOXString(m_strUserID), ChatType_PrivateChat, UTF8TOXString(strContent), NULL);

	m_bResponseMessage = true;
	this->scheduleUpdate();

	m_msgTextField->setInpuntText("");
}

void SoloChatLayer::OnReturnCallEvent(Ref* pSender)
{
    m_msgTextField->detachWithIME();
	MyIMMessageContainer::Instance().SetResponseCallback(NULL);
	removeFromParentAndCleanup(true);
}

void SoloChatLayer::OnMessageResponse(const std::string& strID)
{
	if (strID != m_strUserID)
		return;

	MyIMMessageContainer::Instance().GetMessageList(strID, m_msgList);

	if (m_msgList.size() > 0)
	{
		m_bResponseMessage = true;
		this->scheduleUpdate();
	}
}

void SoloChatLayer::update(float dt)
{
	if (m_bResponseMessage)
	{
		m_bResponseMessage = false;

		while (m_msgList.size() > 0)
		{
			PMyMessageBase pMsg = m_msgList.front();
			m_msgList.pop_front();

			cocos2d::ui::Layout *widget = cocos2d::ui::Layout::create();
			widget->setContentSize(cocos2d::Size(m_chatMessageListView->getContentSize().width, 50));
			widget->setTouchEnabled(true);

			cocos2d::ui::Text *text = cocos2d::ui::Text::create();
			text->setName("Title Text");
			text->setAnchorPoint(cocos2d::Point(0, 0));
			std::string strContent = pMsg->strSendID + " :    " + pMsg->strBody;
			text->setText(strContent);
			text->setTextColor(cocos2d::Color4B(0, 0, 0, 255));
			text->setColor(cocos2d::Color3B(144, 238, 144));
			text->setAnchorPoint(Vec2(0, 0.5f));
			text->setPosition(Vec2(0, widget->getContentSize().height / 2));

			widget->setBackGroundImage("unselected.png");
			widget->setBackGroundImageScale9Enabled(true);
			widget->addChild(text);

			m_chatMessageListView->pushBackCustomItem(widget);

			delete pMsg;
		}

		m_msgList.clear();
	}
}
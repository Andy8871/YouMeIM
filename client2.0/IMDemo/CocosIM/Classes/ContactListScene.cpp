//
//  VolumeDialogLayer.cpp
//  CocosDemo
//
//  Created by zxxing on 16/1/9.
//
//

#include "ContactListScene.h"
#include "cocostudio/CocoStudio.h"
#include "HelloWorldScene.h"
#include "utils.h"
#include "AddContactNotifier.h"
#include "ReceiveContactInviteNotifier.h"
#include "SoloChatLayer.h"
#include "MyIMManager.h"
#include "YouMeMessage.h"
#include "MyIMManager.h"
#include <wchar.h>

USING_NS_CC;

using namespace cocos2d::ui;

ContactList::ContactList()
{

}

bool ContactList::init()
{
	bool bRet = false;

	do {
		CC_BREAK_IF(!LayerColor::initWithColor(Color4B(0, 0, 0, 200)));

		this->initDialog();

		bRet = true;
	} while (0);

	return bRet;
}

void ContactList::initDialog()
{
	auto rootNode = CSLoader::createNode("ContactLayer.csb");
	addChild(rootNode);

	MyIMManager::GetYouMeIMEngine()->SetContactCallback(this);


	cocos2d::ui::Button* btnLogout = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_loginOut"));
	btnLogout->addClickEventListener(CC_CALLBACK_1(ContactList::OnLogoutCallEvent, this));

	cocos2d::ui::Button* btnContactList = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_ContactList"));
	btnContactList->addClickEventListener(CC_CALLBACK_1(ContactList::OnContactListCallEvent, this));

	cocos2d::ui::Button* btnAddContact = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_AddContact"));
	btnAddContact->addClickEventListener(CC_CALLBACK_1(ContactList::OnAddContactCallEvent, this));

	cocos2d::ui::Button* btnDeleteContact = dynamic_cast<cocos2d::ui::Button*>(rootNode->getChildByName("Button_DeleteContact"));
	btnDeleteContact->addClickEventListener(CC_CALLBACK_1(ContactList::OnDeleteContactCallEvent, this));

	m_contactListView = dynamic_cast<cocos2d::ui::ListView*>(rootNode->getChildByName("ListView_Contact"));
	m_contactListView->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
	m_contactListView->setTouchEnabled(true);
	m_contactListView->setBounceEnabled(true);
    m_contactListView->setClippingEnabled(true);
   
	m_contactListView->setGravity(cocos2d::ui::ListView::Gravity::CENTER_HORIZONTAL);
    m_contactListView->addEventListener((cocos2d::ui::ListView::ccListViewCallback)CC_CALLBACK_2(ContactList::selectedItemEvent, this));
    m_contactListView->addEventListener((cocos2d::ui::ListView::ccScrollViewCallback)CC_CALLBACK_2(ContactList::selectedItemEventScrollView, this));

	m_loginIDText = dynamic_cast<cocos2d::ui::Text*>(rootNode->getChildByName("Text_LoginID"));
	m_loginIDText->setString(MyIMManager::Instance().GetLoginUser());
	
// 	cocos2d::ui::Button* default_button = cocos2d::ui::Button::create();
// 	default_button->setName("TextButton");
// 	default_button->setTouchEnabled(true);
// 
// 	cocos2d::ui::Layout* default_item = cocos2d::ui::Layout::create();
// 	default_item->setTouchEnabled(true);
// 	default_item->setSize(default_button->getSize());
// 	default_button->setPosition(cocos2d::Vec2(default_item->getSize().width/2, default_item->getSize().height/2));
// 	default_item->addChild(default_button);
// 
// 	m_contactListView->setItemModel(default_item);
// 	m_contactListView->pushBackDefaultItem();
// 	m_contactListView->insertDefaultItem(0);

	YouMeIMErrorcode errorcode = MyIMManager::GetYouMeIMEngine()->GetContactManager()->GetContactList();

	YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_CONTACTTIP, this);
	YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_LOGOUTSUCCESS, this);
	YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_CONTACTLIST, this);
	YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_RECVCONTACTINVITE, this);

	this->scheduleUpdate();
}

ContactList::~ContactList()
{
	YouMeNotifyCenter::GetInstance().UnRegisterMsgObserver(this);
}

void ContactList::OnLogoutCallEvent(Ref* pSender)
{
	// TODO:  在此添加控件通知处理程序代码
	YouMeIMErrorcode errorcode = MyIMManager::GetYouMeIMEngine()->Logout();
	if (errorcode != YouMeIMErrorcode_Success)
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"登出状态：%d", errorcode);
		std::wstring wstrTips = sTips;
		PopTip(XStringToUTF8(wstrTips), 1);
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "登出状态：%d", errorcode);
        std::string strTips = sTips;
        PopTip(XStringToUTF8(strTips), 1);
#endif
        
        if (YouMeIMErrorcode_NotLogin == errorcode)
        {
            MyIMManager::GetYouMeIMEngine()->SetContactCallback(NULL);
            removeFromParentAndCleanup(true);
        }
	}
}

void ContactList::OnContactListCallEvent(Ref* pSender)
{
	// TODO:  在此添加控件通知处理程序代码
	YouMeIMErrorcode errorcode = MyIMManager::GetYouMeIMEngine()->GetContactManager()->GetContactList();
	if (errorcode != YouMeIMErrorcode_Success)
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"获取联系人列表错误码：%d", errorcode);
		PopTip(XStringToUTF8(std::wstring(sTips)), 1);
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "获取联系人列表错误码：%d", errorcode);
        PopTip(XStringToUTF8(std::string(sTips)), 1);
#endif
	}
}

void ContactList::OnAddContactCallEvent(Ref* pSender)
{
	// TODO:  在此添加控件通知处理程序代码	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto layer = AddContactNotifier::create();
	layer->SetCallBack(this);
	layer->ignoreAnchorPointForPosition(false);
	layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	layer->setContentSize(Size(400, 250));
	layer->setPosition(Vec2((visibleSize.width) / 2, (visibleSize.height) / 2));
	Scene* pCurSene = Director::getInstance()->getRunningScene();
	pCurSene->addChild(layer);
}

void ContactList::OnAddReturnNotify(const std::string& strID, const std::string& strReason)
{
	if (strID == "好友ID" || strID.size() == 0 )
		return;

	IYouMeIMContactManager* pContactManager = MyIMManager::GetYouMeIMEngine()->GetContactManager();
	YouMeIMErrorcode errorcode = pContactManager->AddContact(UTF8TOXString(strID), UTF8TOXString(strReason));
	if (errorcode != YouMeIMErrorcode_Success)
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"添加联系人错误码：%d", errorcode);
		PopTip(XStringToUTF8(std::wstring(sTips)), 1);
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "添加联系人错误码：%d", errorcode);
        PopTip(XStringToUTF8(std::string(sTips)), 1);
#endif
	}
}

void ContactList::OnDeleteContactCallEvent(Ref* pSender)
{
	int iSel = m_contactListView->getCurSelectedIndex();
	if (-1 == iSel)
	{
		return;
	}

	cocos2d::ui::Widget* widget = m_contactListView->getItem(iSel);
	cocos2d::ui::Text* text = static_cast<cocos2d::ui::Text*>(widget->getChildByName("Title Text"));
	std::string sText = text->getString();
	YouMeIMErrorcode errorcode = MyIMManager::GetYouMeIMEngine()->GetContactManager()->DeleteContact(UTF8TOXString(sText));
	if (errorcode != YouMeIMErrorcode_Success)
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"删除联系人错误码：%d", errorcode);
		PopTip(XStringToUTF8(std::wstring(sTips)), 1);
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "删除联系人错误码：%d", errorcode);
        PopTip(XStringToUTF8(std::string(sTips)), 1);
#endif
	}
}

void ContactList::OnBeginChatCallEvent(Ref* pSender)
{
	int iSel = m_contactListView->getCurSelectedIndex();
	if (-1 == iSel)
	{
		return;
	}

	cocos2d::ui::Widget* widget = m_contactListView->getItem(iSel);
	cocos2d::ui::Text* text = static_cast<cocos2d::ui::Text*>(widget->getChildByName("Title Text"));
	std::string sText = text->getString();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto layer = SoloChatLayer::create();
	layer->InitPara(sText);
	layer->ignoreAnchorPointForPosition(false);
	layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	layer->setContentSize(Size(visibleSize.width, visibleSize.height));
	layer->setPosition(Vec2((visibleSize.width) / 2, (visibleSize.height) / 2));
	Scene* pCurSene = Director::getInstance()->getRunningScene();
	pCurSene->addChild(layer);
}

void ContactList::OnGetContactInfo(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue)
{
}

void ContactList::OnGetContactList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue)
{
	if (errorcode != YouMeIMErrorcode_Success)
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"添加获取联系人列表失败了 错误码：%d", errorcode);
		m_strTip = XStringToUTF8(std::wstring(sTips));
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "添加获取联系人列表失败了 错误码：%d", errorcode);
        m_strTip = XStringToUTF8(std::string(sTips));
#endif
        
		YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_CONTACTTIP);
	}
	else
	{
		for (std::list<PYMTestContactInfo>::iterator iter = m_pIMContactInfoList.begin(); iter != m_pIMContactInfoList.end(); iter++)
		{
			delete *iter;
			(*iter) = NULL;
		}
		m_pIMContactInfoList.clear();
		for (std::list<IYouMeIMContactInfo*>::const_iterator iter = pValue->begin(); iter != pValue->end(); iter++)
		{
			PYMTestContactInfo pInfo = new YMTestContactInfo();
			pInfo->_youmeID = XStringToUTF8((*iter)->GeYouMeID());
			pInfo->_status = (*iter)->GetStatus();
			m_pIMContactInfoList.push_back(pInfo);
		}

		YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_CONTACTLIST);
	}
}

//获取黑名单列表
void ContactList::OnGetBlacklistList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue)
{

}

void ContactList::OnAddContactRequest(YouMeIMErrorcode errorcode, const XString& strUserID)
{
#ifdef WIN32
	wchar_t sTips[256] = { 0 };
	swprintf(sTips, L"添加好友的请求回掉 :%d  %s", errorcode, strUserID.c_str());
	m_strTip = XStringToUTF8(std::wstring(sTips));
#else
    char sTips[256] = { 0 };
    sprintf(sTips, "添加好友的请求回掉 :%d  %s", errorcode, strUserID.c_str());
    m_strTip = XStringToUTF8(std::string(sTips));
#endif
	YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_CONTACTTIP);
}

void ContactList::OnAddContactResponse(YouMeIMContactEvent evtType, IYouMeIMContactInfo* pContactInfo)
{
	if (evtType == AddContactEvent_Agree)
	{
#ifdef WIN32
		wchar_t sTips[256] = { 0 };
		swprintf(sTips, L"添加好友成功:%s", pContactInfo->GeYouMeID().c_str());
		m_strTip = XStringToUTF8(std::wstring(sTips));
#else
        char sTips[256] = { 0 };
        sprintf(sTips, "添加好友成功:%s", pContactInfo->GeYouMeID().c_str());
        m_strTip = XStringToUTF8(std::string(sTips));
#endif

		MyIMManager::GetYouMeIMEngine()->GetContactManager()->GetContactList();
	}
	else
	{
#ifdef WIN32
		m_strTip = XStringToUTF8(std::wstring(L"好友请求拒绝"));
#else
		m_strTip = XStringToUTF8(std::string("好友请求拒绝"));
#endif
	}

	YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_CONTACTTIP);
}

void ContactList::OnReceiveContactInvited(IYouMeIMContactInfo*strUserID, const XString& strReason)
{
	m_strInviteContactID = XStringToUTF8(strUserID->GeYouMeID());
	m_strInviteContactReason = XStringToUTF8(strReason);
	YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_RECVCONTACTINVITE);
}

void ContactList::OnInviteReturnNotify(const std::string& strID, bool bAgree)
{
	if (bAgree)
	{
		//同意
		MyIMManager::GetYouMeIMEngine()->GetContactManager()->AgreeContactInvited(UTF8TOXString(strID));
	}
	else
	{
		//拒绝
		MyIMManager::GetYouMeIMEngine()->GetContactManager()->RefuseContactInvited(UTF8TOXString(strID), __XT("没有理由"));
	}
}

void ContactList::OnContactInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMContactEvent evtType, const XString&strUserID)
{
	if (errorcode == YouMeIMErrorcode_Success)
	{
		if (evtType == AddContactEvent_Agree)
		{
#ifdef WIN32
            wchar_t sTips[256] = { 0 };
			swprintf(sTips, L"成功处理添加好友请求:%s", strUserID.c_str());
            m_strTip = XStringToUTF8(std::wstring(sTips));
#else
            char sTips[256] = { 0 };
            sprintf(sTips, "成功处理添加好友请求:%s", strUserID.c_str());
            m_strTip = XStringToUTF8(std::string(sTips));
#endif

			MyIMManager::GetYouMeIMEngine()->GetContactManager()->GetContactList();
		}
		else
		{
#ifdef WIN32
            wchar_t sTips[256] = { 0 };
			swprintf(sTips, L"成功拒绝添加好友请求:%s", strUserID.c_str());
            m_strTip = XStringToUTF8(std::wstring(sTips));
#else
            char sTips[256] = { 0 };
            sprintf(sTips, "成功拒绝添加好友请求:%s", strUserID.c_str());
            m_strTip = XStringToUTF8(std::string(sTips));
#endif
		}
	}
	else
	{
#ifdef WIN32
        wchar_t sTips[256] = { 0 };
		swprintf(sTips, L"处理好友请求失败了：%d", errorcode);
        m_strTip = XStringToUTF8(std::wstring(sTips));
#else
        char sTips[256] = { 0 };
        sprintf(sTips, "处理好友请求失败了：%d", errorcode);
        m_strTip = XStringToUTF8(std::string(sTips));
#endif
	}

	
	YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_CONTACTTIP);
}

void ContactList::OnContactBeDeleted(const XString& strUserID)
{
#ifdef WIN32
	wchar_t sTips[64] = { 0 };
	swprintf(sTips, L"不好意思，被好友:%s 删除了", strUserID.c_str());
	m_strTip = XStringToUTF8(std::wstring(sTips));
#else
    char sTips[64] = { 0 };
    sprintf(sTips, "不好意思，被好友:%s 删除了", strUserID.c_str());
    m_strTip = XStringToUTF8(std::string(sTips));
#endif
    
	YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_CONTACTTIP);

	MyIMManager::GetYouMeIMEngine()->GetContactManager()->GetContactList();
}

void ContactList::OnContactAddedBlacklist(const XString& strUserID)
{

}

void ContactList::OnKickoutedBlacklist(const XString& strUserID)
{

}

void ContactList::OnDeleteContactResponse(YouMeIMErrorcode errorcode, const XString& strUserID)
{
	if (errorcode == YouMeIMErrorcode_Success)
	{
#ifdef WIN32
        wchar_t sTips[256] = { 0 };
		swprintf(sTips, L"删除好友成功:%s", strUserID.c_str());
        m_strTip = XStringToUTF8(std::wstring(sTips));
#else
        char sTips[256] = { 0 };
        sprintf(sTips, "删除好友成功:%s", strUserID.c_str());
        m_strTip = XStringToUTF8(std::string(sTips));
#endif
	}
	else
	{
#ifdef WIN32
        wchar_t sTips[256] = { 0 };
		swprintf(sTips, L"删除好友失败了：%d :%s", errorcode, strUserID.c_str());
        m_strTip = XStringToUTF8(std::wstring(sTips));
#else
        char sTips[256] = { 0 };
        sprintf(sTips, "删除好友失败了：%d :%s", errorcode, strUserID.c_str());
        m_strTip = XStringToUTF8(std::string(sTips));
#endif
	}

	MyIMManager::GetYouMeIMEngine()->GetContactManager()->GetContactList();

	YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_CONTACTTIP);
}

void ContactList::OnAddBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID)
{

}

void ContactList::OnKickoutBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID)
{

}

void ContactList::OnContactOnline(const XString& strYouMeID)
{
	for (std::list<PYMTestContactInfo>::iterator iter = m_pIMContactInfoList.begin(); iter != m_pIMContactInfoList.end(); iter++)
	{
		PYMTestContactInfo pInfo = *iter;
		if (XStringToUTF8(strYouMeID) == pInfo->_youmeID)
		{
			pInfo->_status = 0;
			break;
		}
	}
}

void ContactList::OnContactOffline(const XString& strYouMeID)
{
	for (std::list<PYMTestContactInfo>::iterator iter = m_pIMContactInfoList.begin(); iter != m_pIMContactInfoList.end(); iter++)
	{
		PYMTestContactInfo pInfo = *iter;
		if (XStringToUTF8(strYouMeID) == pInfo->_youmeID)
		{
			pInfo->_status = 1;
			break;
		}
	}
}

void ContactList::update(float dt)
{
	{
		Vector<Widget*>& items = m_contactListView->getItems();
		ssize_t items_count = items.size();
		for (int i = 0; i < items_count; ++i)
		{
			cocos2d::ui::Layout* item = static_cast<cocos2d::ui::Layout*>(m_contactListView->getItem(i));
			cocos2d::ui::Text *text = static_cast<Text*>(item->getChildByName("Title Text"));
			std::string strID = text->getString();

			Button* button = static_cast<Button*>(item->getChildByName("Title Msg"));
			if (MyIMMessageContainer::Instance().HaveMessage(strID))
			{
				button->loadTextures("red.png", "red.png");
			}
			else
			{
				button->loadTextures("green.png", "green.png");
			}

			for (std::list<PYMTestContactInfo>::iterator iter = m_pIMContactInfoList.begin(); iter != m_pIMContactInfoList.end(); iter++)
			{
				PYMTestContactInfo pInfo = *iter;

				if (strID == pInfo->_youmeID)
				{
					if (0 == pInfo->_status) //在线
					{
						text->setTextColor(cocos2d::Color4B(0, 128, 0, 255));
					}
					else if (1 == pInfo->_status) //离线
					{
						text->setTextColor(cocos2d::Color4B(0, 0, 0, 255));
					}
					break;
				}
			}
		}
	}
}

void ContactList::selectedItemEvent(Ref* pSender, cocos2d::ui::ListView::EventType type)
{
	if (cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END == type)
	{
		cocos2d::ui::ListView* listView = static_cast<cocos2d::ui::ListView*>(pSender);
		ssize_t selIndex = listView->getCurSelectedIndex();

		Vector<Widget*>& items = listView->getItems();
		ssize_t items_count = items.size();
		for (int i = 0; i < items_count; ++i)
		{
			cocos2d::ui::Layout* item = static_cast<cocos2d::ui::Layout*>(listView->getItem(i));

			if (i == selIndex)
			{
				item->setBackGroundImage("selected.png");
			}
			else
			{
				item->setBackGroundImage("unselected.png");
			}
		}
	}
}

void ContactList::selectedItemEventScrollView(Ref* pSender, cocos2d::ui::ScrollView::EventType type)
{
//    enum class EventType
//    {
//        SCROLL_TO_TOP,
//        SCROLL_TO_BOTTOM,
//        SCROLL_TO_LEFT,
//        SCROLL_TO_RIGHT,
//        SCROLLING,
//        BOUNCE_TOP,
//        BOUNCE_BOTTOM,
//        BOUNCE_LEFT,
//        BOUNCE_RIGHT,
//        CONTAINER_MOVED
//    };
    switch (type)
    {
            case cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP:
            CCLOG("SCROLL_TO_TOP");
            break;
            case cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
            CCLOG("SCROLL_TO_BOTTOM");
            break;
            case cocos2d::ui::ScrollView::EventType::SCROLL_TO_LEFT:
            CCLOG("SCROLL_TO_LEFT");
            break;
            case cocos2d::ui::ScrollView::EventType::SCROLL_TO_RIGHT:
            CCLOG("SCROLL_TO_RIGHT");
            break;
            case cocos2d::ui::ScrollView::EventType::SCROLLING:
            CCLOG("SCROLLING");
            break;
            case cocos2d::ui::ScrollView::EventType::BOUNCE_TOP:
            CCLOG("BOUNCE_TOP");
            break;
            case cocos2d::ui::ScrollView::EventType::BOUNCE_BOTTOM:
            CCLOG("BOUNCE_BOTTOM");
            break;
            case cocos2d::ui::ScrollView::EventType::BOUNCE_LEFT:
            CCLOG("BOUNCE_LEFT");
            break;
            case cocos2d::ui::ScrollView::EventType::BOUNCE_RIGHT:
            CCLOG("BOUNCE_RIGHT");
            break;
            case cocos2d::ui::ScrollView::EventType::CONTAINER_MOVED:
            CCLOG("CONTAINER_MOVED");
            break;
            default:
            break;
    }
}

void ContactList::OnMessageNotify(YouMeMSGTYPE type)
{
	switch (type)
	{
	case YOUME_MSG_CONTACTTIP:
	{
		PopTip(m_strTip, 1);
		break;
	}
	case YOUME_MSG_LOGOUTSUCCESS:
	{
		MyIMManager::GetYouMeIMEngine()->SetContactCallback(NULL);
		removeFromParentAndCleanup(true);
		break;
	}
	case YOUME_MSG_CONTACTLIST:
	{
		m_contactListView->removeAllItems();
		for (std::list<PYMTestContactInfo>::const_iterator iter = m_pIMContactInfoList.begin(); iter != m_pIMContactInfoList.end(); iter++)
		{
			PYMTestContactInfo pInfo = *iter;

			cocos2d::ui::Layout *widget = cocos2d::ui::Layout::create();
			widget->setContentSize(cocos2d::Size(m_contactListView->getContentSize().width, 50));
			widget->setTouchEnabled(true);

			cocos2d::ui::Text *text = cocos2d::ui::Text::create();
			text->setName("Title Text");
			text->setAnchorPoint(cocos2d::Point(0, 0));
			text->setString(pInfo->_youmeID);
			if (0 == pInfo->_status) //在线
			{
				text->setTextColor(cocos2d::Color4B(0, 128, 0, 255));
			}
			else if (1 == pInfo->_status) //离线
			{
				text->setTextColor(cocos2d::Color4B(0, 0, 0, 255));
			}

			text->setColor(cocos2d::Color3B(144, 238, 144));
			text->setAnchorPoint(Vec2(0, 0.5f));
			text->setPosition(Vec2(0, widget->getContentSize().height / 2));

			widget->setBackGroundImage("unselected.png");
			widget->setBackGroundImageScale9Enabled(true);
			widget->addChild(text);

			cocos2d::ui::Button *buttonChat = cocos2d::ui::Button::create("beginChat.png");
			buttonChat->setName("Title Chat");
			buttonChat->setTouchEnabled(true);
			buttonChat->setAnchorPoint(Vec2(0, 0.5));
			buttonChat->setPosition(Vec2(460, widget->getContentSize().height / 2));
			buttonChat->addClickEventListener(CC_CALLBACK_1(ContactList::OnBeginChatCallEvent, this));

			widget->addChild(buttonChat);

			cocos2d::ui::Button *buttonMsg = cocos2d::ui::Button::create("green.png");
			buttonMsg->setName("Title Msg");
			buttonMsg->setTouchEnabled(true);
			buttonMsg->setAnchorPoint(Vec2(0, 0.5));
			buttonMsg->setPosition(Vec2(390, widget->getContentSize().height / 2));
			widget->addChild(buttonMsg);

			m_contactListView->pushBackCustomItem(widget);
		}

		break;
	}
	case YOUME_MSG_RECVCONTACTINVITE:
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto layer = ReceiveContactInviteNotifier::create();
		layer->InitPara(m_strInviteContactID, m_strInviteContactReason);
		layer->SetCallBack(this);
		layer->ignoreAnchorPointForPosition(false);
		layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		layer->setContentSize(Size(400, 250));
		layer->setPosition(Vec2((visibleSize.width) / 2, (visibleSize.height) / 2));
		Scene* pCurSene = Director::getInstance()->getRunningScene();
		pCurSene->addChild(layer);

		break;
	}
	default:
		break;
	}
}
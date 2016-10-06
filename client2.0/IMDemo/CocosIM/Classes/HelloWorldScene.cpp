#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ContactListScene.h"

#include "utils.h"
#include "YouMeMessage.h"
#include "MyIMManager.h"
#include "YouMeMsgBox.h"
#include <wchar.h>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	IYouMeIMManager::Startup();
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	auto rootNode = CSLoader::createNode("MainScene.csb");
	addChild(rootNode);

	auto layerNode = rootNode->getChildByName("MainNode");

	cocos2d::ui::Button* btnInit = dynamic_cast<cocos2d::ui::Button*>(layerNode->getChildByName("Button_Init"));
	btnInit->addClickEventListener(CC_CALLBACK_1(HelloWorld::OnInitCallEvent, this));

	cocos2d::ui::Button* btnUnInit = dynamic_cast<cocos2d::ui::Button*>(layerNode->getChildByName("Button_UnInit"));
	btnUnInit->addClickEventListener(CC_CALLBACK_1(HelloWorld::OnUnInitCallEvent, this));

	cocos2d::ui::Button* btnLogin = dynamic_cast<cocos2d::ui::Button*>(layerNode->getChildByName("Button_login"));
	btnLogin->addClickEventListener(CC_CALLBACK_1(HelloWorld::OnLoginCallEvent, this));
    
    cocos2d::ui::Button* btnExit = dynamic_cast<cocos2d::ui::Button*>(layerNode->getChildByName("Button_Exit"));
    btnExit->addClickEventListener(CC_CALLBACK_1(HelloWorld::OnExitCallEvent, this));

    m_textID = CursorTextField::textFieldWithPlaceHolder(this, "click and input", "fonts/Marker Felt.ttf", 22);
    m_textID->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_textID->setInputWidth(200);
    m_textID->setPosition(Vec2(220, 570));
    m_textID->setTextColor(Color4B(26, 26, 26, 255));
    this->addChild(m_textID, 1);
    
    m_textPwd = CursorTextField::textFieldWithPlaceHolder(this, "click and input", "fonts/Marker Felt.ttf", 22);
    m_textPwd->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_textPwd->setInputWidth(200);
    m_textPwd->setPosition(Vec2(220, 518));
    m_textPwd->setTextColor(Color4B(26, 26, 26, 255));
    this->addChild(m_textPwd, 1);
    
	m_textNewPwd = CursorTextField::textFieldWithPlaceHolder(this, "click and input", "fonts/Marker Felt.ttf", 22);
	m_textNewPwd->setAnchorPoint(Vec2(0.0f, 0.0f));
	m_textNewPwd->setInputWidth(200);
	m_textNewPwd->setPosition(Vec2(220, 466));
	m_textNewPwd->setTextColor(Color4B(26, 26, 26, 255));
	this->addChild(m_textNewPwd, 1);

	YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_LOGINTFAIL, this);
	YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_LOGINSUCCESS, this);
	YouMeNotifyCenter::GetInstance().RegisterMsgObserver(YOUME_MSG_LOGOUTSUCCESS, this);

	this->scheduleUpdate();
    return true;
}

HelloWorld::~HelloWorld()
{
	MyIMManager::GetYouMeIMEngine()->UnInit();
	YouMeNotifyCenter::GetInstance().UnRegisterMsgObserver(this);
}

void HelloWorld::OnInitCallEvent(Ref* pSender)
{    
	// TODO:  在此添加控件通知处理程序代码
	YouMeIMErrorcode errorcode = MyIMManager::GetYouMeIMEngine()->Init(__XT("YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0"),
		__XT("r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE="),
		__XT("com.youme.joe.test"));
	MyIMManager::GetYouMeIMEngine()->SetLoginCallback(this);

	if (errorcode != YouMeIMErrorcode_Success)
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"初始化失败:%d", errorcode);
		PopTip(XStringToUTF8(std::wstring(sTips)), 1);
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "初始化失败:%d", errorcode);
        PopTip(XStringToUTF8(std::string(sTips)), 1);
#endif
	}
	else
	{
#ifdef WIN32
		PopTip(XStringToUTF8(std::wstring(L"已经成功初始化")), 1);
#else
        PopTip(XStringToUTF8(std::string("已经成功初始化")), 1);
#endif
	}
}

void HelloWorld::OnUnInitCallEvent(Ref* pSender)
{
	// TODO:  在此添加控件通知处理程序代码
	MyIMManager::GetYouMeIMEngine()->UnInit();
#ifdef WIN32
	PopTip(XStringToUTF8(std::wstring(L"已经反初始化")), 1);
#else
    PopTip(XStringToUTF8(std::string("已经反初始化")), 1);
#endif
}

void HelloWorld::OnLoginCallEvent(Ref* pSender)
{
	// TODO:  在此添加控件通知处理程序代码	
	std::string strID = m_textID->getInputText();
    std::string strPwd = m_textPwd->getInputText();
	std::string strNewPwd = m_textNewPwd->getInputText();

	if (strID == "请输入ID" || strPwd == "请输入密码" || strID.size() == 0 || strPwd.size() == 0)
		return;

	MyIMManager::GetYouMeIMEngine()->SetLoginCallback(this);

	YouMeIMErrorcode errorcode;
	if (strNewPwd.size() == 0)
	{
		errorcode = MyIMManager::GetYouMeIMEngine()->Login(UTF8TOXString(strID), UTF8TOXString(strPwd), __XT(""));
	}
	else
	{
		errorcode = MyIMManager::GetYouMeIMEngine()->Login(UTF8TOXString(strID), UTF8TOXString(strNewPwd), UTF8TOXString(strPwd));
	}

	if (errorcode != YouMeIMErrorcode_Success)
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"登陆失败:%d", errorcode);
		PopTip(XStringToUTF8(std::wstring(sTips)), 1);
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "登陆失败:%d", errorcode);
        PopTip(XStringToUTF8(std::string(sTips)), 1);
#endif
	}
	else
	{
#ifdef WIN32
		PopTip(XStringToUTF8(std::wstring(L"正在登陆......")), 2);
#else
        PopTip(XStringToUTF8(std::string("正在登陆......")), 2);
#endif
	}
}

void HelloWorld::OnExitCallEvent(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::OnLogin(YouMeIMErrorcode errorcode, IYouMeIMContactInfo* pUserInfo)
{
	if ((errorcode == YouMeIMErrorcode_Success) && (NULL != pUserInfo))
	{
		MyIMManager::GetYouMeIMEngine()->SetMessageCallback(&MyIMMessageContainer::Instance());
        MyIMManager::GetYouMeIMEngine()->SetDownloadCallback(&MyIMMessageContainer::Instance());
		MyIMManager::Instance().SetLoginUser(XStringToUTF8(pUserInfo->GeYouMeID()));

		MyIMManager::GetYouMeIMEngine()->GetMessageManager()->GetContactOfflineMessage();

		YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_LOGINSUCCESS);
	}
	else
	{
#ifdef WIN32
		wchar_t sTips[64] = { 0 };
		swprintf(sTips, L"登陆失败：%d", errorcode);
		m_strTip = XStringToUTF8(std::wstring(sTips));
#else
        char sTips[64] = { 0 };
        sprintf(sTips, "登陆失败：%d", errorcode);
        m_strTip = XStringToUTF8(std::string(sTips));
#endif

		YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_LOGINTFAIL);
	}
}

void HelloWorld::OnLogout(YouMeIMErrorcode errorcode)
{
#ifdef WIN32
	wchar_t sTips[64] = { 0 };
	swprintf(sTips, L"登出：%d", errorcode);
	m_strTip = XStringToUTF8(std::wstring(sTips));
#else
    char sTips[64] = { 0 };
    sprintf(sTips, "登出：%d", errorcode);
    m_strTip = XStringToUTF8(std::string(sTips));
#endif

	YouMeNotifyCenter::GetInstance().PostMsg(YOUME_MSG_LOGOUTSUCCESS);
    
    MyIMManager::GetYouMeIMEngine()->SetMessageCallback(NULL);
}

void HelloWorld::update(float dt)
{
	YouMeNotifyCenter::GetInstance().DispatchMsg();
}

void HelloWorld::OnMessageNotify(YouMeMSGTYPE type)
{
	switch (type)
	{
	case YOUME_MSG_LOGINTFAIL:
	{
		PopTip(m_strTip, 1);
		break;
	}
	case YOUME_MSG_LOGINSUCCESS:
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto layer = ContactList::create();
		layer->ignoreAnchorPointForPosition(false);
		layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		layer->setContentSize(visibleSize);
		layer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		Scene* pCurSene = Director::getInstance()->getRunningScene();
		pCurSene->addChild(layer);
		break;
	}
	case YOUME_MSG_LOGOUTSUCCESS:
	{
		PopTip(m_strTip, 1);
		break;
	}
	default:
		break;
	}
}
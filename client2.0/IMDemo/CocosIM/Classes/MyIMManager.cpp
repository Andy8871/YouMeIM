#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "MyIMManager.h"
#include <platform/CCFileUtils.h>
#include <SimpleAudioEngine.h>
MyIMManager::MyIMManager()
{
    myInstance = IYouMeIMManager::CreateInstance();
}

MyIMManager::~MyIMManager()
{
	IYouMeIMManager::DestroyInstance(myInstance);
	IYouMeIMManager::CleanUp();
}

MyIMManager& MyIMManager::Instance(){
	
	static MyIMManager _instance;
	return _instance;
}

IYouMeIMManager* MyIMManager::GetYouMeIMEngine()
{
	return Instance().myInstance;
}

void MyIMManager::SetLoginUser(const std::string& strUserID)
{
	m_loginUserID = strUserID;
}
void MyIMMessageContainer::OnDownload(XINT64 ulSerial, YouMeIMErrorcode errorcode,const XString& strSavePath)
{
    if (errorcode == YouMeIMErrorcode_Success) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(strSavePath.c_str(),false);
    }
    
}
std::string MyIMManager::GetLoginUser()
{
	return m_loginUserID;
}

/////////////////////////////////////////////////////////////////////////////////
MyIMMessageContainer::MyIMMessageContainer()
{

}

MyIMMessageContainer& MyIMMessageContainer::Instance()
{
	static MyIMMessageContainer _Instance;
	return _Instance;
}
void MyIMMessageContainer::OnSendAudioMessageStatus(XINT64 iRequestID,YouMeIMErrorcode errorcode,const XString&strText,const XString& strAudioPath)
{
    
}
MyIMMessageContainer::~MyIMMessageContainer()
{

}

void MyIMMessageContainer::OnSendMessageStatus(XINT64 iRequestID,  YouMeIMErrorcode errorcode)
{

}

void MyIMMessageContainer::OnRecvMessage(IYouMeIMMessage* pMessage)
{
	PMyMessageList msgList = NULL;
	std::string strID = XStringToUTF8(pMessage->GetSenderID());
	if (m_msgHistorys.find(strID) == m_msgHistorys.end())
	{
		msgList = new MyMessageList();
		m_msgHistorys[strID] = msgList;
	}
	else
	{
		msgList = m_msgHistorys[strID];
	}
    
    
    IYouMeIMMessageBodyBase* pbodyBase = pMessage->GetMessageBody();
    
    const int nBufferSize = 1024;
    XCHAR buffer[nBufferSize + 1] = { 0 };
#ifdef WIN32
    int dSize = _snwprintf(buffer, nBufferSize, __XT("线程id:%d MessageType:%d UserID: %s"),
                           GetCurrentThreadId(),
                           pbodyBase->GetMessageType(),
                           pMessage->GetSenderID().c_str());
#else
    int dSize = snprintf(buffer, nBufferSize, __XT("线程id：%lu MessageType:%d UserID: %s"),
                         (unsigned long)pthread_self(),
                         pbodyBase->GetMessageType(),
                         pMessage->GetSenderID().c_str());
#endif // WIN32
    
    switch (pbodyBase->GetMessageType())
	{
    case MessageBodyType_Voice:
    {
        IYouMeIMMessageBodyAudio* pAudio = (IYouMeIMMessageBodyAudio*)pbodyBase;
        PMyMessageBase msgbody = new MyMessageBase();
        msgbody->strBody = XStringToUTF8(pAudio->GetText());
        msgbody->ymType = MessageBodyType_Voice;
        msgbody->strSendID = strID;
        msgList->push_back(msgbody);
        
    
        std::string strPath = cocos2d::FileUtils::sharedFileUtils()->getWritablePath()+"test.wav";
        MyIMManager::Instance().GetYouMeIMEngine()->GetMessageManager()->DownloadAudio(pMessage->GetMessageID(),strPath);
        
        if (NULL != m_pResponseCallback)
        {
            m_pResponseCallback->OnMessageResponse(strID);
        }
        
        if (dSize < nBufferSize)
        {
#ifdef WIN32
            _snwprintf(buffer + dSize, nBufferSize - dSize, __XT("MessageBody: %s\n"), pAudio->GetMessageContent().c_str());
#else
            snprintf(buffer + dSize, nBufferSize - dSize, __XT("MessageBody: %s\n"),  pAudio->GetText().c_str());
#endif
            
        }
    }
            break;
	case MessageBodyType_TXT:
		{
			IYouMeIMMessageBodyText* pContent = (IYouMeIMMessageBodyText*)pbodyBase;
			PMyMessageBase msgbody = new MyMessageBase();
			msgbody->strBody = XStringToUTF8(pContent->GetMessageContent());
			msgbody->ymType = MessageBodyType_TXT;
			msgbody->strSendID = strID;
			msgList->push_back(msgbody);

			if (NULL != m_pResponseCallback)
			{
				m_pResponseCallback->OnMessageResponse(strID);
			}
            
            if (dSize < nBufferSize)
            {
#ifdef WIN32
                _snwprintf(buffer + dSize, nBufferSize - dSize, __XT("MessageBody: %s\n"), pContent->GetMessageContent().c_str());
#else
                snprintf(buffer + dSize, nBufferSize - dSize, __XT("MessageBody: %s\n"),  pContent->GetMessageContent().c_str());
#endif
                
            }
		}
		break;
	case MessageBodyType_CustomMesssage:
		{

		}
		break;
	default:
		break;
	}
    
#ifdef WIN32
    OutputDebugStringW(buffer);
#else
    printf("%s",buffer);
#endif
}

void MyIMMessageContainer::SetResponseCallback(IMyIMMessageResponseCallback* pCallback)
{
	m_pResponseCallback = pCallback;
}

void MyIMMessageContainer::GetMessageList(const std::string& strID, MyMessageList& mymsglist)
{
	if (m_msgHistorys.find(strID) == m_msgHistorys.end())
	{
		return;
	}
	PMyMessageList msgList = m_msgHistorys[strID];

	for (std::list<PMyMessageBase>::iterator iter = msgList->begin(); iter != msgList->end(); iter++)
	{
		PMyMessageBase pmsgBean = *iter;
		PMyMessageBase pmymsg = new MyMessageBase(*pmsgBean);
		mymsglist.push_back(pmymsg);
	}

	for (std::list<PMyMessageBase>::iterator iter = msgList->begin(); iter != msgList->end(); iter++)
	{
		PMyMessageBase pmsgBean = *iter;
		delete pmsgBean;
		pmsgBean = NULL;
	}

	msgList->clear();
}

bool MyIMMessageContainer::HaveMessage(const std::string& strID)
{
	if (m_msgHistorys.find(strID) == m_msgHistorys.end())
		return false;

	PMyMessageList msgList = m_msgHistorys[strID];
	if (msgList->size() == 0)
		return false;

	return true;
}
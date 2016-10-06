#ifndef __VOLUMESELECT_SCENE_H__
#define __VOLUMESELECT_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include "IIMNotifier.h"
#include "YouMeNotifyCenter.h"

typedef struct YMTestContactInfo
{
	std::string _youmeID;
	int _status;
} YMTestContactInfo, *PYMTestContactInfo;

class ContactList : public cocos2d::LayerColor,
	public IYouMeIMContactCallback,
	public IIMAddContactNotifyCallback,
	public IIMReceiveInviteNotifyCallback,
	public IYouMeNotifyObserver
{
public:
	ContactList();
	~ContactList();
    
    virtual bool init();
	CREATE_FUNC(ContactList);

protected:
    void initDialog();

	void OnLogoutCallEvent(Ref* pSender);

	void OnContactListCallEvent(Ref* pSender);

	void OnAddContactCallEvent(Ref* pSender);

	void OnDeleteContactCallEvent(Ref* pSender);

	void OnBeginChatCallEvent(Ref* pSender);
protected:

	void selectedItemEvent(Ref* pSender, cocos2d::ui::ListView::EventType type);
    
    void selectedItemEventScrollView(Ref* pSender, cocos2d::ui::ScrollView::EventType type);

	void OnAddReturnNotify(const std::string& strID, const std::string& strReason) override;

	void OnInviteReturnNotify(const std::string& strID, bool bAgree) override;

	void OnMessageNotify(YouMeMSGTYPE type) override;
protected:
	void update(float dt);

private:
	std::string m_strTip;

public:

	virtual void OnGetContactInfo(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override;

	virtual void OnGetContactList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override;

	virtual void OnGetBlacklistList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override;

	virtual void OnAddContactRequest(YouMeIMErrorcode errorcode, const XString& strUserID) override;

	virtual void OnAddContactResponse(YouMeIMContactEvent evtType, IYouMeIMContactInfo* pContactInfo) override;

	virtual void OnReceiveContactInvited(IYouMeIMContactInfo*strUserID, const XString& strReason) override;

	virtual void OnContactInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMContactEvent evtType, const XString&strUserID) override;

	virtual void OnContactBeDeleted(const XString& strUserID) override;

	virtual void OnContactAddedBlacklist(const XString& strUserID) override;

	virtual void OnKickoutedBlacklist(const XString& strUserID) override;

	virtual void OnDeleteContactResponse(YouMeIMErrorcode errorcode, const XString& strUserID) override;

	virtual void OnAddBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID) override;

	virtual void OnKickoutBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strUserID) override;

	virtual void OnContactOnline(const XString& strYouMeID);

	virtual void OnContactOffline(const XString& strYouMeID);
private:

	cocos2d::ui::ListView* m_contactListView;

	cocos2d::ui::Text* m_loginIDText;

	std::list<PYMTestContactInfo> m_pIMContactInfoList;

	std::string m_strInviteContactID;
	std::string m_strInviteContactReason;
};

#endif /* VolumeDialogLayer_hpp */

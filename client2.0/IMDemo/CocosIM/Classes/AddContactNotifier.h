#pragma  once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>
#include "IIMNotifier.h"
#include "CursorTextField.h"
#include "YoumeNotifier.h"

class AddContactNotifier : public YoumeNotifier
{
public:
	AddContactNotifier();
    
    virtual bool init();
	CREATE_FUNC(AddContactNotifier);
    
	void SetCallBack(IIMAddContactNotifyCallback* pCallBack);
protected:
    void initDialog();

	void OnOKCallEvent(Ref* pSender);

	void OnCancleCallEvent(Ref* pSender);

private:

    CursorTextField* m_textContactID;
    CursorTextField* m_textContactReason;

	IIMAddContactNotifyCallback* m_pCallBack = NULL;
};

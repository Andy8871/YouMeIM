#pragma  once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>

#include "YouMeNotifyCenter.h"
#include "YoumeNotifier.h"

class YouMeMsgBox : public YoumeNotifier,
	public IYouMeNotifyObserver
{
public:
	YouMeMsgBox();
	~YouMeMsgBox();
    
    virtual bool init();
	CREATE_FUNC(YouMeMsgBox);

public:
	void InitPara(const std::string& strTips, int nType);

protected:
	void OnMessageNotify(YouMeMSGTYPE type) override;

protected:
    void initDialog();

	void OnOKCallEvent(Ref* pSender);

	void Collapse(float dt);
private:
	cocos2d::ui::Text* m_TextTips;

	int m_nType = 0;
};

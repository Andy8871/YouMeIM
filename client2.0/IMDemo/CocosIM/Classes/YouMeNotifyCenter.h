#pragma once

#include <list>
#include <map>

using std::list;
using std::map;

typedef unsigned int  YouMeMSGTYPE;

class IYouMeNotifyObserver
{
public:
	virtual void OnMessageNotify(YouMeMSGTYPE type) = 0;
};

typedef std::list<IYouMeNotifyObserver*> YouMeObserverList;

class YouMeNotifyCenter
{
	YouMeNotifyCenter();
public:
	static YouMeNotifyCenter& GetInstance();
	~YouMeNotifyCenter();

	void RegisterMsgObserver(YouMeMSGTYPE type, IYouMeNotifyObserver* observer);
	void UnRegisterMsgObserver(YouMeMSGTYPE type, IYouMeNotifyObserver* observer);
	void UnRegisterMsgObserver(IYouMeNotifyObserver* observer);

	void PostMsg(YouMeMSGTYPE type);

	void DispatchMsg();
private:
	std::list<YouMeMSGTYPE>	 m_msgQueue;

	std::map<YouMeMSGTYPE, YouMeObserverList> m_msgObservers;
};
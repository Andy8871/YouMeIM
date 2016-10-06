#include "YouMeNotifyCenter.h"

YouMeNotifyCenter::YouMeNotifyCenter()
{

}

YouMeNotifyCenter& YouMeNotifyCenter::GetInstance()
{
	static YouMeNotifyCenter _Instance;
	return _Instance;
}

YouMeNotifyCenter::~YouMeNotifyCenter()
{

}

void YouMeNotifyCenter::RegisterMsgObserver(YouMeMSGTYPE type, IYouMeNotifyObserver* observer)
{
	YouMeObserverList& observerList = m_msgObservers[type];
	for (YouMeObserverList::iterator iter = observerList.begin(); iter != observerList.end(); iter++)
	{
		IYouMeNotifyObserver* pYmObserver = *iter;
		if (pYmObserver == observer)
			return;
	}

	observerList.push_back(observer);
}

void YouMeNotifyCenter::UnRegisterMsgObserver(YouMeMSGTYPE type, IYouMeNotifyObserver* observer)
{
	if (m_msgObservers.find(type) == m_msgObservers.end())
	{
		return;
	}

	YouMeObserverList& observerList = m_msgObservers[type];
	for (YouMeObserverList::iterator iter = observerList.begin(); iter != observerList.end(); iter++)
	{
		IYouMeNotifyObserver* pYmObserver = *iter;
		if (pYmObserver == observer)
		{
			observerList.erase(iter);
			return;
		}
	}
}

void YouMeNotifyCenter::UnRegisterMsgObserver(IYouMeNotifyObserver* observer)
{
	for (std::map<YouMeMSGTYPE, YouMeObserverList>::iterator iter = m_msgObservers.begin(); iter != m_msgObservers.end(); iter++)
	{
		YouMeObserverList& observerList = iter->second;
		for (YouMeObserverList::iterator iter = observerList.begin(); iter != observerList.end(); iter++)
		{
			IYouMeNotifyObserver* pYmObserver = *iter;
			if (pYmObserver == observer)
			{
				observerList.erase(iter);
				break;
			}
		}
	}
}

void YouMeNotifyCenter::PostMsg(YouMeMSGTYPE type)
{
	m_msgQueue.push_back(type);
}

void YouMeNotifyCenter::DispatchMsg()
{
	if (m_msgQueue.size() == 0)
		return;

	YouMeMSGTYPE type = m_msgQueue.front();
	m_msgQueue.pop_front();

	YouMeObserverList observerList = m_msgObservers[type];
	for (YouMeObserverList::iterator iter = observerList.begin(); iter != observerList.end(); iter++)
	{
		IYouMeNotifyObserver* pYmObserver = *iter;
		pYmObserver->OnMessageNotify(type);
	}
}
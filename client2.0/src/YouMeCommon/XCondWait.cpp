#include "XCondWait.h"
#include <memory>
#include <assert.h>
namespace youmecommon {

CXCondWait::CXCondWait()
{
    m_bIsAlreadySetSignal = false;
}

CXCondWait::~CXCondWait()
{
	
}

int CXCondWait::Wait()
{
    std::unique_lock <std::mutex> lck(m_mutex);
    if (m_bIsAlreadySetSignal) {
        m_bIsAlreadySetSignal = false;
        return WaitResult_NoTimeout;
    }
    m_condivar.wait(lck);
    m_bIsAlreadySetSignal = false;
    return WaitResult_NoTimeout;
}
void CXCondWait::Reset()
{
    std::unique_lock <std::mutex> lck(m_mutex);
    m_bIsAlreadySetSignal = false;
}
WaitResult CXCondWait::WaitTime(uint64_t msTime)
{
    std::unique_lock <std::mutex> lck(m_mutex);
    if (m_bIsAlreadySetSignal) {
        m_bIsAlreadySetSignal = false;
        return WaitResult_NoTimeout;
    }
    std::cv_status status = m_condivar.wait_for(lck, std::chrono::milliseconds (msTime));
    m_bIsAlreadySetSignal = false;
    if (status == std::cv_status::timeout) {
        return WaitResult_Timeout;
    }
    return WaitResult_NoTimeout;
}

int CXCondWait::SetSignal()
{
    std::unique_lock <std::mutex> lck(m_mutex);
    m_bIsAlreadySetSignal = true;
    m_condivar.notify_one();
    return  0;
}
}

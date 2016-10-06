#pragma once
#include <memory>
#include <condition_variable>
#include <mutex>
namespace youmecommon {

enum WaitResult
{
    WaitResult_Timeout=1,
    WaitResult_NoTimeout=2
};


class CXCondWait
{
public:
	CXCondWait();
	~CXCondWait();
    void Reset();
	int Wait();

	//0 成功等到，TIMED_OUT 超时， 其他错误
	WaitResult WaitTime(uint64_t msTime);
	int SetSignal();
private:
    std::mutex m_mutex;
    std::condition_variable m_condivar;
    bool m_bIsAlreadySetSignal;
};
}
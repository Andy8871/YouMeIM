#pragma once
#include <memory>
//共享内存，简单封装一下数组的操作，可用于stl 的容器
namespace youmecommon {

template<class T>
class CXSharedArray
{
public:
	CXSharedArray()
	{
		m_iBufferSize = 0;
	}
	~CXSharedArray()
	{
		
	}
	//释放数组内存
	static void FreeArray(T* pPtr)
	{
		delete[]pPtr;
	}

	CXSharedArray(int iElementCount)
	{
		Allocate(iElementCount);
	}
    void Attach(T* pBuffer,int iElementCount)
    {
        std::shared_ptr<T> pTmp(pBuffer, FreeArray);
        m_iBufferSize = iElementCount * sizeof(T);
        m_pPtr.swap(pTmp);
    }
	//重新分配
	void Allocate(int iElementCount)
	{
		Reset();
		m_iBufferSize = iElementCount * sizeof(T);
		std::shared_ptr<T> pTmp(new T[iElementCount], FreeArray);
		m_pPtr.swap(pTmp);
	}
    //重新设置大小
    void ReSize(int iElementCount)
    {
        int iNeedSize = iElementCount * sizeof(T);
        if (m_iBufferSize >= iNeedSize) {
            //如果当前缓存比需要的大，那就直接改变长度
            m_iBufferSize = iNeedSize;
            return;
        }
        else
        {
            //重新申请空间，并且内存拷贝一下
            std::shared_ptr<T> pTmp(new T[iElementCount], FreeArray);
            memcpy(pTmp.get(), m_pPtr.get(), m_iBufferSize);
            
            m_pPtr.swap(pTmp);
            m_iBufferSize = iNeedSize;
        }
    }
	T * Get()
	{
		return m_pPtr.get();
	}
	int GetBufferLen()
	{
		return m_iBufferSize;
	}

	void Reset()
	{
		m_pPtr.reset();
		m_iBufferSize = 0;
	}
private:
	int m_iBufferSize;
	std::shared_ptr<T> m_pPtr;
};
}
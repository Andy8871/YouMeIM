//
//  RSAUtil.cpp
//  YouMeVoiceEngine
//
//  Created by user on 15/9/22.
//  Copyright (c) 2015年 tencent. All rights reserved.
//

#include "RSAUtil.h"
#include <algorithm>
#include <assert.h>
#define RSA_KEY_LENGTH 1024
namespace youmecommon
{

CRSAUtil::CRSAUtil () : m_publicKey (NULL), m_privateKey (NULL)
{
}
CRSAUtil::~CRSAUtil ()
{
    if (NULL != m_privateKey)
    {
        RSA_free (m_privateKey);
    }
    if (NULL != m_publicKey)
    {
		::youmecommon::RSA_free(m_publicKey);
    }
}
bool CRSAUtil::SetPublicKey (CXSharedArray<unsigned char> &publicKey, CXSharedArray<unsigned char> &module)
{
    if (NULL != m_publicKey)
    {
		::youmecommon::RSA_free(m_publicKey);
    }
    //重新生成
    m_publicKey = ::youmecommon::RSA_new ();
	m_publicKey->e = ::youmecommon::BN_bin2bn((const unsigned char *)publicKey.Get(), (int)publicKey.GetBufferLen(), m_publicKey->e);
	m_publicKey->n = ::youmecommon::BN_bin2bn((const unsigned char *)module.Get(), (int)module.GetBufferLen(), m_publicKey->n);


    return true;
}

bool CRSAUtil::SetPrivateKey (CXSharedArray<unsigned char> &publicKey, CXSharedArray<unsigned char> &privateKey, CXSharedArray<unsigned char> &module)
{
    if (NULL != m_privateKey)
    {
        RSA_free (m_privateKey);
    }
    //重新生成
    m_privateKey = ::youmecommon::RSA_new ();
    m_privateKey->e = BN_bin2bn ((const unsigned char *)publicKey.Get(), (int)publicKey.GetBufferLen(), m_privateKey->e);
    m_privateKey->n = BN_bin2bn ((const unsigned char *)module.Get(), (int)module.GetBufferLen(), m_privateKey->n);
    m_privateKey->d = BN_bin2bn ((const unsigned char *)privateKey.Get(), (int)privateKey.GetBufferLen(), m_privateKey->d);
    return true;
}

//公钥加密，私钥解密
bool CRSAUtil::EncryptByPublicKey (const unsigned char *pBuffer, int iBufferLen, CXSharedArray<unsigned char> &result)
{
	if (NULL == m_publicKey)
	{
		return false;
	}
    //加密之后的密文长度和密钥相同, 100 字节分为一段
    int iResultLen = ((iBufferLen - 1) / 100 + 1) * RSA_size (m_publicKey);
    result.Allocate(iResultLen);
    //开始分段了
    int iAlreadyEncry = 0; //已经加密的明文长度
    // 已经加密的密文长度
    int iEncryDataLen = 0;
    while (iAlreadyEncry < iBufferLen)
    {
        int iNeedEncry = std::min (100, iBufferLen - iAlreadyEncry);

        int ret = RSA_public_encrypt (iNeedEncry, pBuffer + iAlreadyEncry, (unsigned char *)result.Get() + iEncryDataLen, m_publicKey, RSA_PKCS1_PADDING /*RSA_NO_PADDING*/);

        iEncryDataLen += ret;
        iAlreadyEncry += iNeedEncry;
    }


    assert (iEncryDataLen == iResultLen);
    return iEncryDataLen == iResultLen;
}

bool CRSAUtil::DecryptByPrivateKey (const unsigned char *pBuffer, int iBufferLen, CXSharedArray<unsigned char> &result)
{
	if (NULL == m_privateKey)
	{
		return false;
	}
	int iKeyBytes = ::youmecommon::RSA_size(m_privateKey);
    assert (iBufferLen % iKeyBytes == 0);
    result.Allocate(iBufferLen);    //开始分段了
    int iAlreadyDeEncry = 0; //已经解密的密文长度
    // 已经加密的密文长度
    int iDeEncryDataLen = 0;
    while (iAlreadyDeEncry < iBufferLen)
    {
		int ret = ::youmecommon::RSA_private_decrypt(iKeyBytes, pBuffer + iAlreadyDeEncry, (unsigned char *)result.Get() + iDeEncryDataLen, m_privateKey, RSA_PKCS1_PADDING);
        if (-1 == ret)
        {
            break;
        }
        iDeEncryDataLen += ret;
        iAlreadyDeEncry += iKeyBytes;
    }
    result.ReSize(iDeEncryDataLen);
    return iAlreadyDeEncry == iBufferLen;
}


bool CRSAUtil::EncryptByPrivateKey (const unsigned char *pBuffer, int iBufferLen, CXSharedArray<unsigned char> &result)
{
	if (NULL == m_privateKey)
	{
		return false;
	}
    //加密之后的密文长度和密钥相同, 100 字节分为一段
	int iResultLen = ((iBufferLen - 1) / 100 + 1) * ::youmecommon::RSA_size(m_privateKey);
    result.Allocate(iResultLen);
    //开始分段了
    int iAlreadyEncry = 0; //已经加密的明文长度
    // 已经加密的密文长度
    int iEncryDataLen = 0;
    while (iAlreadyEncry < iBufferLen)
    {
        int iNeedEncry = std::min (100, iBufferLen - iAlreadyEncry);

		int ret = ::youmecommon::RSA_private_encrypt(iNeedEncry, pBuffer + iAlreadyEncry, (unsigned char *)result.Get() + iEncryDataLen, m_privateKey, RSA_PKCS1_PADDING /*RSA_NO_PADDING*/);

        iEncryDataLen += ret;
        iAlreadyEncry += iNeedEncry;
    }


    assert (iEncryDataLen == iResultLen);
    return iEncryDataLen == iResultLen;
}
bool CRSAUtil::DecryptByPublicKey (const unsigned char *pBuffer, int iBufferLen, CXSharedArray<unsigned char> &result)
{
	if (NULL == m_publicKey)
	{
		return false;
	}
	int iKeyBytes = ::youmecommon::RSA_size(m_publicKey);
    if (iBufferLen % iKeyBytes != 0)
    {
        return false;
    }
    result.Allocate(iBufferLen);
    //开始分段了
    int iAlreadyDeEncry = 0; //已经解密的密文长度
    // 已经加密的密文长度
    int iDeEncryDataLen = 0;
    while (iAlreadyDeEncry < iBufferLen)
    {
        int ret = RSA_public_decrypt (iKeyBytes, pBuffer + iAlreadyDeEncry, (unsigned char *)result.Get() + iDeEncryDataLen, m_publicKey, RSA_PKCS1_PADDING);
        if (-1 == ret)
        {
            break;
        }
        iDeEncryDataLen += ret;
        iAlreadyDeEncry += iKeyBytes;
    }
    result.ReSize(iDeEncryDataLen);
    return iAlreadyDeEncry == iBufferLen;
}
}
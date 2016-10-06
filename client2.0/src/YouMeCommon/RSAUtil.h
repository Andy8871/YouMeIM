//
//  RSAUtil.h
//  YouMeVoiceEngine
//
//  Created by user on 15/9/22.
//  Copyright (c) 2015年 tencent. All rights reserved.
//

#ifndef __YouMeVoiceEngine__RSAUtil__
#define __YouMeVoiceEngine__RSAUtil__

#include <stdio.h>
#include <string>
#include <crypto/ossl_typ.h>
#include <crypto/crypto.h>
#include <crypto/rsa/rsa.h>
#include "XSharedArray.h"
namespace youmecommon
{
class CRSAUtil
{
public:
    CRSAUtil();
    ~CRSAUtil();
    //openssl 的扩展RSA算法，无论是公钥还是私钥都需要module参数。 对于公钥还需要public，如果是私有则还需要公钥+私钥匙
    bool SetPublicKey(CXSharedArray<unsigned char> & publicKey,CXSharedArray<unsigned char> & module);
    bool SetPrivateKey(CXSharedArray<unsigned char> &publicKey,CXSharedArray<unsigned char> & privateKey,CXSharedArray<unsigned char> & module);
    
    //公钥加密，私钥解密
    //result 参数返回加密之后的数据，在传入之前需要先创建一个空的，tsk_buffer_create_null();,使用完成之后需要
    //tsk_buffer_cleanup。 这玩意还是很坑爹的，明文超过128-11 就加密出错了。需要分段加密
    bool EncryptByPublicKey(const unsigned char* pBuffer,int iBufferLen,CXSharedArray<unsigned char> & result);
    bool DecryptByPrivateKey(const unsigned char* pBuffer,int iBufferLen,CXSharedArray<unsigned char> & result);
    
    
    //私钥加密，公钥解密
    bool EncryptByPrivateKey(const unsigned char* pBuffer,int iBufferLen,CXSharedArray<unsigned char> & result);
    bool DecryptByPublicKey(const unsigned char* pBuffer,int iBufferLen,CXSharedArray<unsigned char> & result);
    

private:
    //公钥和私钥
    ::youmecommon::RSA* m_publicKey;
    ::youmecommon::RSA* m_privateKey;
};
}
#endif /* defined(__YouMeVoiceEngine__RSAUtil__) */

//
//  Header.h
//  YouMeIM
//
//  Created by zxxing on 16/5/12.
//
//

#ifndef YOUME_PACKET_HEAD_INFO_20160512__H
#define YOUME_PACKET_HEAD_INFO_20160512__H

#include "PlatformDef.h"

class IYouMeSystemProvider
{
public:
	virtual ~IYouMeSystemProvider()
	{

	}
public:
    virtual XString getBrand () = 0;
    virtual XString getSystemVersion () = 0;
    virtual XString getCpuArchive () = 0;
    virtual XString getPackageName () = 0;
    virtual XString getUUID () = 0;
    virtual XString getModel () = 0;
    virtual XString getCpuShip () = 0;
    virtual XString getDocumentPath () = 0;
};

#endif /* YOUME_PACKET_HEAD_INFO_20160512__H */

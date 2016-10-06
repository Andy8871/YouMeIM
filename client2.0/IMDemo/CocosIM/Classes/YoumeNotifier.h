#pragma  once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>

class YoumeNotifier : public cocos2d::LayerColor
{
public:
    
    void onEnter();
    void onExit();
    
protected:
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

private:

    cocos2d::EventListenerTouchOneByOne* touchListener;
};

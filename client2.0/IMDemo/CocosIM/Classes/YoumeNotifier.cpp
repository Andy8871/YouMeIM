//
//  VolumeDialogLayer.cpp
//  CocosDemo
//
//  Created by zxxing on 16/1/9.
//
//

#include "YoumeNotifier.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

using namespace cocos2d::ui;

void YoumeNotifier::onEnter()
{
    LayerColor::onEnter();
    touchListener = EventListenerTouchOneByOne::create();//创建单点触摸事件监听器
    touchListener->onTouchBegan = CC_CALLBACK_2(YoumeNotifier::onTouchBegan,this);//触摸开始
    touchListener->onTouchMoved = CC_CALLBACK_2(YoumeNotifier::onTouchMoved,this);//触摸移动
    touchListener->onTouchEnded = CC_CALLBACK_2(YoumeNotifier::onTouchEnded,this);//触摸结束
    touchListener->setSwallowTouches(true);//不向下传递触摸
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,this);//注册分发器
}

void YoumeNotifier::onExit()
{
    LayerColor::onExit();
    _eventDispatcher->removeEventListener(touchListener);
}

bool YoumeNotifier::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    return true;
}
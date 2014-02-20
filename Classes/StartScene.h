#ifndef __StartLayer_SCENE_H__
#define __StartLayer_SCENE_H__

#include "cocos2d.h"

#include "Box2D/Box2D.h"

#include "SimpleAudioEngine.h"

USING_NS_CC;

using namespace CocosDenshion;

class StartLayer : public Layer
{
public:

	StartLayer();
	~StartLayer();
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static Scene* scene();
    
    // a selector callback
    void menuCloseCallback(Object* pSender);

	void menuNewGameCallback(Object* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(StartLayer);
};

#endif  // __HELLOWORLD_SCENE_H__
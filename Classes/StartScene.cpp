#include "StartScene.h"
#include "BubbleConstant.h"
#include "GameScene.h"

StartLayer::StartLayer()
{
	SimpleAudioEngine::getInstance()->playBackgroundMusic("main_music.mp3", true);
}

StartLayer::~StartLayer()
{
	
}

Scene* StartLayer::scene()
{
    Scene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = Scene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        StartLayer *layer = StartLayer::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool StartLayer::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(! Layer::init());

		auto winSize = Director::getInstance()->getWinSize();

		LabelTTF *label = LabelTTF::create("New Game", "Arial", 24 * 2);
		MenuItemLabel *pMenuItem = MenuItemLabel::create(label,CC_CALLBACK_1(StartLayer::menuNewGameCallback, this));
		//MenuItemLabel *pMenuItem = MenuItemLabel::create(label, this, menu_selector(StartLayer::menuNewGameCallback));
		pMenuItem->setTag(1);
		pMenuItem->setPosition(winSize.width / 2, winSize.height / 2);
		
        Menu* pMenu = Menu::create(pMenuItem, NULL);
        pMenu->setPosition(Point::ZERO);
        CC_BREAK_IF(! pMenu);
        this->addChild(pMenu, 1);

        LabelTTF* pLabel = LabelTTF::create("Bubble Cat 2014", "Arial", 24 * 3);
        CC_BREAK_IF(! pLabel);
        pLabel->setPosition(winSize.width / 2, winSize.height - 50);
        this->addChild(pLabel, 1);

        Sprite* pSprite = Sprite::create("StartScene_CN.jpg");
        CC_BREAK_IF(! pSprite);
        pSprite->setPosition(winSize.width/2, winSize.height/2);
        this->addChild(pSprite, 0);

		for (int i =0;i<2;i++)
		{
			BUBBLE_COLOR color = static_cast<BUBBLE_COLOR>(rand() % (COLOR_COUNT/* - 2*/));
			auto bubble = Bubble::create();
			bubble->setSpriteFrame(SpriteFrame::create(g_bubbleName[color],Rect(0,0,65,65)));
			bubble->setPosition(winSize.width/2+i*65, winSize.height/2);
			this->addChild(bubble, 0);
		}
	
        bRet = true;
    } while (0);

    return bRet;
}

void StartLayer::menuCloseCallback(Object* pSender)
{
    // "close" menu item clicked
    Director::getInstance()->end();
}

void StartLayer::menuNewGameCallback(Object* pSender)
{
	Director::getInstance()->replaceScene(GameScene::scene());
}


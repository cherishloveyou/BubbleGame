#include "GameScene.h"
#include "Utility.h"

#include <algorithm>

GameScene::GameScene():
	m_state(GS_START),
	m_nGoldenBubbleCount(0),
	m_nSliveryBubbleCount(0),
	m_nScore(0)
{
	SimpleAudioEngine::getInstance()->playBackgroundMusic("game_scene_bg.mp3",true);
	SimpleAudioEngine::getInstance()->playEffect("ready_go.mp3");
	srand(time(NULL));
}

GameScene::~GameScene()
{
	//clear();
}

Scene* GameScene::scene()
{
	auto scene = Scene::create();

	auto layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!Layer::init());

		//���ر���
		Sprite *background = Sprite::create("background1.jpg");
		CC_BREAK_IF(!background);
		background->setAnchorPoint(Point::ZERO);
		background->setPosition(Point::ZERO);
		this->addChild(background);

		CC_BREAK_IF(!GameScene::initScheduler());
		CC_BREAK_IF(!GameScene::initBoard());
		CC_BREAK_IF(!GameScene::initReadyBubble());
		CC_BREAK_IF(!GameScene::initWaitBubble());

		this->setEnable();
		
		bRet = true;

	} while (0);

	return bRet;
}

bool GameScene::initScheduler()
{
	this->schedule(schedule_selector(GameScene::loop), 1.0f);
	this->scheduleUpdate();
	return true;
}

//��ʼ�����ݶ��У�
bool GameScene::initBoard()
{
	bool bRet = false;
	do 
	{
		for (int row = 0; row < MAX_ROWS; row++)
		{
			for (int col = 0; col < MAX_COLS - row % 2; col++)
			{
				//��ʼ��ǰINIT_LINE�У� ������ΪNULL
				if (row >= INIT_LINE)
				{
					m_board[row][col] = NULL;
					continue;
				}

				Bubble* pBubble = randomBubble();
				if (pBubble == NULL)
				{
					CC_BREAK_IF(!pBubble);
				}
				
				// 
				pBubble->setPosition(getPosByRowAndCol(row, col));
				 
				this->addChild(pBubble);

				m_board[row][col] = pBubble;
				m_board[row][col]->setRowColIndex(row, col);
				m_listBubble.push_back(pBubble);

				bRet = true;
			}
		}
	} while (0);

	return bRet;
}

//���绯���ݷ�����
bool GameScene::initReadyBubble()
{
	bool bRet = false;
	do 
	{
		m_curReady = randomBubble();
		CC_BREAK_IF(!m_curReady);

		auto size = Director::getInstance()->getWinSize();
		m_curReady->setPosition(size.width /2, size.height/10);

		this->addChild(m_curReady);

		bRet = true;
	} while (0);

	return bRet;
}

bool GameScene::initWaitBubble()
{
	bool bRet = false;
	do 
	{
		for (int i = 0; i < MAX_WAIT_BUBBLE; i++)
		{
			Bubble *pBubble = randomBubble();
			CC_BREAK_IF(!pBubble);

			auto size = Director::getInstance()->getWinSize();
			pBubble->setPosition(size.width/2 + (i+1) * BUBBLE_RADIUS * 2,  size.height/20);
			m_wait[i] = pBubble;
			this->addChild(pBubble);

			bRet = true;
		}
	} while (0);

	return bRet;
}
Bubble* GameScene::randomBubble()
{
	BUBBLE_COLOR color = static_cast<BUBBLE_COLOR>(rand() % (COLOR_COUNT/* - 2*/));
	Bubble *pBubble = NULL;
	do 
	{
		pBubble = Bubble::create();

		SpriteFrame *frame = SpriteFrame::create(g_bubbleName[color],Rect(0,0,65,65));
		//pBubble->setSpriteFrame(SpriteFrame::create(g_bubbleName[color],Rect(0,0,65,65)));
		pBubble->setSpriteFrame(frame);
		if(pBubble)// && pBubble->initWithFile(g_bubbleName[color].c_str()))
		{
			pBubble->setBubbleColor(color);
		}
		else
		{
			CC_SAFE_DELETE(pBubble);
		}
	} while (0);

	return pBubble;
}

Bubble* GameScene::createBubble(BUBBLE_COLOR color)
{
	Bubble *pBubble = NULL;
	do 
	{
		pBubble = Bubble::create();

		SpriteFrame *frame = SpriteFrame::create(g_bubbleName[color],Rect(0,0,65,65));
		pBubble->setSpriteFrame(frame);
		if(pBubble)// && pBubble->initWithFile(g_bubbleName[color].c_str()))
		{
			pBubble->setBubbleColor(color);
		}
		else
		{
			CC_SAFE_DELETE(pBubble);
		}
	} while (0);

	return pBubble;
}

void GameScene::clear()
{
	for (int nRow = 0; nRow < MAX_ROWS; nRow++)
	{
		for (int nCol = 0; nCol < MAX_COLS - nRow % 2; nCol++)
		{
			CC_SAFE_DELETE(m_board[nRow][nCol]);
		}
	}

	m_listBubble.clear();
}

bool GameScene::isCollisionWithBorder()	//�Ƿ�ͱ�Ե��ײ
{
	auto size = Director::getInstance()->getWinSize();
	Point pos = m_curReady->getPosition();

	if (pos.x < BUBBLE_RADIUS || pos.x > size.width - BUBBLE_RADIUS)
	{
		return true;
	}

	return false;
}

bool GameScene::isCollisionWithTopBorder(Bubble *pBubble)
{
	if (pBubble == NULL)
	{
		return false;
	}

	auto pos = pBubble->getPosition();
	auto size = Director::getInstance()->getWinSize();
	if (pos.y > size.height - BUBBLE_RADIUS)
	{
		return true;
	}

	return false;
}

bool GameScene::isCollisionWithBubble(Point pos1, float radius1, Point pos2, float radius2)	//�Ƿ���Ϸ�������ײ
{
	return pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2) < pow(radius1 + radius2, 2);	//�ж���Բ�Ƿ��ཻ, ��ʽ����x1-x2)^2 + (y1-y2)^2 < (r1 + r2)^2
}

bool GameScene::isCollision()
{
	bool bRet = false;

	auto size = Director::getInstance()->getWinSize();
	if (m_curReady->getPosition().y > size.height - BUBBLE_RADIUS)
	{
		bRet = true;
		return bRet;
	}

	for (BUBBLE_LIST::reverse_iterator iterBubble = m_listBubble.rbegin(); iterBubble != m_listBubble.rend(); ++iterBubble)
	{
		Bubble *pBubble = *iterBubble;
		if (pBubble && isCollisionWithBubble(pBubble->getPosition(), BUBBLE_RADIUS, m_curReady->getPosition(), BUBBLE_RADIUS))
		{
			bRet = true;
			return bRet;
		}
	}

	return bRet;
}

void GameScene::setEnable()
{
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan,this);
	listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved,this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene::setDisableEnable()
{
	//CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	_eventDispatcher->removeAllEventListeners();
}

bool GameScene::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	return true;
}

void GameScene::onTouchMoved(Touch *pTouch, Event *pEvent)
{

}

void GameScene::onTouchEnded(Touch *pTouch, Event *pEvent)
{
// 	if (m_state == GS_FLY || m_state == GS_FALL)
// 	{
// 		return ;
// 	}
	m_state = GS_FLY;

	Point pos = pTouch->getLocation();
	m_real = (pos-m_curReady->getPosition()).normalize();
	//m_real = ccpNormalize(ccpSub(pos, m_curReady->getPosition()));

	//setDisableEnable();
	//this->scheduleUpdate();
}

void GameScene::loop(float dt)
{

}

void GameScene::update(float delta)
{
	if (isCollisionWithBorder())
	{
		m_real.x = -m_real.x;
	}

	Point pos = m_curReady->getPosition();
	m_curReady->setPosition(pos.x + m_real.x * BUBBLE_SPEED, pos.y + m_real.y * BUBBLE_SPEED);

	if (isCollision())	//�����������ϱ�Ե��ײ�ˣ� ����Ӧ�Ĵ���
	{		
		m_real = Point::ZERO;
		adjustBubblePosition();

		//����ͬ����������������Ӧ��������
		execClearBubble(m_curReady);

		//�����֮�󣬵��䴦������״̬����
		ROWCOL_LIST fallList = checkFallBubble();
		FallBubble(fallList);

		this->unscheduleUpdate();
 		changeWaitToReady();
		setEnable();
	}
}

void GameScene::adjustBubblePosition()
{
	Point curPos = m_curReady->getPosition();

	RowCol rowcol_index = GetRowColByPos(curPos.x, curPos.y);

	Point adjustPos = getPosByRowAndCol(rowcol_index.m_nRow, rowcol_index.m_nCol);
	m_curReady->setPosition(adjustPos);
	m_curReady->setRowColIndex(rowcol_index.m_nRow, rowcol_index.m_nCol);

	m_board[rowcol_index.m_nRow][rowcol_index.m_nCol] = m_curReady;
	m_listBubble.push_back(m_curReady);
}

//��wait״̬���򻻳�ready״̬
void GameScene::changeWaitToReady()
{
	m_curReady = m_wait[0];
	m_curReady->setPosition(READY_BUBBLE_POS);

	auto size = Director::getInstance()->getWinSize();

	for (int index = 0; index < MAX_WAIT_BUBBLE - 1; index++)
	{
		m_wait[index] = m_wait[index + 1];
		m_wait[index]->setPosition(size.width/2 + (index+1) * BUBBLE_RADIUS * 2, size.height/20);
	}

	m_wait[MAX_WAIT_BUBBLE - 1] = randomBubble();
	m_wait[MAX_WAIT_BUBBLE - 1]->setPosition(size.width/2+MAX_WAIT_BUBBLE * BUBBLE_RADIUS *2, size.height/20);

	this->addChild(m_wait[MAX_WAIT_BUBBLE - 1]);
}

ROWCOL_LIST GameScene::findClearBubble(Bubble *pReadyBubble)
{
	ROWCOL_LIST clearRowCollist;
	if (pReadyBubble == NULL)
	{
		return clearRowCollist;
	}

	if (pReadyBubble->getBubbleColor() == BUBBLE_GOLDEN)
	{
		clearRowCollist = findGoldenBubble(pReadyBubble);
	}
	else if (pReadyBubble->getBubbleColor() == BUBBLE_SILVERY)
	{
		clearRowCollist = findSliveryBubble(pReadyBubble);
	}
	else if (pReadyBubble->getBubbleColor() == COLOR_COUNT)
	{
		return clearRowCollist;
	}
	else
	{
		clearRowCollist = findSameBubble(pReadyBubble);
		if (clearRowCollist.size() < REMOVE_COUNT)
		{
			clearRowCollist.clear();
		}
	} 
	
	return clearRowCollist;
}

ROWCOL_LIST GameScene::findSameBubble(Bubble *pReadyBubble)
{
	ROWCOL_LIST samelist;
	BUBBLE_COLOR nColor= pReadyBubble->getBubbleColor();
	int nRow = pReadyBubble->getRowIndex();
	int nCol = pReadyBubble->getColumnIndex();
	samelist.push_back(RowCol(nRow, nCol));

	ROWCOL_LIST::iterator itCur = samelist.begin();

	//ѭ������ͬ������
	do 
	{
		std::vector<RowCol> vecRowCol;

		GetAround(itCur->m_nRow, itCur->m_nCol, vecRowCol);

		for (size_t i = 0; i < vecRowCol.size(); i++)
		{
			Bubble* pCurBubble = m_board[ vecRowCol[i].m_nRow ][ vecRowCol[i].m_nCol ];
			if (pCurBubble && pCurBubble->getBubbleColor() == nColor)
			{
				RowCol rc(vecRowCol[i].m_nRow, vecRowCol[i].m_nCol);
				ROWCOL_LIST::iterator itFind = std::find(samelist.begin(), samelist.end(), rc);
				//�����Ƿ��Ѿ����б���
				if (itFind == samelist.end())
				{
					samelist.push_back(vecRowCol[i]);
				}
			}
		}
		itCur++;
	} while (itCur != samelist.end());
	
	return samelist;
}

//���ҽ�ɫ�����������򼯺�
ROWCOL_LIST GameScene::findGoldenBubble(Bubble *pReadyBubble)
{
	int nRow = pReadyBubble->getRowIndex();
	int nCol = pReadyBubble->getColumnIndex();
	
	ROWCOL_LIST clearGoldenList;
	GetGoldenLine(nRow, nCol, clearGoldenList);

	return clearGoldenList;
}

//������ɫ�����������򼯺�
ROWCOL_LIST GameScene::findSliveryBubble(Bubble* pReadyBubble)
{
	int nRow = pReadyBubble->getRowIndex();
	int nCol = pReadyBubble->getColumnIndex();

	ROWCOL_LIST clearSliveryList;
	GetSliveryLine(nRow, nCol, clearSliveryList);

	return clearSliveryList;
}

/************************************************************************/
/* params: ������Ѿ�������λ�õ���
** NOTE  : ����һ���൱��Ҫ�ĺ�������Ϸ�ĺ��Ĺ��ܶ������ִ���������ݺ����У���Ҫִ�����¹���
		 1������������������ﵽGOLDEN_COUNT������ִ�н�ɫ���ݵ�Ч��
		 2��ͬ���ﵽSLIVERY_COUNT���� ��ִ����ɫ����Ч��
		 3���ﵽ3����С��SLIVERY_COUNT��������ͬ������
		 4��С��3����ʲô������
*/
/************************************************************************/
void GameScene::execClearBubble(Bubble* pReadyBubble)
{
	//Ҫִ������� ��Ҫ���ҵ���Ҫ�������
	ROWCOL_LIST clearBubbleList = findClearBubble(pReadyBubble);
	clearBubble(clearBubbleList);
}

//����bubbleList�����е���
void GameScene::clearBubble(const ROWCOL_LIST &bubbleList)
{
	int nRow, nCol;
	for (ROWCOL_LIST::const_iterator iterBubble = bubbleList.begin();
		iterBubble != bubbleList.end();
		iterBubble++)
	{
		nRow = iterBubble->m_nRow;
		nCol = iterBubble->m_nCol;

		Bubble *obj = m_board[nRow][nCol];
		if (obj)
		{
			removeBubbleAction(obj);
			m_board[nRow][nCol] = NULL;
		}
	
		BUBBLE_LIST::iterator itFind = std::find(m_listBubble.begin(), m_listBubble.end(), obj);
		if (itFind != m_listBubble.end())
		{
			m_listBubble.erase(itFind);
		}	
	}
}

void GameScene::removeBubbleAction(Bubble* pBubble)
{
	pBubble->runAction(
		Sequence::create(
			DelayTime::create(0.2f),
			FadeOut::create(0.5f),
			CallFuncN::create( CC_CALLBACK_1(GameScene::callbackRemoveBubble, this)),
			NULL
		)
	);
}

void GameScene::callbackRemoveBubble(Node *obj)
{
	if (obj != NULL)
	{
		this->removeChild(obj, true);
		obj->autorelease();
	}
}

ROWCOL_LIST GameScene::checkFallBubble()
{
	ROWCOL_LIST LinkBubbleList;	//����������һ�����

	for (int i = 0; i < MAX_COLS; i++)
	{
		if (m_board[0][i] != NULL)
		{
			LinkBubbleList.push_back(RowCol(0, i));
		}
	}
	if (LinkBubbleList.empty())
	{
		return LinkBubbleList;
	}

	//���ϵ��²�����������������ʣ��û���ҵ��ľ��ǲ�������
	ROWCOL_LIST::iterator itCur = LinkBubbleList.begin();
	do 
	{
		std::vector<RowCol> vecRowCol;
		GetAround(itCur->m_nRow, itCur->m_nCol, vecRowCol);

		for (size_t i = 0; i < vecRowCol.size(); i++)
		{
			Bubble *pBubble = m_board[ vecRowCol[i].m_nRow ][ vecRowCol[i].m_nCol ];
			if (pBubble)
			{
				RowCol pos(vecRowCol[i].m_nRow, vecRowCol[i].m_nCol);
				ROWCOL_LIST::iterator itFind = std::find(LinkBubbleList.begin(), LinkBubbleList.end(), pos);

				//�����Ƿ��Ѿ����б���
				if (itFind == LinkBubbleList.end())
				{
					LinkBubbleList.push_back(vecRowCol[i]);
				}
			}
		}
		itCur++;
	} while (itCur != LinkBubbleList.end());

	ROWCOL_LIST NoLinkBubblelist;	//�ҳ�ʣ�µ�����û�����ӵ��򣬾���Ҫ�������
	for (int i = 0; i < MAX_ROWS; i++)
	{
		for (int j = 0; j < MAX_COLS - i % 2; j++)
		{
			if (m_board[i][j] != NULL)
			{
				RowCol findRowCol(i,j);
				ROWCOL_LIST::iterator itFind = std::find(LinkBubbleList.begin(), LinkBubbleList.end(), findRowCol);
				if (itFind == LinkBubbleList.end())
				{
					NoLinkBubblelist.push_back(findRowCol);
				}
			}
		}
	}

	return NoLinkBubblelist;
}

//ִ�п��Ե��������
void GameScene::FallBubble(const ROWCOL_LIST &fallBubbleList)
{
	for (ROWCOL_LIST::const_iterator iter = fallBubbleList.begin(); iter != fallBubbleList.end(); iter++)
	{
		Bubble *pBubble = m_board[ iter->m_nRow ][ iter->m_nCol ];
		if (pBubble != NULL)
		{
			downBubbleAction(pBubble);
			
			BUBBLE_LIST::iterator iterBubble = std::find(m_listBubble.begin(), m_listBubble.end(),pBubble);
			if (iterBubble != m_listBubble.end())
			{
				m_listBubble.erase(iterBubble);
				m_board[ iter->m_nRow ][ iter->m_nCol ] = NULL;
			}
		}
	}
	
}

//��������
void GameScene::downBubbleAction(Bubble *pBubble)
{
	float offY = -100;

	Point pos = pBubble->getPosition();
	pBubble->runAction(
			Sequence::create(
				MoveTo::create((pos.y - offY) / 600.0, Point(pos.x, offY)),
				CallFuncN::create(CC_CALLBACK_1(GameScene::callbackRemoveBubble,this
				)),
				NULL
			)
		);
}
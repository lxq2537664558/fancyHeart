//
//  Formation.h
//  fancyHeart
//
//  Created by zhai on 14-6-18.
//
//

#ifndef __fancyHeart__Formation__
#define __fancyHeart__Formation__

#include <iostream>
#include <map>
#include "cocos2d.h"
#include "Manager.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "BaseUI.h"
#include "JValue.h"
#include "XGate.h"
#include "XNode.h"
#include "GateSelect.h"
#include "Mask.h"
#include "RotateList.h"
USING_NS_CC;
using namespace ui;
using namespace cocostudio;
struct FormationData{
    int64 npcId;
    int isFormation;
};
class Formation:public BaseUI
{
   
public:
	static Formation* create(BaseUI* delegate);
	virtual bool init(std::string fileName);
    virtual void resetUI();
    virtual void onEnter();
    virtual void onExit();
private:
    RotateList* rotateList;
    PageView* pageView;
	void touchEvent(Ref *pSender, TouchEventType type);
    void touchButtonEvent(Ref *pSender, TouchEventType type);
    void pageViewEvent(Ref *pSender, PageView::EventType type);
    void rotateListCallback(RotateList::EventType type,Widget*item,int index);
    void setListMask(int index);
    //临时组
    std::vector<std::vector<FormationData >> groups;
    std::vector<FormationData > npcs;
    //组变化npcId要上阵或者下阵的npcid，groupIndex当前要下阵的组 isFormation 上阵还是下阵
    void changeGroup(int64 npcId,int groupIndex,bool isFormation);
    //初始化group数据
    void initGroup(int groupIndex,bool isInit=true);
    void displayAction(Widget* widget,bool isDown);
    //排序
    static bool sortIds(FormationData fd1,FormationData fd2);
    //判断某npc是否在阵上
    bool isInFormation(int64 npcId);
    //更新组
    void updateGroups(const std::vector<std::vector<FormationData>> groups);
};
#endif /* defined(__fancyHeart__Formation__) */
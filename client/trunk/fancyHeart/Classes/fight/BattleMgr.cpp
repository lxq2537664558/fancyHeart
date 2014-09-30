//
//  BattleMgr.cpp
//  fancyHeart
//
//  Created by 秦亮亮 on 14-5-15.
//
//

#include "BattleMgr.h"

static BattleMgr* instance=nullptr;

BattleMgr* BattleMgr::getInstance(){
    if(instance==nullptr){
        instance=new BattleMgr();
    }
    return instance;
}

void BattleMgr::init(std::vector<long long>heros,int nodeID,int gateID)
{
    auto scene=BattleScene::createScene();
    Manager::getInstance()->switchScence(scene);

    this->gateID=gateID;
    this->nodeID=nodeID;
    this->view=(BattleScene*)scene->getChildByTag(0);
   
    XNode* xn=XNode::record(Value(nodeID));
    
    int battleID=xn->getBattleID();
    XBattle* xb=XBattle::record(Value(battleID));
    if(xb->getMGroup1()>0){
        this->groups.push_back(xb->getMGroup1());
    }
    if(xb->getMGroup2()>0){
        this->groups.push_back(xb->getMGroup2());
    }
    if(xb->getMGroup3()>0){
        this->groups.push_back(xb->getMGroup3());
    }
    if(xb->getMGroup4()>0){
        this->groups.push_back(xb->getMGroup4());
    }
    if(xb->getMGroup5()>0){
        this->groups.push_back(xb->getMGroup5());
    }
    this->npcNum=this->groups.size();
    this->view->initNpcIcon(this->npcNum);
    
    this->initHero(heros);
    
    this->groupID=this->groups.at(0);

    DramaAni::getInstance()->startPre(this->groupID,this->gateID,this->nodeID);
}

void BattleMgr::init(rapidjson::Value& data)
{
    
}

void BattleMgr::startBattle()
{
    this->isOver=false;
    //this->view->bg->setDisplayFrame(Sprite::create("fightBg2.png")->displayFrame());
    this->view->layout->runAction(Sequence::create(FadeOut::create(1),FadeIn::create(1),CallFunc::create(CC_CALLBACK_0(BattleMgr::startBattleAnimEnd, this)) ,NULL));
}

void BattleMgr::startBattleAnimEnd()
{
    BattleMgr::getInstance()->npcs.clear();
    this->groupID=this->groups.at(0);
    this->groups.erase(groups.begin());
    this->initNpc();
    
    //hero
    Size winSize=Director::getInstance()->getWinSize();
    for(int i=0;i<this->heros.size();i++){
        FighterMgr* mf=heros.at(i);
        int pos=mf->pos;
        mf->view->setPosition(Vec2(-GRID_SIZE*3*i,winSize.height/2+(pos%2?-GRID_SIZE:GRID_SIZE)));
        this->view->heroNode->reorderChild(mf->view, 640-abs(mf->view->getPositionY()));
        mf->view->state=fstate::idle;
        mf->start();
    }
}

void BattleMgr::initHero(std::vector<long long> npcs)
{
    //初始化角色100
    std::vector<int> ids={100,101,101,102,999};
    std::vector<BData> heroDatas;
    for(int i=0;i<npcs.size();i++)
    {
        PNpc* npc=Manager::getInstance()->getNpc(npcs[i]);
        std::vector<int> skills;
        for(int i=0;i<npc->skillidlist_size();i++){
            skills.push_back(npc->skillidlist(i));
        }
        //heroDatas.push_back(BData{npc->spriteid(),npc->level(),npc->quality(),npc->star(),i,skills});
        //测试数据
        heroDatas.push_back(BData{ids[i],npc->level(),ids[i]==999?9:npc->quality(),npc->star(),i,skills});
    }
    sort(heroDatas.begin(),heroDatas.end(), BattleMgr::sortGrid);

    Vector<FData*> arr;
    for(int i=0;i<heroDatas.size();i++)
    {
        BData bd=heroDatas.at(i);
        FData* fd=FData::create(bd);
        FighterMgr* hero=FighterMgr::create(fd);
        this->heros.pushBack(hero);
        this->view->createSkill(bd.skills.at(0), bd.pos);
        bd.skills.erase(bd.skills.begin());
    }
}

void BattleMgr::initNpc()
{
//    int groupID=this->groups.at(0);
//    this->groups.erase(groups.begin());
    this->view->resetProgress();

    std::vector<BData> npc;
    XMonster* xm=XMonster::record(Value(this->groupID));
    
    int mid=xm->getMID1();
    npc.push_back({mid/10,xm->getMLv1(),mid%10,xm->getMStar1(),5,this->getMonsterSkill(mid/10)});
    
    mid=xm->getMID2();
    if(mid > 0){
        npc.push_back({mid/10,xm->getMLv2(),mid%10,xm->getMStar2(),6,this->getMonsterSkill(mid/10)});
    }
    
    mid=xm->getMID3();
    if(mid > 0){
        npc.push_back({mid/10,xm->getMLv3(),mid%10,xm->getMStar3(),7,this->getMonsterSkill(mid/10)});
    }
    
    mid=xm->getMID4();
    if(mid > 0){
        npc.push_back({mid/10,xm->getMLv4(),mid%10,xm->getMStar4(),8,this->getMonsterSkill(mid/10)});
    }
    
    mid=xm->getMID5();
    if(mid > 0){
        npc.push_back({mid/10,xm->getMLv5(),mid%10,xm->getMStar5(),9,this->getMonsterSkill(mid/10)});
    }
    
    //init modle & view
    Size winSize=Director::getInstance()->getWinSize();
    sort(npc.begin(), npc.end(), this->sortGrid);
    
    for(int i=0;i<npc.size();i++)
    {
        FData* fd=FData::create(npc.at(i));
        FighterMgr* npc=FighterMgr::create(fd);
        this->npcs.pushBack(npc);
        npc->start();
    }
}

std::vector<int> BattleMgr::getMonsterSkill(int xid)
{
    std::vector<int> skills;
    XRole* xrole=XRole::record(Value(xid));
    
    int skill=xrole->getCommonSkill();
    skills.push_back(skill);
    
    skill=xrole->getSkill1();
    if(skill>0){
        skills.push_back(skill);
    }
    skill=xrole->getSkill2();
    if(skill>0){
        skills.push_back(skill);
    }
    skill=xrole->getSkill3();
    if(skill>0){
        skills.push_back(skill);
    }
    skill=xrole->getSkill4();
    if(skill>0){
        skills.push_back(skill);
    }
    skill=xrole->getSkill5();
    if(skill>0){
        skills.push_back(skill);
    }
    skill=xrole->getSkill6();
    if(skill>0){
        skills.push_back(skill);
    }
    skill=xrole->getSkill7();
    if(skill>0){
        skills.push_back(skill);
    }
    return skills;
}

void BattleMgr::skillAttack(int skill)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("effect/ziguang.plist");
    Clip* clip=Clip::create("effect/ziguang.plist","1053", 12);
    this->view->addChild(clip,1);
    clip->setPosition(npcs.at(0)->view->getPosition());
    clip->play();
}

bool BattleMgr::sortGrid(BData f1,BData f2)
{
    int dis1=XRole::record(Value(f1.xid))->getLockGrid();
    int dis2=XRole::record(Value(f2.xid))->getLockGrid();
    return dis1-dis2<0;
}

std::vector<int> BattleMgr::getFoes(int pos,bool isMe)
{
    std::vector<int> arr;
    if((pos<5 && isMe) || (pos>4 && !isMe)){
        for(FighterMgr* mf : heros){
            if(mf->view->state==fstate::die || mf->view->state==fstate::over) continue;
            arr.push_back(mf->pos);
        }
    }
    if((pos<5 && !isMe) || (pos>4 && isMe)){
        for(FighterMgr* mf : npcs){
            if(mf->view->state==fstate::die || mf->view->state==fstate::over) continue;
            arr.push_back(mf->pos);
        }
    }
    return arr;
}

FighterMgr* BattleMgr::getFirst(int pos)
{
    FighterMgr* pRet=nullptr;
    if(pos > 4){
        pRet=heros.at(0);
        for(FighterMgr* mf : heros){
            if(mf->view->getPositionX() > pRet->view->getPositionX()){
                pRet=mf;
            }
        }
    }else{
        pRet=npcs.at(0);
        for(FighterMgr* mf :npcs){
            if(mf->view->getPositionX() < pRet->view->getPositionX()){
                pRet=mf;
            }
        }
    }
    return pRet;
}

FighterMgr* BattleMgr::getHero(int pos){
    if(pos<5){
        for(FighterMgr* mf : heros){
            if(mf->pos==pos){
                return mf;
            }
        }
    }else{
        for(FighterMgr* mf : npcs){
            if(mf->pos==pos){
                return mf;
            }
        }
    }
    return nullptr;
}

void BattleMgr::stopAllFighter()
{
    for(FighterMgr* npc :heros){
        npc->pause();
        if(npc->view->state!=fstate::die){
            npc->view->state=fstate::over;
        }
    }
    for(FighterMgr* npc : npcs){
        npc->pause();
        if(npc->view->state!=fstate::die){
            npc->view->state=fstate::over;
        }
    }
}

void BattleMgr::startEndDram()
{
    this->isOver=true;

    for(FighterMgr* mf : heros) {
        mf->view->setVisible(false);
        mf->pause();
    }
    DramaAni::getInstance()->startEnd(this->groupID,this->gateID,this->nodeID);
}

void BattleMgr::handleResult()
{
    if(this->winPos<5 && this->groups.size()){
        for(FighterMgr* mf : BattleMgr::getInstance()->heros){
            if(mf->view->state==fstate::die){
                continue;
            }
            mf->view->setVisible(true);
            mf->view->run();
        }
        Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&BattleMgr::startBattle), this, 5, 0, 5, false);
        this->view->setNpcIcon(npcNum-this->groups.size(),true);
    }
    
    if(this->winPos>4 || this->groups.size()==0){
        PResultReq pResultReq;
        pResultReq.set_gateid(this->gateID);
        pResultReq.set_xid(this->nodeID);
        pResultReq.set_star(this->heros.size());
        Manager::getInstance()->socket->send(C_FIGHTRESULT, &pResultReq);
    }
}
//超时失败
void BattleMgr::overTime()
{
    this->stopAllFighter();
    PResultReq pResultReq;
    pResultReq.set_gateid(this->gateID);
    pResultReq.set_xid(this->nodeID);
    pResultReq.set_star(0);
    Manager::getInstance()->socket->send(C_FIGHTRESULT, &pResultReq);
}

void BattleMgr::clearDieNpc(int pos)
{
    FighterMgr* fm=this->getHero(pos);
    if(pos<5 && pos>-1){
        this->heros.eraseObject(fm);
    }else if(pos>4 && pos<10){
        this->npcs.eraseObject(fm);
    }else{
        log("die error pos:%d",pos);
    }
}

void BattleMgr::clear()
{
    this->heros.clear();
    this->npcs.clear();
    this->groups.clear();
    SpriteFrameCache::getInstance()->removeSpriteFrames();
}
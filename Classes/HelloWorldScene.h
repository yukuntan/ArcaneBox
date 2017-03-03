#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "entity_ext/event.h"
#include "entity_ext/builder.h"
#include "entity_ext/entity.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    std::unique_ptr<ECS::EntityManager> m_entity_manager;
    std::unique_ptr<ECS::EventSystem> m_event_system;
    std::unique_ptr<ECS::Builder> m_entity_builder;
};

#endif // __HELLOWORLD_SCENE_H__

#include "HelloWorldScene.h"
#include <cocostudio/CocoStudio.h>

#include "game/component_map.h"
#include "game/component_node.h"
#include "game/component_sprite.h"
#include "game/component_storage.h"
#include "entity_ext/entity.hpp"

using namespace Arcane;
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ) {
        return false;
    }
    
    // entity test begin
    
    m_entity_manager = std::unique_ptr<ECS::EntityManager>(new ECS::EntityManager());
    m_event_system = std::unique_ptr<ECS::EventSystem>(new ECS::EventSystem());
    m_entity_builder = std::unique_ptr<ECS::Builder>(new ECS::Builder(m_entity_manager.get()));
    
    std::string content = cocos2d::FileUtils::getInstance()->getStringFromFile("block.prefab");
    m_entity_builder->loadPrefab("block.prefab", content);
    std::string extend ="\"ThumbnailCom\": { \"group\": { \"open\": \"smallmap_block\"}}";
    std::map<std::string,std::string> ext;
    ext["ThumbnailCom"] = extend;
    std::vector<uint64_t> list;
    const static int32_t count = 5;
    int32_t index = rand() % count;
    for (int32_t i = 0; i < count; i++) {
        if (i == index) {
            auto entity = m_entity_builder->extendPrefab("block.prefab", ext);
            list.push_back(entity.id().id());
        }
        else if (i == 4) {
            AnimeCom com;
            auto entity = m_entity_builder->extendAssign("block.prefab", com);
            list.push_back(entity.id().id());
        }
        else {
            auto entity = m_entity_builder->clone("block.prefab");
            list.push_back(entity.id().id());
        }
        
    }
    
    for (int32_t i = 0; i < 5; i++) {
        ECS::Entity entity(m_entity_manager.get(), list[i]);
        
        if (entity.hasComponent<StorageCom>()) {
            auto storage_com = entity.getComponent<StorageCom>().get();
            storage_com->group++;
        }
        
        if (entity.hasComponent<SpriteCom>()) {
            auto sprite_com = entity.getComponent<SpriteCom>().get();
            sprite_com->group["idle"] = "npc_idle_0";
        }
        
        if (entity.hasComponent<NodeCom>()) {
            auto node_com = entity.getComponent<NodeCom>().get();
            node_com->node_of_parent = "root";
        }
        
        if (entity.hasComponent<ThumbnailCom>()) {
            auto thumbnail_com = entity.getComponent<ThumbnailCom>().get();
            thumbnail_com->group["run"] = "root";
        }
        
        if (entity.hasComponent<AnimeCom>()) {
            auto anime_com = entity.getComponent<AnimeCom>().get();
            anime_com->group["run"] = "root";
        }
    }
    
    // entity test end

	auto mainlayer = cocos2d::CSLoader::createNode("mainlayer.csb");
	auto mainanime = cocos2d::CSLoader::createTimeline("mainlayer.csb");
	addChild(mainlayer);
	mainlayer->runAction(mainanime);
    mainanime->play("enter", false);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

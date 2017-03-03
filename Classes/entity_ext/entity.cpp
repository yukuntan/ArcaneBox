/*

the entity is just container of the components, has an id. 

Author:  yukun tan (codecraft@163.com)

(C) Copyright tanyukun 2015. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears
in all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.

*/
#include "entity.hpp"

namespace ECS {
	const Entity::ID Entity::INVALID;

	void Entity::invalidate()
	{
		m_id = INVALID;
		m_manager = nullptr;
	}

	void Entity::destroy()
	{
		m_manager->destroy(m_id);
		invalidate();
	}

	std::bitset<MAX_COMPONENTS> Entity::componentMask() const
	{
		return m_manager->componentMask(m_id);
	}

	EntityManager::EntityManager()
		: m_event_system(nullptr) {
	}

	EntityManager::~EntityManager() {
		for (auto &pool : m_component_pools) {
			delete pool;
		}
	}

	void EntityManager::setEventSystem(EventSystem *event_system)
	{
		m_event_system = event_system;
	}

	Entity EntityManager::create()
	{
		uint32_t index, version;
		if (m_free_list.empty())
		{
			index = m_index_counter++;
			accommodateEntity(index);
			version = m_entity_version[index] = 1;
		}
		else
		{
			index = m_free_list.back();
			m_free_list.pop_back();
			version = m_entity_version[index];
		}
		return Entity(this, Entity::ID(index, version));
	}

	void EntityManager::destroy(Entity::ID id)
	{
		m_event_system->send(get(id), *BeforeRemoveEntity::getInstance());

        destroyNoNotify(id);
	}
    
    void EntityManager::destroyNoNotify(Entity::ID id) {
        uint32_t index = id.index();
        auto mask = m_entity_component_mask[id.index()];
        for (size_t i = 0; i < m_component_pools.size(); i++)
        {
            BasePool *pool = m_component_pools[i];
            if (pool && mask.test(i))
                pool->destroy(index);
        }
        m_entity_component_mask[index].reset();
        m_entity_version[index]++;
        m_free_list.push_back(index);
    }
    
	void EntityManager::clear() {
		for (auto pool : m_component_pools) {
			delete pool;
		}

		m_entity_version.clear();
		m_free_list.clear();
		m_entity_component_mask.clear();
		m_component_pools.clear();
		m_index_counter = 0;
	}
}

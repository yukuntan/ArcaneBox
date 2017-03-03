//
//  entity_.h
//  MTTower
//
//  Created by codecraftcn on 15/11/6.
//
//

#ifndef _ENTITY_HPP__
#define _ENTITY_HPP__

#include "entity.h"
#include "event_internal.h"

namespace ECS {
    template <typename ComponentType>
    ComponentRef<ComponentType> Entity::assignComponent()
    {
        return m_manager->assignComponent<ComponentType>(m_id);
    }
    
    template <typename ComponentType>
    ComponentRef<ComponentType> Entity::assignComponentFrom(const ComponentType &component)
    {
        return m_manager->assignComponentFrom<ComponentType>(m_id, component);
    }
    
    template <typename ComponentType>
    void Entity::removeComponent()
    {
        m_manager->removeComponent<ComponentType>(m_id);
    }
    
    template <typename ComponentType>
    ComponentRef<ComponentType> Entity::getComponent()
    {
        return m_manager->getComponent<ComponentType>(m_id);
    }
    
    template <typename ComponentType>
    ComponentRef<ComponentType, const EntityManager> Entity::getComponent() const
    {
        return m_manager->getComponent<ComponentType, const EntityManager>(m_id);
    }
    
    template <typename ... C_N>
    std::tuple<ComponentRef<C_N>...> Entity::getComponents()
    {
        return m_manager->getComponents<C_N...>();
    }
    
    template <typename ... C_N>
    std::tuple<ComponentRef<const C_N, const EntityManager>...> Entity::getComponents() const
    {
        return const_cast<const EntityManager*>(m_manager)->getComponents<const C_N...>();
    }
    
    template <typename ComponentType>
    bool Entity::hasComponent() const {
        return m_manager->hasComponent<ComponentType>(m_id);
    }

	template <typename ComponentType>
	bool Entity::hasComponentOr() const {
		return m_manager->hasComponentOr<ComponentType>(m_id);
	}
    
    template <typename C_1, typename C_2, typename ... C_N>
    bool Entity::hasComponent() const {
        return m_manager->hasComponent<C_1, C_2, C_N ...>(m_id);
    }

	template <typename C_1, typename C_2, typename ... C_N>
	bool Entity::hasComponentOr() const {
		return m_manager->hasComponentOr<C_1, C_2, C_N ...>(m_id);
	}
    
    template <typename C_1, typename ... C_N>
    void Entity::unpackComponents(ComponentRef<C_1> &c1, ComponentRef<C_N> & ... cn) {
        m_manager->unpack(m_id, c1, cn ...);
    }
    
    inline bool Entity::valid() const {
        return m_manager && m_manager->valid(m_id);
    }
    
    inline bool EntityManager::valid(Entity::ID id) const
    {
        return id.index() < m_entity_version.size() && m_entity_version[id.index()] == id.version();
    }
    
    inline size_t EntityManager::size() const
    {
        return m_entity_component_mask.size() - m_free_list.size();
    }
    
    inline size_t EntityManager::capacity() const
    {
        return m_entity_component_mask.size();
    }
    
    inline Entity EntityManager::get(Entity::ID id)
    {
        return Entity(this, id);
    }
    
    inline Entity::ID EntityManager::createId(uint32_t index) const
    {
        return Entity::ID(index, m_entity_version[index]);
    }
    
    template <typename ComponentType>
    BaseComponent::Family EntityManager::component_family() const
    {
        return Component<typename std::remove_const<ComponentType>::type>::family();
    }
    
    template <typename ComponentType>
    bool EntityManager::hasComponent(Entity::ID id) const {
        BaseComponent::Family family = component_family<ComponentType>();
        if (family >= m_component_pools.size())
            return false;
        BasePool *pool = m_component_pools[family];
        if (!pool || !m_entity_component_mask[id.index()][family])
            return false;
        return true;
    }

	template <typename ComponentType>
	bool EntityManager::hasComponentOr(Entity::ID id) const {
		return hasComponent<ComponentType>(id);
	}
    
    template <typename C_1, typename C_2, typename ...C_N>
    bool EntityManager::hasComponent(Entity::ID id) const {
        return hasComponent<C_1>(id) && hasComponent<C_2, C_N ...>(id);
    }

	template <typename C_1, typename C_2, typename ...C_N>
	bool EntityManager::hasComponentOr(Entity::ID id) const {
		return hasComponentOr<C_1>(id) || hasComponentOr<C_2, C_N ...>(id);
	}
    
    template <typename ComponentType>
    ComponentRef<ComponentType> EntityManager::getComponent(Entity::ID id)
    {
        BaseComponent::Family family = component_family<ComponentType>();
        if (family >= m_component_pools.size())
            return ComponentRef<ComponentType, EntityManager>();
        BasePool *pool = m_component_pools[family];
        if (!pool || !m_entity_component_mask[id.index()][family])
            return ComponentRef<ComponentType, EntityManager>();
        return ComponentRef<ComponentType, EntityManager>(this, id);
    }
    
    template <typename ComponentType>
    const ComponentRef<ComponentType, const EntityManager> EntityManager::getComponent(Entity::ID id) const
    {
        BaseComponent::Family family = component_family<ComponentType>();
        if (family > m_component_pools.size())
            return ComponentRef<ComponentType, const EntityManager>();
        BasePool *pool = m_component_pools[family];
        if (!pool || !m_entity_component_mask[id.index()][family])
            return ComponentRef<ComponentType, const EntityManager>();
        return ComponentRef<ComponentType, const EntityManager>(this, id);
    }
    
    template <typename ... C_N>
    std::tuple<ComponentRef<C_N>...> EntityManager::getComponents(Entity::ID id)
    {
        return std::make_tuple(getComponent<C_N>(id)...);
    }
    
    template <typename ... C_N>
    std::tuple<ComponentRef<const C_N, const EntityManager>...> EntityManager::getComponents(Entity::ID id) const
    {
        return std::make_tuple(getComponent<const C_N, const EntityManager>(id)...);
    }
    
	template <typename Component>
	ComponentRef<Component> EntityManager::assignComponent(Entity::ID id)
	{
		BaseComponent::Family family = component_family<Component>();
        accommodateComponent<Component>();
		m_entity_component_mask[id.index()].set(family);

		ComponentRef<Component> component(this, id);

		if (m_event_system) {
			Entity entity(this, id);
			m_event_system->send(entity, *OnAddedComponent::getInstance());
		}
		return component;
	}

	template <typename Component>
	ComponentRef<Component> EntityManager::assignComponentFrom(Entity::ID id, const Component &source)
	{
		BaseComponent::Family family = component_family<Component>();
		Pool<Component> *pool = accommodateComponent<Component>();
		(*(Component*)pool->get(id.index())) = source;

		m_entity_component_mask[id.index()].set(family);

		ComponentRef<Component> component(this, id);

		if (m_event_system) {
			Entity entity(this, id);
			m_event_system->send(entity, *OnAddedComponent::getInstance());
		}
		return component;
	}
    
    template <typename ComponentType>
    void EntityManager::removeComponent(Entity::ID id)
    {
        BaseComponent::Family family = component_family<ComponentType>();
        const uint32_t index = id.index();
        
        BasePool *pool = m_component_pools[family];
        ComponentRef<ComponentType> component(this, id);
        m_entity_component_mask[id.index()].reset(family);
        
        if (m_event_system) {
            Entity entity(this, id);
            m_event_system->send(entity, *BeforeRemoveComponent::getInstance());
        }
        pool->destroy(index);
    }
    
    template <typename ComponentType>
    ComponentType *EntityManager::getComponentPtr(Entity::ID id)
    {
        BaseComponent::Family family = component_family<ComponentType>();
        BasePool *pool = m_component_pools[family];
        return static_cast<ComponentType*>(pool->get(id.index()));
    }
    
    template <typename ComponentType>
    const ComponentType *EntityManager::getComponentPtr(Entity::ID id) const
    {
        BaseComponent::Family family = component_family<ComponentType>();
        BasePool *pool = m_component_pools[family];
        return static_cast<const ComponentType*>(pool->get(id.index()));
    }
    
    template <typename ComponentType>
    EntityManager::ComponentMask EntityManager::componentMask()
    {
        ComponentMask mask;
        mask.set(component_family<ComponentType>());
        return mask;
    }
    
    template <typename ComponentType>
    EntityManager::ComponentMask EntityManager::componentMask(const ComponentRef<ComponentType> &ref)
    {
        return componentMask<ComponentType>();
    }
    
    template <typename C_1, typename C_2, typename ... C_N>
    EntityManager::ComponentMask EntityManager::componentMask()
    {
        return componentMask<C_1>() | componentMask<C_2, C_N ...>();
    }
    
    template <typename C_1, typename ... C_N>
    EntityManager::ComponentMask EntityManager::componentMask(const ComponentRef<C_1> &c1, const ComponentRef<C_N> & ... cn)
    {
        return componentMask<C_1, C_N ...>();
    }
    
    inline EntityManager::ComponentMask EntityManager::componentMask(Entity::ID id)
    {
        return m_entity_component_mask.at(id.index());
    }
    
    template <typename ComponentType>
    inline Pool<ComponentType> *EntityManager::accommodateComponent()
    {
        BaseComponent::Family family = component_family<ComponentType>();
        if (m_component_pools.size() <= family)
        {
            m_component_pools.resize(family + 1, nullptr);
        }
        
        if (!m_component_pools[family])
        {
            Pool<ComponentType> *pool = new Pool<ComponentType>();
            pool->expand(m_index_counter);
            m_component_pools[family] = pool;
        }
        return static_cast<Pool<ComponentType> *>(m_component_pools[family]);
    }
    
    inline void EntityManager::accommodateEntity(uint32_t index)
    {
        if (m_entity_component_mask.size() <= index)
        {
            m_entity_component_mask.resize(index + 1);
            m_entity_version.resize(index + 1);
            for (BasePool *pool : m_component_pools)
                if (pool)
                    pool->expand(index + 1);
        }
    }
    
    template <typename ComponentType>
    void EntityManager::unpack(Entity::ID id, ComponentRef<ComponentType> &c)
    {
        c = getComponent<ComponentType>(id);
    }
    
    template <typename C_1, typename ... C_N>
    void EntityManager::unpack(Entity::ID id, ComponentRef<C_1> &c1, ComponentRef<C_N> &... cn)
    {
        c1 = getComponent<C_1>(id);
        unpack<C_N ...>(id, cn ...);
    }
    
    template <typename ... C_N>
    EntityManager::View<C_N...> EntityManager::entitiesWithComponents()
    {
        auto mask = componentMask<C_N ...>();
        return View<C_N...>(this, mask);
    }
    
    template <typename ... C_N>
    void EntityManager::each(typename identity<std::function<void(Entity entity, C_N&...)>>::type f)
    {
        return entitiesWithComponents<C_N...>().each(f);
    }
    
    template <typename ComponentType, typename ContainerType>
    inline ComponentRef<ComponentType, ContainerType>::operator bool() const
    {
        return valid();
    }
    
    template <typename ComponentType, typename ContainerType>
    inline bool ComponentRef<ComponentType, ContainerType>::valid() const
    {
        return m_container && m_container->valid(m_id) && m_container->template hasComponent<ComponentType>(m_id);
    }
    
    template <typename ComponentType, typename ContainerType>
    inline ComponentType *ComponentRef<ComponentType, ContainerType>::operator -> ()
    {
        return m_container->template getComponentPtr<ComponentType>(m_id);
    }
    
    template <typename ComponentType, typename ContainerType>
    inline const ComponentType *ComponentRef<ComponentType, ContainerType>::operator -> () const
    {
        return m_container->template getComponentPtr<ComponentType>(m_id);
    }
    
    template <typename ComponentType, typename ContainerType>
    inline ComponentType *ComponentRef<ComponentType, ContainerType>::get()
    {
        return m_container->template getComponentPtr<ComponentType>(m_id);
    }
    
    template <typename ComponentType, typename ContainerType>
    inline const ComponentType *ComponentRef<ComponentType, ContainerType>::get() const
    {
        return m_container->template getComponentPtr<ComponentType>(m_id);
    }
    
    template <typename ComponentType, typename ContainerType>
    inline void ComponentRef<ComponentType, ContainerType>::remove()
    {
        m_container->template remove<ComponentType>(m_id);
    }
    
};

#endif /* entity__h */

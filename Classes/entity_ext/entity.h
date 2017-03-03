#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <cstdint>
#include <tuple>
#include <new>
#include <cstdlib>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <type_traits>
#include <functional>
#include "component.h"
#include "../tool/pool.h"

namespace ECS {
    static const size_t MAX_COMPONENTS = 256;
    class EntityManager;
    class EventSystem;
    template <typename ComponentType, typename ContainerType = EntityManager>
    class ComponentRef;
    
    class Entity {
    public:
        struct ID
        {
            ID() : m_id(0) {}
			ID(uint64_t id) : m_id(id) {}
            explicit ID(uint32_t index, uint32_t version) : m_id(uint64_t(index) | uint64_t(version) << 32UL) {}

            uint64_t id() const { return m_id; }
            uint32_t index() const { return m_id & 0xffffffffUL; }
            uint32_t version() const { return m_id >> 32; }
            
            bool operator == (const ID &other) const { return m_id == other.m_id; }
            bool operator != (const ID &other) const { return m_id != other.m_id; }
            bool operator < (const ID &other) const { return m_id < other.m_id; }
            
        private:
            friend class EntityManager;
            uint64_t m_id = 0;
        };
        
    public:
        Entity() = default;
        Entity(const Entity &other) = default;
        Entity(EntityManager *manager, ID id) : m_manager(manager), m_id(id) {}
        Entity &operator = (const Entity &other) = default;
		// for other system to send event
		EntityManager *getManager() { return m_manager; }
        ID id() const { return m_id; }
        
        operator bool() const
        {
            return valid();
        }
        
        bool operator == (const Entity &other) const
        {
            return other.m_manager == m_manager && other.m_id == m_id;
        }
        
        bool operator != (const Entity &other) const
        {
            return !(other == *this);
        }
        
        bool operator < (const Entity &other) const
        {
            return other.m_id < m_id;
        }
        
        bool valid() const;
        
        void invalidate();
        
        void destroy();
        
        std::bitset<MAX_COMPONENTS> componentMask() const;
        
        template <typename ComponentType>
        ComponentRef<ComponentType> assignComponent();
        
        template <typename ComponentType>
        ComponentRef<ComponentType> assignComponentFrom(const ComponentType &component);
        
        template <typename ComponentType>
        void removeComponent();
        
        template <typename ComponentType>
        ComponentRef<ComponentType> getComponent();
        
        template <typename ComponentType>
        ComponentRef<ComponentType, const EntityManager> getComponent() const;
        
        template <typename ... C_N>
        std::tuple<ComponentRef<C_N>...> getComponents();
        
        template <typename ... C_N>
        std::tuple<ComponentRef<const C_N, const EntityManager>...> getComponents() const;
        
        template <typename Component>
        bool hasComponent() const;
        
        template <typename C1, typename C2, typename ... C_N>
        bool hasComponent() const;

		template <typename Component>
		bool hasComponentOr() const;

		template <typename C1, typename C2, typename ... C_N>
		bool hasComponentOr() const;
        
        template <typename C_1, typename ... C_N>
        void unpackComponents(ComponentRef<C_1> &c1, ComponentRef<C_N> & ... cn);
        
    public:
        static const ID INVALID;
        
    private:
        EntityManager *m_manager = nullptr;
        ID m_id = INVALID;
    };
    
    class EntityManager
    {
    public:
        typedef std::bitset<MAX_COMPONENTS> ComponentMask;
        
        template <class Delegate, bool All = false>
        class ViewIterator : public std::iterator<std::input_iterator_tag, Entity::ID> {
        public:
            Delegate &operator ++() {
                ++i_;
                next();
                return *static_cast<Delegate*>(this);
            }
            bool operator == (const Delegate& rhs) const { return i_ == rhs.i_; }
            bool operator != (const Delegate& rhs) const { return i_ != rhs.i_; }
            Entity operator * () { return Entity(manager_, manager_->createId(i_)); }
            const Entity operator * () const { return Entity(manager_, manager_->createId(i_)); }
            
        protected:
            ViewIterator(EntityManager *manager, uint32_t index)
            : manager_(manager), i_(index), capacity_(manager_->capacity()), free_cursor_(~0UL) {
                if (All) {
                    std::sort(manager_->m_free_list.begin(), manager_->m_free_list.end());
                    free_cursor_ = 0;
                }
            }
            ViewIterator(EntityManager *manager, const ComponentMask mask, uint32_t index)
            : manager_(manager), mask_(mask), i_(index), capacity_(manager_->capacity()), free_cursor_(~0UL) {
                if (All) {
                    std::sort(manager_->m_free_list.begin(), manager_->m_free_list.end());
                    free_cursor_ = 0;
                }
            }
            
            void next() {
                while (i_ < capacity_ && !predicate()) {
                    ++i_;
                }
                
                if (i_ < capacity_) {
                    Entity entity = manager_->get(manager_->createId(i_));
                    static_cast<Delegate*>(this)->next_entity(entity);
                }
            }
            
            inline bool predicate() {
                return (All && valid_entity()) || (manager_->m_entity_component_mask[i_] & mask_) == mask_;
            }
            
            inline bool valid_entity() {
                const std::vector<uint32_t> &free_list = manager_->m_free_list;
                if (free_cursor_ < free_list.size() && free_list[free_cursor_] == i_) {
                    ++free_cursor_;
                    return false;
                }
                return true;
            }
            
            EntityManager *manager_ = nullptr;
            ComponentMask mask_;
            uint32_t i_ = 0;
            size_t capacity_ = 0;
            size_t free_cursor_ = 0;
        };
        
        template <bool All>
        class BaseView {
        public:
            class Iterator : public ViewIterator<Iterator, All> {
            public:
                Iterator(EntityManager *manager,
                         const ComponentMask mask,
                         uint32_t index) : ViewIterator<Iterator, All>(manager, mask, index) {
                    ViewIterator<Iterator, All>::next();
                }
                
                void next_entity(Entity &entity) {}
            };
            
            Iterator begin() { return Iterator(manager_, mask_, 0); }
            Iterator end() { return Iterator(manager_, mask_, uint32_t(manager_->capacity())); }
            const Iterator begin() const { return Iterator(manager_, mask_, 0); }
            const Iterator end() const { return Iterator(manager_, mask_, manager_->capacity()); }
            
        private:
            friend class EntityManager;
            
            explicit BaseView(EntityManager *manager) : manager_(manager) { mask_.set(); }
            BaseView(EntityManager *manager, ComponentMask mask) :
            manager_(manager), mask_(mask) {}
            
            EntityManager *manager_ = nullptr;
            ComponentMask mask_;
        };
        
        template <bool All, typename ... Components>
        class TypedView : public BaseView<All> {
        public:
            template <typename T> struct identity { typedef T type; };
            
            void each(typename identity<std::function<void(Entity entity, Components&...)>>::type f) {
                for (auto it : *this)
                    f(it, *(it.template getComponent<Components>().get())...);
            }
            
        private:
            friend class EntityManager;
            
            explicit TypedView(EntityManager *manager) : BaseView<All>(manager) {}
            TypedView(EntityManager *manager, ComponentMask mask) : BaseView<All>(manager, mask) {}
        };
        
        template <typename ... Components> using View = TypedView<false, Components...>;
        typedef BaseView<true> DebugView;
        
        template <typename ... Components>
        class UnpackingView {
        public:
            struct Unpacker {
                explicit Unpacker(ComponentRef<Components> & ... handles) :
                handles(std::tuple<ComponentRef<Components> & ...>(handles...)) {}
                
                void unpack(Entity &entity) const {
                    unpack_<0, Components...>(entity);
                }
                
                
            private:
                template <int N, typename C>
                void unpack_(Entity &entity) const {
                    std::get<N>(handles) = entity.getComponent<C>();
                }
                
                template <int N, typename C0, typename C1, typename ... Cn>
                void unpack_(Entity &entity) const {
                    std::get<N>(handles) = entity.getComponent<C0>();
                    unpack_<N + 1, C1, Cn...>(entity);
                }
                
                std::tuple<ComponentRef<Components> & ...> handles;
            };
            
            
            class Iterator : public ViewIterator<Iterator> {
            public:
                Iterator(EntityManager *manager,
                         const ComponentMask mask,
                         uint32_t index,
                         const Unpacker &unpacker) : ViewIterator<Iterator>(manager, mask, index), unpacker_(unpacker) {
                    ViewIterator<Iterator>::next();
                }
                
                void next_entity(Entity &entity) {
                    unpacker_.unpack(entity);
                }
                
            private:
                const Unpacker &unpacker_;
            };
            
            
            Iterator begin() { return Iterator(manager_, mask_, 0, unpacker_); }
            Iterator end() { return Iterator(manager_, mask_, static_cast<uint32_t>(manager_->capacity()), unpacker_); }
            const Iterator begin() const { return Iterator(manager_, mask_, 0, unpacker_); }
            const Iterator end() const { return Iterator(manager_, mask_, static_cast<uint32_t>(manager_->capacity()), unpacker_); }
            
            
        private:
            friend class EntityManager;
            
            UnpackingView(EntityManager *manager, ComponentMask mask, ComponentRef<Components> & ... handles) :
            manager_(manager), mask_(mask), unpacker_(handles...) {}
            
            EntityManager *manager_ = nullptr;
            ComponentMask mask_;
            Unpacker unpacker_;
        };
        
    public:
        EntityManager();
        ~EntityManager();
        
        void setEventSystem(EventSystem *event_system);
        
        void clear();
        
        EventSystem *getEventSystem() {
            return m_event_system;
        }
        
        inline bool valid(Entity::ID id) const;
        
        inline size_t size() const;
        
        inline size_t capacity() const;
        
        inline Entity get(Entity::ID id);
        
        inline Entity::ID createId(uint32_t index) const;
        
        Entity create();
        
        void destroy(Entity::ID id);
        
        void destroyNoNotify(Entity::ID id);
        
        template <typename ComponentType>
        BaseComponent::Family component_family() const;
        
        template <typename ComponentType>
        bool hasComponent(Entity::ID id) const;
        
        template <typename C_1, typename C_2, typename ...C_N>
        bool hasComponent(Entity::ID id) const;

		template <typename ComponentType>
		bool hasComponentOr(Entity::ID id) const;

		template <typename C_1, typename C_2, typename ...C_N>
		bool hasComponentOr(Entity::ID id) const;
        
        template <typename ComponentType>
        ComponentRef<ComponentType> getComponent(Entity::ID id);
        
        template <typename ComponentType>
        const ComponentRef<ComponentType, const EntityManager> getComponent(Entity::ID id) const;
        
        template <typename ... C_N>
        std::tuple<ComponentRef<C_N>...> getComponents(Entity::ID id);
        
        template <typename ... C_N>
        std::tuple<ComponentRef<const C_N, const EntityManager>...> getComponents(Entity::ID id) const;
        
		template <typename Component>
		ComponentRef<Component> assignComponent(Entity::ID id);

		template <typename Component>
		ComponentRef<Component> assignComponentFrom(Entity::ID id, const Component &source);
        
        template <typename ComponentType>
        void removeComponent(Entity::ID id);
        
        template <typename ComponentType>
        ComponentType *getComponentPtr(Entity::ID id);
        
        template <typename ComponentType>
        const ComponentType *getComponentPtr(Entity::ID id) const;
        
        template <typename ComponentType>
        ComponentMask componentMask();
        
        template <typename ComponentType>
        ComponentMask componentMask(const ComponentRef<ComponentType> &ref);
        
        template <typename C_1, typename C_2, typename ... C_N>
        ComponentMask componentMask();
        
        template <typename C_1, typename ... C_N>
        ComponentMask componentMask(const ComponentRef<C_1> &c1, const ComponentRef<C_N> &... cn);
        
        inline ComponentMask componentMask(Entity::ID id);
        
        template <typename ComponentType>
        inline Pool<ComponentType> *accommodateComponent();
        
        inline void accommodateEntity(uint32_t index);
        
        template <typename ComponentType>
        void unpack(Entity::ID id, ComponentRef<ComponentType> &a);
        
        template <typename C_1, typename ... C_N>
        void unpack(Entity::ID id, ComponentRef<C_1> &c1, ComponentRef<C_N> &... cn);
        
        template <typename ... C_N>
        View<C_N...> entitiesWithComponents();
        
        template <typename T> struct identity { typedef T type; };
        
        template <typename ... C_N>
        void each(typename identity<std::function<void(Entity entity, C_N&...)>>::type f);
        
    private:
        friend class Entity;
        template <typename ComponentType, typename Container>
        friend class ComponentRef;
        
    private:
        uint32_t m_index_counter = 0;
        std::vector<BasePool*> m_component_pools;
        std::vector<ComponentMask> m_entity_component_mask;
        std::vector<uint32_t> m_entity_version;
        std::vector<uint32_t> m_free_list;
        EventSystem *m_event_system = nullptr;
    };
    
    
    /****************************************************/
    template <typename ComponentType, typename ContainerType>
    class ComponentRef
    {
    public:
        ComponentRef() : m_container(nullptr) {}
        
        bool valid() const;
        operator bool() const;
        
        ComponentType *operator -> ();
        const ComponentType *operator -> () const;
        
        ComponentType *get();
        const ComponentType *get() const;
        
        bool operator == (const ComponentRef<ComponentType, ContainerType> &other) const {
            return m_container == other.m_container && m_id == other.m_id;
        }
        
        bool operator != (const ComponentRef<ComponentType, ContainerType> &other) const {
            return !(*this == other);
        }
        
        /**
         * Remove the component from its entity and destroy it.
         */
        void remove();
        
    private:
        friend class EntityManager;
        
        ComponentRef(ContainerType *container, Entity::ID id) :
        m_container(container), m_id(id) {}
        
        ContainerType *m_container = nullptr;
        Entity::ID m_id;
    };   
}

#endif

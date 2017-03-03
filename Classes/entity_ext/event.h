#ifndef _EVENT_H_
#define _EVENT_H_

#include <cstdint>
#include <cstddef>
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <utility>
#include "entity.h"

namespace ECS {
	class EventBase
	{
	public:
		enum Priority
		{
			PRIORITY_CRITICAL,
			PRIORITY_HIGH,
			PRIORITY_NORMAL,
			PRIORITY_LOW,
			PRIORITY_TRIVIAL,
			PRIORITY_COUNT
		};

	public:
		virtual ~EventBase() {}

		virtual std::string getName() const = 0;
	};

	class EventConsumble : public EventBase
	{
	public:
		virtual bool isConsumed() = 0;

		virtual void consume() = 0;
	};

	class AbstractEventConsumble : public EventConsumble
	{
	public:
		void consume() { m_consumed = true; }

		bool isConsumed() { return m_consumed; }

		void reset() { m_consumed = false; }

	protected:
		bool m_consumed = false;
	};

	class EventReceiverBase
	{
	public:
		virtual ~EventReceiverBase() {}
        
		virtual void process(Entity entity, EventBase *evt) = 0;
		virtual bool isValidFor(Entity entity) const = 0;
		virtual EventBase::Priority getPriority() const = 0;
	};

	typedef std::shared_ptr<EventReceiverBase> EventReceiverPtr;

	class RequireComponentBase
	{
	public:
		virtual ~RequireComponentBase() {}

		virtual bool isValidFor(Entity entity) const = 0;
	};

	// for the dynamic configuration based requirements
	class RequireComponentDecorateBase : public RequireComponentBase
	{
	public:
		RequireComponentDecorateBase(RequireComponentDecorateBase *decorated)
			: m_base(decorated)
		{
		}

        bool isValidFor(Entity entity) const;

	private:
		std::unique_ptr<RequireComponentBase> m_base;
	};

	template <typename ... Components>
	class RequireComponentOr : public RequireComponentDecorateBase {
	public:
		RequireComponentOr(RequireComponentDecorateBase *base)
			: RequireComponentDecorateBase(base) {
		}

        bool isValidFor(Entity entity) const {
            return entity.hasComponentOr<Components ...>() || RequireComponentDecorateBase::isValidFor(entity);
        }
	};
    
	template <typename ... Components>
	class RequireComponent : public RequireComponentBase
	{
	public:
        bool isValidFor(Entity entity) const {
            return entity.hasComponent<Components ...>();
        }
	};

	template <typename E>
	class EventReceiver : public EventReceiverBase
	{
	public:
		EventReceiver(EventBase::Priority priority, 
			std::function<void(Entity entity, E &evt)> call, RequireComponentBase *requirement = nullptr)
			: m_priority(priority)
			, m_call(call)
			, m_requirement(requirement)
		{
		}

		~EventReceiver() {

		}

        virtual void process(Entity entity, EventBase *evt) {
            if (m_call && evt)
                m_call(entity, *(static_cast<E*>(evt)));
        }

        virtual bool isValidFor(Entity entity) const {
            if (m_requirement)
                return m_requirement->isValidFor(entity);
            return true;
        }

		virtual EventBase::Priority getPriority() const
		{
			return m_priority;
		}

	private:
		EventBase::Priority m_priority;
		std::function<void(Entity entity, E &evt)> m_call;
		std::unique_ptr<RequireComponentBase> m_requirement;
	};
    
       
	class EventSystem {
	public:
		EventSystem();
		~EventSystem();
        
		template <typename Receiver, typename E>
		void registerEventReceiver(EventBase::Priority priority, Receiver &receiver, E &e, void(Receiver::*receive)(Entity entity, E &evt), RequireComponentBase * requirement = nullptr)
		{
			auto wrapper = EventReceiverPtr(static_cast<EventReceiverBase*>(new EventReceiver<E>(priority,
				std::bind(receive, &receiver, std::placeholders::_1, std::placeholders::_2), requirement)));
			addReceiver((uint32_t)typeid(E).hash_code(),(uint32_t)typeid(Receiver).hash_code(), wrapper);
		}

		template <typename Receiver, typename E>
		void registerEventReceiver(EventBase::Priority priority, Receiver &receiver, E &e, void(*func)(Entity entity, E &e), RequireComponentBase * requirement = nullptr)
		{
			auto wrapper = EventReceiverPtr(static_cast<EventReceiverBase*>(new EventReceiver<E>(priority,
				func, requirement)));
			addReceiver(typeid(E).hash_code(), typeid(Receiver).hash_code(), wrapper);
		}

		template <typename Receiver, typename E>
		void unregisterEventReceiver()
		{
			removeReceiver(typeid(E).hash_code(), typeid(Receiver).hash_code());
		}

		template <typename E>
        void send(Entity entity, E &e) {
            sendInner(entity, (uint32_t)typeid(e).hash_code(), &e);
        }

	private:
		typedef std::vector<EventReceiverPtr> ReceiverList;
		typedef std::unordered_map<uint32_t, EventReceiverPtr> ReceiverStore;

	private:
		void addReceiver(uint32_t familyEvent, uint32_t familyReceiver, EventReceiverPtr receiver);

		void removeReceiver(uint32_t familyEvent, uint32_t familyReceiver);

		void sendInner(Entity entity, uint32_t familyEvent, EventBase *evt);

		void getValidReceiversFor(Entity entity, uint32_t familyEvent, ReceiverList &list);

		void sendConsumed(Entity entity, AbstractEventConsumble *evt, ReceiverList &list);

		void sendNormal(Entity entity, EventBase *evt, ReceiverList &list);

	private:
		std::unordered_map<uint32_t, ReceiverStore> m_receivers;
	};
}
#endif

#ifndef _EVENT_INTERNAL_H_
#define _EVENT_INTERNAL_H_

#include "event.h"

namespace ECS {
	class BeforeDeactiveComponent : public EventBase
	{
	public:
		static BeforeDeactiveComponent *getInstance();

		std::string getName() const;
	};

	class BeforeEntityCreated : public EventBase
	{
	public:
		static BeforeEntityCreated *getInstance();

		std::string getName() const;
	};

	class AfterEntityCreated : public EventBase
	{
	public:
		static AfterEntityCreated * getInstance();

		std::string getName() const;
	};

	class BeforeEntityRun : public EventBase {
	public:
		static BeforeEntityRun * getInstance();

		std::string getName() const;
	};

	class BeforeRemoveEntity : public EventBase
	{
	public:
		static BeforeRemoveEntity *getInstance();

		std::string getName() const;
	};

	class BeforeRemoveComponent : public EventBase
	{
	public:
		static BeforeRemoveComponent *getInstance();

		std::string getName() const;
	};

	class OnActivatedComponent : public EventBase
	{
	public:
		static OnActivatedComponent *getInstance();

		std::string getName() const;
	};

	class OnAddedComponent : public EventBase
	{
	public:
		static OnAddedComponent *getInstance();

		std::string getName() const;
	};

	class OnChangedComponent : public EventBase
	{
	public:
		static OnChangedComponent *getInstance();

		std::string getName() const;
	};
}

#endif

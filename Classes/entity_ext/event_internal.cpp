/*

the event is the core for the interaction between the systems

Author:  yukun tan (codecraft@163.com)

(C) Copyright tanyukun 2015. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears
in all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.

*/
#include "event_internal.h"

namespace ECS {
	BeforeEntityCreated * BeforeEntityCreated::getInstance() {
		static BeforeEntityCreated *sInstance = nullptr;
		if (!sInstance)
		{
			sInstance = new BeforeEntityCreated();
		}
		return sInstance;
	}

	std::string BeforeEntityCreated::getName() const {
		return std::string("BeforeEntityCreated");
	}

	BeforeRemoveEntity * BeforeRemoveEntity::getInstance() {
		static BeforeRemoveEntity *sInstance = nullptr;
		if (!sInstance) {
			sInstance = new BeforeRemoveEntity();
		}
		return sInstance;
	}

	std::string BeforeRemoveEntity::getName() const {
		return std::string("BeforeRemoveEntity");
	}

	AfterEntityCreated * AfterEntityCreated::getInstance() {
		static AfterEntityCreated *sInstance = nullptr;
		if (!sInstance) {
			sInstance = new AfterEntityCreated();
		}
		return sInstance;
	}

	std::string AfterEntityCreated::getName() const {
		return std::string("AfterEntityCreated");
	}

	BeforeDeactiveComponent *BeforeDeactiveComponent::getInstance()
	{
		static BeforeDeactiveComponent *sInstance = nullptr;
		if (!sInstance)
		{
			sInstance = new BeforeDeactiveComponent();
		}
		return sInstance;
	}

	std::string BeforeDeactiveComponent::getName() const
	{
		return std::string("BeforeDeactiveComponent");
	}

	BeforeRemoveComponent *BeforeRemoveComponent::getInstance()
	{
		static BeforeRemoveComponent *sInstance = nullptr;
		if (!sInstance)
		{
			sInstance = new BeforeRemoveComponent();
		}
		return sInstance;
	}

	std::string BeforeRemoveComponent::getName() const
	{
		return std::string("BeforeRemoveComponent");
	}

	OnActivatedComponent * OnActivatedComponent::getInstance()
	{
		static OnActivatedComponent *sInstance = nullptr;
		if (!sInstance)
		{
			sInstance = new OnActivatedComponent();
		}
		return sInstance;
	}

	std::string OnActivatedComponent::getName() const
	{
		return std::string("OnActivatedComponent");
	}

	OnAddedComponent * OnAddedComponent::getInstance()
	{
		static OnAddedComponent *sInstance = nullptr;
		if (!sInstance)
		{
			sInstance = new OnAddedComponent();
		}
		return sInstance;
	}

	std::string OnAddedComponent::getName() const
	{
		return std::string("OnAddedComponent");
	}

	OnChangedComponent * OnChangedComponent::getInstance()
	{
		static OnChangedComponent *sInstance = nullptr;
		if (!sInstance)
		{
			sInstance = new OnChangedComponent();
		}
		return sInstance;
	}

	std::string OnChangedComponent::getName() const {
		return std::string("OnChangedComponent");
	}

	BeforeEntityRun * BeforeEntityRun::getInstance() {
		static BeforeEntityRun *sInstance = nullptr;
		if (!sInstance) {
			sInstance = new BeforeEntityRun();
		}
		return sInstance;
	}

	std::string BeforeEntityRun::getName() const {
		return std::string("BeforeEntityRun");
	}
};

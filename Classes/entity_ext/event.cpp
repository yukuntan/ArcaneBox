/*

the event is the core for the interaction between the systems

Author:  yukun tan (codecraft@163.com)

(C) Copyright tanyukun 2015. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears
in all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.

*/
#include "event.h"

namespace ECS
{
	bool RequireComponentDecorateBase::isValidFor(Entity entity) const {
		if (m_base)
			return m_base->isValidFor(entity);
		return false;
	}

	EventSystem::EventSystem() {
	}

	EventSystem::~EventSystem() {
	}

	void EventSystem::sendConsumed(Entity entity, AbstractEventConsumble *evt, ReceiverList &list) {
		for (auto receiver : list) {
			if (receiver)
				receiver->process(entity, evt);

			if (evt->isConsumed())
				return;
		}
	}

	void EventSystem::sendNormal(Entity entity, EventBase *evt, ReceiverList &list) {
		for (auto receiver : list) {
			if (receiver)
				receiver->process(entity, evt);
		}
	}

	void EventSystem::addReceiver(uint32_t familyEvent, uint32_t familyReceiver, EventReceiverPtr receiver) {
		auto it = m_receivers.find(familyEvent);

		if (it == m_receivers.end()) {
			m_receivers.insert(std::make_pair(familyEvent, ReceiverStore()));
			it = m_receivers.find(familyEvent);
		}

		if (it != m_receivers.end()) {
			it->second[familyReceiver] = receiver;
		}
	}

	void EventSystem::removeReceiver(uint32_t familyEvent, uint32_t familyReceiver) {
		auto it = m_receivers.find(familyEvent);

		if (it != m_receivers.end()) {
			it->second.erase(familyReceiver);
		}
	}

	void EventSystem::sendInner(Entity entity, uint32_t familyEvent, EventBase * evt) {
		ReceiverList list;
		getValidReceiversFor(entity, familyEvent, list);
		auto consumble = dynamic_cast<AbstractEventConsumble*>(evt);
		if (consumble) {
			consumble->reset();
			sendConsumed(entity, consumble, list);
		}
		else {
			sendNormal(entity, evt, list);
		}
	}

	class CompairReceiver {
	public:
		bool operator () (EventReceiverPtr r1, EventReceiverPtr r2) {
			return r2->getPriority() > r1->getPriority();
		}
	};

	void EventSystem::getValidReceiversFor(Entity entity, uint32_t familyEvent, ReceiverList & list) {
		auto it = m_receivers.find(familyEvent);
		if (it != m_receivers.end()) {
			for (auto &receiver : it->second) {
				if (receiver.second.get() && receiver.second->isValidFor(entity))
					list.push_back(receiver.second);
			}
		}
		std::sort(list.begin(), list.end(), CompairReceiver());
	}
}

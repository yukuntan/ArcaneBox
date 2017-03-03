/*

the entity builder is used for creating prefabs, and duplicate the instances.

Author:  yukun tan (codecraft@163.com)

(C) Copyright tanyukun 2015. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears
in all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.

*/

#include "entity.h"
#include "event_internal.h"
#include "builder.h"
#include "serialize.h"

namespace ECS {
	Builder::Builder(EntityManager *dest)
		: m_dest(dest) {
		m_source = std::unique_ptr<EntityManager>(new EntityManager());
	}

	Builder::~Builder() {
	}

	void Builder::loadPrefab(const char *filename, std::string &content) {
		if (content.size() == 0)
			return;

		Entity entity = m_source->create();
		rpocojson::json_value value;
		rpocojson::parse(content, value);
		SerializeList list;

		for (auto &item : *value.map()) {
			auto name = item.first;
			auto info = SerializerManager::getByName(name.c_str());
			if (!info) {
				// TODO... LOG HERE
				continue;
			}
            std::istringstream stream(rpocojson::to_json(item.second));
			info->assign(entity);
			info->parse(stream, entity);
			list.push_back(info);
		}
		m_template.insert(std::make_pair(filename, std::make_tuple(entity, list)));
	}

	bool Builder::hasPrefab(const char * filename) {
		auto it = m_template.find(filename);
		if (it == m_template.end())
			return false;
		return true;
	}

	Entity Builder::clone(const char *prefab) {
		auto dest = clonePrefab(prefab);
		afterCreated(dest);
		return dest;
	}

	Entity Builder::extendPrefab(const char *prefab, std::map<std::string, std::string> &extends) {
		auto dest = clonePrefab(prefab);
		if (!dest.valid())
			return dest;
		
		for (auto &item : extends) {
			auto name = item.first;
			auto info = SerializerManager::getByName(name.c_str());
			if (!info) {
				// TODO... LOG HERE
				continue;
			}
			std::istringstream stream(item.second);
			info->parse(stream, dest);
		}
		afterCreated(dest);
		return dest;
	}

	Entity Builder::extendPrefabNotNotify(const char *prefab, std::map<std::string, std::string> &extends) {
		auto dest = clonePrefab(prefab);
		if (!dest.valid())
			return dest;

		for (auto &item : extends) {
			auto name = item.first;
			auto info = SerializerManager::getByName(name.c_str());
			if (!info) {
				// TODO... LOG HERE
				continue;
			}
			std::istringstream stream(item.second);
			info->parse(stream, dest);
		}
		return dest;
	}
	
	Entity Builder::clonePrefab(const char *prefab) {
		auto it = m_template.find(prefab);
		if (it == m_template.end())
			return Entity(m_dest, Entity::INVALID);

		Entity dest = m_dest->create();

		auto event_system = m_dest->getEventSystem();
		if (event_system)
			event_system->send<BeforeEntityCreated>(dest, *BeforeEntityCreated::getInstance());
		auto source = std::get<0>(it->second);
		auto &list = std::get<1>(it->second);
		for (auto info : list) {
			info->clone(source, dest);
		}
		return dest;
	}

	void Builder::afterCreated(Entity dest) {
		auto event_system = m_dest->getEventSystem();
		if (event_system) {
			event_system->send<AfterEntityCreated>(dest, *AfterEntityCreated::getInstance());
			event_system->send<BeforeEntityRun>(dest, *BeforeEntityRun::getInstance());
		}
	}
}

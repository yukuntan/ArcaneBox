#ifndef _RPOCO_BUILDER_H_
#define _RPOCO_BUILDER_H_

#include <unordered_map>
#include <map>

#include "entity.h"

/**
* extended by tyk
* for the file based reflection
*/
namespace ECS {
	class Entity;
	class EntityManager;
	class Serializer;
	class Builder {
	public:
		// @param dest the manager clone to
		Builder(EntityManager *dest);
		~Builder();

        void loadPrefab(const char *filename, std::string &content);

		bool hasPrefab(const char *filename);

		// clone an entity from the prefabricate
		Entity clone(const char *prefab);
		// clone the base entity and extend the attributes of the entity
		Entity extendPrefab(const char *prefab, std::map<std::string,std::string> &extends);
		// for preload, just save for templates
		Entity extendPrefabNotNotify(const char *prefab, std::map<std::string, std::string> &extends);

        // extend the prefab existed with components, components here must be created
		template <typename ...Components>
		Entity extend(const char *prefab, Components & ...coms) {
			auto dest = clonePrefab(prefab);
			if (dest.valid()) {
				copy(dest, coms ...);
				afterCreated(dest);
			}
			return dest;
		}

        // extend the prefab existed with the component, component here must not be created
		template <typename Component>
		Entity extendAssign(const char *prefab, Component &com) {
			auto dest = clonePrefab(prefab);
			if (dest.valid()) {
				auto com_self = dest.assignComponent<Component>().get();
				*com_self = com;
				afterCreated(dest);
			}
			return dest;
		}

        // extend the prefab existed with components, components here must not be created
		template <typename ...Components>
		Entity extendAssigns(const char *prefab, Components & ...coms) {
			auto dest = clonePrefab(prefab);
			if (dest.valid()) {
				assign(dest, coms ...);
				afterCreated(dest);
			}
			return dest;
		}

	protected:
		template <typename Component>
		void copy(Entity dest, Component &com) {
			auto com_self = dest.getComponent<Component>().get();
			*com_self = com;
		}

		template <typename C1, typename ... C_N>
		void copy(Entity dest, C1 &c1, C_N & ... cn) {
			copy(dest, c1);
			copy(dest, cn ...);
		}

		template <typename Component>
		void assign(Entity dest, Component &com) {
			auto com_self = dest.assignComponent<Component>().get();
			*com_self = com;
		}

		template <typename C1, typename ... C_N>
		void assign(Entity dest, C1 &c1, C_N & ... cn) {
			assign(dest, c1);
			assign(dest, cn ...);
		}

	protected:
		Entity clonePrefab(const char *prefab);

		void afterCreated(Entity dest);

	private:
		typedef std::vector<Serializer*> SerializeList;
		std::unique_ptr<EntityManager> m_source;
		// file based template
		std::unordered_map<std::string, std::tuple<Entity, SerializeList>> m_template;
		EntityManager *m_dest;
	};
}

#endif

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <cstddef>

namespace ECS {
	/**
	* the base component :
    * for generating family
    */
	class BaseComponent {
	public:
		typedef size_t Family;

	public:
		virtual ~BaseComponent() {}

		static Family s_family_counter;
	};

	template <typename Derived>
	class Component : public BaseComponent {
	public:
		static Family family();
	};

	template <typename Derived>
	BaseComponent::Family Component<Derived>::family() {
		static Family family = s_family_counter++;
		return family;
	}
}

#endif

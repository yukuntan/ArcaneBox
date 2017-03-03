#ifndef _COMPONENT_SYSTEM_H
#define _COMPONENT_SYSTEM_H

/*

the component system process special components 

Author:  yukun tan (codecraft@163.com)

(C) Copyright tanyukun 2015. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears
in all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.

*/

namespace ECS {
	class EntityManager;
	class Context;
	class ComponentSystem
	{
	public:
		virtual ~ComponentSystem() {}

		virtual void initialize(Context *context) = 0;

		virtual void preBegin() = 0;

		virtual void postBegin() = 0;

		virtual void preSave() = 0;

		virtual void postSave() = 0;

		virtual void shutdown() = 0;
	};

	class BaseComponentSystem : public ComponentSystem
	{
	public:
		void initialize(Context *context) {}

		void preBegin() {}

		void postBegin() {}

		void preSave() {}

		void postSave() {}

		void shutdown() {}
	};

	class UpdateSubscriberSystem : public ComponentSystem
	{
	public:
		virtual void update(float delta) = 0;
	};
}
#endif

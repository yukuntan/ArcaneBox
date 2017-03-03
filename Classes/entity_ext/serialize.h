#ifndef _COMPONENTINFO_H_
#define _COMPONENTINFO_H_

#include "../tool/rpocojson.hpp"
#include "entity.hpp"

namespace ECS {
	class Entity;
	class RequireComponentDecorateBase;
    class Serializer {
	public:
		virtual ~Serializer() {}

		// assign the component to 
		virtual void assign(Entity dest) = 0;
		// add the component to 
		virtual void parse(std::istream &in, Entity dest) = 0;
		// clone the component of the entity to the dest
		virtual void clone(Entity source, Entity dest) = 0;
		// serialize the com
		virtual void serialize(Entity source, const std::string &key) = 0;
		// unserialize the com
		virtual void unserialize(Entity source, const std::string &key) = 0;
	};
    
    class SerializerManager {
    public:
        template <typename ComponentType>
        static void registerByName(const char *name);
        
        static Serializer *getByName(const char *name);
        
        static SerializerManager *getInstance();
        
    private:
        std::unordered_map<std::string, std::unique_ptr<Serializer>> m_infos;
    };
    
    template <typename ComponentType>
	class SerializerImpl : public Serializer {
	public:
		virtual ~SerializerImpl() {}

		void assign(Entity dest);

		void parse(std::istream &in, Entity dest);

		void clone(Entity source, Entity dest);

		void serialize(Entity source, const std::string &key);

		void unserialize(Entity source, const std::string &key);
	};
    
    template <typename ComponentType>
    void SerializerManager::registerByName(const char *name) {
        auto ptr = std::unique_ptr<Serializer>(new SerializerImpl<ComponentType>());
        if (getInstance())
            getInstance()->m_infos.insert(std::make_pair(std::string(name), std::move(ptr)));
    }

	template <typename ComponentType>
	void SerializerImpl<ComponentType>::assign(Entity dest) {
		dest.assignComponent<ComponentType>();
	}

	template <typename ComponentType>
	void SerializerImpl<ComponentType>::parse(std::istream &in, Entity dest) {
		auto com = dest.getComponent<ComponentType>();
		if (com) {
			rpocojson::parse(in, *com.get());
		}
	}

	template <typename ComponentType>
	void SerializerImpl<ComponentType>::clone(Entity source, Entity dest) {
		auto com = source.getComponent<ComponentType>().get();
		dest.assignComponentFrom<ComponentType>(*com);
	}

	template <typename ComponentType>
	void SerializerImpl<ComponentType>::serialize(Entity source, const std::string &key) {
	}

	template <typename ComponentType>
	void SerializerImpl<ComponentType>::unserialize(Entity source, const std::string &key) {
	}

	// register the component serializers
#define REGISTER_COMPONENT(ComponentType) \
    class SerializeRegisterHelper##ComponentType \
	{	\
	public:	\
		SerializeRegisterHelper##ComponentType()	\
		{	\
            ECS::SerializerManager::registerByName<ComponentType>(#ComponentType);	\
		}	\
	};	\
	static SerializeRegisterHelper##ComponentType sHelper##ComponentType;
}

#endif

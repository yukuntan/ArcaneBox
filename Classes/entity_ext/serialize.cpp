/*

the component infomation for creating components dynamically

Author:  yukun tan (codecraft@163.com)

(C) Copyright tanyukun 2017. Permission to copy, use, modify, sell and
distribute this software is granted provided this copyright notice appears
in all copies. This software is provided "as is" without express or implied
warranty, and with no claim as to its suitability for any purpose.

*/

#include "serialize.h"

namespace ECS {
    SerializerManager *SerializerManager::getInstance() {
        static SerializerManager *sInstance = nullptr;
        if (sInstance == nullptr)
            sInstance = new SerializerManager();
        return sInstance;
    }
    
	Serializer * SerializerManager::getByName(const char * name) {
		auto it = getInstance()->m_infos.find(name);
		if (it != getInstance()->m_infos.end())
			return it->second.get();
		return nullptr;
	}
}

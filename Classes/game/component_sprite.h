#ifndef _COMPONENT_SPRITE_H_
#define _COMPONENT_SPRITE_H_

#include "../tool/rpocojson.hpp"


namespace Arcane {
	struct SpriteCom {
		std::map<std::string, std::string> group;

		RPOCO(group);
	};
    
    struct AnimeCom {
        std::map<std::string, std::string> group;
        
        RPOCO(group);
    };
}

#endif

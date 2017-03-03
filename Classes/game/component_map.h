#ifndef _COMPONENT_MAP_H_
#define _COMPONENT_MAP_H_

#include "../tool/rpoco.hpp"

namespace Arcane {

	struct ThumbnailCom {
		std::map<std::string, std::string> group;
		int32_t zorder = 0;

		RPOCO(group, zorder);
	};
}

#endif

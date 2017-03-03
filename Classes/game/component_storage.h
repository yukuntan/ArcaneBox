#ifndef _COMPONENT_STORAGE_H_
#define _COMPONENT_STORAGE_H_

#include "../tool/rpocojson.hpp"

namespace Arcane {
	struct StorageCom {
		int32_t group = -1;
		int32_t key = -1;

		RPOCO(group, key);
	};
}

#endif

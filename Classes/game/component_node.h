#ifndef _COMPONENT_NODE_H_
#define _COMPONENT_NODE_H_

#include "../tool/rpocojson.hpp"

namespace Arcane {
	struct NodeCom {
		// for add to parent node
		std::string node_of_parent;

		RPOCO(node_of_parent);
	};

	struct NodeDefaultCom {
		int32_t invalid = 0;

		RPOCO(invalid);
	};
}

#endif

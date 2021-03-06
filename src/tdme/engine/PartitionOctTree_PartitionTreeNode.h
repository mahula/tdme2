#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/utils/fwd-tdme.h>

using tdme::engine::PartitionOctTree;
using tdme::engine::primitives::BoundingBox;

using std::list;
using std::map;
using std::string;
using std::vector;

/** 
 * Partition oct tree node
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::PartitionOctTree_PartitionTreeNode
{
	// partition size
	float partitionSize;

	// x, y, z position
	int32_t x;
	int32_t y;
	int32_t z;

	// parent node
	PartitionOctTree_PartitionTreeNode* parent { nullptr };

	// node bounding volume
	BoundingBox bv;

	// sub nodes of oct tree nodes
	list<PartitionOctTree_PartitionTreeNode> subNodes;

	// sub nodes of oct tree nodes by partition coordinate, only used in root node
	map<string, PartitionOctTree_PartitionTreeNode*> subNodesByCoordinate;

	// or finally our partition entities
	vector<Entity*> partitionEntities;
};

#include <tdme/utils/PathFinding.h>

#include <algorithm>
#include <string>
#include <map>
#include <stack>

#include <tdme/engine/Transformations.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/PathFindingNode.h>
#include <tdme/utils/PathFindingCustomTest.h>

using std::map;
using std::reverse;
using std::stack;
using std::string;
using std::to_string;

using tdme::engine::Transformations;
using tdme::engine::physics::World;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::utils::PathFindingNode;
using tdme::utils::PathFindingCustomTest;

using tdme::utils::PathFinding;

PathFinding::PathFinding(World* world, PathFindingCustomTest* customtest, int stepsMax, float stepSize, float stepSizeLast, float actorStepUpMax) {
	this->world = world;
	this->customTest = customtest;
	this->end = new PathFindingNode();
	this->actorObv = nullptr;
	this->actorCbv = nullptr;
	this->stepsMax = stepsMax;
	this->stepSize = stepSize;
	this->stepSizeLast = stepSizeLast;
	this->actorStepUpMax = actorStepUpMax;
	this->collisionRigidBodyTypes = 0;
}

PathFinding::~PathFinding() {
	delete end;
	if (customTest != nullptr) delete customTest;
	if (actorObv != nullptr) delete actorObv;
	if (actorCbv != nullptr) delete actorCbv;
}

string PathFinding::toKeyFloat(float value)
{
	string floatString = to_string(value);
	return floatString.substr(0, floatString.length() - 3);
}

void PathFinding::reset() {
	for (auto nodeIt = openNodes.begin(); nodeIt != openNodes.end(); ++nodeIt) {
		delete nodeIt->second;
	}
	openNodes.clear();
	for (auto nodeIt = closedNodes.begin(); nodeIt != closedNodes.end(); ++nodeIt) {
		delete nodeIt->second;
	}
	closedNodes.clear();
}

bool PathFinding::isWalkable(float x, float y, float z, float& height) {
	// determine y height of ground plate of actor bounding volume
	float xHalfExtension = actorCbv->computeDimensionOnAxis(sideVector) / 2.0f;
	float zHalfExtension = actorCbv->computeDimensionOnAxis(forwardVector) / 2.0f;
	float _z = z - zHalfExtension;
	height = -10000.0f;
	Vector3 actorPosition;
	for (int i = 0; i < 2; i++) {
		float _x = x - xHalfExtension;
		for (int j = 0; j < 2; j++) {
			Vector3 actorPositionCandidate;
			if (world->determineHeight(
				collisionRigidBodyTypes,
				actorStepUpMax,
				actorPositionCandidate.set(_x, y, _z),
				actorPosition) == nullptr) {
				return false;
			}
			if (actorPosition.getY() > height) height = actorPosition.getY();
			if (customTest != nullptr && customTest->isWalkable(actorCbv, actorPosition.getX(), actorPosition.getY(), actorPosition.getZ()) == false) {
				return false;
			}
			_x+= xHalfExtension * 2.0f;
		}
		_z+= zHalfExtension * 2.0f;
	}

	// set up correct height
	actorTransformations.getTranslation().set(x, height + 0.1f, z);
	actorTransformations.update();
	actorCbv->fromBoundingVolumeWithTransformations(actorObv, &actorTransformations);

	// check if collides with world
	vector<RigidBody*> collidedRigidBodies;
	return world->doesCollideWith(collisionRigidBodyTypes, actorCbv, collidedRigidBodies) == false;
}

void PathFinding::start(Vector3 startPosition, Vector3 endPosition) {
	// start node
	auto& startXYZ = startPosition.getArray();
	PathFindingNode* start = new PathFindingNode();
	start->x = startXYZ[0];
	start->y = startXYZ[1];
	start->z = startXYZ[2];
	start->costsAll = 0.0f;
	start->costsReachPoint = 0.0f;
	start->costsEstimated = 0.0f;
	start->previousNode = nullptr;
	start->key = toKeyFloat(start->x) + "," + toKeyFloat(start->y) + "," + toKeyFloat(start->z);

	// end node
	auto& endXYZ = endPosition.getArray();
	end->x = endXYZ[0];
	end->y = endXYZ[1];
	end->z = endXYZ[2];
	end->costsAll = 0.0f;
	end->costsReachPoint = 0.0f;
	end->costsEstimated = 0.0f;
	end->previousNode = nullptr;
	end->key = toKeyFloat(end->x) + "," + toKeyFloat(end->y) + "," + toKeyFloat(end->z);

	// set up start node costs
	start->costsEstimated = computeDistance(start, end);
	start->costsAll = start->costsEstimated;

	// put to open nodes
	openNodes[start->key] = start;
}

PathFinding::PathFindingStatus PathFinding::step() {
	// check if there are still open nodes available
	if (openNodes.size() == 0) {
		return PathFindingStatus::PATH_NOWAY;
	}

	// Choose node form open nodes thats least expensive to check its successors
	PathFindingNode* node = nullptr;
	for (auto nodeIt = openNodes.begin(); nodeIt != openNodes.end(); ++nodeIt) {
		if (node == nullptr || nodeIt->second->costsAll < node->costsAll) node = nodeIt->second;
	}

	//
	if (equalsLastNode(node, end)) {
		end->previousNode = node->previousNode;
		return PathFindingStatus::PATH_FOUND;
	} else {
		// Find valid successors
		for (int z = -1; z <= 1; z++)
		for (int x = -1; x <= 1; x++)
		if ((z != 0 || x != 0)  &&
			(Math::abs(x) == 1 && Math::abs(z) == 1) == false) {
			//
			float yHeight;
			float successorX = x * stepSize + node->x;
			float successorZ = z * stepSize + node->z;
			// first node or walkable?
			if (isWalkable(successorX, node->y, successorZ, yHeight) == true) {
				// check if successor node equals previous node / node
				if (equals(node, successorX, yHeight, successorZ)) {
					continue;
				}
				// Add the node to the available sucessorNodes
				PathFindingNode* successorNode = new PathFindingNode();
				successorNode->x = successorX;
				successorNode->z = successorZ;
				successorNode->y = yHeight;
				successorNode->costsAll = 0.0f;
				successorNode->costsReachPoint = 0.0f;
				successorNode->costsEstimated = 0.0f;
				successorNode->previousNode = nullptr;
				successorNode->key = toKeyFloat(successorNode->x) + "," + toKeyFloat(successorNode->y) + "," + toKeyFloat(successorNode->z);
				successorNodes.push(successorNode);
			}
		}
	}

	// Check successor nodes
	while (successorNodes.empty() == false) {
		PathFindingNode* successorNode = successorNodes.top();
		successorNodes.pop();
		auto& successorNodeKey = successorNode->key;

		// Compute successor node's costs by costs to reach nodes point and the computed distance from node to successor node
		float successorCostsReachPoint = node->costsReachPoint + computeDistance(successorNode, node);

		// Find sucessor node in open nodes list
		PathFindingNode* openListNode = nullptr;
		auto openListNodeIt = openNodes.find(successorNodeKey);
		if (openListNodeIt != openNodes.end()) {
			openListNode = openListNodeIt->second;
		}

		// found it in open nodes list
		if (openListNode != nullptr) {
			// is the node from open nodes less expensive, discard successor node
			if (openListNode->costsReachPoint <= successorCostsReachPoint) {
				delete successorNode;
				// discard it
				continue;
			}
		}

		// Find successor node in closed nodes list
		PathFindingNode* closedListNode = nullptr;
		auto closedListNodeIt = closedNodes.find(successorNodeKey);
		if (closedListNodeIt != closedNodes.end()) {
			closedListNode = closedListNodeIt->second;
		}

		// found it in closed nodes list
		if (closedListNode != nullptr) {
			// is the node from closed nodes list less expensive, discard successor node
			if (closedListNode->costsReachPoint <= successorCostsReachPoint) {
				delete successorNode;
				// discard it
				continue;
			}
		}

		// Sucessor node is the node with least cost to this point
		successorNode->previousNode = node;
		successorNode->costsReachPoint = successorCostsReachPoint;
		successorNode->costsEstimated = computeDistance(successorNode, end);
		successorNode->costsAll = successorNode->costsReachPoint + successorNode->costsEstimated;

		// Remove found node from open nodes list, since it was less optimal
		if (openListNode != nullptr) {
			// remove open list node
			delete openListNodeIt->second;
			openNodes.erase(openListNodeIt);
		}

		// Remove found node from closed nodes list, since it was less optimal
		if (closedListNode != nullptr) {
			delete closedListNodeIt->second;
			closedNodes.erase(closedListNodeIt);
		}

		// Add successor node to open nodes list, as we might want to check its successors to find a path to the end
		openNodes[successorNodeKey] = successorNode;
	}

	// node key
	auto& nodeKey = node->key;

	// Remove node from open nodes, as we checked its successors
	openNodes.erase(nodeKey);

	// add node to closed nodes list, as we checked its successors
	closedNodes[nodeKey] = node;

	//
	return PathFindingStatus::PATH_STEP;
}

bool PathFinding::findPath(BoundingVolume* actorObv, Transformations* actorTransformations, const Vector3& endPosition, const uint32_t collisionRigidBodyTypes, vector<Vector3>& path) {
	// clear path
	path.clear();

	//
	this->collisionRigidBodyTypes = collisionRigidBodyTypes;

	// init path finding
	this->actorObv = actorObv->clone();
	this->actorCbv = actorObv->clone();
	this->actorTransformations.fromTransformations(actorTransformations);

	// positions
	Vector3 startPosition;
	startPosition.set(this->actorTransformations.getTranslation());
	Vector3 endPositionComputed;
	endPositionComputed.set(endPosition);

	// check if target is reachable
	float endYHeight = 0.0f;
	if (isWalkable(
			endPositionComputed.getX(),
			endPositionComputed.getY(),
			endPositionComputed.getZ(),
			endYHeight
		) == false) {
		//
		return false;
	} else {
		endPositionComputed.setY(endYHeight);
	}

	// otherwise start path finding
	start(startPosition, endPositionComputed);

	// do the steps
	bool done = false;
	bool success = false;
	int stepIdx;
	for (stepIdx = 0; done == false && stepIdx < stepsMax; stepIdx++) {
		PathFindingStatus status = step();
		switch(status) {
			case PATH_STEP:
				{
					break;
				}
			case PATH_NOWAY:
				{
					done = true;
					break;
				}
			case PATH_FOUND:
				{
					for (PathFindingNode* node = end; node != nullptr; node = node->previousNode) {
						path.push_back(Vector3(node->x, node->y, node->z));
					}
					reverse(path.begin(), path.end());
					if (path.size() > 0) path.erase(path.begin());
					done = true;
					success = true;
					break;
				}
		}
	}

	// delete old actor obv, cbv
	delete this->actorObv;
	delete this->actorCbv;

	// unset
	this->actorObv = nullptr;
	this->actorCbv = nullptr;

	// reset
	reset();

	// return success
	return success;
}
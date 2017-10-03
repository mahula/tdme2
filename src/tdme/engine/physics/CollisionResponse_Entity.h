// Generated from /tdme/src/tdme/engine/physics/CollisionResponse.java

#pragma once

#include <vector>

#include <fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/utils/_Console.h>

using std::vector;

using tdme::engine::physics::CollisionResponse_Entity;
using tdme::math::Vector3;
using tdme::utils::_Console;

/** 
 * Collision Response Entity
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::physics::CollisionResponse_Entity final
{
public: /* protected */
	float distance {  };
	Vector3 normal {  };
	vector<Vector3> hitPoints {  };

public:

	/** 
	 * @return distance
	 */
	inline float getDistance() {
		return distance;
	}

	/** 
	 * Set distance
	 * @param distance
	 */
	inline void setDistance(float distance) {
		this->distance = distance;
	}

	/** 
	 * @return penetration
	 */
	inline float getPenetration() {
		return -distance;
	}

	/** 
	 * @return normal
	 */
	inline Vector3& getNormal() {
		return normal;
	}

	/** 
	 * Adds a hit point
	 * @param hit point
	 */
	inline void addHitPoint(const Vector3& hitPoint) {
		for (auto i = 0; i < hitPoints.size(); i++) {
			if (hitPoints.at(i).equals(hitPoint, 0.1f))
				return;
		}
		hitPoints.push_back(hitPoint);
	}

	/** 
	 * @return hit points count
	 */
	inline int32_t getHitPointsCount() {
		return hitPoints.size();
	}

	/** 
	 * Get hit point of given index 
	 * @param i
	 * @return hit point for given hit points index
	 */
	inline Vector3& getHitPointAt(int32_t i) {
		return hitPoints.at(i);
	}

	/**
	 * Public constructor
	 */
	inline CollisionResponse_Entity() {
	}
};

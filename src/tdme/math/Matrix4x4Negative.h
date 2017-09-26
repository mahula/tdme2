// Generated from /tdme/src/tdme/math/Matrix4x4Negative.java

#pragma once

#include <fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Matrix4x4.h>

using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/** 
 * Class to check if matrix is negative
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::math::Matrix4x4Negative
{
private:
	Vector3 xAxis {  };
	Vector3 yAxis {  };
	Vector3 zAxis {  };
	Vector3 tmpAxis {  };

public:

	/** 
	 * Check if matrix is negative
	 * @param matrix
	 * @return negative
	 */
	inline bool isNegative(Matrix4x4& matrix) {
		auto& transformationsMatrixData = matrix.getArray();
		xAxis.set(transformationsMatrixData[0], transformationsMatrixData[1], transformationsMatrixData[2]);
		yAxis.set(transformationsMatrixData[4], transformationsMatrixData[5], transformationsMatrixData[6]);
		zAxis.set(transformationsMatrixData[8], transformationsMatrixData[9], transformationsMatrixData[10]);
		return Vector3::computeDotProduct(Vector3::computeCrossProduct(&xAxis, &yAxis, &tmpAxis), &zAxis) < 0.0f;
	}

	/**
	 * Public constructor
	 */
	inline Matrix4x4Negative() {
	}
};

#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/subsystems/object/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/fwd-tdme.h>

using std::map;
using std::vector;
using std::wstring;

using tdme::utils::FloatBuffer;
using tdme::utils::ShortBuffer;
using tdme::engine::Engine;
using tdme::engine::model::Group;
using tdme::engine::model::TextureCoordinate;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

class tdme::engine::subsystems::object::Object3DGroupMesh final
{
public: /* protected */
	Group* group {  };
	int32_t faceCount {  };
	vector<int16_t> indices {  };
	vector<TextureCoordinate>* textureCoordinates;
	vector<Vector3>* vertices {  };
	vector<Vector3>* normals {  };
	vector<Vector3>* tangents {  };
	vector<Vector3>* bitangents {  };
	vector<Vector3> transformedVertices;
	vector<Vector3> transformedNormals;
	vector<Vector3> transformedTangents;
	vector<Vector3> transformedBitangents;
	Engine::AnimationProcessingTarget animationProcessingTarget {  };

private:
	int32_t cSkinningMaxVertexWeights {  };
	vector<vector<float>> cSkinningJointWeight {  };

public: /* protected */
	Matrix4x4* cGroupTransformationsMatrix {  };

private:
	vector<vector<Matrix4x4*>> cSkinningJointTransformationsMatrices {  };

public: /* protected */
	bool skinning {  };
	int32_t skinningJoints {  };

private:
	bool recreatedBuffers {  };

public: /* protected */

	/** 
	 * Creates a object3d group mesh from group
	 * @param animation processing target
	 * @param group
	 * @param transformationm matrices
	 * @param skinning matrices 
	 * @return object 3d group mesh
	 */
	static Object3DGroupMesh* createMesh(Engine::AnimationProcessingTarget animationProcessingTarget, Group* group, map<wstring, Matrix4x4*>* transformationMatrices,map<wstring, Matrix4x4*>* skinningMatrices);

	/** 
	 * Computes mesh transformations
	 * @param group
	 */
	void computeTransformations(Group* group);

	/** 
	 * Recreates group float buffers
	 */
	void recreateBuffers();

	/** 
	 * @return if buffers has been recreated and unsets state
	 */
	bool hasRecreatedBuffers();

	/** 
	 * Set up vertex indices buffer
	 * @param renderer
	 * @param vbo id
	 */
	void setupVertexIndicesBuffer(GLRenderer* renderer, int32_t vboId);

	/** 
	 * Set up texture coordinates buffer
	 * @param renderer
	 * @param vbo id
	 */
	void setupTextureCoordinatesBuffer(GLRenderer* renderer, int32_t vboId);

	/** 
	 * Set up vertices buffer
	 * @param renderer
	 * @param vbo id
	 */
	void setupVerticesBuffer(GLRenderer* renderer, int32_t vboId);

	/** 
	 * Set up normals buffer
	 * @param renderer
	 * @param vbo id
	 */
	void setupNormalsBuffer(GLRenderer* renderer, int32_t vboId);

	/** 
	 * Set up tangents buffer
	 * @param renderer
	 * @param vbo id
	 */
	void setupTangentsBuffer(GLRenderer* renderer, int32_t vboId);

	/** 
	 * Set up bitangents buffer
	 * @param renderer
	 * @param vbo id
	 */
	void setupBitangentsBuffer(GLRenderer* renderer, int32_t vboId);

	// Generated

public:
	/**
	 * Public constructor
	 */
	Object3DGroupMesh();
};

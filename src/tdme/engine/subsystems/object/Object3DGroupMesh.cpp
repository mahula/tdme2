#include <tdme/engine/subsystems/object/Object3DGroupMesh.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/math/Math.h>
#include <tdme/utils/ByteBuffer.h>
#include <tdme/utils/FloatBuffer.h>
#include <tdme/utils/ShortBuffer.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/JointWeight.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/object/ObjectBuffer.h>
#include <tdme/engine/subsystems/renderer/GLRenderer.h>
#include <tdme/math/MathTools.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::map;
using std::vector;
using std::wstring;

using tdme::engine::subsystems::object::Object3DGroupMesh;
using tdme::math::Math;
using tdme::utils::ByteBuffer;
using tdme::utils::FloatBuffer;
using tdme::utils::ShortBuffer;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::model::Joint;
using tdme::engine::model::JointWeight;
using tdme::engine::model::Skinning;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::object::ObjectBuffer;
using tdme::engine::subsystems::renderer::GLRenderer;
using tdme::math::MathTools;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Object3DGroupMesh::Object3DGroupMesh()
{
	vertices = nullptr;
	normals = nullptr;
	textureCoordinates = nullptr;
	tangents = nullptr;
	bitangents = nullptr;
	cSkinningMaxVertexWeights = -1;
	cGroupTransformationsMatrix = nullptr;
	skinning = false;
	skinningJoints = -1;
}

Object3DGroupMesh* Object3DGroupMesh::createMesh(Engine::AnimationProcessingTarget animationProcessingTarget, Group* group, map<wstring, Matrix4x4*>* transformationMatrices, map<wstring, Matrix4x4*>* skinningMatrices)
{
	auto mesh = new Object3DGroupMesh();
	mesh->group = group;
	auto groupVertices = group->getVertices();
	auto groupNormals = group->getNormals();
	auto groupTextureCoordinates = group->getTextureCoordinates();
	auto groupTangents = group->getTangents();
	auto groupBitangents = group->getBitangents();
	mesh->faceCount = group->getFaceCount();
	mesh->animationProcessingTarget = animationProcessingTarget;
	auto skinning = group->getSkinning();
	mesh->skinning = skinning != nullptr;
	if ((skinning != nullptr && animationProcessingTarget == Engine::AnimationProcessingTarget::CPU) || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
		mesh->transformedVertices.resize(groupVertices->size());
		mesh->vertices = &mesh->transformedVertices;
		for (auto j = 0; j < mesh->vertices->size(); j++) {
			(*mesh->vertices)[j].set((*groupVertices)[j]);
		}
		mesh->transformedNormals.resize(groupNormals->size());
		mesh->normals = &mesh->transformedNormals;
		for (auto j = 0; j < mesh->normals->size(); j++) {
			(*mesh->normals)[j].set((*groupNormals)[j]);
		}
		if (groupTangents->size() > 0) {
			mesh->transformedTangents.resize(groupTangents->size());
			mesh->tangents = &mesh->transformedTangents;
			for (auto j = 0; j < mesh->tangents->size(); j++) {
				(*mesh->tangents)[j].set((*groupTangents)[j]);
			}
		}
		if (groupBitangents->size() > 0) {
			mesh->transformedBitangents.resize(groupBitangents->size());
			mesh->bitangents = &mesh->transformedBitangents;
			for (auto j = 0; j < mesh->bitangents->size(); j++) {
				(*mesh->bitangents)[j].set((*groupBitangents)[j]);
			}
		}
	} else {
		mesh->vertices = groupVertices;
		mesh->normals = groupNormals;
		if (groupTangents->size() > 0) {
			mesh->tangents = groupTangents;
		}
		if (groupBitangents->size() > 0) {
			mesh->bitangents = groupBitangents;
		}
	}
	auto indicesCount = 0;
	for (auto& facesEntity : *group->getFacesEntities()) {
		indicesCount += 3 * facesEntity.getFaces()->size();
	}
	mesh->indices.resize(indicesCount);
	{
		auto j = 0;
		for (auto& facesEntity : *group->getFacesEntities())
		for (auto& face : *facesEntity.getFaces())
		for (auto& vertexIndex : *face.getVertexIndices()) {
			mesh->indices[j++] = static_cast< int16_t >(vertexIndex);
		}
	}
	mesh->textureCoordinates = groupTextureCoordinates;
	mesh->recreatedBuffers = false;
	if (mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
		auto transformationMatrixIt = transformationMatrices->find(group->getId());
		mesh->cGroupTransformationsMatrix = transformationMatrixIt != transformationMatrices->end()?transformationMatrixIt->second:nullptr;
	}
	if ((skinning != nullptr && (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING))) {
		if (mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			mesh->cSkinningJointWeight.resize(groupVertices->size());
			mesh->cSkinningJointTransformationsMatrices.resize(groupVertices->size());
			auto joints = skinning->getJoints();
			auto weights = skinning->getWeights();
			auto jointsWeights = skinning->getVerticesJointsWeights();
			for (auto vertexIndex = 0; vertexIndex < groupVertices->size(); vertexIndex++) {
				auto vertexJointWeights = (*jointsWeights)[vertexIndex].size();
				if (vertexJointWeights > mesh->cSkinningMaxVertexWeights)
					mesh->cSkinningMaxVertexWeights = vertexJointWeights;

				mesh->cSkinningJointWeight[vertexIndex].resize(vertexJointWeights);
				mesh->cSkinningJointTransformationsMatrices[vertexIndex].resize(vertexJointWeights);
				auto jointWeightIdx = 0;
				for (auto& jointWeight : (*jointsWeights)[vertexIndex]) {
					auto& joint = (*joints)[jointWeight.getJointIndex()];
					mesh->cSkinningJointWeight[vertexIndex][jointWeightIdx] = (*weights)[jointWeight.getWeightIndex()];
					auto skinningMatrixIt = skinningMatrices->find(joint.getGroupId());
					mesh->cSkinningJointTransformationsMatrices[vertexIndex][jointWeightIdx] = skinningMatrixIt != skinningMatrices->end()?skinningMatrixIt->second:nullptr;
					jointWeightIdx++;
				}
			}
		}
	}
	mesh->recreateBuffers();
	return mesh;
}

void Object3DGroupMesh::computeTransformations(Group* group)
{
	Vector3 tmpVector3;
	auto groupVertices = group->getVertices();
	auto groupNormals = group->getNormals();
	auto groupTangent = group->getTangents();
	auto groupBitangent = group->getBitangents();
	auto skinning = group->getSkinning();
	if (skinning != nullptr && (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING)) {
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			auto jointsWeights = skinning->getVerticesJointsWeights();
			Vector3* vertex;
			Vector3* transformedVertex;
			Vector3* normal;
			Vector3* transformedNormal;
			Vector3* tangent;
			Vector3* transformedTangent;
			Vector3* bitangent;
			Vector3* transformedBitangent;
			float totalWeights;
			float weightNormalized;
			for (auto vertexIndex = 0; vertexIndex < groupVertices->size(); vertexIndex++) {
				vertex = &(*groupVertices)[vertexIndex];
				transformedVertex = &(*vertices)[vertexIndex].set(0.0f, 0.0f, 0.0f);
				normal = &(*groupNormals)[vertexIndex];
				transformedNormal = &(*normals)[vertexIndex].set(0.0f, 0.0f, 0.0f);
				tangent = groupTangent->size() > 0 ? &(*groupTangent)[vertexIndex] : static_cast< Vector3* >(nullptr);
				transformedTangent = tangents != nullptr ? &(*tangents)[vertexIndex].set(0.0f, 0.0f, 0.0f) : static_cast< Vector3* >(nullptr);
				bitangent = groupTangent->size() > 0 ? &(*groupBitangent)[vertexIndex] : static_cast< Vector3* >(nullptr);
				transformedBitangent = bitangents != nullptr ? &(*bitangents)[vertexIndex].set(0.0f, 0.0f, 0.0f) : static_cast< Vector3* >(nullptr);
				totalWeights = 0.0f;
				for (auto vertexJointWeightIdx = 0; vertexJointWeightIdx < (*jointsWeights)[vertexIndex].size(); vertexJointWeightIdx++) {
					auto weight = cSkinningJointWeight[vertexIndex][vertexJointWeightIdx];
					auto cTransformationsMatrix = cSkinningJointTransformationsMatrices[vertexIndex][vertexJointWeightIdx];
					transformedVertex->add(cTransformationsMatrix->multiply(*vertex, tmpVector3).scale(weight));
					transformedNormal->add(cTransformationsMatrix->multiplyNoTranslation(*normal, tmpVector3).scale(weight));
					if (tangent != nullptr) {
						transformedTangent->add(cTransformationsMatrix->multiplyNoTranslation(*tangent, tmpVector3).scale(weight));
					}
					if (bitangent != nullptr) {
						transformedBitangent->add(cTransformationsMatrix->multiplyNoTranslation(*bitangent, tmpVector3).scale(weight));
					}
					totalWeights += weight;
				}
				if (Math::abs(totalWeights - 1.0f) > MathTools::EPSILON) {
					weightNormalized = 1.0f / totalWeights;
					transformedVertex->scale(weightNormalized);
					transformedNormal->scale(weightNormalized);
					if (transformedTangent != nullptr) {
						transformedTangent->scale(weightNormalized);
					}
					if (transformedBitangent != nullptr) {
						transformedBitangent->scale(weightNormalized);
					}
				}
				transformedNormal->normalize();
			}
			recreateBuffers();
		}
	} else if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
		for (auto vertexIndex = 0; vertexIndex < groupVertices->size(); vertexIndex++) {
			(*vertices)[vertexIndex].set(cGroupTransformationsMatrix->multiply((*groupVertices)[vertexIndex], tmpVector3));
		}
		for (auto normalIndex = 0; normalIndex < groupNormals->size(); normalIndex++) {
			(*normals)[normalIndex].set(cGroupTransformationsMatrix->multiplyNoTranslation((*groupNormals)[normalIndex], tmpVector3).normalize());
		}
		recreateBuffers();
	}
}

void Object3DGroupMesh::recreateBuffers()
{
	recreatedBuffers = true;
}

bool Object3DGroupMesh::hasRecreatedBuffers()
{
	if (recreatedBuffers == true) {
		recreatedBuffers = false;
		return true;
	} else {
		return false;
	}
}

void Object3DGroupMesh::setupVertexIndicesBuffer(GLRenderer* renderer, int32_t vboId)
{
	auto sbIndices = ObjectBuffer::getByteBuffer(faceCount * 3 * sizeof(int16_t))->asShortBuffer();
	for (auto index : indices) {
		sbIndices.put(index);
	}
	renderer->uploadIndicesBufferObject(vboId, sbIndices.getPosition() * sizeof(int16_t), &sbIndices);
}

void Object3DGroupMesh::setupTextureCoordinatesBuffer(GLRenderer* renderer, int32_t vboId)
{
	auto groupTextureCoordinates = group->getTextureCoordinates();
	if (groupTextureCoordinates == nullptr) return;
	auto fbTextureCoordinates = ObjectBuffer::getByteBuffer(groupTextureCoordinates->size() * 2 * sizeof(float))->asFloatBuffer();
	for (auto& textureCoordinate : *groupTextureCoordinates) {
		fbTextureCoordinates.put(textureCoordinate.getArray());
	}
	renderer->uploadBufferObject(vboId, fbTextureCoordinates.getPosition() * sizeof(float), &fbTextureCoordinates);
}

void Object3DGroupMesh::setupVerticesBuffer(GLRenderer* renderer, int32_t vboId)
{
	auto fbVertices = ObjectBuffer::getByteBuffer(vertices->size() * 3 * sizeof(float))->asFloatBuffer();
	for (auto& vertex : *vertices) {
		fbVertices.put(vertex.getArray());
	}
	renderer->uploadBufferObject(vboId, fbVertices.getPosition() * sizeof(float), &fbVertices);
}

void Object3DGroupMesh::setupNormalsBuffer(GLRenderer* renderer, int32_t vboId)
{
	auto fbNormals = ObjectBuffer::getByteBuffer(normals->size() * 3 * sizeof(float))->asFloatBuffer();
	for (auto& normal : *normals) {
		fbNormals.put(normal.getArray());
	}
	renderer->uploadBufferObject(vboId, fbNormals.getPosition() * sizeof(float), &fbNormals);
}

void Object3DGroupMesh::setupTangentsBuffer(GLRenderer* renderer, int32_t vboId)
{
	if (tangents == nullptr) return;
	auto fbTangents = ObjectBuffer::getByteBuffer(tangents->size() * 3 * sizeof(float))->asFloatBuffer();
	for (auto& tangent : *tangents) {
		fbTangents.put(tangent.getArray());
	}
	renderer->uploadBufferObject(vboId, fbTangents.getPosition() * sizeof(float), &fbTangents);
}

void Object3DGroupMesh::setupBitangentsBuffer(GLRenderer* renderer, int32_t vboId)
{
	if (bitangents == nullptr) return;
	auto fbBitangents = ObjectBuffer::getByteBuffer(bitangents->size() * 3 * sizeof(float))->asFloatBuffer();
	for (auto& bitangent : *bitangents) {
		fbBitangents.put(bitangent.getArray());
	}
	renderer->uploadBufferObject(vboId, fbBitangents.getPosition() * sizeof(float), &fbBitangents);
}


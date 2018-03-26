#include <tdme/engine/subsystems/rendering/Object3DVBORenderer.h>

#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/PointsParticleSystemEntity.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/rendering/BatchVBORendererPoints.h>
#include <tdme/engine/subsystems/rendering/BatchVBORendererTriangles.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>
#include <tdme/engine/subsystems/rendering/Object3DGroup.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupMesh.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupVBORenderer.h>
#include <tdme/engine/subsystems/rendering/Object3DVBORenderer_TransparentRenderFacesGroupPool.h>
#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFace.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesGroup.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderPoint.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderPointsPool.h>
#include <tdme/engine/subsystems/particlesystem/ParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/PointsParticleSystemEntityInternal.h>
#include <tdme/engine/subsystems/renderer/GLRenderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Matrix4x4Negative.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/ByteBuffer.h>
#include <tdme/utils/FloatBuffer.h>
#include <tdme/utils/Pool.h>
#include <tdme/utils/Console.h>

using std::find;
using std::map;
using std::set;
using std::sort;
using std::vector;
using std::string;
using std::to_string;

using tdme::engine::subsystems::rendering::Object3DVBORenderer;
using tdme::engine::Engine;
using tdme::engine::Object3D;
using tdme::engine::PointsParticleSystemEntity;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::rendering::BatchVBORendererPoints;
using tdme::engine::subsystems::rendering::BatchVBORendererTriangles;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::Object3DGroupMesh;
using tdme::engine::subsystems::rendering::Object3DGroupVBORenderer;
using tdme::engine::subsystems::rendering::Object3DVBORenderer_1;
using tdme::engine::subsystems::rendering::Object3DVBORenderer_TransparentRenderFacesGroupPool;
using tdme::engine::subsystems::rendering::ObjectBuffer;
using tdme::engine::subsystems::rendering::TransparentRenderFace;
using tdme::engine::subsystems::rendering::TransparentRenderFacesGroup;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;
using tdme::engine::subsystems::rendering::TransparentRenderPoint;
using tdme::engine::subsystems::rendering::TransparentRenderPointsPool;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::subsystems::particlesystem::PointsParticleSystemEntityInternal;
using tdme::engine::subsystems::renderer::GLRenderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Matrix4x4Negative;
using tdme::math::Vector3;
using tdme::utils::ByteBuffer;
using tdme::utils::FloatBuffer;
using tdme::utils::Pool;
using tdme::utils::Console;

constexpr int32_t Object3DVBORenderer::BATCHVBORENDERER_MAX;

Object3DVBORenderer::Object3DVBORenderer(Engine* engine, GLRenderer* renderer) 
{
	this->engine = engine;
	this->renderer = renderer;
	transparentRenderFacesGroupPool = new Object3DVBORenderer_TransparentRenderFacesGroupPool();
	transparentRenderFacesPool = new TransparentRenderFacesPool();
	pseTransparentRenderPointsPool = new TransparentRenderPointsPool(16384);
	psePointBatchVBORenderer = new BatchVBORendererPoints(renderer, 0);
}

Object3DVBORenderer::~Object3DVBORenderer() {
	for (auto batchVBORenderer: trianglesBatchVBORenderers) {
		delete batchVBORenderer;
	}
	delete transparentRenderFacesGroupPool;
	delete transparentRenderFacesPool;
	delete pseTransparentRenderPointsPool;
	delete psePointBatchVBORenderer;
}

void Object3DVBORenderer::initialize()
{
	psePointBatchVBORenderer->initialize();
	auto vboManaged = Engine::getInstance()->getVBOManager()->addVBO("tdme.object3dvborenderer.instancedrendering", 3);
	vboInstancedRenderingIds = vboManaged->getVBOGlIds();
}

void Object3DVBORenderer::dispose()
{
	// dispose batch vbo renderer
	for (auto batchVBORenderer: trianglesBatchVBORenderers) {
		batchVBORenderer->dispose();
		batchVBORenderer->release();
	}
	psePointBatchVBORenderer->dispose();
}

BatchVBORendererTriangles* Object3DVBORenderer::acquireTrianglesBatchVBORenderer()
{
	// check for free batch vbo renderer
	auto i = 0;
	for (auto batchVBORenderer: trianglesBatchVBORenderers) {
		if (batchVBORenderer->acquire()) return batchVBORenderer;
		i++;
	}
	// try to add one
	if (i < BATCHVBORENDERER_MAX) {
		auto batchVBORenderer = new BatchVBORendererTriangles(renderer, i);
		batchVBORenderer->initialize();
		trianglesBatchVBORenderers.push_back(batchVBORenderer);
		if (batchVBORenderer->acquire()) return batchVBORenderer;

	}
	// nope
	Console::println(string("Object3DVBORenderer::acquireTrianglesBatchVBORenderer()::failed"));
	return nullptr;
}

void Object3DVBORenderer::reset()
{
	visibleObjectsByModels.clear();
}

void Object3DVBORenderer::render(const vector<Object3D*>& objects, bool renderTransparentFaces, int32_t renderTypes)
{
	// clear transparent render faces data
	transparentRenderFacesPool->reset();
	releaseTransparentFacesGroups();
	// sort objects by model
	for (auto objectIdx = 0; objectIdx < objects.size(); objectIdx++) {
		auto object = objects.at(objectIdx);
		auto modelId = object->getModel()->getId();
		auto& visibleObjectsByModel = visibleObjectsByModels[modelId];
		visibleObjectsByModel.push_back(object);
	}
	// render objects
	for (auto& objectsByModelIt: visibleObjectsByModels) {
		auto& objectsByModel = objectsByModelIt.second;
		if (objectsByModel.size() > 0) {
			renderObjectsOfSameType(objectsByModel, renderTransparentFaces, renderTypes);
			objectsByModel.clear();
		}
	}
	// render transparent render faces if any exist
	auto transparentRenderFaces = transparentRenderFacesPool->getTransparentRenderFaces();
	if (transparentRenderFaces->size() > 0) {
		// sort transparent render faces from far to near
		sort(transparentRenderFaces->begin(), transparentRenderFaces->end(), TransparentRenderFace::compare);
		// second render pass, draw color buffer for transparent objects
		// 	set up blending, but no culling and no depth buffer
		//	TODO: enabling depth buffer let shadow disappear
		renderer->disableDepthBuffer();
		renderer->disableCulling();
		renderer->enableBlending();
		// actually this should not make any difference as culling is disabled
		// but having a fixed value is not a bad idea except that it is a GL call
		// TODO: confirm this
		renderer->setFrontFace(renderer->FRONTFACE_CCW);
		for (auto transparentRenderFace: *transparentRenderFacesPool->getTransparentRenderFaces()) {
			// do we have any faces yet?
			if (groupTransparentRenderFaces.size() == 0) {
				// nope, so add this one
				groupTransparentRenderFaces.push_back(transparentRenderFace);
			} else
			// do we have more than face already?
			if (groupTransparentRenderFaces.size() > 0) {
				// check if we have more of first type
				if (groupTransparentRenderFaces.at(0)->object3DGroup == transparentRenderFace->object3DGroup) {
					// yep, we can add this one
					groupTransparentRenderFaces.push_back(transparentRenderFace);
				} else {
					// no, render grouped faces
					prepareTransparentFaces(groupTransparentRenderFaces);
					// reset
					groupTransparentRenderFaces.clear();
					// add current face
					groupTransparentRenderFaces.push_back(transparentRenderFace);
				}
			}
		}
		// 	check if there are any left overs
		if (groupTransparentRenderFaces.size() > 0) {
			prepareTransparentFaces(groupTransparentRenderFaces);
			groupTransparentRenderFaces.clear();
		}
		// render transparent faces groups
		renderTransparentFacesGroups();
		//	no blending, but culling and depth buffer
		renderer->disableBlending();
		renderer->enableCulling();
		renderer->enableDepthBuffer();
		// done!
	}
}

void Object3DVBORenderer::prepareTransparentFaces(const vector<TransparentRenderFace*>& transparentRenderFaces)
{
	// all those faces should share the object and object 3d group, ...
	auto object3DGroup = transparentRenderFaces.at(0)->object3DGroup;
	auto object3D = dynamic_cast<Object3D*>(object3DGroup->object);
	// model view matrix to be used with given transparent render faces
	Matrix4x4 modelViewMatrix;
	if (object3DGroup->mesh->skinning == true) {
		modelViewMatrix.identity();
	} else {
		if (renderer->isInstancedRenderingAvailable() == true) {
			modelViewMatrix.set(*object3DGroup->groupTransformationsMatrix).multiply(object3D->getTransformationsMatrix());
		} else {
			modelViewMatrix.set(*object3DGroup->groupTransformationsMatrix).multiply(object3D->getTransformationsMatrix()).multiply(renderer->getCameraMatrix());
		}
	}
	//
	auto model = object3DGroup->object->getModel();
	auto facesEntities = object3DGroup->group->getFacesEntities();
	FacesEntity* facesEntity = nullptr;
	// attributes we collect for a transparent render face group
	auto depthBuffer = false;
	auto& effectColorAdd = object3D->getEffectColorAdd();
	auto& effectColorMul = object3D->getEffectColorMul();
	Material* material = nullptr;
	auto textureCoordinates = false;
	Vector3 transformedVector;
	Vector3 transformedNormal;
	// render transparent faces
	for (auto i = 0; i < transparentRenderFaces.size(); i++) {
		auto transparentRenderFace = transparentRenderFaces.at(i);
		auto facesEntityIdx = transparentRenderFace->facesEntityIdx;
		// determine if faces entity and so material did switch between last face and current face
		if (facesEntity != &(*facesEntities)[facesEntityIdx]) {
			facesEntity = &(*facesEntities)[facesEntityIdx];
			material = facesEntity->getMaterial();
		}
		textureCoordinates = facesEntity->isTextureCoordinatesAvailable();
		// create group key
		string transparentRenderFacesGroupKey = TransparentRenderFacesGroup::createKey(model, object3DGroup, facesEntityIdx, effectColorAdd, effectColorMul, material, textureCoordinates);
		// get group
		TransparentRenderFacesGroup* trfGroup = nullptr;
		auto trfGroupIt = transparentRenderFacesGroups.find(transparentRenderFacesGroupKey);
		if (trfGroupIt != transparentRenderFacesGroups.end()) {
			trfGroup = trfGroupIt->second;
		}
		if (trfGroup == nullptr) {
			// we do not have the group, create group
			trfGroup = transparentRenderFacesGroupPool->allocate();
			trfGroup->set(this, model, object3DGroup, facesEntityIdx, effectColorAdd, effectColorMul, material, textureCoordinates);
			transparentRenderFacesGroups[transparentRenderFacesGroupKey] = trfGroup;
		}
		for (auto vertexIdx = 0; vertexIdx < 3; vertexIdx++) {
			auto arrayIdx = transparentRenderFace->object3DGroup->mesh->indices[transparentRenderFace->faceIdx * 3 + vertexIdx];
			trfGroup->addVertex(
				modelViewMatrix.multiply((*transparentRenderFace->object3DGroup->mesh->vertices)[arrayIdx], transformedVector),
				modelViewMatrix.multiplyNoTranslation((*transparentRenderFace->object3DGroup->mesh->normals)[arrayIdx], transformedNormal),
				transparentRenderFace->object3DGroup->mesh->textureCoordinates->size() >0 ?
					&(*transparentRenderFace->object3DGroup->mesh->textureCoordinates)[arrayIdx] :
					nullptr
			);
		}
	}
}

void Object3DVBORenderer::renderTransparentFacesGroups()
{
	for (auto it: transparentRenderFacesGroups) {
		it.second->render(renderer);
	}
}

void Object3DVBORenderer::releaseTransparentFacesGroups()
{
	for (auto it: transparentRenderFacesGroups) {
		transparentRenderFacesGroupPool->release(it.second);
	}
	transparentRenderFacesGroups.clear();
}

void Object3DVBORenderer::renderObjectsOfSameType(const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes) {
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderObjectsOfSameTypeInstanced(objects, collectTransparentFaces, renderTypes);
	} else {
		renderObjectsOfSameTypeNonInstanced(objects, collectTransparentFaces, renderTypes);
	}
}

void Object3DVBORenderer::renderObjectsOfSameTypeNonInstanced(const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes)
{
	// do pre render steps
	for (auto object: objects) {
		for (auto j = 0; j < object->object3dGroups.size(); j++) {
			auto object3DGroup = object->object3dGroups[j];
			object3DGroup->renderer->preRender(this);
		}
	}
	//
	auto shadowMapping = engine->getShadowMapping();
	Matrix4x4 modelViewMatrix;
	Matrix4x4 cameraMatrix;
	cameraMatrix.set(renderer->getModelViewMatrix());
	// render faces entities
	auto currentFrontFace = -1;
	auto firstObject = objects.at(0);
	// all objects share the same object 3d group structure, so we just take the first one
	vector<int32_t>* boundVBOBaseIds = nullptr;
	vector<int32_t>* boundVBOTangentBitangentIds = nullptr;
	for (auto object3DGroupIdx = 0; object3DGroupIdx < firstObject->object3dGroups.size(); object3DGroupIdx++) {
		auto object3DGroup = firstObject->object3dGroups[object3DGroupIdx];
		// render each faces entity
		auto facesEntities = object3DGroup->group->getFacesEntities();
		auto faceIdx = 0;
		auto facesEntityIdxCount = facesEntities->size();
		for (auto faceEntityIdx = 0; faceEntityIdx < facesEntityIdxCount; faceEntityIdx++) {
			auto facesEntity = &(*facesEntities)[faceEntityIdx];
			auto isTextureCoordinatesAvailable = facesEntity->isTextureCoordinatesAvailable();
			auto faces = facesEntity->getFaces()->size();
			// material
			auto material = facesEntity->getMaterial();
			// determine if transparent
			auto transparentFacesEntity = false;
			//	via material
			if (material != nullptr) {
				if (material->hasColorTransparency() == true || material->hasTextureTransparency() == true) transparentFacesEntity = true;
				if (material->hasDiffuseTextureTransparency() == true && material->hasDiffuseTextureMaskedTransparency() == true) {
					renderer->disableCulling();
				}
			}
			// skip, if requested
			if (transparentFacesEntity == true) {
				// add to transparent render faces, if requested
				auto objectCount = objects.size();
				for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
					auto object = objects.at(objectIdx);
					auto _object3DGroup = object->object3dGroups[object3DGroupIdx];
					// set up textures
					Object3DGroup::setupTextures(renderer, object3DGroup, faceEntityIdx);
					// set up transparent render faces
					if (collectTransparentFaces == true) {
						transparentRenderFacesPool->createTransparentRenderFaces(
							(_object3DGroup->mesh->skinning == true ?
								modelViewMatrix.identity() :
								modelViewMatrix.set(*_object3DGroup->groupTransformationsMatrix)
							).
								multiply(object->getTransformationsMatrix()).multiply(cameraMatrix),
								object->object3dGroups[object3DGroupIdx],
								faceEntityIdx,
								faceIdx
							);
					}
				}
				// keep track of rendered faces
				faceIdx += faces;
				// skip to next entity
				continue;
			}
			// optional texture coordinates
			if (isTextureCoordinatesAvailable == true) {
				// enable texturing client state if not yet done
				if (renderer->renderingTexturingClientState == false) {
					renderer->enableClientState(renderer->CLIENTSTATE_TEXTURECOORD_ARRAY);
					renderer->renderingTexturingClientState = true;
				}
			} else {
				// disable texturing client state if not yet done
				if (renderer->renderingTexturingClientState == true) {
					renderer->disableClientState(renderer->CLIENTSTATE_TEXTURECOORD_ARRAY);
					renderer->renderingTexturingClientState = false;
				}
			}
			// draw this faces entity for each object
			bool materialUpdateOnly = false;
			auto objectCount = objects.size();
			for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
				auto object = objects.at(objectIdx);
				auto _object3DGroup = object->object3dGroups[object3DGroupIdx];

				// set up material on first object
				string materialKey;
				setupMaterial(_object3DGroup, faceEntityIdx, renderTypes, materialUpdateOnly, materialKey);
				// only update materials for next calls
				materialUpdateOnly = true;

				//	check transparency via effect
				if (object->effectColorMul.getAlpha() < 1.0f - Math::EPSILON ||
					object->effectColorAdd.getAlpha() < -Math::EPSILON) {
					// add to transparent render faces, if requested
					if (collectTransparentFaces == true) {
						transparentRenderFacesPool->createTransparentRenderFaces(
							(_object3DGroup->mesh->skinning == true ?
								modelViewMatrix.identity() :
								modelViewMatrix.set(*_object3DGroup->groupTransformationsMatrix)
							).multiply(object->getTransformationsMatrix()).multiply(cameraMatrix),
							_object3DGroup,
							faceEntityIdx,
							faceIdx
						);
					}
					// skip to next object
					continue;
				}
				// bind buffer base objects if not bound yet
				auto currentVBOGlIds = _object3DGroup->renderer->vboBaseIds;
				if (boundVBOBaseIds != currentVBOGlIds) {
					boundVBOBaseIds = currentVBOGlIds;
					//	texture coordinates
					if (isTextureCoordinatesAvailable == true &&
						(((renderTypes & RENDERTYPE_TEXTUREARRAYS) == RENDERTYPE_TEXTUREARRAYS) ||
						((renderTypes & RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY && material != nullptr && material->hasDiffuseTextureMaskedTransparency() == true))) {
						renderer->bindTextureCoordinatesBufferObject((*currentVBOGlIds)[3]);
					}
					// 	indices
					renderer->bindIndicesBufferObject((*currentVBOGlIds)[0]);
					// 	vertices
					renderer->bindVerticesBufferObject((*currentVBOGlIds)[1]);
					// 	normals
					if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS) renderer->bindNormalsBufferObject((*currentVBOGlIds)[2]);
				}
				// bind tangent, bitangend buffers if not yet bound
				auto currentVBOTangentBitangentIds = _object3DGroup->renderer->vboTangentBitangentIds;
				if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS &&
					renderer->isNormalMappingAvailable() && currentVBOTangentBitangentIds != nullptr && currentVBOTangentBitangentIds != boundVBOTangentBitangentIds) {
					// tangent
					renderer->bindTangentsBufferObject((*currentVBOTangentBitangentIds)[0]);
					// bitangent
					renderer->bindBitangentsBufferObject((*currentVBOTangentBitangentIds)[1]);
				}
				// set up local -> world transformations matrix
				renderer->getModelViewMatrix().set(
					(_object3DGroup->mesh->skinning == true ?
						modelViewMatrix.identity() :
						modelViewMatrix.set(*_object3DGroup->groupTransformationsMatrix)
					).
						multiply(object->getTransformationsMatrix()).
						multiply(cameraMatrix)
				);
				renderer->onUpdateModelViewMatrix();
				// set up front face
				auto objectFrontFace = matrix4x4Negative.isNegative(renderer->getModelViewMatrix()) == false ? renderer->FRONTFACE_CCW : renderer->FRONTFACE_CW;
				if (objectFrontFace != currentFrontFace) {
					renderer->setFrontFace(objectFrontFace);
					currentFrontFace = objectFrontFace;
				}
				// set up effect color
				if ((renderTypes & RENDERTYPE_EFFECTCOLORS) == RENDERTYPE_EFFECTCOLORS) {
					renderer->setEffectColorMul(object->effectColorMul.getArray());
					renderer->setEffectColorAdd(object->effectColorAdd.getArray());
					renderer->onUpdateEffect();
				}
				// do transformation start to shadow mapping
				if ((renderTypes & RENDERTYPE_SHADOWMAPPING) == RENDERTYPE_SHADOWMAPPING &&
					shadowMapping != nullptr) {
					shadowMapping->startObjectTransformations(
						(_object3DGroup->mesh->skinning == true ?
							modelViewMatrix.identity() :
							modelViewMatrix.set(*_object3DGroup->groupTransformationsMatrix)
						).multiply(object->getTransformationsMatrix()));
				}
				// draw
				renderer->drawIndexedTrianglesFromBufferObjects(faces, faceIdx);
				// do transformations end to shadow mapping
				if ((renderTypes & RENDERTYPE_SHADOWMAPPING) == RENDERTYPE_SHADOWMAPPING &&
					shadowMapping != nullptr) {
					shadowMapping->endObjectTransformations();
				}
			}
			// keep track of rendered faces
			faceIdx += faces;
			if (material != nullptr) {
				if (material->hasDiffuseTextureTransparency() == true && material->hasDiffuseTextureMaskedTransparency() == true) {
					renderer->enableCulling();
				}
			}
		}
	}
	// unbind buffers
	renderer->unbindBufferObjects();
	// restore model view matrix / view matrix
	renderer->getModelViewMatrix().set(cameraMatrix);
}


void Object3DVBORenderer::renderObjectsOfSameTypeInstanced(const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes)
{
	Matrix4x4 cameraMatrix(renderer->getModelViewMatrix());
	Matrix4x4 modelViewMatrixTemp;
	Matrix4x4 modelViewMatrix;

	// store layouts
	vector<Matrix4x4> modelMatrixArray;
	vector<Color4> effectColorMulArray;
	vector<Color4> effectColorAddArray;

	//	objects
	vector<Object3D*> objectsToRender;
	vector<Object3D*> objectsNotRendered;

	{
		// do pre render steps
		for (auto object: objects) {
			for (auto j = 0; j < object->object3dGroups.size(); j++) {
				auto object3DGroup = object->object3dGroups[j];
				object3DGroup->renderer->preRender(this);
			}
		}
	}

	//
	auto shadowMapping = engine->getShadowMapping();

	// render faces entities
	auto firstObject = objects.at(0);

	// all objects share the same object 3d group structure, so we just take the first one
	for (auto object3DGroupIdx = 0; object3DGroupIdx < firstObject->object3dGroups.size(); object3DGroupIdx++) {
		auto object3DGroup = firstObject->object3dGroups[object3DGroupIdx];
		// render each faces entity
		auto facesEntities = object3DGroup->group->getFacesEntities();
		auto faceIdx = 0;
		auto facesEntityIdxCount = facesEntities->size();
		for (auto faceEntityIdx = 0; faceEntityIdx < facesEntityIdxCount; faceEntityIdx++) {
			auto facesEntity = &(*facesEntities)[faceEntityIdx];
			auto isTextureCoordinatesAvailable = facesEntity->isTextureCoordinatesAvailable();
			auto faces = facesEntity->getFaces()->size();
			// material
			auto material = facesEntity->getMaterial();
			// determine if transparent
			auto transparentFacesEntity = false;
			//	via material
			if (material != nullptr) {
				if (material->hasColorTransparency() == true || material->hasTextureTransparency() == true) transparentFacesEntity = true;
				if (material->hasDiffuseTextureTransparency() == true && material->hasDiffuseTextureMaskedTransparency() == true) {
					renderer->disableCulling();
				}
			}
			// skip, if requested
			if (transparentFacesEntity == true) {
				// add to transparent render faces, if requested
				auto objectCount = objects.size();
				for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
					auto object = objects.at(objectIdx);
					auto _object3DGroup = object->object3dGroups[object3DGroupIdx];
					// set up textures
					Object3DGroup::setupTextures(renderer, object3DGroup, faceEntityIdx);
					// set up transparent render faces
					if (collectTransparentFaces == true) {
						transparentRenderFacesPool->createTransparentRenderFaces(
							(_object3DGroup->mesh->skinning == true ?
								modelViewMatrixTemp.identity() :
								modelViewMatrixTemp.set(*_object3DGroup->groupTransformationsMatrix)
							).
								multiply(object->getTransformationsMatrix()).multiply(cameraMatrix),
								object->object3dGroups[object3DGroupIdx],
								faceEntityIdx,
								faceIdx
							);
					}
				}
				// keep track of rendered faces
				faceIdx += faces;
				// skip to next entity
				continue;
			}
			// optional texture coordinates
			if (isTextureCoordinatesAvailable == true) {
				// enable texturing client state if not yet done
				if (renderer->renderingTexturingClientState == false) {
					renderer->enableClientState(renderer->CLIENTSTATE_TEXTURECOORD_ARRAY);
					renderer->renderingTexturingClientState = true;
				}
			} else {
				// disable texturing client state if not yet done
				if (renderer->renderingTexturingClientState == true) {
					renderer->disableClientState(renderer->CLIENTSTATE_TEXTURECOORD_ARRAY);
					renderer->renderingTexturingClientState = false;
				}
			}

			// draw this faces entity for each object
			objectsToRender = objects;
			do {
				auto currentFrontFace = -1;
				string materialKey;
				bool materialUpdateOnly = false;
				vector<int32_t>* boundVBOBaseIds = nullptr;
				vector<int32_t>* boundVBOTangentBitangentIds = nullptr;
				auto objectCount = objectsToRender.size();
				for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
					auto object = objectsToRender.at(objectIdx);
					auto _object3DGroup = object->object3dGroups[object3DGroupIdx];

					//	check transparency via effect
					if (object->effectColorMul.getAlpha() < 1.0f - Math::EPSILON ||
						object->effectColorAdd.getAlpha() < -Math::EPSILON) {
						// add to transparent render faces, if requested
						if (collectTransparentFaces == true) {
							transparentRenderFacesPool->createTransparentRenderFaces(
								(_object3DGroup->mesh->skinning == true ?
									modelViewMatrixTemp.identity() :
									modelViewMatrixTemp.set(*_object3DGroup->groupTransformationsMatrix)
								).multiply(object->getTransformationsMatrix()).multiply(cameraMatrix),
								_object3DGroup,
								faceEntityIdx,
								faceIdx
							);
						}
						// skip to next object
						continue;
					}

					// set up material on first object
					string materialKeyCurrent;
					setupMaterial(_object3DGroup, faceEntityIdx, renderTypes, materialUpdateOnly, materialKeyCurrent);
					// only update material for next material calls
					materialUpdateOnly = true;

					// check if material key has not been set yet
					if (materialKey.size() == 0) {
						materialKey = materialKeyCurrent;
					} else
					// otherwise check if it did change
					if (materialKey != materialKeyCurrent) {
						objectsNotRendered.push_back(object);
						continue;
					}

					// bind buffer base objects if not bound yet
					auto currentVBOBaseIds = _object3DGroup->renderer->vboBaseIds;
					if (boundVBOBaseIds == nullptr) {
						boundVBOBaseIds = currentVBOBaseIds;
						//	texture coordinates
						if (isTextureCoordinatesAvailable == true &&
							(((renderTypes & RENDERTYPE_TEXTUREARRAYS) == RENDERTYPE_TEXTUREARRAYS) ||
							((renderTypes & RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY && material != nullptr && material->hasDiffuseTextureMaskedTransparency() == true))) {
							renderer->bindTextureCoordinatesBufferObject((*currentVBOBaseIds)[3]);
						}
						// 	indices
						renderer->bindIndicesBufferObject((*currentVBOBaseIds)[0]);
						// 	vertices
						renderer->bindVerticesBufferObject((*currentVBOBaseIds)[1]);
						// 	normals
						if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS) {
							renderer->bindNormalsBufferObject((*currentVBOBaseIds)[2]);
						}
					} else
					// check if buffers did change, then skip and render in next step
					if (boundVBOBaseIds != currentVBOBaseIds) {
						objectsNotRendered.push_back(object);
						continue;
					}
					// bind tangent, bitangend buffers
					auto currentVBOTangentBitangentIds = _object3DGroup->renderer->vboTangentBitangentIds;
					if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS &&
						renderer->isNormalMappingAvailable() && currentVBOTangentBitangentIds != nullptr) {
						// bind tangent, bitangend buffers if not yet done
						if (boundVBOTangentBitangentIds == nullptr) {
							// tangent
							renderer->bindTangentsBufferObject((*currentVBOTangentBitangentIds)[0]);
							// bitangent
							renderer->bindBitangentsBufferObject((*currentVBOTangentBitangentIds)[1]);
							//
							boundVBOTangentBitangentIds = currentVBOTangentBitangentIds;
						} else
						// check if buffers did change, then skip and render in next step
						if (currentVBOTangentBitangentIds != boundVBOTangentBitangentIds) {
							objectsNotRendered.push_back(object);
							continue;
						}
					}

					// set up local -> world transformations matrix
					modelViewMatrix.set(
						(_object3DGroup->mesh->skinning == true ?
							modelViewMatrixTemp.identity() :
							modelViewMatrixTemp.set(*_object3DGroup->groupTransformationsMatrix)
						).
							multiply(object->getTransformationsMatrix())
					);

					// set up front face
					auto objectFrontFace = matrix4x4Negative.isNegative(modelViewMatrix) == false ? renderer->FRONTFACE_CCW : renderer->FRONTFACE_CW;
					// if not yet done
					if (currentFrontFace == -1) {
						renderer->setFrontFace(objectFrontFace);
						currentFrontFace = objectFrontFace;
					} else
					// if front face changed just render in next step
					if (objectFrontFace != currentFrontFace) {
						objectsNotRendered.push_back(object);
						continue;
					}

					// set up effect color
					if ((renderTypes & RENDERTYPE_EFFECTCOLORS) == RENDERTYPE_EFFECTCOLORS) {
						effectColorMulArray.push_back(object->effectColorMul);
						effectColorAddArray.push_back(object->effectColorAdd);
					}

					// push mv, mvp to layouts
					modelMatrixArray.push_back(modelViewMatrix);
				}

				// upload model view matrices
				{
					FloatBuffer fbMvMatrices = ObjectBuffer::getByteBuffer(modelMatrixArray.size() * 16 * sizeof(float))->asFloatBuffer();
					for (auto& mvMatrix: modelMatrixArray) fbMvMatrices.put(mvMatrix.getArray());
					renderer->uploadBufferObject((*vboInstancedRenderingIds)[0], fbMvMatrices.getPosition() * sizeof(float), &fbMvMatrices);
					renderer->bindModelMatricesBufferObject((*vboInstancedRenderingIds)[0]);
				}

				// upload effects
				if ((renderTypes & RENDERTYPE_EFFECTCOLORS) == RENDERTYPE_EFFECTCOLORS) {
					// upload effect color mul
					FloatBuffer fbEffectColorMuls = ObjectBuffer::getByteBuffer(effectColorMulArray.size() * 4 * sizeof(float))->asFloatBuffer();
					for (auto& effectColorMul: effectColorMulArray) fbEffectColorMuls.put(effectColorMul.getArray());
					renderer->uploadBufferObject((*vboInstancedRenderingIds)[1], fbEffectColorMuls.getPosition() * sizeof(float), &fbEffectColorMuls);
					renderer->bindEffectColorMulsBufferObject((*vboInstancedRenderingIds)[1]);
					// upload effect color add
					FloatBuffer fbEffectColorAdds = ObjectBuffer::getByteBuffer(effectColorAddArray.size() * 4 * sizeof(float))->asFloatBuffer();
					for (auto& effectColorAdd: effectColorAddArray) fbEffectColorAdds.put(effectColorAdd.getArray());
					renderer->uploadBufferObject((*vboInstancedRenderingIds)[2], fbEffectColorAdds.getPosition() * sizeof(float), &fbEffectColorAdds);
					renderer->bindEffectColorAddsBufferObject((*vboInstancedRenderingIds)[2]);
				}

				// draw
				renderer->drawInstancedIndexedTrianglesFromBufferObjects(faces, faceIdx, modelMatrixArray.size());

				// reset layouts
				modelMatrixArray.clear();
				effectColorMulArray.clear();
				effectColorAddArray.clear();

				// set up next objects to render
				objectsToRender = objectsNotRendered;
				// clear list of objects we did not render
				objectsNotRendered.clear();
			} while (objectsToRender.size() > 0);

			// keep track of rendered faces
			faceIdx += faces;
			if (material != nullptr) {
				if (material->hasDiffuseTextureTransparency() == true && material->hasDiffuseTextureMaskedTransparency() == true) {
					renderer->enableCulling();
				}
			}
		}
	}

	// unbind buffers
	renderer->unbindBufferObjects();
	// restore model view matrix / view matrix
	renderer->getModelViewMatrix().set(cameraMatrix);
}

void Object3DVBORenderer::setupMaterial(Object3DGroup* object3DGroup, int32_t facesEntityIdx, int32_t renderTypes, bool updateOnly, string& materialKey)
{
	auto facesEntities = object3DGroup->group->getFacesEntities();
	auto material = (*facesEntities)[facesEntityIdx].getMaterial();
	// get material or use default
	if (material == nullptr) material = Material::getDefaultMaterial();

	// material key
	materialKey = material->getId();

	// setup textures
	Object3DGroup::setupTextures(renderer, object3DGroup, facesEntityIdx);

	//
	if (updateOnly == false) {
		// apply materials
		if ((renderTypes & RENDERTYPE_MATERIALS) == RENDERTYPE_MATERIALS) {
			renderer->setMaterialAmbient(material->getAmbientColor().getArray());
			renderer->setMaterialDiffuse(material->getDiffuseColor().getArray());
			renderer->setMaterialSpecular(material->getSpecularColor().getArray());
			renderer->setMaterialEmission(material->getEmissionColor().getArray());
			renderer->setMaterialShininess(material->getShininess());
			renderer->setMaterialDiffuseTextureMaskedTransparency(material->hasDiffuseTextureMaskedTransparency());
			renderer->onUpdateMaterial();
		}
		if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES) {
			// bind specular texture
			if (renderer->isSpecularMappingAvailable() == true) {
				renderer->setTextureUnit(LightingShader::TEXTUREUNIT_SPECULAR);
				renderer->bindTexture(object3DGroup->materialSpecularTextureIdsByEntities[facesEntityIdx]);
			}
			// bind displacement texture
			if (renderer->isDisplacementMappingAvailable() == true) {
				renderer->setTextureUnit(LightingShader::TEXTUREUNIT_DISPLACEMENT);
				renderer->bindTexture(object3DGroup->materialDisplacementTextureIdsByEntities[facesEntityIdx]);
			}
			// bind normal texture
			if (renderer->isNormalMappingAvailable() == true) {
				renderer->setTextureUnit(LightingShader::TEXTUREUNIT_NORMAL);
				renderer->bindTexture(object3DGroup->materialNormalTextureIdsByEntities[facesEntityIdx]);
			}
			// switch back texture unit to diffuse unit
			renderer->setTextureUnit(LightingShader::TEXTUREUNIT_DIFFUSE);
		}
	}

	// bind diffuse texture
	if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES ||
		((renderTypes & RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY)) {
		renderer->setMaterialDiffuseTextureMaskedTransparency(material->hasDiffuseTextureMaskedTransparency());
		renderer->onUpdateMaterial();
		if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES ||
			material->hasDiffuseTextureMaskedTransparency() == true) {
			auto diffuseTextureId = object3DGroup->dynamicDiffuseTextureIdsByEntities[facesEntityIdx] != Object3DGroup::GLTEXTUREID_NONE ? object3DGroup->dynamicDiffuseTextureIdsByEntities[facesEntityIdx] : object3DGroup->materialDiffuseTextureIdsByEntities[facesEntityIdx];
			materialKey+= "," + to_string(diffuseTextureId);
			renderer->setTextureUnit(LightingShader::TEXTUREUNIT_DIFFUSE);
			renderer->bindTexture(diffuseTextureId);
		}
	}
}

void Object3DVBORenderer::clearMaterial()
{
	// TODO: optimize me! We do not need always to clear material
	// unbind diffuse texture
	renderer->setTextureUnit(LightingShader::TEXTUREUNIT_DIFFUSE);
	renderer->bindTexture(renderer->ID_NONE);
	// unbind specular texture
	if (renderer->isSpecularMappingAvailable() == true) {
		renderer->setTextureUnit(LightingShader::TEXTUREUNIT_SPECULAR);
		renderer->bindTexture(renderer->ID_NONE);
	}
	// unbind displacement texture
	if (renderer->isDisplacementMappingAvailable() == true) {
		renderer->setTextureUnit(LightingShader::TEXTUREUNIT_DISPLACEMENT);
		renderer->bindTexture(renderer->ID_NONE);
	}
	// unbind normal texture
	if (renderer->isNormalMappingAvailable()) {
		renderer->setTextureUnit(LightingShader::TEXTUREUNIT_NORMAL);
		renderer->bindTexture(renderer->ID_NONE);
	}
	// set diffuse texture unit
	renderer->setTextureUnit(LightingShader::TEXTUREUNIT_DIFFUSE);
}

const string Object3DVBORenderer::createPseKey(Color4& effectColorAdd, Color4& effectColorMul, bool depthBuffer, bool sort)
{
	auto& efcaData = effectColorAdd.getArray();
	auto& efcmData = effectColorMul.getArray();
	string key =
		to_string(efcmData[0]) +
		"," +
		to_string(efcmData[1]) +
		"," +
		to_string(efcmData[2]) +
		"," +
		to_string(efcmData[3]) +
		"," +
		to_string(efcaData[0]) +
		"," +
		to_string(efcaData[1]) +
		"," +
		to_string(efcaData[2]) +
		"," +
		to_string(efcaData[3]) +
		"," +
		(depthBuffer == true ? "DBT" : "DBF") +
		"," +
		(sort == true ? "DS" : "NS");
	return key;
}

void Object3DVBORenderer::render(const vector<PointsParticleSystemEntity*>& visiblePses)
{
	if (visiblePses.size() == 0) return;

	// store model view matrix
	Matrix4x4 modelViewMatrix;
	modelViewMatrix.set(renderer->getModelViewMatrix());
	//
	auto depthBuffer = false;
	// set up GL state
	renderer->enableBlending();
	renderer->disableDepthBuffer();
	renderer->disableClientState(renderer->CLIENTSTATE_NORMAL_ARRAY);
	renderer->enableClientState(renderer->CLIENTSTATE_COLOR_ARRAY);
	// 	disable texturing client state if not yet done
	if (renderer->renderingTexturingClientState == false) {
		renderer->enableClientState(renderer->CLIENTSTATE_TEXTURECOORD_ARRAY);
		renderer->renderingTexturingClientState = true;
	}
	// 	model view matrix
	renderer->getModelViewMatrix().identity();
	renderer->onUpdateModelViewMatrix();
	// find all keys which differentiate with effect colors and depth buffer
	set<string> pseKeys;
	for (auto i = 0; i < visiblePses.size(); i++) {
		PointsParticleSystemEntityInternal* ppse = visiblePses.at(i);
		string pseKey = createPseKey(ppse->getEffectColorAdd(), ppse->getEffectColorMul(), ppse->isPickable(), ppse->getParticleEmitter()->getColorStart().equals(ppse->getParticleEmitter()->getColorEnd()) == false);
		pseKeys.insert(pseKey);
	}
	// process each key
	for (auto pseKey: pseKeys) {
		auto pseSort = false;
		PointsParticleSystemEntityInternal* currentPse = nullptr;
		// iterate all pses
		for (auto j = 0; j < visiblePses.size(); j++) {
			// check if ppse belongs to current key, otherwise skip this pse
			PointsParticleSystemEntityInternal* ppse = visiblePses.at(j);
			string innerPseKey = createPseKey(ppse->getEffectColorAdd(), ppse->getEffectColorMul(), ppse->isPickable(), ppse->getParticleEmitter()->getColorStart().equals(ppse->getParticleEmitter()->getColorEnd()) == false);
			if (pseKey != innerPseKey) {
				continue;
			} else {
				currentPse = visiblePses.at(j);
				pseSort = ppse->getParticleEmitter()->getColorStart().equals(ppse->getParticleEmitter()->getColorEnd()) == false;
			}
			// merge ppse pool
			pseTransparentRenderPointsPool->merge(ppse->getRenderPointsPool());
		}
		// sort
		if (pseSort == true) pseTransparentRenderPointsPool->sort();
		// put sorted points into batch renderer
		for (auto& point: *pseTransparentRenderPointsPool->getTransparentRenderPoints()) {
			if (point.acquired == false)
				break;

			psePointBatchVBORenderer->addPoint(&point);
		}
		//
		renderer->setEffectColorAdd(currentPse->getEffectColorAdd().getArray());
		renderer->setEffectColorMul(currentPse->getEffectColorMul().getArray());
		renderer->onUpdateEffect();
		depthBuffer = currentPse->isPickable();
		if (depthBuffer) {
			renderer->enableDepthBuffer();
		} else {
			renderer->disableDepthBuffer();
		}
		// render, clear
		psePointBatchVBORenderer->render();
		psePointBatchVBORenderer->clear();
		pseTransparentRenderPointsPool->reset();
	}
	renderer->disableBlending();
	if (depthBuffer == false) renderer->enableDepthBuffer();
	// restore gl state
	renderer->unbindBufferObjects();
	renderer->enableClientState(renderer->CLIENTSTATE_NORMAL_ARRAY);
	renderer->disableClientState(renderer->CLIENTSTATE_COLOR_ARRAY);
	renderer->getModelViewMatrix().set(modelViewMatrix);
}
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DAnimation.h>
#include <tdme/engine/subsystems/rendering/Object3DGroup.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupRenderer.h>
#include <tdme/engine/subsystems/rendering/Object3DInternal.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Quaternion.h>

using std::string;

using tdme::engine::Camera;
using tdme::engine::Entity;
using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::Object3DAnimation;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::Object3DGroupRenderer;
using tdme::engine::subsystems::rendering::Object3DInternal;
using tdme::engine::subsystems::shadowmapping::ShadowMap;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Quaternion;

/** 
 * Object 3D to be used with engine class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Object3D final
	: public Object3DInternal
	, public Entity
{

public:
	enum RenderPass { RENDERPASS_OBJECTS, RENDERPASS_POST_POSTPROCESSING };

private:
	friend class Engine;
	friend class LODObject3D;
	friend class Object3DRenderGroup;
	friend class ObjectParticleSystem;
	friend class SkinnedObject3DRenderGroup;
	friend class tdme::engine::subsystems::shadowmapping::ShadowMap;

	Engine* engine { nullptr };
	Entity* parentEntity { nullptr };
	bool frustumCulling { true };
	string shaderId { "default" };
	string distanceShaderId { "" };
	float distanceShaderDistance { 50.0f };
	RenderPass renderPass { RENDERPASS_OBJECTS };
	bool enableEarlyZRejection { false };
	bool disableDepthTest { false };
	int64_t frameTransformationsLast { -1LL };
	int64_t timeTransformationsLast { -1LL };

	/**
	 * Compute animations
	 * @param context context
	 */
	inline void computeTransformations(void* context) {
		if (getModel()->hasSkinning() == true || getModel()->hasAnimations() == true) {
			auto timing = engine->getTiming();
			auto currentFrameAtTime = timing->getCurrentFrameAtTime();
			auto currentFrame = timing->getFrame();
			auto distanceFromCamera = getTranslation().clone().sub(engine->getCamera()->getLookFrom()).computeLengthSquared();
			if (distanceFromCamera > Math::square(Engine::getTransformationsComputingReduction2Distance())) {
				if (frameTransformationsLast != -1LL && currentFrame - frameTransformationsLast < 4) return;
			} else
			if (distanceFromCamera > Math::square(Math::square(Engine::getTransformationsComputingReduction1Distance()))) {
				if (frameTransformationsLast != -1LL && currentFrame - frameTransformationsLast < 2) return;
			}
			computeTransformations(context, timeTransformationsLast, currentFrameAtTime);
			frameTransformationsLast = timing->getFrame();
			timeTransformationsLast = currentFrameAtTime;
		}
	}

	/**
	 * Compute transformations
	 * @param context context
	 * @param lastFrameAtTime time of last animation computation
	 * @param currentFrameAtTime time of current animation computation
	 */
	inline void computeTransformations(void* context, int64_t lastFrameAtTime, int64_t currentFrameAtTime) override {
		Object3DInternal::computeTransformations(context, lastFrameAtTime, currentFrameAtTime);
	}

	/**
	 * Pre render step like uploading VBOs and such
	 * @param context context
	 */
	inline void preRender(void* context) {
		for (auto object3DGroup: object3dGroups) {
			if (object3DGroup->renderer->needsPreRender() == true) {
				object3DGroup->renderer->preRender(context);
			}
		}
	}

	// overridden methods
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}
	inline Entity* getParentEntity() override {
		return parentEntity;
	}
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		for (auto& transformations: instanceTransformations) transformations.applyParentTransformations(parentTransformations);
		updateBoundingBox();
	}

public:

	void setEngine(Engine* engine) override;
	void setRenderer(Renderer* renderer) override;
	void fromTransformations(const Transformations& transformations) override;
	void update() override;
	void setEnabled(bool enabled) override;
	bool isFrustumCulling() override;
	void setFrustumCulling(bool frustumCulling) override;

	/**
	 * Public constructor
	 * @param id id
	 * @param model model
	 * @param instances instances to compute and render by duplication, which only is intended to be used with skinned meshes
	 */
	Object3D(const string& id, Model* model, int instances);

	/**
	 * Public constructor
	 * @param id id
	 * @param model model
	 */
	Object3D(const string& id, Model* model);

	// overriden methods
	void dispose() override;

	inline BoundingBox* getBoundingBox() override {
		return Object3DInternal::getBoundingBox();
	}

	inline BoundingBox* getBoundingBoxTransformed() override {
		return Object3DInternal::getBoundingBoxTransformed();
	}

	inline const Color4& getEffectColorAdd() const override {
		return Object3DInternal::getEffectColorAdd();
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		return Object3DInternal::setEffectColorAdd(effectColorAdd);
	}

	inline const Color4& getEffectColorMul() const override {
		return Object3DInternal::getEffectColorMul();
	}

	inline void setEffectColorMul(const Color4& effectColorMul) override {
		return Object3DInternal::setEffectColorMul(effectColorMul);
	}

	inline const string& getId() override {
		return Object3DInternal::getId();
	}

	void initialize() override;

	inline bool isContributesShadows() override {
		return Object3DInternal::isContributesShadows();
	}

	inline void setContributesShadows(bool contributesShadows) override {
		Object3DInternal::setContributesShadows(contributesShadows);
	}

	inline bool isReceivesShadows() override {
		return Object3DInternal::isReceivesShadows();
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		Object3DInternal::setReceivesShadows(receivesShadows);
	}

	inline bool isEnabled() override {
		return Object3DInternal::isEnabled();
	}

	inline bool isPickable() override {
		return Object3DInternal::isPickable();
	}

	inline void setPickable(bool pickable) override {
		Object3DInternal::setPickable(pickable);
	}

	inline const Vector3& getTranslation() const override {
		return instanceTransformations[currentInstance].getTranslation();
	}

	inline void setTranslation(const Vector3& translation) override {
		instanceTransformations[currentInstance].setTranslation(translation);
	}

	inline const Vector3& getScale() const override {
		return instanceTransformations[currentInstance].getScale();
	}

	inline void setScale(const Vector3& scale) override {
		instanceTransformations[currentInstance].setScale(scale);
	}

	inline const Vector3& getPivot() const override {
		return instanceTransformations[currentInstance].getPivot();
	}

	inline void setPivot(const Vector3& pivot) override {
		instanceTransformations[currentInstance].setPivot(pivot);
	}

	inline const int getRotationCount() const override {
		return instanceTransformations[currentInstance].getRotationCount();
	}

	inline Rotation& getRotation(const int idx) override {
		return instanceTransformations[currentInstance].getRotation(idx);
	}

	inline void addRotation(const Vector3& axis, const float angle) override {
		instanceTransformations[currentInstance].addRotation(axis, angle);
	}

	inline void removeRotation(const int idx) override {
		instanceTransformations[currentInstance].removeRotation(idx);
	}

	inline const Vector3& getRotationAxis(const int idx) const override {
		return instanceTransformations[currentInstance].getRotationAxis(idx);
	}

	inline void setRotationAxis(const int idx, const Vector3& axis) override {
		instanceTransformations[currentInstance].setRotationAxis(idx, axis);
	}

	inline const float getRotationAngle(const int idx) const override {
		return instanceTransformations[currentInstance].getRotationAngle(idx);
	}

	inline void setRotationAngle(const int idx, const float angle) override {
		instanceTransformations[currentInstance].setRotationAngle(idx, angle);
	}

	inline const Quaternion& getRotationsQuaternion() const override {
		return instanceTransformations[currentInstance].getRotationsQuaternion();
	}

	inline const Matrix4x4& getTransformationsMatrix() const override {
		return instanceTransformations[currentInstance].getTransformationsMatrix();
	}

	inline const Transformations& getTransformations() const override {
		return instanceTransformations[currentInstance];
	}

	/**
	 * @return shader id
	 */
	inline const string& getShader() {
		return shaderId;
	}

	/**
	 * Set shader
	 * @param id shader id
	 */
	inline void setShader(const string& id) {
		shaderId = id;
	}

	/**
	 * @return distance shader id
	 */
	inline const string& getDistanceShader() {
		return distanceShaderId;
	}

	/**
	 * Set distance shader
	 * @param id shader id
	 */
	inline void setDistanceShader(const string& id) {
		distanceShaderId = id;
	}

	/**
	 * @return distance shader distance
	 */
	inline float getDistanceShaderDistance() {
		return distanceShaderDistance;
	}

	/**
	 * Set distance shader distance
	 * @param distanceShaderDistance shader
	 */
	inline void setDistanceShaderDistance(float distanceShaderDistance) {
		this->distanceShaderDistance = distanceShaderDistance;
	}

	/**
	 * @return render pass
	 */
	inline RenderPass getRenderPass() const {
		return renderPass;
	}

	/**
	 * Set render pass
	 * @param renderPass render pass
	 */
	inline void setRenderPass(RenderPass renderPass) {
		this->renderPass = renderPass;
	}

	/**
	 * @return If early z rejection is enabled
	 */
	inline bool isEnableEarlyZRejection() const {
		return enableEarlyZRejection;
	}

	/**
	 * Enable/disable early z rejection
	 * @param enableEarlyZRejection enable early z rejection
	 */
	inline void setEnableEarlyZRejection(bool enableEarlyZRejection) {
		this->enableEarlyZRejection = enableEarlyZRejection;
	}

	/**
	 * @return if depth test is disabled
	 */
	inline bool isDisableDepthTest() const {
		return disableDepthTest;
	}

	/**
	 * Set disable depth test
	 * @param disableDepthTest disable depth test
	 */
	inline void setDisableDepthTest(bool disableDepthTest) {
		this->disableDepthTest = disableDepthTest;
	}

};

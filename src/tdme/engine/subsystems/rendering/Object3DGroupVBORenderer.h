#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>

using std::vector;

using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::Object3DVBORenderer;

/** 
 * Object 3D group render 
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::Object3DGroupVBORenderer final
{
	friend class Object3DVBORenderer;

private:
	Object3DGroup* object3DGroup {  };
	vector<int32_t>* vboBaseIds {  };
	vector<int32_t>* vboTangentBitangentIds {  };
	vector<int32_t>* vboSkinningIds {  };
public:
	void preRender(Object3DVBORenderer* object3DVBORenderer);

	/** 
	 * Disposes the object 3d group
	 * @param gl
	 */
	void dispose();

	/**
	 * Constructor
	 * @param object 3D group
	 */
	Object3DGroupVBORenderer(Object3DGroup* object3DGroup);
};
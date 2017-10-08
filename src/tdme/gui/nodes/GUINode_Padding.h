#pragma once

#include <tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode;


struct default_init_tag;

/** 
 * Padding
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUINode_Padding
{

public:
	int32_t left {  };
	int32_t top {  };
	int32_t right {  };
	int32_t bottom {  };

	// Generated
	GUINode_Padding();
protected:
	GUINode_Padding(const ::default_init_tag&);

private:
	friend class GUINode;
	friend class GUINode_Flow;
	friend class GUINode_AlignmentHorizontal;
	friend class GUINode_AlignmentVertical;
	friend class GUINode_Alignments;
	friend class GUINode_RequestedConstraints;
	friend class GUINode_RequestedConstraints_RequestedConstraintsType;
	friend class GUINode_ComputedConstraints;
	friend class GUINode_Border;
};

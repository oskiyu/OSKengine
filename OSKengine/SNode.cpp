#include "SNode.h"

using namespace OSK::Animation;

void SNode::Clear() {
	for (uint32_t i = 0; i < children.size(); i++)
		children[i].Clear();;
}
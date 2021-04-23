#include "SNode.h"

using namespace OSK::Animation;

void SNode::Clear() {
	for (uint32_t i = 0; i < NumberOfChildren; i++)
		Children[i].Clear();

	if (Children)
		delete[] Children;
}
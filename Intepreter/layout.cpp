#include "structures.h"

/*
 * Created by Stuart Irwin on 28/2/2019.
 * Layout building functions
 */

//Create single SlotNode
SlotNode::SlotNode(SlotType& type, Feature& feature) {
	this->type = type;
	this->feature = &feature;
}

//Create single LayoutNode
LayoutNode::LayoutNode(LayoutType& type) {
	this->type = type;
}

//Add layout inside of LayoutNode
void LayoutNode::operator+=(const Layout& child) {
	inside = &child;
}

//Build layout with single slot
Layout::Layout(SlotType& type, Feature& feature) {
	*this += new SlotNode(type, feature);
}

//Build layout with single LayoutNode
Layout::Layout(LayoutType& type) {
	*this += new LayoutNode(type);
}

//Build layout around existing layout
Layout::Layout(LayoutType& type, Layout& child) {
	*this += new LayoutNode(type);
	*head += child;
}
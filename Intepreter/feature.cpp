#include "structures.h"

/*
 * Created by Stuart Irwin on 27/2/2019.
 * Feature building functions
 */

//Create base FeatureNode
FeatureNode::FeatureNode(FeatureNode& type) {
	this->type = type;
}

//Copy values of FeatureNode
void FeatureNode::copy(const FeatureNode& source) {
	type = source.type;
}

//Create numerical FeatureNode
NumFeatureNode::NumFeatureNode(FeatureType& type, int value1, int value2) {
	FeatureNode(type);
	this->value1 = value1;
	this->value2 = value2;
}

//Copy values of NumFeatureNode
void NumFeatureNode::copy(const NumFeatureNode& source) {
	type = source.type;
	value1 = source.value1;
	value2 = source.value2;
}

//Create pattern based FeatureNode
PatternFeatureNode::PatternFeatureNode(FeatureType& type, Pattern& value) {
	FeatureNode(type);
	this->value = value;
}

//Copy values of PatternFeatureNode
void PatternFeatureNode::copy(const PatternFeatureNode& source) {
	type = source.type;
	value = source.value;
}

//Create single layer Feature
Feature::Feature(FeatureNode& node) {
	head = &node;
}
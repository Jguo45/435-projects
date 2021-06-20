
#include "KDTree.hpp"


KDTree::KDTree(ObjectList* objects) {
	_root = new Node(objects);

	// splits the tree
	splitTree(_root);
}

KDTree::~KDTree() {
	clearTree(_root);
}

void KDTree::clearTree(Node* node) {
	if (node == nullptr) return;

	clearTree(node->_left);
	clearTree(node->_right);
	delete node;
}

void KDTree::splitTree(Node* node) {
	ObjectList* leftList = new ObjectList();
	ObjectList* rightList = new ObjectList();
	float min, max = 0;

	// determines split axis and position
	node->_axis = node->_objects->determineSplitAxis(min, max);
	node->_splitPos = 0.5f * (min + max);

	// loops through all the objects contained within the node and determines if it needs to be split
	for (int i = 0; i < node->_objects->size(); i++) {
		Object* obj = node->_objects->get(i);
		Vec3 center = obj->getCenter();
		float radius = obj->getRadius();
		float minValue = center[node->_axis] - radius;
		float maxValue = center[node->_axis] + radius;

		// object lies on the right side of the split
		if (minValue < node->_splitPos && maxValue < node->_splitPos) {
			leftList->addObject(obj);
			node->_objects->removeObject(i);
			i--;
		}
		// object lies on the left side of the split
		else if (minValue > node->_splitPos && maxValue > node->_splitPos) {
			rightList->addObject(obj);
			node->_objects->removeObject(i);
			i--;
		}
	}
	
	// creates new nodes when necessary
	if (!leftList->empty()) {
		node->_left = new Node(leftList);
		splitTree(node->_left);
	}
	if (!rightList->empty()) {
		node->_right = new Node(rightList);
		splitTree(node->_right);
	}
}

void KDTree::printTree() {
	printTreeRec(_root);
}

void KDTree::printTreeRec(Node* node) {
	if (node == nullptr) return;

	std::cout << node->_axis << " : " << node->_splitPos << " : " << node->_objects->size() << std::endl;
	printTreeRec(node->_left);
	printTreeRec(node->_right);
}

int KDTree::countObjects() {
	return countObjectsRec(_root);
}

int KDTree::countObjectsRec(Node* node) {
	if (node == nullptr) return 0;

	int count = 0;
	count += node->_objects->size();
	count += countObjectsRec(node->_left) + countObjectsRec(node->_right);

	return count;
}

float KDTree::planeIntersect(Ray r, Node* node) {
	// solves for the value t that intersects with the splitting plane
	float t = float((node->_splitPos - r.E[node->_axis])) / float(r.D[node->_axis]);
	return t;
}

void KDTree::traverse(Intersection& intersect, Node* node, Ray r, Vec3 p) {
	if (node == nullptr) return;

	Intersection i = node->_objects->trace(r);

	if (r.near < i.t && i.t < r.far) {
		r.far = i.t;
		intersect = i;
	}

	if (p[node->_axis] < node->_splitPos) {
		traverse(intersect, node->_left, r, p);
		float t = 0;
		t = planeIntersect(r, node);

		if (r.near < t && t < r.far) {
			traverse(intersect, node->_right, r, r.E + t * r.D);
		}
	}
	else {
		traverse(intersect, node->_right, r, p);
		float t = 0;
		t = planeIntersect(r, node);
		if (r.near < t && t < r.far) {
			traverse(intersect, node->_left, r, r.E + t * r.D);
		}
	}
}
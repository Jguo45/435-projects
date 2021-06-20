#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "Vec3.hpp"
#include "Object.hpp"
#include "ObjectList.hpp"
#include "Sphere.hpp"

#include <vector>
#include <iostream>

class Node {
public:
    friend class KDTree;

    Node(ObjectList* objects) {
        _objects = objects;
        _left = nullptr;
        _right = nullptr;
        _axis = -1;
        _splitPos = 0;
    }

public:
    ObjectList* _objects;
    Node* _left;    // objects to the left of the splitting plane
    Node* _right;   // objects to the right of the splitting plane
    int _axis;      // 0 = x, 1 = y, 2 = z
    float _splitPos;
};


class KDTree {
public:
     KDTree(ObjectList* objects);
    ~KDTree();

    void splitTree(Node* node);     // recursively splits the tree

    void printTree();               // prints the elements of the tree
    void printTreeRec(Node* node);  // recursive helper
    int countObjects();             // returns the number of objects in the tree
    int countObjectsRec(Node* node);// recursive helper
    void clearTree(Node* node);     // deletes tree nodes
    
    float planeIntersect(Ray r, Node* node);    // intersection point of ray and plane
    void traverse(Intersection& intersect, Node* node, Ray r, Vec3 p);  // traverses the tree to find the closest object

public:
    Node* _root;
};

#endif
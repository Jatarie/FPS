#pragma once

#include "game.h"
#include "DataStructures.h"

b32 BoxIntersection(v3 a, Box b);
b32 BoxIntersection(Entity* a, Box b);
b32 BoxIntersection(Entity* a, Entity* b);

struct OctTree{
	u32 depth;
	OctTree** children;
	Array members;
	Box bounding_box;

	void Init(u32 depth, Box bounding_box) {
		this->depth = depth;
		this->bounding_box = bounding_box;
		members = Array(1, sizeof(Entity*));
		if(depth == 0){
			return;
		}
		this->children = (OctTree**)Malloc(sizeof(OctTree*) * 8);

		Box child_box = bounding_box;
		child_box.max.x = Ave(child_box.max.x, child_box.min.x);
		child_box.max.y = Ave(child_box.max.y, child_box.min.y);
		child_box.max.z = Ave(child_box.max.z, child_box.min.z);
		children[0] = (OctTree*)Malloc(sizeof(OctTree));
		children[0]->depth = depth - 1;
		children[0]->bounding_box = child_box;
		children[0]->Init(depth - 1, child_box);

		child_box = bounding_box;
		child_box.min.x = (child_box.min.x + child_box.max.x) / 2.0f;
		child_box.max.y = Ave(child_box.max.y, child_box.min.y);
		child_box.max.z = Ave(child_box.max.z, child_box.min.z);
		children[1] = (OctTree*)Malloc(sizeof(OctTree));
		children[1]->depth = depth - 1;
		children[1]->bounding_box = child_box;
		children[1]->Init(depth - 1, child_box);

		child_box = bounding_box;
		child_box.min.y = (child_box.min.y + child_box.max.y) / 2.0f;
		child_box.max.x = Ave(child_box.max.x, child_box.min.x);
		child_box.max.z = Ave(child_box.max.z, child_box.min.z);
		children[2] = (OctTree*)Malloc(sizeof(OctTree));
		children[2]->depth = depth - 1;
		children[2]->bounding_box = child_box;
		children[2]->Init(depth - 1, child_box);

		child_box = bounding_box;
		child_box.min.z = (child_box.min.z + child_box.max.z) / 2.0f;
		child_box.max.x = Ave(child_box.max.x, child_box.min.x);
		child_box.max.y = Ave(child_box.max.y, child_box.min.y);
		children[3] = (OctTree*)Malloc(sizeof(OctTree));
		children[3]->depth = depth - 1;
		children[3]->bounding_box = child_box;
		children[3]->Init(depth - 1, child_box);

		child_box = bounding_box;
		child_box.min.x = (child_box.min.x + child_box.max.x) / 2.0f;
		child_box.min.y = (child_box.min.y + child_box.max.y) / 2.0f;
		child_box.max.z = Ave(child_box.max.z, child_box.min.z);
		children[4] = (OctTree*)Malloc(sizeof(OctTree));
		children[4]->depth = depth - 1;
		children[4]->bounding_box = child_box;
		children[4]->Init(depth - 1, child_box);
		
		child_box = bounding_box;
		child_box.min.x = (child_box.min.x + child_box.max.x) / 2.0f;
		child_box.min.z = (child_box.min.z + child_box.max.z) / 2.0f;
		child_box.max.y = Ave(child_box.max.y, child_box.min.y);
		children[5] = (OctTree*)Malloc(sizeof(OctTree));
		children[5]->depth = depth - 1;
		children[5]->bounding_box = child_box;
		children[5]->Init(depth - 1, child_box);

		child_box = bounding_box;
		child_box.min.y = (child_box.min.y + child_box.max.y) / 2.0f;
		child_box.min.z = (child_box.min.z + child_box.max.z) / 2.0f;
		child_box.max.x = Ave(child_box.max.x, child_box.min.x);
		children[6] = (OctTree*)Malloc(sizeof(OctTree));
		children[6]->depth = depth - 1;
		children[6]->bounding_box = child_box;
		children[6]->Init(depth - 1, child_box);
		
		child_box = bounding_box;
		child_box.min.x = (child_box.min.x + child_box.max.x) / 2.0f;
		child_box.min.y = (child_box.min.y + child_box.max.y) / 2.0f;
		child_box.min.z = (child_box.min.z + child_box.max.z) / 2.0f;
		children[7] = (OctTree*)Malloc(sizeof(OctTree));
		children[7]->depth = depth - 1;
		children[7]->bounding_box = child_box;
		children[7]->Init(depth - 1, child_box);
	}

	void Insert(Entity* e){
		members.Add((void*) (&e));
		if (depth > 0) {
			for (u32 i = 0; i < 8; i++) {
				if (BoxIntersection(e, children[i]->bounding_box)) {
					children[i]->Insert(e);
				}
			}
		}
	}
};

b32 CheckCollisionRayFace(v3 point_1, v3 point_2, v3 ray_direction, b32 positive_normal);
b32 CheckCollisionRayFace(v3 point_1, v3 point_2, v3 normal, v3 ray_direction);
b32 CheckCollisionRayBox(Raycast ray, Box box);
b32 RayEntityCollision(Raycast ray, Entity* entity);
Entity* CheckCollisionRayWorld(Raycast ray, OctTree* tree);
Entity* CheckCollision(Entity* a, OctTree* tree);
Entity* CheckCollision(v3 pos, Box bounding_box, OctTree* tree);
b32 WithinBounds(Entity* a, Entity* b, Dimension d);

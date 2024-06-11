#include "mygl.h"
#include "lib.h"
#include "collision.h"

b32 BoxIntersection(v3 a, Box b){
	if (a.x < b.min.x){
		return false;
	}
	if (a.y < b.min.y){
		return false;
	}
	if (a.z < b.min.z){
		return false;
	}
	if (a.x > b.max.x){
		return false;
	}
	if (a.y > b.max.y){
		return false;
	}
	if (a.z > b.max.z){
		return false;
	}
	return true;
}

b32 BoxIntersection(Entity* a, Box b){
	if (a->world_p.x + a->bounding_box.max.x < b.min.x){
		return false;
	}
	if (a->world_p.y + a->bounding_box.max.y < b.min.y){
		return false;
	}
	if (a->world_p.z + a->bounding_box.max.z < b.min.z){
		return false;
	}
	if (a->world_p.x + a->bounding_box.min.x > b.max.x){
		return false;
	}
	if (a->world_p.y + a->bounding_box.min.y > b.max.y){
		return false;
	}
	if (a->world_p.z + a->bounding_box.min.z > b.max.z){
		return false;
	}
	return true;
}

b32 BoxIntersection(Entity* a, Entity* b){
	TIMED_FUNCTION
	if (a->world_p.x + a->bounding_box.max.x < b->world_p.x + b->bounding_box.min.x){
		return false;
	}
	if (a->world_p.y + a->bounding_box.max.y < b->world_p.y + b->bounding_box.min.y){
		return false;
	}
	if (a->world_p.z + a->bounding_box.max.z < b->world_p.z + b->bounding_box.min.z){
		return false;
	}
	if (a->world_p.x + a->bounding_box.min.x > b->world_p.x + b->bounding_box.max.x){
		return false;
	}
	if (a->world_p.y + a->bounding_box.min.y > b->world_p.y + b->bounding_box.max.y){
		return false;
	}
	if (a->world_p.z + a->bounding_box.min.z > b->world_p.z + b->bounding_box.max.z){
		return false;
	}
	return true;
}

b32 CheckCollisionRayFace(v3 point_1, v3 point_2, v3 ray_direction, b32 positive_normal){
	b32 intersection = true;
	r32 ratio;
	if(positive_normal){
		ratio = point_2.x / ray_direction.x;
	}
	else{
		ratio = point_1.x / ray_direction.x;
	}
	if(ratio < 0){
		return false;
	}
	ray_direction *= ratio;
	if(ray_direction.y > point_2.y){
		intersection = false;
	}
	if(ray_direction.z > point_2.z){
		intersection = false;
	}
	if(ray_direction.y < point_1.y){
		intersection = false;
	}
	if(ray_direction.z < point_1.z){
		intersection = false;
	}
	return intersection;

}

b32 CheckCollisionRayFace(v3 point_1, v3 point_2, v3 normal, v3 ray_direction) {
	b32 intersection = true;

	if(normal == v3Left()){
		intersection = CheckCollisionRayFace(point_1, point_2, ray_direction, false);
	}

	else if(normal == v3Right()){
		intersection = CheckCollisionRayFace(point_1, point_2, ray_direction, true);
	}

	else if(normal == v3Down()){
		intersection = CheckCollisionRayFace(point_1.yxz(), point_2.yxz(), ray_direction.yxz(), false);
	}

	else if(normal == v3Up()){
		intersection = CheckCollisionRayFace(point_1.yxz(), point_2.yxz(), ray_direction.yxz(), true);
	}

	else if(normal == v3Forward()){
		intersection = CheckCollisionRayFace(point_1.zxy(), point_2.zxy(), ray_direction.zxy(), true);
	}

	else if(normal == v3Backwards()){
		intersection = CheckCollisionRayFace(point_1.zxy(), point_2.zxy(), ray_direction.zxy(), false);
	}

	return intersection;
}

b32 CheckCollisionRayBox(Raycast ray, Box box){

	box.min -= ray.origin;
	box.max -= ray.origin;

	b32 collision = BoxIntersection(ray.origin, box);
	collision = collision || ((ray.origin.x < box.min.x) && CheckCollisionRayFace(v3 { box.min.x, box.min.y, box.min.z }, v3 { box.min.x, box.max.y, box.max.z }, v3Left(), ray.direction));
	collision = collision || ((ray.origin.x > box.max.x) && CheckCollisionRayFace(v3 { box.max.x, box.min.y, box.min.z }, v3 { box.max.x, box.max.y, box.max.z }, v3Right(), ray.direction));
	collision = collision || ((ray.origin.y > box.max.y) && CheckCollisionRayFace(v3 { box.min.x, box.max.y, box.min.z }, v3 { box.max.x, box.max.y, box.max.z }, v3Up(), ray.direction));
	collision = collision || ((ray.origin.y < box.min.y) && CheckCollisionRayFace(v3 { box.min.x, box.min.y, box.min.z }, v3 { box.max.x, box.min.y, box.max.z }, v3Down(), ray.direction));
	collision = collision || ((ray.origin.z > box.max.z) && CheckCollisionRayFace(v3 { box.min.x, box.min.y, box.max.z }, v3 { box.max.x, box.max.y, box.max.z }, v3Backwards(), ray.direction));
	collision = collision || ((ray.origin.z < box.min.z) && CheckCollisionRayFace(v3 { box.min.x, box.min.y, box.min.z }, v3 { box.max.x, box.max.y, box.min.z }, v3Forward(), ray.direction));
	return collision;
}

b32 CheckCollisionRayEntity(Raycast ray, Entity* entity){
	Box box;
	box.min = entity->world_p + entity->bounding_box.min;
	box.max = entity->world_p + entity->bounding_box.max;
	return CheckCollisionRayBox(ray, box);
}


Entity* CheckCollisionRayWorld(Raycast ray, OctTree* tree) {
	TIMED_FUNCTION;

	b32 collision = CheckCollisionRayBox(ray, tree->bounding_box);

	Entity* e = NULL;
	if (collision) {
		if (!tree->children) {
			for(int i = 0; i < tree->members.count; i++){
				Entity* test_entity = ((Entity**)tree->members.data)[i];
				if (CheckCollisionRayEntity(ray, test_entity)) {
					return test_entity;
				}
			}
		}
		else {
			for (u32 i = 0; i < 8; i++) {
				e = CheckCollisionRayWorld(ray, tree->children[i]);
				if (e) {
					break;
				}
			}
		}
	}
	return e;
}

Entity* CheckCollision(Entity* a, OctTree* tree) {
	TIMED_FUNCTION
	v3 a_low, a_high;
	Entity* e = NULL;

	a_low.x = a->world_p.x + a->bounding_box.min.x;
	a_low.y = a->world_p.y + a->bounding_box.min.y;
	a_low.z = a->world_p.z + a->bounding_box.min.z;

	a_high.x = a->world_p.x + a->bounding_box.max.x;
	a_high.y = a->world_p.y + a->bounding_box.max.y;
	a_high.z = a->world_p.z + a->bounding_box.max.z;

	if(tree->depth == 0){
		for(u32 i = 0; i < tree->members.count; i++){
			Entity* test = ((Entity**)(tree->members.data))[i];
			if(BoxIntersection(a, test)){
				return test;
			}
		}
	}
	else{
		for(u32 i = 0; i < 8; i ++){
			OctTree* child = tree->children[i];
			if (BoxIntersection(a, child->bounding_box)){
				return CheckCollision(a, child);
			}
		}
	}
	return e;
}

Entity* CheckCollision(v3 pos, Box bounding_box, OctTree* tree) {
	TIMED_FUNCTION
	Entity a;
	a.world_p = pos;
	a.bounding_box = bounding_box;
	return CheckCollision(&a, tree);
}

//b32 WithinBounds(Entity* a, Entity* b, Dimension d){
//	TIMED_FUNCTION
//	if(d == Dimension_x){
//		if((a->world_p.x + a->bounding_box.max.x) > (b->world_p.x + b->bounding_box.min.x) && (a->world_p.x + a->bounding_box.max.x) < (b->world_p.x + b->bounding_box.max.x)){
//			return true;
//		}
//		if((a->world_p.x + a->bounding_box.min.x) > (b->world_p.x + b->bounding_box.min.x) && (a->world_p.x + a->bounding_box.min.x) < (b->world_p.x + b->bounding_box.max.x)){
//			return true;
//		}
//	}
//	else if(d == Dimension_y){
//		if((a->world_p.y + a->bounding_box.max.y) > (b->world_p.y + b->bounding_box.min.y) && (a->world_p.y + a->bounding_box.max.y) < (b->world_p.y + b->bounding_box.max.y)){
//			return true;
//		}
//		if((a->world_p.y + a->bounding_box.min.y) > (b->world_p.y + b->bounding_box.min.y) && (a->world_p.y + a->bounding_box.min.y) < (b->world_p.y + b->bounding_box.max.y)){
//			return true;
//		}
//	}
//	else if(d == Dimension_z){
//		if((a->world_p.z + a->bounding_box.max.z) > (b->world_p.z + b->bounding_box.min.z) && (a->world_p.z + a->bounding_box.max.z) < (b->world_p.z + b->bounding_box.max.z)){
//			return true;
//		}
//		if((a->world_p.z + a->bounding_box.min.z) > (b->world_p.z + b->bounding_box.min.z) && (a->world_p.z + a->bounding_box.min.z) < (b->world_p.z + b->bounding_box.max.z)){
//			return true;
//		}
//	}
//	return false;
//}
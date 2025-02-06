#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera) : mesh(mesh), camera(camera)
{
	transform = std::shared_ptr<Transform>(new Transform());
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

void Entity::SetMesh(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
};

std::shared_ptr<Transform> Entity::GetTransform()
{
	return transform;
}

#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera, std::shared_ptr<Material> material) : mesh(mesh), camera(camera), material(material)
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

std::shared_ptr<Material> Entity::GetMaterial()
{
	return material;
}

void Entity::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

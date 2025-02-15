#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"

class Entity
{
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Material> material;

public:
	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera, std::shared_ptr<Material> material);

	std::shared_ptr<Mesh> GetMesh();
	void SetMesh(std::shared_ptr<Mesh> mesh);

	std::shared_ptr<Transform> GetTransform();

	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> material);
};


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
	Material material;

public:
	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera, Material material);

	std::shared_ptr<Mesh> GetMesh();
	void SetMesh(std::shared_ptr<Mesh> mesh);

	std::shared_ptr<Transform> GetTransform();

	Material* GetMaterial();
	void SetMaterial(Material material);
};


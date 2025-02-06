#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera);

	std::shared_ptr<Mesh> GetMesh();
	void SetMesh(std::shared_ptr<Mesh> mesh);

	std::shared_ptr<Transform> GetTransform();

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Camera> camera;
};


#pragma once

// Engine
#include <Object3D.h>

class Field
{
public:
	void Initialize();
	void Update();
	void Draw();

	void Debug();

private:
	// オブジェクト
	ModelManager::ModelData modelFloor_;
	std::unique_ptr<Object3D> objectFloor_;
};


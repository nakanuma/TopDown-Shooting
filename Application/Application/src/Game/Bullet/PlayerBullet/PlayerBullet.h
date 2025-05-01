#pragma once

// Application
#include <src/Game/Bullet/Bullet.h>

class PlayerBullet : public Bullet {
public:
	void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

#pragma once

// Application
#include <src/Game/Bullet/Base/Bullet.h>

class PlayerBullet : public Bullet {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
};

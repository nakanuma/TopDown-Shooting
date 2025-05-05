#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

/// <summary>
/// ’Êí“G
/// </summary>
class NormalEnemy : public Enemy
{
public:
	/// <summary>
	/// ‰Šú‰»ˆ—
	/// </summary>
	void Initialize(const Float3& position, ModelManager::ModelData* model) override;

	/// <summary>
	/// XVˆ—
	/// </summary>
	void Update() override;
	
	/// <summary>
	/// •`‰æˆ—
	/// </summary>
	void Draw() override;
};


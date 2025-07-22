#pragma once

// C++
#include <memory>

// Application
#include <src/Game/Bullet/Base/Bullet.h>

/// <summary>
/// 弾の管理クラス
/// </summary>
class BulletManager {
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	static BulletManager* GetInstance();

	/// <summary>
	/// 弾の追加
	/// </summary>
	void AddBullet(std::unique_ptr<Bullet> bullet);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	// 弾リスト
	std::vector<std::unique_ptr<Bullet>> bullets_;
};

#include "BulletManager.h"

// ---------------------------------------------------------
// インスタンス取得
// ---------------------------------------------------------
BulletManager* BulletManager::GetInstance() { 
	static BulletManager instance;
	return &instance;
}

// ---------------------------------------------------------
// 弾の追加
// ---------------------------------------------------------
void BulletManager::AddBullet(std::unique_ptr<Bullet> bullet) { 
	bullets_.push_back(std::move(bullet)); 
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void BulletManager::Update() {
	// 全ての弾を更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// 弾の削除処理
	for (auto& bullet : bullets_) {
		if (bullet->IsDead()) {
			bullet->OnDestroy();
		}
	}
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), 
		[](const std::unique_ptr<Bullet>& bullet) { return bullet->IsDead(); }), 
		bullets_.end());
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void BulletManager::Draw() { 
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

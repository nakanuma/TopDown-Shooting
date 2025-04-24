#pragma once

// Engine
#include <Object3D.h>
#include <Input/Input.h>

// Application
#include <src/Game/Bullet/PlayerBullet/PlayerBullet.h>

class Player
{
public:
	void Initialize();
	void Update();
	void Draw();

private:
	/* プレイヤー動作 */


	/* その他 */

	// デバッグ表示
	void Debug();
	// カーソル方向の計算
	Float3 CaluclateCursorDirection();

private:
	// 基盤機能
	Input* input_ = nullptr;

	// オブジェクト
	ModelManager::ModelData modelPlayer_;
	std::unique_ptr<Object3D> objectPlayer_;

	// パラメーター
	Float3 velocity_ = {0.0f, 0.0f, 0.0f};
	float speed_ = 0.25f;

	// 未分類
	std::vector<std::unique_ptr<PlayerBullet>> bullets_; // 弾リスト
};


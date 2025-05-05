#pragma once

// Engine
#include <Object3D.h>
#include <Input/Input.h>
#include <SpriteCommon.h>
#include <Sprite.h>

// Application
#include <src/Game/Bullet/PlayerBullet/PlayerBullet.h>

/// <summary>
/// プレイヤー
/// </summary>
class Player
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// UI描画処理
	/// </summary>
	void DrawUI();

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// カーソル位置のワールド座標を取得
	/// </summary>
	Float3 CalclateCursorPosition();

	/// <summary>
	/// 移動処理
	/// </summary>
	void HandleMove();

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	void HandleShooting();

private:
	// ---------------------------------------------------------
	// システム関連
	// ---------------------------------------------------------

	// 入力
	Input* input_ = nullptr;

	// スプライト基盤
	std::unique_ptr<SpriteCommon> spriteCommon_;

	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// プレイヤーモデル
	ModelManager::ModelData modelPlayer_;
	// プレイヤーオブジェクト
	std::unique_ptr<Object3D> objectPlayer_;

	// プレイヤー弾モデル
	ModelManager::ModelData modelBullet_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// 速度
	Float3 velocity_ = {0.0f, 0.0f, 0.0f};

	// 速さ
	float speed_ = 0.25f;

	// ---------------------------------------------------------
	// スプライト
	// ---------------------------------------------------------

	std::unique_ptr<Sprite> spriteTarget_;

	// ---------------------------------------------------------
	// 未分類
	// ---------------------------------------------------------

	// 弾リスト
	std::vector<std::unique_ptr<PlayerBullet>> bullets_;
};


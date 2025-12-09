#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <memory>
#include <unordered_map>
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <src/Game/GameState/Base/IGameState.h>

// =========================================================
// ゲーム状態管理クラス
// =========================================================
class GameStateManager
{
public:
	/// <summary>
	/// 状態を登録します。
	/// </summary>
	/// <param name="name">状態名</param>
	/// <param name="state">状態オブジェクト</param>
	void RegisterState(const std::string& name, std::unique_ptr<IGameState> state);

	/// <summary>
	/// 状態を変更します。
	/// </summary>
	/// <param name="name"></param>
	void ChangeState(const std::string& name);

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// 通常モデルのシャドウマップ描画処理を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// スキニングモデルのシャドウマップ描画処理を行います。
	/// </summary>
	void DrawShadowSkinning();

	/// <summary>
	/// UI描画処理を行います。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// デバッグ表示を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 現在の状態を取得します。
	/// </summary>
	/// <returns></returns>
	IGameState* GetCurrentState() const { return currentState_; }

	/// <summary>
	/// 現在の状態名を取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetCurrentStateName() const { return currentStateName_; }

private:
	// =========================================================
	// Member Variables
	// =========================================================
	std::unordered_map<std::string, std::unique_ptr<IGameState>> states_;	// 名前で状態を保持
	IGameState* currentState_ = nullptr;									// 現在の状態
	std::string currentStateName_;											// 現在の状態の名前
};


#pragma once

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Loader/Loader.h>
#include <src/Game/EventTrigger/EventTrigger.h>

// =========================================================
// イベントトリガー管理クラス
// =========================================================
class EventManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="datas"></param>
	void Initialize(const std::vector<Loader::TransformData>& datas);

	/// <summary>
	/// 毎フレーム更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// データのクリアを行います。
	/// </summary>
	void Clear();

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// 特定のIDを持つトリガーが引かれたかをチェックする
	/// </summary>
	/// <param name="eventID"></param>
	/// <returns></returns>
	bool CheckTrigger(const std::string& eventID);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Container -----
	std::vector<std::unique_ptr<EventTrigger>> triggers_; /* イベントトリガーを格納したコンテナ */
};

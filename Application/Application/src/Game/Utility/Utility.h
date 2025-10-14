#pragma once

// Engine
#include <MyMath.h>

/// <summary>
/// 汎用ユーティリティクラス
/// </summary>
class Utility {
public:
	/// <summary>
	/// ワールド座標をスクリーン座標に変換
	/// </summary>
	static Float3 WorldToScreen(Float3 worldPosition);

	/// <summary>
	/// カーソル位置のワールド座標を取得
	/// </summary>
	static Float3 CalclateCursorPosition();
};

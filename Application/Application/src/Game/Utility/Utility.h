#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>

// =========================================================
// 汎用ユーティリティクラス
// =========================================================
class Utility {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// ワールド座標をスクリーン座標に変換します。
	/// </summary>
	/// <param name="worldPosition">ワールド座標</param>
	/// <returns>スクリーン座標</returns>
	static Float3 WorldToScreen(Float3 worldPosition);

	/// <summary>
	/// カーソル位置のワールド座標を取得します。
	/// </summary>
	/// <returns>カーソル位置のワールド座標</returns>
	static Float3 CalculateCursorPosition();

	/// <summary>
	/// カーソルがクライアント領域内にあるか確認します。
	/// </summary>
	/// <returns>カーソル領域内判定フラグ</returns>
	static bool IsInsideClientCursor();
};

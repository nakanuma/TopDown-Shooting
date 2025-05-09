#pragma once

// Engine
#include <MyMath.h>

class Utility
{
public:
	/// <summary>
	/// ワールド座標をスクリーン座標に変換
	/// </summary>
	static Float3 WorldToScreen(Float3 worldPosition);
};


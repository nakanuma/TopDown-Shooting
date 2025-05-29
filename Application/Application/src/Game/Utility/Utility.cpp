#include "Utility.h"

// Engine
#include <Camera.h>
#include <Engine/Input/Input.h>

// ---------------------------------------------------------
// ワールド座標をスクリーン座標に変換
// ---------------------------------------------------------
Float3 Utility::WorldToScreen(Float3 worldPosition)
{
	Matrix worldViewProjMatrix = Camera::GetCurrent()->GetViewProjectionMatrix();
	Float3 screenPosition = Float3::Transform(worldPosition, worldViewProjMatrix);

	screenPosition.x = (screenPosition.x + 1.0f) * 0.5f * Window::GetWidth();
	screenPosition.y = (1.0f - screenPosition.y) * 0.5f * Window::GetHeight();

	return screenPosition;
}

// ---------------------------------------------------------
// ワールド座標をスクリーン座標に変換
// ---------------------------------------------------------
Float3 Utility::CalclateCursorPosition() 
{
	// マウス位置の取得
	Float2 mousePos = Float2(Input::GetInstance()->GetMousePosition().x, Input::GetInstance()->GetMousePosition().y);

	// 画面サイズ取得
	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	// スクリーン座標 -> 正規化デバイス座標（NDC）へ変換
	float ndcX = (2.0f * mousePos.x / screenWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y / screenHeight);

	// NDC -> ワールド空間へ変換
	Float4 clipNear = Float4(ndcX, ndcY, 0.0f, 1.0f);
	Float4 clipFar = Float4(ndcX, ndcY, 1.0f, 1.0f);

	Matrix matVPInv = Matrix::Inverse(Camera::GetCurrent()->GetViewProjectionMatrix());

	// ワールド座標に変換
	Float4 worldNear = Float4::Transform(clipNear, matVPInv);
	Float4 worldFar = Float4::Transform(clipFar, matVPInv);

	// w除算で正しい座標に変換
	worldNear /= worldNear.w;
	worldFar /= worldFar.w;

	// レイの始点と方向
	Float3 rayOrigin = {worldNear.x, worldNear.y, worldNear.z};
	Float3 rayDir = Float3::Normalize({worldFar.x - worldNear.x, worldFar.y - worldNear.y, worldFar.z - worldNear.z});

	if (std::abs(rayDir.y) > 0.0001f) {
		float t = -rayOrigin.y / rayDir.y;

		Float3 hitPos = {rayOrigin.x + rayDir.x * t, 1.0f, rayOrigin.z + rayDir.z * t};

		return hitPos;
	}

	// 計算できない場合には無効値を返す
	return Float3(0.0f, 0.0f, 0.0f);
}

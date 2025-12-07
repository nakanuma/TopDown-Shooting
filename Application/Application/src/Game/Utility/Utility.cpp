#include "Utility.h"

// Engine
#include <Camera.h>
#include <Engine/Input/Input.h>

Cygnus::Float3 Utility::WorldToScreen(const Cygnus::Float3& worldPosition) {
	// ビュー射影行列を取得
	Cygnus::Matrix worldViewProjMatrix = Cygnus::Camera::GetCurrent()->GetViewProjectionMatrix();
	// ワールド->クリップへの座標変換
	Cygnus::Float3 screenPosition = Cygnus::Float3::Transform(worldPosition, worldViewProjMatrix);
	// NDC->スクリーンへの座標変換
	screenPosition.x = (screenPosition.x + 1.0f) * 0.5f * Cygnus::Window::GetWidth();
	screenPosition.y = (1.0f - screenPosition.y) * 0.5f * Cygnus::Window::GetHeight();

	return screenPosition;
}

Cygnus::Float3 Utility::CalculateCursorPosition() {
	// マウス位置の取得
	Cygnus::Float2 mousePos = {static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().x), static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().y)};

	// 画面サイズ取得
	float screenWidth = static_cast<float>(Cygnus::Window::GetWidth());
	float screenHeight = static_cast<float>(Cygnus::Window::GetHeight());

	// スクリーン座標 -> 正規化デバイス座標（NDC）へ変換
	float ndcX = (2.0f * mousePos.x / screenWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y / screenHeight);

	// NDC -> ワールド空間へ変換
	Cygnus::Float4 clipNear = Cygnus::Float4(ndcX, ndcY, 0.0f, 1.0f);
	Cygnus::Float4 clipFar = Cygnus::Float4(ndcX, ndcY, 1.0f, 1.0f);

	Cygnus::Matrix matVPInv = Cygnus::Matrix::Inverse(Cygnus::Camera::GetCurrent()->GetViewProjectionMatrix());

	// ワールド座標に変換
	Cygnus::Float4 worldNear = Cygnus::Float4::Transform(clipNear, matVPInv);
	Cygnus::Float4 worldFar = Cygnus::Float4::Transform(clipFar, matVPInv);

	// w除算で正しい座標に変換
	worldNear /= worldNear.w;
	worldFar /= worldFar.w;

	// レイの始点と方向
	Cygnus::Float3 rayOrigin = {worldNear.x, worldNear.y, worldNear.z};
	Cygnus::Float3 rayDir = Cygnus::Float3::Normalize({worldFar.x - worldNear.x, worldFar.y - worldNear.y, worldFar.z - worldNear.z});

	if (std::abs(rayDir.y) > kBlink) {
		float t = -rayOrigin.y / rayDir.y;

		Cygnus::Float3 hitPos = {rayOrigin.x + rayDir.x * t, 1.0f, rayOrigin.z + rayDir.z * t};

		return hitPos;
	}

	// 計算できない場合には無効値を返す
	return Cygnus::Float3(0.0f, 0.0f, 0.0f);
}

bool Utility::IsInsideClientCursor() {
	// ウインドウハンドル取得
	HWND hwnd = Cygnus::Window::GetHandle();

	// マウス位置の取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// クライアント座標に変換
	ScreenToClient(hwnd, &cursorPos);

	// クライアント領域の取得
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// クライアント領域内にあるか判定
	return {cursorPos.x >= 0.0f && cursorPos.x < (clientRect.right - clientRect.left) && cursorPos.y >= 0.0f && cursorPos.y < (clientRect.bottom - clientRect.top)};
}

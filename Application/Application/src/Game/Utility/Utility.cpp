#include "Utility.h"

// Engine
#include <Camera.h>

// ---------------------------------------------------------
// ���[���h���W���X�N���[�����W�ɕϊ�
// ---------------------------------------------------------
Float3 Utility::WorldToScreen(Float3 worldPosition)
{
	Matrix worldViewProjMatrix = Camera::GetCurrent()->GetViewProjectionMatrix();
	Float3 screenPosition = Float3::Transform(worldPosition, worldViewProjMatrix);

	screenPosition.x = (screenPosition.x + 1.0f) * 0.5f * Window::GetWidth();
	screenPosition.y = (1.0f - screenPosition.y) * 0.5f * Window::GetHeight();

	return screenPosition;
}

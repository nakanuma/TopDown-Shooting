#pragma once

// Engine
#include <MyMath.h>

class Utility
{
public:
	/// <summary>
	/// ���[���h���W���X�N���[�����W�ɕϊ�
	/// </summary>
	static Float3 WorldToScreen(Float3 worldPosition);

	/// <summary>
	/// �J�[�\���ʒu�̃��[���h���W���擾
	/// </summary>
	static Float3 CalclateCursorPosition();
};


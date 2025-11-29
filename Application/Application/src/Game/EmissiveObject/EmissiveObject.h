#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Object3D.h>

// =========================================================
// 発光オブジェクトクラス（テスト）
// =========================================================
class EmissiveObject {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	void Initialize();
	void Update();
	void Draw();
	void Debug();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kEmissiveColor = {1.0f, 0.5f, 0.0f};	/* 発光色 */
	static constexpr float kEmissiveIntensity = 2.0f;				/* 発光強度 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Object3D> objectRectangleLight_;
	std::unique_ptr<Object3D> objectDiskLight_;
	std::unique_ptr<Object3D> objectTubeLight_;
	std::unique_ptr<Object3D> objectSphereLight_;
};
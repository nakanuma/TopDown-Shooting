#include "NumberSprite.h"

// C++
#include <iomanip>
#include <sstream>

// Engine
#include <Engine/Texture/TextureManager.h>

void NumberSprite::Initialize(float value, uint32_t decimalPlaces) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// スプライト基盤
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxBase);

	// テクスチャ読み込み
	uint32_t texture = TextureManager::Load("UI/number.png");

	// 値を文字列化
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(decimalPlaces) << value; // 小数点以下の桁数を指定
	std::string str = oss.str();                                    // 文字列を取得

	// 桁数の確定
	digitCount_ = static_cast<uint32_t>(str.size());
	digits_.resize(digitCount_);
	digitValues_.resize(digitCount_);

	// 引数で受け取った桁数のスプライトを生成
	digits_.resize(digitCount_);
	for (uint32_t i = 0; i < digitCount_; i++) {
		char c = str[i];
		uint32_t index = 0;

		if (c >= '0' && c <= '9')
			index = c - '0'; // 0~9の値だった場合には'0'のASCIIコードを引いて数値を格納
		else if (c == '.')
			index = kDecimalPointIndex; // 小数点だった場合には10を指定（連番テクスチャの最後に配置しているため）
		else
			continue;

		digitValues_[i] = index;

		digits_[i] = std::make_unique<Sprite>();
		digits_[i]->Initialize(spriteCommon_.get(), texture);
		digits_[i]->SetSize(kDigitSize);
		digits_[i]->SetTextureSize(kDigitSize);
		digits_[i]->SetAnchorPoint(kAnchorPoint);
	}
}

void NumberSprite::Update(Float2 position) {
	if (digitCount_ == 0)
		return;

	// 全体幅を計算
	float totalWidth = 0.0f;
	for (uint32_t i = 0; i < digitCount_; i++) {
		if (digitValues_[i] == kDecimalPointIndex)
			totalWidth += kDigitSize.x * kDecimalPointWidthRatio; // 小数点ならかなり幅を狭める
		else if (digitValues_[i] == 1)
			totalWidth += kDigitSize.x * kDigitOneWidthRatio; // 1なら少し幅を狭める
		else
			totalWidth += kDigitSize.x; // それ以外は同じ幅
	}

	// 中心位置を計算
	Float2 startPos = position - Float2{totalWidth / 2.0f, 0.0f};

	// 各文字の位置を決定
	float offsetX = 0.0f;
	for (uint32_t i = 0; i < digitCount_; i++) {
		float width;
		if (digitValues_[i] == kDecimalPointIndex)
			width = kDigitSize.x * kDecimalPointWidthRatio; // 小数点ならかなり幅を狭める
		else if (digitValues_[i] == 1)
			width = kDigitSize.x * kDigitOneWidthRatio; // 1なら少し幅を狭める
		else
			width = kDigitSize.x * kDefaultDigitWidthRatio; // それ以外は同じ幅

		Float2 digitPos = startPos + Float2{offsetX + width / 2.0f, 0.0f}; // アンカーポイントが中心のため補正

		digits_[i]->SetTextureLeftTop({digitValues_[i] * kDigitSize.x, 0.0f}); // 連番テクスチャから表示する数字を抽出
		digits_[i]->SetPosition(digitPos);
		digits_[i]->Update();

		offsetX += width; // 次の文字のオフセットを進める
	}
}

void NumberSprite::Draw() {
	for (auto& digit : digits_) {
		digit->Draw();
	}
}

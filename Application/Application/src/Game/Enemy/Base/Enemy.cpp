#include "Enemy.h"

void Enemy::SetupHPBarSprite(std::unique_ptr<Sprite>& spritePtr, const Float4& color) { 
	uint32_t texture = TextureManager::Load("white.png"); 
	spritePtr = std::make_unique<Sprite>();
	spritePtr->Initialize(spriteCommon_.get(), texture);
	spritePtr->SetSize(kHPBarSize);
	spritePtr->SetColor(color);
}

void Enemy::SetupReloadSprite(std::unique_ptr<Sprite>& spritePtr) { 
	uint32_t texture = TextureManager::Load("white.png"); 
	spritePtr = std::make_unique<Sprite>();
	spritePtr->Initialize(spriteCommon_.get(), texture);
	spritePtr->SetSize(kReloadSize);
}

#include "Sprite.h"
#include "SpriteCommon.h"
#include "DirectXUtil.h"
#include "TextureManager.h"

void Sprite::Initialize(SpriteCommon* spriteCommon, uint32_t textureIndex)
{
	// 引数で受け取ってメンバ変数に記録する
	this->spriteCommon = spriteCommon;
	// テクスチャを保存
	textureIndex_ = textureIndex;


	// VertexResourceを作る
	vertexResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(VertexData) * 4);
	// IndexResourceを作る
	indexResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(uint32_t) * 6);
	// materialResourceを作る
	materialResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(Material));
	// TransformationResourceを作る
	transformationMatrixResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(TransformationMatrix));



	// VertexBufferViewを作成する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	// IndexBufferViewを作成する
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;



	// VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	// materialResourceにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// materialDataの初期値を書き込む
	materialData_->color = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = Matrix::Identity();

	// TransformationResourceにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 単位行列を書き込んでおく
	transformationMatrixData_->WVP = Matrix::Identity();
	transformationMatrixData_->World = Matrix::Identity();



	// Transformの初期化
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {0.0f, 0.0f, 0.0f} };



	// テクスチャの解像度を取得して、スプライト自体のサイズに反映させる
	AdjustTextureSize();
}

void Sprite::Update()
{
	// 座標を反映
	transform_.translate = { position_.x, position_.y, 0.0f };
	// 回転を反映
	transform_.rotate = { 0.0f, 0.0f, rotation };
	// サイズを反映
	transform_.scale = { size_.x, size_.y, 1.0f };
	// アンカーポイント
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	// 左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	// 上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// テクスチャ範囲指定の反映
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance().GetMetaData(textureIndex_);
	float tex_left = textureLeftTop_.x / metadata.width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
	float tex_top = textureLeftTop_.y / metadata.height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;

	// 頂点リソースにデータを書き込む
	// 左下
	vertexData_[0].position = { left, bottom, 0.0f, 1.0f };
	vertexData_[0].texcoord = { tex_left, tex_bottom };
	vertexData_[0].normal = { 0.0f, 0.0f, -1.0f };
	// 左上
	vertexData_[1].position = { left, top, 0.0f, 1.0f };
	vertexData_[1].texcoord = { tex_left, tex_top };
	vertexData_[1].normal = { 0.0f, 0.0f, -1.0f };
	// 右下
	vertexData_[2].position = { right, bottom, 0.0f, 1.0f };
	vertexData_[2].texcoord = { tex_right, tex_bottom };
	vertexData_[2].normal = { 0.0f, 0.0f, -1.0f };
	// 右上
	vertexData_[3].position = { right, top, 0.0f, 1.0f };
	vertexData_[3].texcoord = { tex_right, tex_top };
	vertexData_[3].normal = { 0.0f, 0.0f, -1.0f };

	// インデックスリソースにデータを書き込む
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;


	// Transform情報を作る
	Matrix worldMatrix = transform_.MakeAffineMatrix();
	Matrix viewMatrix = Matrix::Identity();
	Matrix projectionMatrix = Matrix::Orthographic(static_cast<float>(Window::GetWidth()), static_cast<float>(Window::GetHeight()), 0.0f, 1000.0f);
	Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

void Sprite::Draw()
{
	// VertexBufferViewを設定
	spriteCommon->GetDxBase()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// IBVを設定
	spriteCommon->GetDxBase()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	// マテリアルCBufferの場所を設定
	spriteCommon->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// TransformatinMatrixCBufferの場所を設定
	spriteCommon->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定
	TextureManager::SetDescriptorTable(2, spriteCommon->GetDxBase()->GetCommandList(), textureIndex_);
	//描画（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画
	spriteCommon->GetDxBase()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::AdjustTextureSize()
{
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance().GetMetaData(textureIndex_);

	textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	size_ = textureSize_;
}

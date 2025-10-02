#include "Object3d.hlsli"

cbuffer ShadowCB : register(b1)
{
    float4x4 World;
    float4x4 LightViewProj;
}

struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t4 weight : WEIGHT0;
    int32_t4 index : INDEX0;
};

struct Skinned
{
    float32_t4 position;
};

Skinned Skinning(VertexShaderInput input)
{
    Skinned skinned;
    
    // 位置の変換
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f; // 確実に1を入れる
    
    return skinned;
}

VertexShaderOutput main(VertexShaderInput input) // 入力頂点は当然SkeletonSpace
{
    VertexShaderOutput output;
    Skinned skinned = Skinning(input);
    float4 worldPos = mul(skinned.position, World);
    output.position = mul(worldPos, LightViewProj);
    return output;
}

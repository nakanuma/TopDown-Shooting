#include "LineDrawer.hlsli"

struct TransformationMatrix
{
    float32_t4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float32_t3 position : POSITION0;
    float32_t4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(float32_t4(input.position, 1.0f), gTransformationMatrix.WVP);
    output.color = input.color;
    return output;
}
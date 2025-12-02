#include "Object3D.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const int KERNEL_SIZE = 15;
static const int KERNEL_HALF = 7;
static const float32_t PI = 3.14159265;

float gauss(float x, float sigma)
{
    float exponent = -(x * x) * rcp(2.0f * sigma * sigma);
    float denominator = sqrt(2.0f * PI * sigma * sigma);
    return exp(exponent) * rcp(denominator);
}

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャサイズを取得
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 texelSize = float32_t2(rcp(width), rcp(height));
    
    // ぼかしの強度調整
    float32_t sigma = 8.0f;
    float32_t blurScale = 3.0f;
    
    // 1次元ガウシアンカーネルを計算
    float32_t kernel[KERNEL_SIZE];
    float32_t weight = 0.0f;
    
    for (int i = 0; i < KERNEL_SIZE; ++i)
    {
        int offset = i - KERNEL_HALF;
        kernel[i] = gauss(float(offset), sigma);
        weight += kernel[i];
    }
    
    // カーネルを正規化
    for (int j = 0; j < KERNEL_SIZE; ++j)
    {
        kernel[j] *= rcp(weight);
    }
    
    // 垂直にブラーを適用
    float32_t3 result = float32_t3(0.0f, 0.0f, 0.0f);
    
    for (int k = 0; k < KERNEL_SIZE; ++k)
    {
        int offset = k - KERNEL_HALF;
        float32_t2 texcoord = input.texcoord + float32_t2(0.0f, float(offset) * blurScale * texelSize.y);
        float32_t3 sampleColor = gTexture.Sample(gSampler, texcoord).rgb;
        result += sampleColor * kernel[k];
    }
    
    output.color = float32_t4(result, 1.0f);
    return output;
}
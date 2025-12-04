#include "Object3d.hlsli"

struct Material {
    float32_t4 color;
    int32_t enableLighting;
    int32_t useEnvironmentMap;
    float32_t2 padding;
    float32_t4x4 uvTransform;
    float32_t shininess;
    float32_t environmentStrength;
    float32_t2 padding2;
    float32_t3 emissiveColor;
    float32_t emissiveIntensity;
};
ConstantBuffer<Material> gMaterial : register(b0);


struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);


struct Camera
{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);


struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float radius;
    float decay; 
};
ConstantBuffer<PointLight> gPointLight : register(b3);


struct SpotLight
{
    float32_t4 color; // ライトの色
    float32_t3 position; // ライトの位置
    float32_t intensity; // 輝度
    float32_t3 direction; // スポットライトの方向
    float32_t distance; // ライトの届く最大距離
    float32_t decay; // 減衰率
    float32_t cosAngle; // スポットライトの余弦
    float32_t cosFalloffStart; // Falloff開始の角度
    uint32_t isActive;
};
static const int kMaxLight = 64;

struct SpotLights
{
    SpotLight spotLights[kMaxLight];
};
ConstantBuffer<SpotLights> gSpotLight : register(b4);


struct EmissiveLight
{
    float32_t4 color;
    float32_t3 position;
    float32_t intensity;
    float32_t radius;
    float32_t decay;
    uint32_t isActive;
    float32_t padding;
};

static const int kMaxEmissiveLight = 16;

struct EmissiveLights
{
    EmissiveLight emissiveLights[kMaxEmissiveLight];
    int32_t numActiveLights;
    float32_t3 padding;
};

ConstantBuffer<EmissiveLights> gEmissiveLight : register(b8);


struct AreaLight
{
    float32_t4 color;
    float32_t3 position;
    float32_t intensity;
    float32_t3 right;
    float32_t width;
    float32_t3 up;
    float32_t height;
    float32_t3 normal;
    float32_t range;
    uint32_t isActive;
    uint32_t lightType; // 0.RectAngle, 1.Disk, 2.Tube, 3.Sphere
    float32_t2 padding;
};

static const int kMaxAreaLight = 16;

struct AreaLights
{
    AreaLight areaLights[kMaxAreaLight];
    int32_t numActiveLights;
    float32_t3 padding;
};

ConstantBuffer<AreaLights> gAreaLight : register(b9);

// 矩形エリアライトの照明計算
float3 CalculateRectangleLight(AreaLight light, float3 worldPos, float3 normal)
{
    // ライト平面への投影
    float3 toLight = light.position - worldPos;
    float distToPlane = dot(toLight, light.normal);
    
    // 裏面チェック
    if (distToPlane < 0.0f)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }

    // ライト平面上の投影点
    float3 projectedPoint = worldPos + light.normal * distToPlane;
    
    // ライトのローカル座標系での位置
    float3 localPos = projectedPoint - light.position;
    float u = dot(localPos, light.right);
    float v = dot(localPos, light.up);
    
    // ライトの境界内にクランプ
    float halfWidth = light.width * 0.5f;
    float halfheight = light.height * 0.5f;
    u = clamp(u, -halfWidth, halfWidth);
    v = clamp(v, -halfheight, halfheight);
    
    // 最も近いライト表面上の点
    float3 nearestPoint = light.position + light.right * u + light.up * v;
    
    // ライト方向と距離
    float3 L = nearestPoint - worldPos;
    float distance = length(L);
    
    // 影響範囲外チェック
    if (distance > light.range)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    
    L = normalize(L);
    
    // 距離減衰（二乗減衰）
    float attenuation = saturate(1.0f - (distance / light.range));
    attenuation *= attenuation;
    
    // ランバート拡散反射
    float NdotL = max(dot(normal, L), 0.0f);

    // エリアサイズによる補正
    float area = light.width * light.height;
    float areaFactor = area / (distance * distance + 1.0f);
    areaFactor = min(areaFactor * 0.5f, 1.0f);
    
    return light.color.rgb * light.intensity * NdotL * attenuation * (1.0f + areaFactor);
}

// 線分ライトの照明計算
float3 CalculateTubeLight(AreaLight light, float3 worldPos, float3 normal)
{
    // チューブの両端点
    float halfWidth = light.width * 0.5f;
    float3 L0 = light.position - light.right * halfWidth;
    float3 L1 = light.position + light.right * halfWidth;
    
    // 線分上の最近傍点を求める
    float3 L01 = L1 - L0;
    float3 L0P = worldPos - L0;
    float t = dot(L0P, L01) / dot(L01, L01);
    t = saturate(t);
    
    float3 closestPoint = L0 + L01 * t;
    
    // ライト方向と距離
    float3 L = closestPoint - worldPos;
    float distance = length(L);
    
    // 影響範囲外チェック
    if (distance > light.range)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    
    L = normalize(L);
    
    // 距離減衰
    float attenuation = saturate(1.0f - (distance / light.range));
    attenuation *= attenuation;
    
    // ランバート拡散反射
    float NdotL = max(dot(normal, L), 0.0f);
    
    // チューブの長さによる補正
    float lengthFactor = light.width / (distance * 2.0f + 1.0f);
    lengthFactor = min(lengthFactor, 1.0f);
    
    return light.color.rgb * light.intensity * NdotL * attenuation * (1.0f + lengthFactor * 0.5f);

}

// 円盤ライトの照明計算
float3 CalculateDiskLight(AreaLight light, float3 worldPos, float3 normal)
{
    // ライト平面への投影
    float3 toLight = light.position - worldPos;
    float distToPlane = dot(toLight, light.normal);
    
    // 裏面チェック
    if (distToPlane < 0.0f)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    
    // ライト平面上の投影点
    float3 projectedPoint = worldPos + light.normal * distToPlane;
    float3 localPos = projectedPoint - light.position;
    
    // 円盤の半径
    float radius = light.width * 0.5f;
    float distFromCenter = length(localPos);
    
    // 円盤の範囲内にクランプ
    if (distFromCenter > radius)
    {
        localPos = normalize(localPos) * radius;
    }
    
    float3 nearestPoint = light.position + localPos;
    
    // ライト方向と距離
    float3 L = nearestPoint - worldPos;
    float distance = length(L);
    
    // 影響範囲外チェック
    if (distance > light.range)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    
    L = normalize(L);

    // 距離減衰
    float attenuation = saturate(1.0f - (distance / light.range));
    attenuation *= attenuation;
    
    // ランパート拡散反射
    float NdotL = max(dot(normal, L), 0.0f);

    // 円盤の面積による補正
    float area = 3.14159 * radius * radius;
    float areaFactor = area / (distance * distance + 1.0f);
    areaFactor = min(areaFactor * 0.5f, 1.0f);
    
    return light.color.rgb * light.intensity * NdotL * attenuation * (1.0f + areaFactor);
}

// 球形ライト照明計算
float3 CalculateSphereLight(AreaLight light, float3 worldPos, float3 normal)
{
    float3 L = light.position - worldPos;
    float distance = length(L);
    
    // 影響範囲外チェック
    if (distance > light.range)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    
    L = normalize(L);
    
    // 距離減衰
    float attenuation = saturate(1.0f - (distance / light.range));
    attenuation *= attenuation;
    
    // ランバート拡散反射
    float NdotL = max(dot(normal, L), 0.0f);
    
    return light.color.rgb * light.intensity * NdotL * attenuation;
}

// エリアライトの総合計算
float3 CaluculateAreaLight(AreaLight light, float3 worldPos, float3 normal)
{
    switch (light.lightType)
    {
        case 0:
            return CalculateRectangleLight(light, worldPos, normal);
        case 1:
            return CalculateDiskLight(light, worldPos, normal);
        case 2:
            return CalculateTubeLight(light, worldPos, normal);
        case 3:
            return CalculateSphereLight(light, worldPos, normal);
        default:
            return float3(0.0f, 0.0f, 0.0f);
    }
}


Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// environment texture
TextureCube<float32_t4> gEnvironmentTexture : register(t2);

// shadowMap
Texture2D<float> gShadowMap : register(t3);
SamplerComparisonState gShadowSampler : register(s1); // 比較サンプラー

static const float kShadowMapTexelSize = 1.0f / 384.0f;
static const int kNumSamples = 9;

struct LightCameraCB
{
    float4x4 lightViewProj;
};
ConstantBuffer<LightCameraCB> gLightCameraCB : register(b7);

float SampleShadow(float3 worldPos)
{
    // ワールド座標->ライト空間座標
    float4 lightSpacePos = mul(float4(worldPos, 1.0f), gLightCameraCB.lightViewProj);
    // 透視除算
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    // X,Yを0~1に変換
    projCoords.xy = projCoords.xy * 0.5f + 0.5f;
    // Zも同様に0~1へ
    projCoords.z = projCoords.z - 0.001f;
    // 画面外のチェック
    if(projCoords.x < 0.0f || projCoords.x > 1.0f || 
        projCoords.y < 0.0f || projCoords.y > 1.0f || 
        projCoords.z > 1.0f)
    {
        return 1.0f; // 影の外
    }
    // Y座標の反転
    projCoords.y = 1.0f - projCoords.y;
    
    float shadowFactorSum = 0.0f;
    // 3x3のサンプリングを行う
    float2 offset[kNumSamples] =
    {
        float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f),
        float2(-1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 0.0f),
        float2(-1.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f),
    };
    // オフセットをテクセルサイズにスケーリング
    float texelSize = kShadowMapTexelSize;
    
    for (int i = 0; i < kNumSamples; i++)
    {
        // オフセットを適用したテクスチャ座標
        float2 uvOffset = projCoords.xy + offset[i] * texelSize;
        // SampleCmpLevelZeroでDepth比較
        shadowFactorSum += gShadowMap.SampleCmpLevelZero(gShadowSampler, uvOffset, projCoords.z);
    }
    
    return shadowFactorSum / kNumSamples; // 0~1の値
}

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    
    
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition); // Cameraへの方向を算出
    
    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position); // 入射光を計算
    
    float32_t distance = length(gPointLight.position - input.worldPosition); // ポイントライトへの距離
    float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay); // 指数によるコントロール
    
    
    
 
    
    
    //float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal)); // 入射光の反射ベクトル
    
    // textureのa値が0.5以下のときにPixelを棄却
    if (textureColor.a <= 0.5){
        discard;
    }
    // textureのa値が0のときにPielを棄却
    if (textureColor.a == 0.0){
        discard;
    }
    // output.coloのa値が0のときにPixelを棄却
    if (output.color.a == 0.0){
        discard;
    }
    
    if (gMaterial.enableLighting != 0) { // Lightingする場合  
        ///
        /// DirectionalLight
        ///
        
        // 拡散反射
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        /*float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);*/ // half lambert
        float lambert = max(NdotL, 0.0f);
        
        float baseLight = 0.7f;
        float lightStrength = lerp(baseLight, 1.0f, lambert);
        
        float32_t3 diffuseDirectionalLight =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * lightStrength * gDirectionalLight.intensity;
        
        // シャドウ判定を掛ける
        float shadow = SampleShadow(input.worldPosition);
        float shadowFactor = lerp(0.5f, 1.0f, shadow); // ここで影の濃さを調整
        diffuseDirectionalLight *= shadowFactor;
        
        // 鏡面反射
        //float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        //float NdotH = dot(normalize(input.normal), halfVector);
        //float specularPow = pow(saturate(NdotH), gMaterial.shininess); // 反射強度
        
        //float32_t3 specularDirectionalLight =
        //gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        ///
        /// PointLight
        ///
        
        // 拡散反射
        float NdotL_point = dot(normalize(input.normal), -pointLightDirection);
        float cos_point = pow(NdotL_point * 0.5f + 0.5, 2.0f);
        float32_t3 diffusePointLight =
        gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos_point * gPointLight.intensity * factor;

        // 鏡面反射
        float32_t3 halfVector_point = normalize(-pointLightDirection + toEye);
        float NdotH_point = dot(normalize(input.normal), halfVector_point);
        float specularPow_point = pow(saturate(NdotH_point), gMaterial.shininess);
        float32_t3 specularPointLight =
        gPointLight.color.rgb * gPointLight.intensity * specularPow_point * float32_t3(1.0f, 1.0f, 1.0f) * factor;
        
        ///
        /// SpotLight
        ///
        
        float32_t3 diffuseSpotLight = {0.0f, 0.0f, 0.0f};
        
         // 拡散反射
        for (uint32_t i = 0; i < kMaxLight; i++)
        {
            if (gSpotLight.spotLights[i].isActive == false)
            {
                continue;
            }
            
            float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.spotLights[i].position);
    
            float32_t distance_spot = length(gSpotLight.spotLights[i].position - input.worldPosition);
            float32_t attenuatuinFactor = pow(saturate(-distance / gSpotLight.spotLights[i].distance + 1.0f), gSpotLight.spotLights[i].decay);
    
            float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.spotLights[i].direction);
            float32_t falloffFactor = saturate((cosAngle - gSpotLight.spotLights[i].cosAngle) / (gSpotLight.spotLights[i].cosFalloffStart - gSpotLight.spotLights[i].cosAngle));
            
            float NdotL_spot = dot(normalize(input.normal), -spotLightDirectionOnSurface);
            float cos_spot = pow(NdotL_spot * 0.5f + 0.5, 2.0f);
            diffuseSpotLight +=
            -gSpotLight.spotLights[i].color.rgb * cos_spot * gSpotLight.spotLights[i].intensity * attenuatuinFactor * falloffFactor;
        }
        
        diffuseSpotLight *= gMaterial.color.rgb * textureColor.rgb;

            
        // 鏡面反射
        //float32_t3 halfVector_spot = normalize(-spotLightDirectionOnSurface + toEye);
        //float NdotH_spot = dot(normalize(input.normal), halfVector_spot);
        //float specularPow_spot = pow(saturate(NdotH_spot), gMaterial.shininess);
        //float32_t3 specularSpotLight =
        //gSpotLight.color.rgb * gSpotLight.intensity * specularPow_spot * float32_t3(1.0f, 1.0f, 1.0f) * attenuatuinFactor * falloffFactor;
        
        ///
        /// EmissiveLight
        ///
        
        float32_t3 diffuseEmissiveLight = float3(0.0f, 0.0f, 0.0f);
        
        for (int i = 0; i < gEmissiveLight.numActiveLights; i++)
        {
            // 無効化状態ならスキップ
            if (gEmissiveLight.emissiveLights[i].isActive == 0)
            {
                continue;
            }
            
            // ライト方向を計算
            float32_t3 emissiveLightDir = gEmissiveLight.emissiveLights[i].position - input.worldPosition;
            float32_t emissiveDistance = length(emissiveLightDir);
            
            // 影響範囲外なら計算スキップ
            if (emissiveDistance > gEmissiveLight.emissiveLights[i].radius)
            {
                continue;
            }
            
            emissiveLightDir = normalize(emissiveLightDir);
            
            // 距離減衰を計算
            float32_t attenuation = pow(saturate(1.0f - (emissiveDistance / gEmissiveLight.emissiveLights[i].radius)), gEmissiveLight.emissiveLights[i].decay);
            
            // 拡散反射を計算
            float NdotL_emissive = dot(normalize(input.normal), emissiveLightDir);
            float lambert_emissive = max(NdotL_emissive, 0.0f);
            
            // ライトの寄与を加算
            diffuseEmissiveLight +=
            gEmissiveLight.emissiveLights[i].color.rgb * 
            gEmissiveLight.emissiveLights[i].intensity * 
            lambert_emissive * 
            attenuation;
        }
        
        diffuseEmissiveLight *= gMaterial.color.rgb * textureColor.rgb;
        
        ///
        /// AreaLight
        ///
        
        float32_t3 diffuseAreaLight = float3(0.0f, 0.0f, 0.0f);
        
        for (int i = 0; i < gAreaLight.numActiveLights; i++)
        {
            // 無効化状態ならスキップ
            if (gAreaLight.areaLights[i].isActive == 0)
            {
                continue;
            }
            
            // エリアライトの照明計算
            float3 areaContribution = CaluculateAreaLight(gAreaLight.areaLights[i], input.worldPosition, normalize(input.normal));
            
            diffuseAreaLight += areaContribution;
        }
        
        diffuseAreaLight *= gMaterial.color.rgb * textureColor.rgb;
        
        ///
        /// EnvironmentMap
        ///
        
        float32_t3 environmentContribution = float3(0.0f, 0.0f, 0.0f);
        
        if (gMaterial.useEnvironmentMap != 0)
        {
            float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
            float32_t3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
            float32_t4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
            
            environmentContribution = environmentColor.rgb * gMaterial.environmentStrength;
        }
        
        // ライティング結果を合成
        output.color.rgb =
        diffuseDirectionalLight + // DirectionalLight
        diffusePointLight + specularPointLight + // PointLight
        diffuseSpotLight + // SpotLight
        diffuseEmissiveLight + // EmissiveLight
        diffuseAreaLight +
        environmentContribution; // EnvironmentMap
        
        ///
        /// Emissive
        ///
        
        // エミッシブを加算
        if (gMaterial.emissiveIntensity > 0.0f)
        {
            output.color.rgb += gMaterial.emissiveColor * gMaterial.emissiveIntensity;
        }
       
        // アルファは今まで通り
        output.color.a = gMaterial.color.a * textureColor.a;
        
    // ライティング無効時
    } else {
        output.color = gMaterial.color * textureColor;
        
        // ライティング無効時でもエミッシブは適用
        output.color.rgb += gMaterial.emissiveColor * gMaterial.emissiveIntensity;
    }
    
    // 最終的なアルファ値チェック
    if (output.color.a == 0.0f)
    {
        discard;
    }
    
    return output;
}
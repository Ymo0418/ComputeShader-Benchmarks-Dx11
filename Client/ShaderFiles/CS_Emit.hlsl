#include "CS_ParticleGlobal.hlsli"

struct ForRender
{
    float4  vColor;
    float3  vWorldPos;
    float   fScale;
};
struct ForUpdate
{
    float3  vWorldVelocity;
    float2  vLifeTime;
};

Texture2D                       RandomBuffer        : register(t0);
RWStructuredBuffer<ForRender>   PtkRenderBuffer     : register(u0);
RWStructuredBuffer<ForUpdate>   PtkUpdateBuffer     : register(u1);
ConsumeStructuredBuffer<uint>   DeadListUAV         : register(u2);
cbuffer                         EmitConstantBuffer  : register(b1)
{
    uint    Emit_MaxParticle;
    float3  Emit_Pivot;
    float4  Emit_Range;
    float2  Emit_LifeTime;
    float2  EmitConst_Pad;
};
cbuffer EmitInfoBuffer : register(b2)
{
    row_major float4x4  matWorld;
    uint                iEmitCount;
    uint                iEmitOffset;
    float2              EmitInfo_Pad;
}

[numthreads(256, 1, 1)]
void CS_EMIT(uint3 DTid : SV_DispatchThreadID /* Which in Whole Group xyz */
            /*, uint3 GTid : SV_GroupThreadID */ /* Which in Current Group xyz */
            /* , uint3 Gid : SV_GroupID */       /* Which Group xyz */)
{
    if(DTid.x < iEmitCount)
    {
        ForRender PR = (ForRender) 0;
        ForUpdate PU = (ForUpdate) 0;
                  
        float2 vRandomUV = float2((DTid.x + iEmitOffset) / 1024.0, g_fAccumTime);
        float4 vRandom = RandomBuffer.SampleLevel(SamplerWrapLinear, vRandomUV, 0);
        float4 vSingedRandom = vRandom * 2.f - 1.f;
                                
        PR.vWorldPos = normalize(vSingedRandom.xyz) * Emit_Range.xyz;
        PU.vWorldVelocity = PR.vWorldPos - Emit_Pivot;
                
        float fLifeTime = lerp(Emit_LifeTime.x, Emit_LifeTime.y, vRandom.a);
        PU.vLifeTime = float2(fLifeTime, fLifeTime);
        
        uint index = DeadListUAV.Consume();
        PtkRenderBuffer[index] = PR;
        PtkUpdateBuffer[index] = PU;        
    }
}

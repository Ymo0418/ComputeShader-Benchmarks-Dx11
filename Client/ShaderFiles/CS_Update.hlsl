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

Texture2D                       RandomBuffer         : register(t0);
RWStructuredBuffer<ForRender>   ForRenderBuffer      : register(u0);
RWStructuredBuffer<ForUpdate>   ForUpdateBuffer      : register(u1);
AppendStructuredBuffer<uint>    DeadIndexBuffer      : register(u2);
RWStructuredBuffer<uint>        AliveIndexBuffer     : register(u3);
RWBuffer<uint>                  DrawArgs             : register(u4);
cbuffer                         UpdateConstantBuffer : register(b1)
{
    float4  Update_StartColor, Update_EndColor;
    float   Update_Speed, Update_Scale;
    float2  UpdateConstant_Pad;
}

[numthreads(1024, 1, 1)]
void CS_UPDATE(uint3 DTid : SV_DispatchThreadID     /* Which in Whole Group xyz */
            /*, uint3 GTid : SV_GroupThreadID   */  /* Which in Current Group xyz */
            /* , uint3 Gid : SV_GroupID         */  /* Which Group xyz */)
{
    if (DTid.x == 0)
    {
        DrawArgs[0] = 1;    //VertexCountPerInstance
        DrawArgs[1] = 0;    //IntanceCount
        DrawArgs[2] = 0;
        DrawArgs[3] = 0;
        DrawArgs[4] = 0;
    }
    GroupMemoryBarrierWithGroupSync();
    
    ForRender PR = ForRenderBuffer[DTid.x];
    ForUpdate PU = ForUpdateBuffer[DTid.x];
    
    if(PU.vLifeTime.y > 0.f)
    {
        PU.vLifeTime.y -= g_fTimeDelta;
        
        if(PU.vLifeTime.y <= 0.f)
        {
            DeadIndexBuffer.Append(DTid.x);
        }
        else
        {
            uint index = AliveIndexBuffer.IncrementCounter();
            AliveIndexBuffer[index] = DTid.x;
            InterlockedAdd(DrawArgs[1], 1);
            
            float fLifeRatio = 1.f - (PU.vLifeTime.y / PU.vLifeTime.x);
            
            PR.vColor = lerp(Update_StartColor, Update_EndColor, fLifeRatio);
            PR.fScale = Update_Scale;
            
            float3 vViewPosition;

            PR.vWorldPos.xyz = PR.vWorldPos.xyz + PU.vWorldVelocity.xyz * g_fTimeDelta;
        }
    }
       
    ForRenderBuffer[DTid.x] = PR;
    ForUpdateBuffer[DTid.x] = PU;    
}
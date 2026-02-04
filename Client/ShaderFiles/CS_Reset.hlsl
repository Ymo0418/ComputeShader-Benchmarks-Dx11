#include "CS_ParticleGlobal.hlsli"

cbuffer DeadConstantBuffer : register(b0)
{
    uint    iMaxParticle;
    uint3   DeadConstantBuffer_Pad;
};

AppendStructuredBuffer<uint> DeadIndexBuffer : register(u0);

[numthreads(256, 1, 1)]
void CS_RESET(uint3 DTid : SV_DispatchThreadID /* Which in Whole Group xyz */
            /*, uint3 GTid : SV_GroupThreadID */ /* Which in Current Group xyz */
            /* , uint3 Gid : SV_GroupID */       /* Which Group xyz */)
{   
    if(DTid.x < iMaxParticle)
    {
        DeadIndexBuffer.Append(DTid.x);
    }
}
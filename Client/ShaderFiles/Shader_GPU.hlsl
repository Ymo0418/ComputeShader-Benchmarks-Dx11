#include "Shader_Defines.hlsli"
#include "CS_ParticleGlobal.hlsli"

struct ForRender
{
    float4 vColor;
    float3 vWorldPos;
    float fScale;
};

StructuredBuffer<ForRender>     Particles   : register(t0);
StructuredBuffer<uint>          IndexBuffer : register(t1);

struct VS_OUT_BASE
{
    float4  vColor          : COLOR0;
    float3  vViewPosition   : TEXCOORD0;
    float   fScale          : TEXCOORD1;
};

VS_OUT_BASE VS_BASE(uint InstanceId : SV_InstanceID)
{
    VS_OUT_BASE Out = (VS_OUT_BASE) 0;
        
    ForRender PR = Particles[IndexBuffer[InstanceId]];
    
    Out.vColor = PR.vColor;
    Out.vViewPosition = mul(float4(PR.vWorldPos, 1.f), g_matView).xyz;
    Out.fScale = PR.fScale;
        
    return Out;
}

struct GS_OUT_BASE
{
    float4 vPosition : SV_POSITION;
    float4 vColor    : COLOR0;
};

[maxvertexcount(4)]
void GS_BASE(point VS_OUT_BASE In[1], inout TriangleStream<GS_OUT_BASE> Triangles)
{
    const float2 VertexOffsets[4] = { float2(.5f, .5f), float2(-.5f, .5f), float2(-.5f, -.5f), float2(.5f, -.5f) };
    
    GS_OUT_BASE Out[4];
       
    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        float2 offset = VertexOffsets[i] * In[0].fScale;
    
        float3 vFacingCam = In[0].vViewPosition.xyz;
        vFacingCam.xy += offset;
    
        Out[i].vPosition = mul(float4(vFacingCam, 1), g_matProj);
        Out[i].vColor = In[0].vColor;
    }
    
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[0]);
    Triangles.Append(Out[3]);
}

struct PS_OUT_BASE
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BASE PS_BASE(GS_OUT_BASE In)
{
    PS_OUT_BASE Out = (PS_OUT_BASE) 0;
    Out.vColor = In.vColor;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass PASS_BASE //0
    {
        SetRasterizerState(RS_NonCull);
        SetDepthStencilState(DSS_NOWRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BASE();
        GeometryShader = compile gs_5_0 GS_BASE();
        PixelShader = compile ps_5_0 PS_BASE();
    }
}

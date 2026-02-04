#include "Shader_Defines.hlsli"

cbuffer PerFrameInfo : register(b0)
{
    float4x4 g_ViewMatrix;
    float4x4 g_ProjMatrix;
};

struct VS_IN
{
    float4 vColor       : COLOR0;
    float3 vWorldPos    : TEXCOORD0;
    float  fScale       : TEXCOORD1;
    uint   idx          : SV_InstanceID;
};

struct VS_OUT
{
    float4 vColor       : COLOR0;
    float3 vWorldPos    : TEXCOORD0;
    float  fScale       : TEXCOORD1;
    uint   idx          : NOINTERPOLATION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vColor = In.vColor;
    Out.vWorldPos = In.vWorldPos;
    Out.fScale = In.fScale;
    Out.idx = In.idx;
    
    return Out;
}

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR0;
};

[maxvertexcount(4)]
void GS_MAIN(point VS_OUT In[1], inout TriangleStream<GS_OUT> Triangles)
{    
    float4 vViewPosition = mul(float4(In[0].vWorldPos, 1.f), g_ViewMatrix);
    const float2 VertexOffsets[4] = { float2(.5f, .5f), float2(-.5f, .5f), float2(-.5f, -.5f), float2(.5f, -.5f) };
    
    GS_OUT Out[4];    
    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        float2 offset = VertexOffsets[i] * In[0].fScale;
    
        float3 vFacingCam = vViewPosition.xyz;
        vFacingCam.xy += offset;
    
        Out[i].vPosition = mul(float4(vFacingCam, 1), g_ProjMatrix);
        Out[i].vColor = In[0].vColor;
    }
    
    /*
    float3 vPos = In[0].vWorldPos;
    float fScale = In[0].fScale;
    
    float3 vRight = float3(g_ViewMatrix._11, g_ViewMatrix._21, g_ViewMatrix._31);
    float3 vUp = float3(g_ViewMatrix._12, g_ViewMatrix._22, g_ViewMatrix._32);

    float3 vOffsets[4] =
    {
        (vRight + vUp) * fScale,
        (-vRight + vUp) * fScale,
        (-vRight - vUp) * fScale,
        (vRight - vUp) * fScale
    };
    
    GS_OUT Out[4];
    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        float3 vWorldPos = vPos + vOffsets[i];
        
        // World -> View -> Proj 순서로 연산
        float4 vViewPos = mul(float4(vWorldPos, 1.f), g_ViewMatrix);
        Out[i].vPosition = mul(vViewPos, g_ProjMatrix);
        
        // 인스턴스 ID에 따른 디버그 색상
        Out[i].vColor = float4((In[0].idx % 256) / 255.f, (In[0].idx * 10 % 256) / 255.f, 1.f, 1.f);
    }
*/
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[0]);
    Triangles.Append(Out[3]);
    
    Triangles.RestartStrip();
}

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(GS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = In.vColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass PASS_BASE
    {
        SetRasterizerState(RS_NonCull);
        SetDepthStencilState(DSS_NOWRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
cbuffer PerFrameInfo : register(b0)
{
    float4x4    g_matView;
    float4x4    g_matProj;
    float4x4    g_matViewProj;
    float4      g_vCamPos;
    float       g_fAccumTime;
    float       g_fTimeDelta;
    float2      g_PerFrameInfo_Pad;
};

SamplerState SamplerWrapLinear : register(s0);

#define PI 3.14159265
#define pow2(a) ((a)*(a))
#define arraysize(a) (sizeof(a) / sizeof(a[0]))

float min3(float a, float b, float c)
{
    return min(min(a, b), c);
}
float max3(float a, float b, float c)
{
    return max(max(a, b), c);
}
float med3(float a, float b, float c)
{
    return max(min(a, b), min(max(a, b), c));
}
inline uint flatten2D(uint2 coord, uint2 dim)
{
    return coord.x + coord.y * dim.x;
}
inline uint2 unflatten2D(uint idx, uint2 dim)
{
    return uint2(idx % dim.x, idx / dim.x);
}
inline uint flatten3D(uint3 coord, uint3 dim)
{
    return (coord.z * dim.x * dim.y) + (coord.y * dim.x) + coord.x;
}
inline uint3 unflatten3D(uint idx, uint3 dim)
{
    const uint z = idx / (dim.x * dim.y);
    idx -= (z * dim.x * dim.y);
    const uint y = idx / dim.x;
    const uint x = idx % dim.x;
    return uint3(x, y, z);
}
float4 qmul(float4 q1, float4 q2)
{
    return float4(
		q2.xyz * q1.w + q1.xyz * q2.w + cross(q1.xyz, q2.xyz),
		q1.w * q2.w - dot(q1.xyz, q2.xyz)
	);
}
float3 rotate_vector(float3 v, float4 r)
{
    float4 r_c = r * float4(-1, -1, -1, 1);
    return qmul(r, qmul(float4(v, 0), r_c)).xyz;
}
inline float3 get_right(float4x4 m)
{
    return float3(m[0][0], m[0][1], m[0][2]);
}
inline float3 get_up(float4x4 m)
{
    return float3(m[1][0], m[1][1], m[1][2]);
}
inline float3 get_look(float4x4 m)
{
    return float3(m[2][0], m[2][1], m[2][2]);
}
inline float3 get_pos(float4x4 m)
{
    return float3(m[3][0], m[3][1], m[3][2]);
}
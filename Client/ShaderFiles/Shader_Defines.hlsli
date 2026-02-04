// Sampler =====================================
sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// Render State ==============================
RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_NonCull
{
    FillMode = Solid;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

// DepthStencil State ==============================
DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_NOWRITE
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
    DepthFunc = less_equal;
};

// Blend State ==============================
BlendState BS_Default
{
    BlendEnable[0] = false;
    BlendEnable[1] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;
};
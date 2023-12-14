/* ���̴����� ���� ������ Ŭ���̾�Ʈ���� ���� ���޹ޱ� ���� ����� �׸��̱⵵ �ϴ�. */

/* ��� */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;	// ��ǻ��

/* �� */
vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* ���͸��� */
vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* ī�޶� */
vector			g_vCamPosition;

sampler DefaultSampler = sampler_state
{	
	Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;	
	float4		vWorldPos : TEXCOORD1;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	/* �� ������ ���ؼ� ����� ����� ���� ���庯ȯ �����ش�.*/
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* ������� ���� */
	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT 
{
	float4		vColor : SV_TARGET0;
};

/* �ȼ����̴� : �ȼ��� ��!!!! �� �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vMtrlDiffuse  = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord); // �ӽ÷� �����ص״� ���� �ؽ�ó�� ä������.

	if (vMtrlDiffuse.a < 0.3f) // ��Ƴ��� [�׸���] (������������ ����)
		discard;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* ����ŧ���� ���������ϴ� ���������� 1��, �ƴ� ���������� 0���� ���ǵǴ� ����ŧ���� ���Ⱑ �ʿ��ϴ�. */
	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;


	return Out;
}


technique11 DefaultTechnique
{	
	pass Model	// �� pass
	{		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

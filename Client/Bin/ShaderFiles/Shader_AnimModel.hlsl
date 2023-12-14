/* 행열 */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 디퓨즈(텍스처) */
texture2D		g_DiffuseTexture;

/* 뼈 */
matrix			g_BoneMatrices[256]; // 한 메쉬에 최대 256개의 뼈까지 받을 수 있다.
/*	
	최대 뼈 개수를 256개로 제한을 둔 이유.
	- 그래픽 카드의 메모리(셰이더)는 가상메모리 사용이 불가능하다.
	- 가상 메모리 사용이 불가능하면, 하드웨어 적으로만 활용해야하는데, 이는 그래픽 카드의 성능에따라 제한되는 크기가 달라진다.
	- 따라서 수업코드 방식에선 256개로 제한을 둔 것. (하드웨어 성능이 좋다면 더 늘려서 사용해도 된다.)
	
	해결 기법
	- 이를 해결하기 위해 VTF(버텍스, 텍스처, 패치)기법이 존재하는데, 텍스처에 행열을 저장하여 불러오는것이다.
	- 직접 행열을 선언하는 것 보다 가볍고 빠르게 얻어올 수 있지만, 사용하기 위해 시간을 많이 쏟아야해서 개인작업때는 비추천.
*/

/* 빛 */
vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* 재질 */
vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* 카메라 */
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

	uint4		vBlendIndices : BLENDINDEX;
	float4		vBlendWeights : BLENDWEIGHT;
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

	/*	*자체적으로 애니메이션 정보가 없는 녀석들 : 검, 그림자
		
		검이나 그림자 같은 녀석들은 자체적인 애니메이션 정보 없이 다른 뼈에 붙어서 움직이기 때문에 값이 모두 0으로 들어오게된다.
		여기서 모두 0으로 들어오게되면 그려지지 않으니, 같은 이름을 찾아서 붙이는 작업은 외부에서 하고, 여기선 0값이 나온 녀석들을 강제적으로 1로 만들어두자.
	*/
	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);	// Weights의 x, y, z가 모두 0일경우 1로 강제되게 한다.

	/* 뼈 4개를 모두 받아서 혼합하여 뼈 매트릭스를 만든다. */
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;	// 강제로 1로만든 값을 넣어준다.

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
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

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

	if (vMtrlDiffuse.a < 0.3f)
		discard;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* 스펙큘러가 보여져야하는 영역에서는 1로, 아닌 영역에서는 0으로 정의되는 스펙큘러의 세기가 필요하다. */
	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;


	return Out;
}


technique11 DefaultTechnique
{
	pass Model
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

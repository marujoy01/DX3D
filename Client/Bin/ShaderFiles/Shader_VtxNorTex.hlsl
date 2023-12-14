/* �Լ� 
pow : ���� �Լ�
min : �ּҰ� �Լ�
max : �ִ밪 �Լ�
*/


matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

// ������
vector			g_vLightPos = vector(50.f, 10.f, 50.f, 1.f);	// ��ġ (������ ������� ��)
float			g_fLightRange = 30.f;							// ���� (������ ���߰���� ����)

/* �� ���� ���� */
vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);		// ���� ���� : ��ü�� ���� ���Ϳ� �����Ͽ� ���� �ް��Ѵ�.	
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);	// ���ݻ�	 : ������ �¾�� ���� ������ ���ݻ��̴�.

vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);	// ȯ��ݻ�	 : ����ó�� �������� ���� �����ʾƵ� ������� ���� �ް��ϱ� ���ѳ༮.
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);	// �ݻ籤	 : ������ �ִ� ǥ�鿡 ���� ���� �Ի�� �� ���� �����ݻ縦 ���� �ش�.

// ��ü�� ����
vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);	// Ambient�� ���� ��ü���� ���� �����Ѵ�.
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
//texture2D		g_DiffuseTexture;	// Diffuse�� ���� ��ü���� ���� �����Ѵ�.

// ���� ����
texture2D		g_DiffuseTexture[2];
texture2D		g_MaskTexture;

// �귯��
texture2D		g_BrushTexture;	// �ؽ�ó
vector			g_vBrushPos;//= vector(30.f, 0.f, 30.f, 1.f);	// �귯���� �׸� ��ġ
float			g_fBrushRange = 1.f;	// �귯���� ����

// ī�޶�
vector			g_vCamPosition;

sampler DefaultSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

//TODO ���̴��� �ϴ� ��
//! ������ ��ȯ ( ���庯ȯ, �亯ȯ, ������ȯ ) �� �����Ѵ�. ( ������� ��������� ���ߴٰ� ���� �����̽��� �ִ� ���̾ƴϴ�. �ݵ�� w������ ���� ��ģ �������� ���� �����̽� ��ȯ�� �ƴٰ� �� �� �ִ�. )
//! ������ ���������� �߰�, ���� ���� ������ �����Ѵ�.

struct VS_IN
{
	float3		vPosition : POSITION; //! ��ġ���� ��Ÿ���� �ø�ƽ, ���� ������ �ؾ��ϴ� �ڷ������� �˸�.
	float3		vNormal	  : NORMAL;
	float2		vTexcoord : TEXCOORD0; //! �ؽ�ó�� ��ǥ���� ��Ÿ���� �ø�ƽ, UV ��ǥ��� 0 ~ 1������ ���� ������ ��, Ư�� ��ġ�� ������ �˱�����?
};

struct VS_OUT // OUT�� IN�� �����ֱ� ���� ���� �Ȱ��� �߰��Ǿ����
{
	float4		vPosition : SV_POSITION; //! �������ֱ����� ��ġ. ������������������ ���� �� w���� �ִ�ä�����ؼ� float4�� �ٲ����, SV_POSITION �ø�ƽ�� �༭ ������ ���� �ڷ����̶� ���� ���̴����� �˸�
	float4		vNormal	  : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; // ���� ��ġ
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0; //! HLSL������ ��밡���� ����ü ���θ޸�
	
	//! �Ű����ڷ� ���� In�� ���� �����̴�
	//! In.vPosition * ���� * �� * ������ ������Ѵ�.
	
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//TODO mul �Լ��� ��İ��� ������ ��� ��ĵ��� ������ �������ش�. 

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);		//! Ŭ�󿡼� ������ ������ ��ġ������ ��İ��� �����ϰ� ���� �����ְ� w���̴� 1�� �ʱ�ȭ ������ ��İ� ����,��,���� ���� ��ģ ����� �����ش�.
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);  /* Out.vNomal�� ���ý����̽��� ����̴�. ���� ���⺤�ʹ� ���彺���̽� �̱⶧���� ������ ���� ������ ����� ��İ����� ���� ����� �÷��ش�. */
	Out.vTexcoord = In.vTexcoord; //! �ؽ����� ������ �ʴ´ٴ� ���� �� �� �ִ�.
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // + (����ŧ��, ������ ����)���� ��ġ �߰�

	return Out;
}

//TODO ����
//! ������ �׸��� �� �ᱹ �ε����� �׸��Ŵ�.
//! �簢���� ���÷� ���ڸ�  ���� 4��, �ε��� 6���� ������ �ִ�. ������ { 0,1,2,3 } �ε����� { {0,1,2}, {0,2,3} }�� ������ �ִ�.
//! 0��° ������ ������ VS_IN���� ����
//! ����� ������ ���
//! 1��° ������ ������ VS_IN���� ����
//! ����� ������ ���
//! 2��° ������ ������ VS_IN���� ����
//! ����� ������ ���
//! ���� ����� ������ ������ 3���� �Ǿ����� �׸��� �۾��� �����Ѵ� ( Why? �ۿ��� Ʈ���̾ޱ۸���Ʈ�� �׸��ٰ� �����ϱ� ( �ﰢ������ �׸��� ) )
//! ���� 3���� ���ؼ� w������ ��ȯ, ����Ʈ ��ȯ, ���� ���� �� ���� ���� �� ä��ٴ� �۾��帧�� �̾���.
//! �������̴��� 6�� ���
//! �ȼм��̴��� ���� ������ * ���� ������ ���� 800 * 600 �� �� = 480,000 �� ȣ��

struct PS_IN // OUT�� IN�� �����ֱ� ���� ���� �Ȱ��� �߰��Ǿ����
{
	float4		vPosition : SV_POSITION; //! �� PS_IN�ε� ������ �����ٴ� �ø�ƽ����? �����Ͷ����� ������ ���ƴٰ� �ϸ� �翬�� �ȼ�ȭ�� �� �����̴� ������ ������.
	float4		vNormal	  : NORMAL;	// �� ������ ���� ��� �߰�
	float2		vTexcoord :	TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; // ���� ��ġ
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! �ٷ�9������ COLOR��� �߾����� 11���� �ٲ����. �ξ� �������� �������ٰ� �� �� �ִ�. ��? ���° ����Ÿ�ٿ� �׸����� ��� �������̴ϱ�
};

//TODO �ȼ� ���̴� : �ȼ��� ��!!!!!�� �����Ѵ�.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	///* ù��° ������ ������� �ι�° ������ ��ġ�� �ִ� �ȼ��� ���� ���´�. */
	//vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord * 100.0f);

	/* ù��° ������ ������� �ι�° ������ ��ġ�� �ִ� �ȼ��� ���� ���´�. */
	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexcoord * 100.0f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(DefaultSampler, In.vTexcoord * 100.0f);

	vector		vMask = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);

	// �귯���� ������ ���� �׷�����.
	vector		vBrush = vector(0.f, 0.f, 0.f, 0.f);	// �ӽ� ������ �����غ���.

	// vector		vBrush = g_BrushTexture.Sample(DefaultSampler, In.vTexcoord); // �� Diffuse��� �Ȱ��� ���ָ�ȴ�. (���� 100�� �����ָ� Ÿ��ó�� �������� �򸰴�.)

	// �귯���� ��� � ũ��� �����Ұ��� ������ ���� ���������.
	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV; // UV�� ���� �������Ѵ�.

		/*	UV�� ���� ������ִ� ����
			���� UV�� �ع����� ��ü �ؽ�ó�� �Ϻ� ������ ������ �׸��Եȴ�. (ex : ���� �Ϻ��� ���� �׸��ٴ���..)
			���� ���� UV�� �״�� ���ΰ� �� UV�� ���� �׷��ִ� ���̴�. 
		*/
		// ����
		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);	// ����
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);	// ����

		// �귯�� ä���ֱ�
		vBrush = g_BrushTexture.Sample(DefaultSampler, vUV);
	}

	/*	fShade = �� ������ ���� �ʿ��� ��ҵ�
		1. g_vLightDir : ������ �̸� �����ص� ���� ���� 
		2. * -1.f	   : ���� ������ �ݴ�� ������, cos�׷����� �����ϰ� ������ִ� �뵵
		3. In.vNormal  : ���� �޴� ��ü�� ��������
		4. nrmalize	   : ��ֶ�����
		5. dot		   : �� ������ ��� ��ģ �������͸� �������ִ� �����Լ�
		6. max		   : ������ ������ �ٲ��ְԵǴ� �Լ�
	*/

	//																				������ ä���� �귯���� ��ǻ� ���ؼ� �׷��ֱ� (����� �������̶� �����ָ� �˸°� ����.)
	vector		vMtrlDiffuse = vMask * vDestDiffuse + (1.f - vMask) * vSourDiffuse + vBrush;

	//			 ���  = ����(���� ���� * -1.f������, �ȼ��� ��������)
	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* ����ŧ���� ���������ϴ� ���������� 1��, �ƴ� ���������� 0���� ���ǵǴ� ����ŧ���� ���Ⱑ �ʿ��ϴ�. */
	//		�ü� ���� =   �ȼ� ��ġ  -   ī�޶� ��ġ
	vector		vLook = In.vWorldPos - g_vCamPosition;	// �ü� ���� (ī�޶�)

	//			�ݻ� ���� = ���� ����� �ȼ� ������ �������͸� �־� �ݻ纤�͸� ��ȯ�ϴ� �Լ�
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));	// �ݻ� ����

	//			����ŧ��  = �����Լ�(�ִ밪(����(����ȭ(�ü�����) * -1.f������, ����ȭ(�ݻ� ����)), �ִ밪), ����30);
	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	//			����ŧ�� ���� = �� ���� - ������ / �� ����
	//float		fAtt = max(g_fLightRange - length(vLightDir), 0.f) / g_fLightRange;
	//float		fAtt = max(g_fLightRange - length(g_vLightDir), 0.f) / g_fLightRange;

	// ���� ����
	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * fShade * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f) // ;Ȯ��..
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular; // ���, ����ŧ�� ���� �ȵǴ� ���� : VIBuffer_Terrain���� �븻�� �ȹٲ����


	/*	 ����  =	���� ���ݻ�	 * ��ü�� ���ݻ�* �ִ밪 ����(����� �� + (���� ȯ��ݻ� * ��ü�� ȯ��ݻ�)), �ִ밪1); */
	//Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);

	return Out;
}

// ������
PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	/* ù��° ������ ������� �ι�° ������ ��ġ�� �ִ� �ȼ��� ���� ���´�. */
	vector		vMtrlDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexcoord * 100.0f);

	vector		vLightDir = In.vWorldPos - g_vLightPos;


	float		fAtt = max(g_fLightRange - length(vLightDir), 0.f) / g_fLightRange;

	float		fShade = max(dot(normalize(vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* ����ŧ���� ���������ϴ� ���������� 1��, �ƴ� ���������� 0���� ���ǵǴ� ����ŧ���� ���Ⱑ �ʿ��ϴ�. */
	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(vLightDir), normalize(In.vNormal));

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fAtt; // * fAtt�� �������ν� ���� ������ 1, �־����� 0���� ����ŧ���� �������ش�.

	return Out;
}

//	   (���)
technique11 DefaultTechnique //! �ٷ�9 ���ķ� ��ũ��ũ�ڿ� ������ �ٿ������. �츰 �ٷ�11�̴� 11�� �ٿ���
{
	/* ���� ���ϴ� Ư�� ���̴����� �׸��� �𵨿� �����Ѵ�. */
	pass Terrain	// �� ���� �׽�Ʈ�� �ͷ��� �߰�
	{
		/* ������������ */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Delete // ������ (���߿� ����Ŷ� Delete�� �������)
	{
		/* ������������ */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	//pass UI
	//{
	//	//! ����������Ʈ�� �ü��� ����.
	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	GeometryShader = NULL;
	//	HullShader = NULL;
	//	DomainShader = NULL;
	//	PixelShader = compile ps_5_0 PS_MAIN();
	//}

	///* ���� �ٸ� ���¿� ���� ���ϴ� Ư�� ���̴����� �׸��� �𵨿� �����Ѵ�. */
	//pass Particle
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	PixelShader = compile ps_5_0 PS_MAIN();
	//}
};
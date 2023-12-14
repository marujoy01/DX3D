/* ������ ���̴� ���ϰ� ���� ��η� Ŭ���̾�Ʈ�� �Ȱ��� ����������. bat���Ͽ� ��ε� �������. (���� �� �̺�Ʈ ��� �־�����) */
//				����, ��, ���� �࿭�� ����.
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);
//				������ ��������.

struct VS_IN
{				
	float3		vPosition : POSITION; // ������ ��
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	/* In.vPosition * ���� * �� * ���� */
	matrix		matWV, matWVP;

	/* ��ġ���� ���ִ� ������������������ ���� ������. */
	/* ���� -> ���� -> �� �����̽� -> ���� */
	matWV = mul(g_WorldMatrix, g_ViewMatrix); // ���� * �� = �� �����̽� 
	matWVP = mul(matWV, g_ProjMatrix); // �� �����̽� * ���� = ���� �����̽�

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP); // ������ ���� * ���� = ���� ��ġ ����
	// ��ġ���ʹ� w�� 1��, ���⺤�ʹ� w�� 0���� ����Ѵ�!
	

	return Out; // �ϼ��� ������ ��ġ���͸� ��ȯ�Ѵ�.
}

/* ����� ������ ��� .*/

/* ������ȯ : /w */ /* -> -1, 1 ~ 1, -1 */
				/* ����Ʈ��ȯ-> 0, 0 ~ WINSX, WINSY */
				/* �����Ͷ����� : ���������� ����Ͽ� �ȼ��� ������ �����. */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_vColor;

	return Out;
}


technique11 DefaultTechnique
{
	pass Cell
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

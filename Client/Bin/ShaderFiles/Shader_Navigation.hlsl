/* 엔진의 셰이더 파일과 같은 경로로 클라이언트에 똑같이 복사해주자. bat파일에 경로도 잡아주자. (빌드 후 이벤트 경로 넣어주자) */
//				월드, 뷰, 투영 행열을 받자.
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);
//				색상을 정해주자.

struct VS_IN
{				
	float3		vPosition : POSITION; // 포지션 값
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
	matrix		matWV, matWVP;

	/* 장치에서 해주던 랜더링파이프라인을 직접 해주자. */
	/* 로컬 -> 월드 -> 뷰 스페이스 -> 투영 */
	matWV = mul(g_WorldMatrix, g_ViewMatrix); // 월드 * 뷰 = 뷰 스페이스 
	matWVP = mul(matWV, g_ProjMatrix); // 뷰 스페이스 * 투영 = 투영 스페이스

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP); // 로컬의 정점 * 투영 = 투영 위치 벡터
	// 위치벡터는 w를 1로, 방향벡터는 w를 0으로 줘야한다!
	

	return Out; // 완성된 정점의 위치벡터를 반환한다.
}

/* 통과된 정점을 대기 .*/

/* 투영변환 : /w */ /* -> -1, 1 ~ 1, -1 */
				/* 뷰포트변환-> 0, 0 ~ WINSX, WINSY */
				/* 래스터라이즈 : 정점정보에 기반하여 픽셀의 정보를 만든다. */


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

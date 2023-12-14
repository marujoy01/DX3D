#include "..\Public\Picker.h"

IMPLEMENT_SINGLETON(CPicker) // �̱���

CPicker::CPicker()
{
}

CPicker::~CPicker()
{
}

HRESULT CPicker::Initialize(HWND _hWnd)
{
	// ���������� ����
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return S_OK;
}

// �� �����Ӹ��� ���콺 ��ġ ��ǥ�� ���ϱ����� Tick���� ���Ѵ�.
void CPicker::Tick()
{
	POINT	ptCursor;	// ���콺 ��ǥ�� ���� �׸�
	RECT	rcClient;	// Ŭ���̾�Ʈ
	GetCursorPos(&ptCursor); // ���콺 ��ǥ
	ScreenToClient(m_hWnd, &ptCursor);	// �� ��Ʈ
	GetClientRect(m_hWnd, &rcClient);	// ȭ��(Ŭ���̾�Ʈ)

	// ȭ�鿡���� ���콺 ��ġ ��ǥ�� ����Ѵ�.
	//_float3		vCursor(2.f * ptCursor.x / rcClient.right - 1.f, -2.f * ptCursor.y / rcClient.bottom + 1.f, 0.f);
	_vector		vCursor = { 2.f * ptCursor.x / rcClient.right - 1.f,
							-2.f * ptCursor.y / rcClient.bottom + 1.f,
							0.f };

	// ���������ο� ������ִ� ��, ������ �޾ƿ´�.
	_matrix	mView = m_pPipeLine->Get_TransformMatrixInverse(m_pPipeLine->D3DTS_VIEW);
	_matrix	mProj = m_pPipeLine->Get_TransformMatrixInverse(m_pPipeLine->D3DTS_PROJ);

	// XMVector3TransformCoord(vCurs, mProj); �� �ȵƴ��� ��, ���ڱ� ��
	_vector		vWorld = { 0.f, 0.f, 0.f };

	// ������� ���� ��ġ, ������ �����Ѵ�.
	m_vRay[IDX(PICKER::WORLD)] = _float3(XMVector3Normalize(XMVector3TransformNormal(XMVector3TransformCoord(vCursor, mProj), mView)).m128_f32); // Ŀ���� �� �����̽����� ������, ���� �����̽��� ���ؼ� ������� �����ش�.
	m_vOrg[IDX(PICKER::WORLD)] = _float3(XMVector3TransformCoord(vWorld, mView).m128_f32); // ���� �����̽��� ���� �� 0, 0, 0�� ���� ����� ������.

	// ���⼭�� ���� �Ѱ��ش�.
}

void CPicker::Render()
{

}

//						(���� ���� ��ȯ���� �༮, �ε���A, �ε���B, �ε���C, ����)
_bool CPicker::Intersect(_Out_ _float3& _vOut, const _float3 _vA, const _float3 _vB, const _float3 _vC, _In_opt_ const _float4x4 _mWorld)
{
	_vOut = _float3(0.f, 0.f, 0.f); // �⺻��

	_matrix	mUnit = XMMatrixIdentity();

	_matrix mWorld = XMLoadFloat4x4(&_mWorld);

	_vector vOrg = XMLoadFloat3(&m_vOrg[IDX(PICKER::WORLD)]);

	_vector vRay = XMLoadFloat3(&m_vRay[IDX(PICKER::WORLD)]);



	// ���� ���� ��� ���ڰ��� � ������ �Ǵ��ϰ� ��ȯ�Ѵ�. -> ���� ���� ���ڰ��� g_mUnit�� ���ٸ� WORLD, �ƴ϶�� LOCAL ��ȯ
	PICKER eSystem = XMMatrixIsIdentity(mUnit) ? PICKER::WORLD : PICKER::LOCAL;
	
	// ���� ���� ���� ���� ����� ���
	//if (PICKER::WORLD == eSystem)
	{ // ���ñ��� ������ ������ ��ġ�� ������ �����ش�.
		XMStoreFloat3(&m_vOrg[IDX(PICKER::LOCAL)], XMVector3TransformCoord(vOrg, mWorld));
		XMStoreFloat3(&m_vRay[IDX(PICKER::LOCAL)], XMVector3TransformCoord(vRay, mWorld));
	}

	//		�Ÿ�
	_float fDist;

	_vector vA = XMLoadFloat3(&_vA);
	_vector vB = XMLoadFloat3(&_vB);
	_vector vC = XMLoadFloat3(&_vC);

	// Intersects : �츮�� ���� �ߴ� ���콺 ������ �浹ü�� �Ÿ������ �˾Ƽ� ���ϰ� ��ȯ���ش�.
	// �浹�� �������� 0�� ����, �浹�� ������ ��(fDist)�� �Բ� 1�� �����Ѵ�. -> ���ǹ� �ȿ� �ٷ� �־� ��밡��.
	if (DirectX::TriangleTests::Intersects(vOrg, vRay, vA, vB, vC, fDist))
	{
		// ����� = ���� ��ġ + ���� * �Ÿ�(����)
		_vector vOut = vOrg + vRay * fDist;

		XMStoreFloat3(&_vOut, vOut);

		//if (PICKER::WORLD == eSystem)
		{
			XMStoreFloat3(&_vOut, XMVector3TransformCoord(XMLoadFloat3(&_vOut), mWorld));
		}

		return true;
	}

	return false;
}

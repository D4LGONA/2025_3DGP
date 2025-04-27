#pragma once
#include "GameObject.h"

class CEnemyTank : public CExplosiveObject
{
private:
    CGameObject* body = nullptr;

    float m_Delay = 0.0f; // �߻� 
    float m_Range = 50.0f;

public:
    CEnemyTank();
    virtual ~CEnemyTank();

    void Fire();

    virtual void Update(float fElapsedTime);    // �̵� + ���� �Ǵ�
    virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
    int PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance);
};


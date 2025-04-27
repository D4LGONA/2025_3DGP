#pragma once
#include "GameObject.h"

class CEnemyTank : public CExplosiveObject
{
private:
    CGameObject* body = nullptr;

    float m_Delay = 0.0f; // 발사 
    float m_Range = 50.0f;

public:
    CEnemyTank();
    virtual ~CEnemyTank();

    void Fire();

    virtual void Update(float fElapsedTime);    // 이동 + 공격 판단
    virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
    int PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance);
};


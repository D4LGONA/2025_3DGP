#pragma once
#include "GameObject.h"

class CEnemyTank : public CExplosiveObject
{
private:
    CGameObject* body = nullptr;

    float m_Delay = 0.0f; // น฿ป็ 
    float m_Range = 50.0f;

    float targetRot = 0.0f;

public:
    CEnemyTank();
    virtual ~CEnemyTank();

    void Fire();

    void LookAtPlayerIfClose(XMFLOAT3& playerPos, float detectRange);
    virtual void Update(float fElapsedTime);
    virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
    int PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance);
};


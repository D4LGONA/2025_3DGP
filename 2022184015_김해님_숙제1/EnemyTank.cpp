#include "stdafx.h"
#include "EnemyTank.h"

CEnemyTank::CEnemyTank()
{
    body = new CGameObject();                            // ���� ������Ʈ ����
    body->SetMesh(new CCubeMesh(10.0f, 5.0f, 10.0f));   // �ܼ� Cube�� Body ����
    body->SetColor(RGB(255, 0, 0));

}

CEnemyTank::~CEnemyTank()
{
    if (body) delete body;
}

void CEnemyTank::Fire()
{
}

void CEnemyTank::Update(float fElapsedTime)
{
    if (body != nullptr)
    {
        body->SetPosition(CGameObject::GetPosition());
        body->UpdateBoundingBox();
    }
    CExplosiveObject::Animate(fElapsedTime);
    UpdateBoundingBox();
}

void CEnemyTank::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
    if (m_bBlowingUp)
    {
        for (int i = 0; i < EXPLOSION_DEBRISES; i++)
        {
            CGameObject::Render(hDCFrameBuffer, &m_pxmf4x4Transforms[i], m_pExplosionMesh);
        }
    }
    else {
        CGameObject::Render(hDCFrameBuffer, pCamera);
        if(m_bActive == true)
            body->Render(hDCFrameBuffer, pCamera);
    }
}

int CEnemyTank::PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance)
{
     return body->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, pfHitDistance) + CGameObject::PickObjectByRayIntersection(xmvPickPosition, xmmtxView, pfHitDistance);
}

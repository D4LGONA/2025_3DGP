#include "stdafx.h"
#include "EnemyTank.h"

CEnemyTank::CEnemyTank()
{
    body = new CGameObject();                         
    body->SetMesh(new CCubeMesh(10.0f, 5.0f, 10.0f)); 
    body->SetColor(RGB(255, 0, 0));

}

CEnemyTank::~CEnemyTank()
{
    if (body) delete body;
}

void CEnemyTank::Fire()
{
}

void CEnemyTank::LookAtPlayerIfClose(XMFLOAT3& playerPos, float detectRange)
{
    XMFLOAT3 myPos = GetPosition();
    XMFLOAT3 toPlayer = Vector3::Subtract(playerPos, myPos);
    float dist = Vector3::Length(toPlayer);

    if (dist <= detectRange)
    {
        XMFLOAT3 up = Vector3::Normalize(toPlayer);

        XMFLOAT3 tempLook = XMFLOAT3(0, -1, 0);
        XMFLOAT3 right = Vector3::Normalize(Vector3::CrossProduct(tempLook, up));
        XMFLOAT3 look = Vector3::Normalize(Vector3::CrossProduct(up, right));

        XMFLOAT4X4 world = m_xmf4x4World;

        world._11 = right.x; world._12 = right.y; world._13 = right.z;
        world._21 = up.x;    world._22 = up.y;    world._23 = up.z;
        world._31 = look.x;  world._32 = look.y;  world._33 = look.z;

        m_xmf4x4World = world;
    }
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

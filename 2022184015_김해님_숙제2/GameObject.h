#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	XMFLOAT4X4 m_xmf4x4World;
protected:
	CMesh* m_pMesh = NULL;
	CShader* m_pShader = NULL;
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances);

public:
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	//게임 객체의 위치를 설정한다. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	//게임 객체를 회전(x-축, y-축, z-축)한다. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

public:
	//상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

public:
	//게임 객체가 카메라에 보인는 가를 검사한다.
	bool IsVisible(CCamera* pCamera = NULL);

public:
	//모델 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다.
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis =
			xmf3RotationAxis;
	}
	virtual void Animate(float fTimeElapsed);
};

//----------------------------------------------------

class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool m_bBlowingUp = false;
	float m_fElapsedTimes = 0.0f;
	float m_fDuration = 2.0f;
	float m_fExplosionSpeed = 10.0f;
	float m_fExplosionRotation = 720.0f;

	CInstancingShader* m_pInstancingShader = nullptr;
	CMesh* m_expMesh = nullptr;

	void setExplosionMesh(CMesh* mesh);
	void Animate(float fElapsedTime);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void SetExpShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature);
	void StartExplosion();
	void Reset();
};

//----------------------------------------------------
// explosive에서 사용할 그냥 움직이는 아이
class CMovingObject : public CGameObject
{
public:
	CMovingObject(XMFLOAT3 dir, float speed, float rotationSpeed);
	virtual void Animate(float fElapsedTime) override;
	void Reset();
private:
	XMFLOAT3 m_xmf3Direction;
	float m_fSpeed;
	float m_fRotationSpeed;
	float m_fTimeElapsed = 0.0f;
};

//----------------------------------------------------

class CBulletObject : public CGameObject
{
public:
	CBulletObject(float fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Animate(float fElapsedTime);

	float						m_fBulletEffectiveRange = 50.0f;
	float						m_fMovingDistance = 0.0f;
	float						m_fRotationAngle = 0.0f;
	XMFLOAT3					m_xmf3FirePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float						m_fElapsedTimeAfterFire = 0.0f;
	float						m_fLockingDelayTime = 0.3f;
	float						m_fLockingTime = 4.0f;
	CGameObject* m_pLockedObject = NULL;

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void Reset();
};
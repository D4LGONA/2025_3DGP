#pragma once
#include "Mesh.h"
#include "Camera.h"
class CShader;
class CInstancingShader;

class CGameObject
{
public:
	bool bActive = true;

	CGameObject();
	virtual ~CGameObject();

// Reference 관련
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

// 변수들
public:
	int m_nSubMeshes = 1;

	char m_pstrFrameName[64];
	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Transform;

	CMesh* m_pMesh = NULL;
	CShader* m_pShader = NULL;

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;

// 초기화 관련
public:
	void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);
	void SetScale(float x, float y, float z);

	void ReleaseUploadBuffers();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances);


// 계층구조
	CGameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(char* pstrFrameName);

public:
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

public:
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

	static CMeshLoadInfo* LoadMeshInfoFromFile(FILE* pInFile);
	CGameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FILE* pInFile);
	CGameObject* LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName);

public:
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);


	bool CGameObject::CheckCollisionRecursive(CGameObject* pObject, const BoundingOrientedBox& bulletBox);

	BoundingOrientedBox GetBoundingBox()
	{
		if (m_pMesh == nullptr) {
			// 빈 박스: 중심 (0,0,0), extents (0,0,0), 회전 없음
			BoundingOrientedBox emptyBox;
			emptyBox.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
			emptyBox.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);
			emptyBox.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			return emptyBox;
		}
		BoundingOrientedBox boundingBox = m_pMesh->m_xmBoundingBox;
		boundingBox.Transform(boundingBox, XMLoadFloat4x4(&m_xmf4x4World));
		return boundingBox;
	}

	bool IsVisible(CCamera* pCamera);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
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

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();
private:
	CHeightMapImage* m_pHeightMapImage;
	int m_nWidth;
	int m_nLength;
	
	XMFLOAT3 m_xmf3Scale;
		
public:
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x /
			m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z /
			m_xmf3Scale.z)));
	}
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(float fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Animate(float fElapsedTime);

	float						m_fBulletEffectiveRange = 1000.0f;
	float						m_fMovingDistance = 0.0f;
	float						m_fRotationAngle = 0.0f;
	XMFLOAT3					m_xmf3FirePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float						m_fElapsedTimeAfterFire = 0.0f;
	float						m_fLockingTime = 3.0f;
	float						m_fRotationSpeed = 150.0f;
	CGameObject* m_pLockedObject = NULL;

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void SetMovingDirection(XMFLOAT3 xmf3Direction) {
		m_xmf3MovingDirection = xmf3Direction;
	}
	void Reset();
};

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
	void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void SetExpShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature);
	void StartExplosion();
	void Reset();
};

class CMovingObject : public CGameObject
{
public:
	CMovingObject(XMFLOAT3 dir, float speed, float rotationSpeed);
	void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	void Reset();
private:
	XMFLOAT3 m_xmf3Direction;
	float m_fSpeed;
	float m_fRotationSpeed;
	float m_fTimeElapsed = 0.0f;
};

class CTankObject : public CExplosiveObject
{
public:
	CGameObject* m_pTurretFrame = NULL;
	CGameObject* m_pCannonFrame = NULL;
	CGameObject* m_pBodyFrame = NULL;

	LPVOID m_pObjectUpdatedContext;

	XMFLOAT3 m_xmf3MoveDirection = XMFLOAT3(0.0f, 0.0f, 1.0f); // 기본 전진 방향
	float m_fSpeed = 1.0f;

	CTankObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		void* pContext, int nMeshes);
	virtual ~CTankObject();
	void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void SetShader(CShader* pShader)
	{
		CGameObject::SetShader(pShader);
	}
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
	void SetObjectUpdatedContext(LPVOID pContext) { m_pObjectUpdatedContext = pContext; }
	void OnInitialize();

	void CollectBoundingBoxes(std::vector<BoundingOrientedBox>& boxes)
	{
		boxes.push_back(m_pTurretFrame->GetBoundingBox());
		boxes.push_back(m_pCannonFrame->GetBoundingBox());
		boxes.push_back(m_pBodyFrame->GetBoundingBox());
	}
};
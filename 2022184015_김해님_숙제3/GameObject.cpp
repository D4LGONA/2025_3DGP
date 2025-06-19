#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

//-------------------------------------------------------------------

CGameObject::CGameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();

	if (m_pSibling) m_pSibling->SetShader(pShader);
	if (m_pChild) m_pChild->SetShader(pShader);
}

void CGameObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World);
}

void CGameObject::OnPrepareRender()
{
	
}

bool CGameObject::IsVisible(CCamera* pCamera)
{
	if (m_pMesh == nullptr) return true;
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox localOBB = m_pMesh->m_xmBoundingBox;
	BoundingOrientedBox worldOBB;
	localOBB.Transform(worldOBB, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) bIsVisible = pCamera->IsInFrustum(localOBB);
	return(bIsVisible);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (false == bActive) return;
	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
	m_pShader->OnPrepareRender(pd3dCommandList);

	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nSubMeshes; i++)
	{
		if (m_pMesh) 
			m_pMesh->Render(pd3dCommandList, i);
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances)
{
	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

//게임 객체의 로컬 z-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}

//게임 객체의 로컬 y-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}

//게임 객체의 로컬 x-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}

//게임 객체를 로컬 x-축 방향으로 이동한다. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 y-축 방향으로 이동한다. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 z-축 방향으로 이동한다. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 주어진 각도로 회전한다. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{
}

CMeshLoadInfo* CGameObject::LoadMeshInfoFromFile(FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	CMeshLoadInfo* pMeshInfo = new CMeshLoadInfo;

	pMeshInfo->m_nVertices = ::ReadIntegerFromFile(pInFile);
	::ReadStringFromFile(pInFile, pMeshInfo->m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nPositions = ::ReadIntegerFromFile(pInFile);
			if (nPositions > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_POSITION;
				pMeshInfo->m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(pInFile);
			if (nColors > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_COLOR;
				pMeshInfo->m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(pInFile);
			if (nNormals > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_NORMAL;
				pMeshInfo->m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			nIndices = ::ReadIntegerFromFile(pInFile);
			if (nIndices > 0)
			{
				pMeshInfo->m_pnIndices = new UINT[nIndices];
				nReads = (UINT)::fread(pMeshInfo->m_pnIndices, sizeof(int), nIndices, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			pMeshInfo->m_nSubMeshes = ::ReadIntegerFromFile(pInFile);
			if (pMeshInfo->m_nSubMeshes > 0)
			{
				pMeshInfo->m_pnSubSetIndices = new int[pMeshInfo->m_nSubMeshes];
				pMeshInfo->m_ppnSubSetIndices = new UINT * [pMeshInfo->m_nSubMeshes];
				for (int i = 0; i < pMeshInfo->m_nSubMeshes; i++)
				{
					pMeshInfo->m_ppnSubSetIndices[i] = NULL;
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = ::ReadIntegerFromFile(pInFile);
						pMeshInfo->m_pnSubSetIndices[i] = ::ReadIntegerFromFile(pInFile);
						if (pMeshInfo->m_pnSubSetIndices[i] > 0)
						{
							pMeshInfo->m_ppnSubSetIndices[i] = new UINT[pMeshInfo->m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(pMeshInfo->m_ppnSubSetIndices[i], sizeof(UINT), pMeshInfo->m_pnSubSetIndices[i], pInFile);
						}

					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
	return(pMeshInfo);
}

CGameObject* CGameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0;

	CGameObject* pGameObject = NULL;

	for (;;)
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject();

			nFrame = ::ReadIntegerFromFile(pInFile);
			::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CMeshLoadInfo* pMeshInfo = pGameObject->LoadMeshInfoFromFile(pInFile);
			if (pMeshInfo)
			{
				CMesh* pMesh = NULL;
				if (pMeshInfo->m_nType & VERTEXT_NORMAL)
				{
					pMesh = new CMeshFromFile(pd3dDevice, pd3dCommandList, pMeshInfo);
				}
				if (pMesh) pGameObject->SetMesh(pMesh);
				delete pMeshInfo;
			}
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject* pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pInFile);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_RUNTIME_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Child Frame: %p) (Parent Frame: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

CGameObject* CGameObject::LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CGameObject* pGameObject = NULL;
	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Hierarchy>:"))
		{
			pGameObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pInFile);
		}
		else if (!strcmp(pstrToken, "</Hierarchy>"))
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pGameObject);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}

int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;

	// 1. 현재 오브젝트의 메시 충돌 검사
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		nIntersected += m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}

	// 2. 자식 오브젝트 재귀 검사
	if (m_pChild)
		nIntersected += m_pChild->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, pfHitDistance);

	// 3. 형제 오브젝트 재귀 검사
	if (m_pSibling)
		nIntersected += m_pSibling->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, pfHitDistance);

	return(nIntersected);
}

bool CGameObject::CheckCollisionRecursive(CGameObject* pObject, const BoundingOrientedBox& bulletBox)
{
	if (!pObject) return false;
	if (pObject->GetBoundingBox().Intersects(bulletBox)) return true;
	if (CheckCollisionRecursive(pObject->m_pChild, bulletBox)) return true;
	if (CheckCollisionRecursive(pObject->m_pSibling, bulletBox)) return true;
	return false;
}

//----------------------------------------------------------------------------------------

CRotatingObject::CRotatingObject(int nMeshes) : CGameObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, 
	int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) 
	: CGameObject()
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//지형을 렌더링하기 위한 셰이더를 생성한다.
	CTerrainShader* pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

	for (int z = 0; z < czBlocks; ++z) {
		for (int x = 0; x < cxBlocks; ++x) {
			int xStart = x * cxQuadsPerBlock;
			int zStart = z * czQuadsPerBlock;

			CHeightMapGridMesh* pGridMesh = new CHeightMapGridMesh(
				pd3dDevice, pd3dCommandList,
				xStart, zStart, nBlockWidth, nBlockLength,
				xmf3Scale, xmf4Color, m_pHeightMapImage
			);

			CGameObject* pBlockObject = new CGameObject();
			pBlockObject->SetMesh(pGridMesh);
			SetChild(pBlockObject); 
			pBlockObject->SetShader(pShader);
		}
	}
	SetShader(pShader);
}

CHeightMapTerrain::~CHeightMapTerrain()
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

CBulletObject::CBulletObject(float fEffectiveRange)
{
	m_fBulletEffectiveRange = fEffectiveRange;
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{
	m_xmf3FirePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Reset()
{
	m_pLockedObject = NULL;
	m_fElapsedTimeAfterFire = 0;
	m_fMovingDistance = 0;
	m_fRotationAngle = 0.0f;
	bActive = false;
}

void CBulletObject::Animate(float fElapsedTime)
{
	if (!bActive) return;
	m_fElapsedTimeAfterFire += fElapsedTime;

	float fDistance = m_fRotationSpeed * fElapsedTime;

	if (m_pLockedObject)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		XMVECTOR xmvPosition = XMLoadFloat3(&xmf3Position);

		XMFLOAT3 xmf3TargetPosition = m_pLockedObject->GetPosition();
		XMVECTOR xmvTargetPosition = XMLoadFloat3(&xmf3TargetPosition);

		XMVECTOR xmvToTarget = XMVector3Normalize(xmvTargetPosition - xmvPosition);
		XMVECTOR xmvCurrent = XMLoadFloat3(&m_xmf3MovingDirection);
		XMVECTOR xmvSmoothed = XMVector3Normalize(XMVectorLerp(xmvCurrent, xmvToTarget, 0.25f));
		XMStoreFloat3(&m_xmf3MovingDirection, xmvSmoothed);
	}

	// 이동
	XMFLOAT3 xmf3Move = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	XMFLOAT3 xmf3Position = Vector3::Add(GetPosition(), xmf3Move);
	SetPosition(xmf3Position);
	m_fMovingDistance += fDistance;


	// 유효 범위 또는 생존 시간 초과 시 리셋
	if ((m_fMovingDistance > m_fBulletEffectiveRange) || (m_fElapsedTimeAfterFire > m_fLockingTime))
		Reset();
}

//----------------------------------------------------------------------------------------

CExplosiveObject::CExplosiveObject()
{

}

CExplosiveObject::~CExplosiveObject()
{
}

#define EXPLOSION_DEBRISES		240
void CExplosiveObject::setExplosionMesh(CMesh* mesh)
{
	std::vector<CGameObject*> tmp;
	for (int i = 0; i < EXPLOSION_DEBRISES; ++i)
	{
		XMVECTOR dirVec = ::RandomUnitVectorOnSphere();
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, dirVec);
		auto obj = new CMovingObject(direction, m_fExplosionSpeed, m_fExplosionRotation);
		obj->SetMesh(mesh);

		tmp.emplace_back(obj);
	}

	m_pInstancingShader = new CInstancingShader(tmp);
}

void CExplosiveObject::Animate(float fElapsedTime, XMFLOAT4X4* pxmf4x4Parent)
{
	if (!m_bBlowingUp)
	{
		CGameObject::Animate(fElapsedTime);
	}
	else
	{
		m_fElapsedTimes += fElapsedTime;

		m_pInstancingShader->AnimateObjects(fElapsedTime);

		if (m_fElapsedTimes >= m_fDuration) {
			m_bBlowingUp = false;
			bActive = false;
			Reset();
		}
	}
}

void CExplosiveObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp && m_pInstancingShader)
		m_pInstancingShader->Render(pd3dCommandList, pCamera);
	else
		CGameObject::Render(pd3dCommandList, pCamera);
}

void CExplosiveObject::SetExpShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature)
{
	if (m_pInstancingShader)
	{
		m_pInstancingShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
		m_pInstancingShader->BuildObjects(pd3dDevice, pd3dCommandList);
	}
}

void CExplosiveObject::StartExplosion()
{
	m_bBlowingUp = true;
	m_fElapsedTimes = 0.0f;

	// 이 시점에 초기 위치를 지정해줘야 각 debris 위치가 기준점 주변에서 시작됨
	XMFLOAT3 origin = GetPosition();
	m_pInstancingShader->SetPosition(origin);
}

void CExplosiveObject::Reset()
{
	m_bBlowingUp = false;
	m_fElapsedTimes = 0.0f;
	m_pInstancingShader->Reset();
}

//----------------------------------------------------------------------------------------

CMovingObject::CMovingObject(XMFLOAT3 dir, float speed, float rotationSpeed)
	: m_xmf3Direction(dir), m_fSpeed(speed), m_fRotationSpeed(rotationSpeed)
{
	m_fTimeElapsed = 0.0f;

	// 기본적으로 방향 단위 벡터로 정규화 (안전성 확보)
	float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if (length > 0.0f)
	{
		m_xmf3Direction.x /= length;
		m_xmf3Direction.y /= length;
		m_xmf3Direction.z /= length;
	}

	// 월드 행렬 초기화 (Identity 또는 필요 시 위치/회전 초기화)
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

void CMovingObject::Animate(float fElapsedTime, XMFLOAT4X4* pxmf4x4Parent)
{
	m_fTimeElapsed += fElapsedTime;

	if (m_fTimeElapsed <= 2.0f)
	{
		XMFLOAT3 position = GetPosition();

		// 위치 이동
		position.x += m_xmf3Direction.x * m_fSpeed * fElapsedTime;
		position.y += m_xmf3Direction.y * m_fSpeed * fElapsedTime;
		position.z += m_xmf3Direction.z * m_fSpeed * fElapsedTime;
		SetPosition(position);

		// 회전
		XMMATRIX mtxWorld = XMLoadFloat4x4(&m_xmf4x4World);
		XMMATRIX mtxRot = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Direction), XMConvertToRadians(m_fRotationSpeed * fElapsedTime));
		mtxWorld = XMMatrixMultiply(mtxRot, mtxWorld);
		XMStoreFloat4x4(&m_xmf4x4World, mtxWorld);
	}
}

void CMovingObject::Reset()
{
	m_fTimeElapsed = 0.0f;
}

CTankObject::CTankObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth() * 0.5f + 10.0f, pTerrain->GetLength() * 0.5f + 10.0f);
	SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f + 10.0f, fHeight + TANK_HEIGHT, pTerrain->GetLength() * 0.5f + 10.0f)); // 여기 초기위치
	SetObjectUpdatedContext(pTerrain);

	CGameObject* pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "M26.bin");

	pGameObject->Rotate(0.0f, 180.0f, 0.0f);
	pGameObject->SetScale(5.0f, 5.0f, 5.0f);
	SetChild(pGameObject, true);

	CPlayerShader* pShader = new CPlayerShader();
	setExplosionMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f));
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);

	SetExpShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	OnInitialize();

	CCubeMeshDiffused* pBulletMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 2.0f, 2.0f, 2.0f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CTankObject::~CTankObject()
{
}

void CTankObject::OnInitialize()
{
	m_pTurretFrame = FindFrame("TURRET");
	m_pCannonFrame = FindFrame("cannon");
	m_pBodyFrame = FindFrame("BODY");

	// 앞에 보게 초기 회전
	XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(-17.0f));
	m_pTurretFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTurretFrame->m_xmf4x4Transform);
	Rotate(0.0f, 180.0f, 0.0f);

	float angle = RandF(0.0f, XM_2PI); 
	m_xmf3MoveDirection = XMFLOAT3(cosf(angle), 0.0f, sinf(angle));
	m_fSpeed = 10.0f; 
}

void CTankObject::Animate(float fElapsedTime, XMFLOAT4X4* pxmf4x4Parent)
{
	if (!m_bBlowingUp) 
	{
		XMFLOAT3 currentPos = GetPosition();

		currentPos.x += m_xmf3MoveDirection.x * m_fSpeed * fElapsedTime;
		currentPos.z += m_xmf3MoveDirection.z * m_fSpeed * fElapsedTime;

		CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pObjectUpdatedContext;
		float terrainHeight = pTerrain->GetHeight(currentPos.x, currentPos.z) + TANK_HEIGHT;
		currentPos.y = terrainHeight;

		SetPosition(currentPos);
	}

	CExplosiveObject::Animate(fElapsedTime, pxmf4x4Parent);

}

void CTankObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	if (m_bBlowingUp && m_pInstancingShader) {
		m_pInstancingShader->Render(pd3dCommandList, pCamera);
	}
	else
	{
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

int CTankObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		nIntersected += m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	// 자식 오브젝트 재귀 검사
	for (CGameObject* pChild = m_pChild; pChild != nullptr; pChild = pChild->m_pSibling)
	{
		nIntersected += pChild->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, pfHitDistance);
	}
	return nIntersected;
}

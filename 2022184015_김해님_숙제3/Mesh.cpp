﻿#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	// 서브메시 관련 리소스 해제
	if (m_ppd3dSubSetIndexBuffers) {
		for (int i = 0; i < m_nSubMeshes; ++i) {
			if (m_ppd3dSubSetIndexBuffers[i]) m_ppd3dSubSetIndexBuffers[i]->Release();
			if (m_ppd3dSubSetIndexUploadBuffers && m_ppd3dSubSetIndexUploadBuffers[i]) m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		delete[] m_ppd3dSubSetIndexBuffers;
		delete[] m_ppd3dSubSetIndexUploadBuffers;
		delete[] m_pd3dSubSetIndexBufferViews;
		delete[] m_pnSubSetIndices;
	}
}

void CMesh::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer) { m_pd3dVertexUploadBuffer->Release(); m_pd3dVertexUploadBuffer = NULL; }
	if (m_ppd3dSubSetIndexUploadBuffers) {
		for (int i = 0; i < m_nSubMeshes; ++i) {
			if (m_ppd3dSubSetIndexUploadBuffers[i]) {
				m_ppd3dSubSetIndexUploadBuffers[i]->Release();
				m_ppd3dSubSetIndexUploadBuffers[i] = NULL;
			}
		}
	}
};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_ppd3dSubSetIndexBuffers[0])
	{
		pd3dCommandList->IASetIndexBuffer(&m_pd3dSubSetIndexBufferViews[0]);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

int CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float* pfNearHitDistance)
{
	////하나의 메쉬에서 광선은 여러 개의 삼각형과 교차할 수 있다. 교차하는 삼각형들 중 가장 가까운 삼각형을 찾는다.
	//int nIntersections = 0;
	//BYTE* pbPositions = (BYTE*)m_pVertices;
	//int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	///*메쉬의 프리미티브(삼각형)들의 개수이다. 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각
 //  형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 – 2)이다.*/
	//int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
	//	(m_nVertices / 3) : (m_nVertices - 2);
	//if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology ==
	//	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);
	////광선은 모델 좌표계로 표현된다.
	//XMVECTOR xmRayOrigin = DirectX::XMLoadFloat3(&xmf3RayOrigin);
	//XMVECTOR xmRayDirection = DirectX::XMLoadFloat3(&xmf3RayDirection);
	////모델 좌표계의 광선과 메쉬의 바운딩 박스(모델 좌표계)와의 교차를 검사한다.
	//bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection,
	//	*pfNearHitDistance);
	////모델 좌표계의 광선이 메쉬의 바운딩 박스와 교차하면 메쉬와의 교차를 검사한다.
	//if (bIntersected)
	//{
	//	float fNearHitDistance = FLT_MAX;
	//	/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다. 충돌하는 모든 삼각형을 찾아 광선의
	//   시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/
	//	for (int i = 0; i < nPrimitives; i++)
	//	{
	//		XMVECTOR v0 = DirectX::XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_nStride));
	//		XMVECTOR v1 = DirectX::XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_nStride));
	//		XMVECTOR v2 = DirectX::XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_nStride));
	//		float fHitDistance;
	//		BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,
	//			v1, v2, fHitDistance);
	//		if (bIntersected)
	//		{
	//			if (fHitDistance < fNearHitDistance)
	//			{
	//				*pfNearHitDistance = fNearHitDistance = fHitDistance;
	//			}
	//			nIntersections++;
	//		}
	//	}
	//}
	//return(nIntersections);
	return 0;
}

//-----------------------------------------------------------------------------

CTriangleMesh::CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

//-----------------------------------------------------------------------------

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_nIndices = 36;
	UINT pnIndices[36];
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	// 서브메시 정보 초기화
	m_nSubMeshes = 1;
	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_pnSubSetIndices[0] = m_nIndices; // 전체 인덱스를 하나의 서브메시로 처리

	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

	// 서브메시 인덱스 버퍼 생성
	m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(
		pd3dDevice, pd3dCommandList,
		pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_ppd3dSubSetIndexUploadBuffers[0]
	);

	// 인덱스 버퍼 뷰 설정
	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_nIndices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

//-----------------------------------------------------------------------------

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth,
	XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//위의 그림과 같은 비행기 메쉬를 표현하기 위한 정점 데이터이다. 
	CDiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy -
		y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//비행기 메쉬의 위쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 아래쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 오른쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/오른쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 왼쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/왼쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3
	xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	//파일을 열고 읽는다. 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다.
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x +
				(y * m_nWidth)];
		}
	}
	if (pHeightMapPixels) delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터이다.
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));
	/*높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한 픽셀 값을 사용하여 법선 벡터를
   계산한다.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	//(x, z), (x+1, z), (z, z+1)의 픽셀에서 지형의 높이를 구한다.
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	//xmf3Edge1은 (0, y3, m_xmf3Scale.z) - (0, y1, 0) 벡터이다.
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	//xmf3Edge2는 (m_xmf3Scale.x, y2, 0) - (0, y1, 0) 벡터이다.
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	//법선 벡터는 xmf3Edge1과 xmf3Edge2의 외적을 정규화하면 된다.
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);
	return(xmf3Normal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMapImage::GetHeight(float fx, float fz)
{
	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다. 높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 높이는
   0이다.*/
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;
	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-좌표가 1, 3, 5, ...인 경우 인덱스가 오른쪽에서 왼쪽으로 나열된다.
	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
		/*지형의 삼각형들이 오른쪽에서 왼쪽 방향으로 나열되는 경우이다. 다음 그림의 오른쪽은 (fzPercent < fxPercent)
	   인 경우이다. 이 경우 TopLeft의 픽셀 값은 (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))로 근사한다.
	   다음 그림의 왼쪽은 (fzPercent ≥ fxPercent)인 경우이다. 이 경우 BottomRight의 픽셀 값은 (fBottomRight =
	   fBottomLeft + (fTopRight - fTopLeft))로 근사한다.*/
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		/*지형의 삼각형들이 왼쪽에서 오른쪽 방향으로 나열되는 경우이다. 다음 그림의 왼쪽은 (fzPercent < (1.0f -
	   fxPercent))인 경우이다. 이 경우 TopRight의 픽셀 값은 (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))로
	   근사한다. 다음 그림의 오른쪽은 (fzPercent ≥ (1.0f - fxPercent))인 경우이다. 이 경우 BottomLeft의 픽셀 값은
	   (fBottomLeft = fTopLeft + (fBottomRight - fTopRight))로 근사한다.*/
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int
	nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	//격자의 교점(정점)의 개수는 (nWidth * nLength)이다.
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(CDiffusedVertex);
	//격자는 삼각형 스트립으로 구성한다.
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	CDiffusedVertex* pVertices = new CDiffusedVertex[m_nVertices];
	/*xStart와 zStart는 격자의 시작 위치(x-좌표와 z-좌표)를 나타낸다. 커다란 지형은 격자들의 이차원 배열로 만들 필
   요가 있기 때문에 전체 지형에서 각 격자의 시작 위치를 나타내는 정보가 필요하다.*/
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//정점의 높이와 색상을 높이 맵으로부터 구한다.
			XMFLOAT3 xmf3Position = XMFLOAT3((x * m_xmf3Scale.x), OnGetHeight(x, z, pContext),
				(z * m_xmf3Scale.z));
			XMFLOAT4 xmf3Color = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			pVertices[i] = CDiffusedVertex(xmf3Position, xmf3Color);
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	delete[] pVertices;
	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	UINT* pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다.
			for (int x = 0; x < nWidth; x++)
			{
				//첫 번째 줄을 제외하고 줄이 바뀔 때마다(x == 0) 첫 번째 인덱스를 추가한다.
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다.
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (nWidth-1)) 첫 번째 인덱스를 추가한다.
				if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다.
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	// 서브메시 정보 초기화
	m_nSubMeshes = 1;
	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_pnSubSetIndices[0] = m_nIndices; // 전체 인덱스를 하나의 서브메시로 처리

	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

	// 서브메시 인덱스 버퍼 생성
	m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(
		pd3dDevice, pd3dCommandList,
		pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_ppd3dSubSetIndexUploadBuffers[0]
	);

	// 인덱스 버퍼 뷰 설정
	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_nIndices;
	delete[] pnIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

//높이 맵 이미지의 픽셀 값을 지형의 높이로 반환한다. 
float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	//조명의 방향 벡터(정점에서 조명까지의 벡터)이다.
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	//조명의 색상(세기, 밝기)이다. 
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	/*정점 (x, z)에서 조명이 반사되는 양(비율)은 정점 (x, z)의 법선 벡터와 조명의 방향 벡터의 내적(cos)과 인접한 3개
   의 정점 (x+1, z), (x, z+1), (x+1, z+1)의 법선 벡터와 조명의 방향 벡터의 내적을 평균하여 구한다. 정점 (x, z)의 색
   상은 조명 색상(세기)과 반사되는 양(비율)을 곱한 값이다.*/
	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1),
		xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;
	//fScale은 조명 색상(밝기)이 반사되는 비율이다.
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}

CMeshLoadInfo::~CMeshLoadInfo()
{
	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;

	if (m_pnSubSetIndices) delete[] m_pnSubSetIndices;

	for (int i = 0; i < m_nSubMeshes; i++) if (m_ppnSubSetIndices[i]) delete[] m_ppnSubSetIndices[i];
	if (m_ppnSubSetIndices) delete[] m_ppnSubSetIndices;
}

CMeshFromFile::CMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMeshLoadInfo* pMeshInfo)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	// 정점 버퍼 생성
	m_nVertices = pMeshInfo->m_nVertices;
	m_nStride = sizeof(DirectX::XMFLOAT3); // 또는 실제 정점 구조 크기

	// todo: 인덱스버퍼랑 버텍스버퍼 미리 저장해놔야함 CMesh에

	m_pd3dVertexBuffer = CreateBufferResource(
		pd3dDevice, pd3dCommandList,
		pMeshInfo->m_pxmf3Positions, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer
	);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	// 서브메시(부분 메시) 지원
	m_nSubMeshes = pMeshInfo->m_nSubMeshes;
	if (m_nSubMeshes > 0)
	{
		m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
		m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
		m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];
		m_pnSubSetIndices = new int[m_nSubMeshes];

		for (int i = 0; i < m_nSubMeshes; i++)
		{
			m_pnSubSetIndices[i] = pMeshInfo->m_pnSubSetIndices[i];
			m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(
				pd3dDevice, pd3dCommandList,
				pMeshInfo->m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i],
				D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
				&m_ppd3dSubSetIndexUploadBuffers[i]
			);
			m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
			m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
			m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
		}
	}
}

CMeshFromFile::~CMeshFromFile()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();

	if (m_nSubMeshes > 0)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexBuffers[i]) m_ppd3dSubSetIndexBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexBuffers) delete[] m_ppd3dSubSetIndexBuffers;
		if (m_pd3dSubSetIndexBufferViews) delete[] m_pd3dSubSetIndexBufferViews;

		if (m_pnSubSetIndices) delete[] m_pnSubSetIndices;
	}
}

void CMeshFromFile::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if ((m_nSubMeshes > 0) && m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexUploadBuffers[i]) m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexUploadBuffers) delete[] m_ppd3dSubSetIndexUploadBuffers;
		m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
}

void CMeshFromFile::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
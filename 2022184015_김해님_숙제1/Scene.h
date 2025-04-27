#pragma once
#include "stdafx.h"
#include "Camera.h"

// Scene 클래스: 4개 씬들의 부모 클래스입니다.

class CScene 
{
public:
    virtual ~CScene() {}

	virtual void BuildObjects() {};
	virtual void ReleaseObjects() {};

	virtual void Animate(float fElapsedTime) {};
	virtual void Render(HDC hDCFrameBuffer) {};

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime) = 0;
};
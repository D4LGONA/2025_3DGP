// SceneManager.h
#pragma once

#include "stdafx.h"
#include "Scene.h"

class SceneManager 
{
private:
    std::map<std::string, CScene*> scenes;
    CScene* currentScene = nullptr;

public:
    ~SceneManager();

    void AddScene(const std::string& name, CScene* scene);
    void ChangeScene(const std::string& name);
    void Update(float deltaTime);
    void Render();
    void Release();
};
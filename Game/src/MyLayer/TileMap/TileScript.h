#pragma once

#include "Scene/ScriptableClass.h"

class TileScript : public Game::ScriptableClass
{
public:
    enum class States
    {
        Visible = 0b1,
        Solid = 0b01
    }

    TileScript() = default;
	virtual ~ScriptableClass() = default;
public:

	virtual void OnAttach()
    {

    }
	
    virtual void OnAwake(){}
	virtual void OnUpdate(Timestamp dt){}
	virtual void OnDestroy(){}

private:
     int32_t m_States;
};
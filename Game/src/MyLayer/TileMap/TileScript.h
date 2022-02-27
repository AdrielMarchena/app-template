#pragma once

#include "Scene/ScriptableClass.h"

class TileScript : public Game::ScriptableClass
{
public:
    enum class States
    {
        Visible = 0b1,
        Solid = 0b01
    };

    TileScript() = default;
	virtual ~TileScript() = default;
public:

	virtual void OnAttach() override
    {

    }
	
    virtual void OnAwake() override
    {

    }

	virtual void OnUpdate(Game::Timestamp dt) override
    {

    }

	virtual void OnDestroy() override
    {

    }

private:
     int32_t m_States;
};
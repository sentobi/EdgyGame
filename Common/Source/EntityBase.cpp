#include "EntityBase.h"

EntityBase::EntityBase(string name) 
	: position(0.0f, 0.0f, 0.0f)
	, scale(1.0f, 1.0f, 1.0f)
	, isDone(false)
	, m_bCollider(false)
	, bLaser(false)
    , m_bIsInSceneGraph(false)
    , b_isLowResRender(false)
    , name(name)
{
}

EntityBase::~EntityBase()
{
}

void EntityBase::Update(double _dt)
{
}

void EntityBase::Render()
{
}

void EntityBase::RenderUI()
{
}

bool EntityBase::IsDone()
{
	return isDone;
}

void EntityBase::SetIsDone(bool _value)
{
	isDone = _value;
}

// Check if this entity has a collider class parent
bool EntityBase::HasCollider(void) const
{
	return m_bCollider;
}

// Set the flag to indicate if this entity has a collider class parent
void EntityBase::SetCollider(const bool _value)
{
	m_bCollider = _value;
}

// Set the flag, bLaser
void EntityBase::SetIsLaser(const bool bLaser)
{
	this->bLaser = bLaser;
}

// Get the flag, bLaser
bool EntityBase::GetIsLaser(void) const
{
	return bLaser;
}

string EntityBase::GetName() const
{
    return name;
}

bool EntityBase::GetIsInSceneGraph()
{
    return m_bIsInSceneGraph;
}

void EntityBase::SetInSceneGraph(const bool b_isInGraph)
{
    m_bIsInSceneGraph = b_isInGraph;
}

bool EntityBase::GetIsLowResRender() const
{
    return b_isLowResRender;
}

void EntityBase::SetLowResRender(bool b_lowRes)
{
    b_isLowResRender = b_lowRes;
}
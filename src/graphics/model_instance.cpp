#include "graphics/model_instance.h"
#include "core/resource_manager.h"
#include "core/resource_manager_base.h"
#include "graphics/model.h"

namespace Lux
{


ModelInstance::ModelInstance()
	: m_model(NULL)
	, m_matrix(Matrix::IDENTITY)
{
}


ModelInstance::~ModelInstance()
{
	setModel(NULL);
}


void ModelInstance::setModel(Model* model)
{
	if (m_model)
	{
		m_model->getObserverCb().unbind<ModelInstance, &ModelInstance::modelUpdate>(this);
		m_model->getResourceManager().get(ResourceManager::MODEL)->unload(*m_model);
	}
	m_model = model;
	if (m_model)
	{
		m_model->getObserverCb().bind<ModelInstance, &ModelInstance::modelUpdate>(this);
		m_pose.resize(m_model->getBoneCount());
		m_model->getPose(m_pose);
	}
}


void ModelInstance::modelUpdate(Resource::State, Resource::State new_state)
{
	if(new_state == Resource::State::READY)
	{
		m_pose.resize(m_model->getBoneCount());
		m_model->getPose(m_pose);
	}
	else if(new_state == Resource::State::UNLOADING)
	{
		m_pose.resize(0);
	}
}


void ModelInstance::setMatrix(const Matrix& mtx)
{
	m_matrix = mtx;
}


} // ~namespace Lux

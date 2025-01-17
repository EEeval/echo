#include "camera_2d.h"
#include "engine/core/main/Engine.h"
#include "engine/core/scene/node_tree.h"

namespace Echo
{
	Camera2D::Camera2D()
	{

	}

	Camera2D::~Camera2D()
	{

	}

	void Camera2D::bindMethods()
	{
		CLASS_BIND_METHOD(Camera2D, getWidth);
		CLASS_BIND_METHOD(Camera2D, getHeight);
	}

	float Camera2D::getWidth() const
	{
		Camera* camera = NodeTree::instance()->get2dCamera();
		return camera ? camera->getWidth() : 640;
	}

	float Camera2D::getHeight() const
	{
		Camera* camera = NodeTree::instance()->get2dCamera();
		return camera ? camera->getHeight() : 480;
	}

	void Camera2D::syncDataToCamera(Camera* camera)
	{
		camera->setPosition(getWorldPosition());
		camera->setOrientation(getWorldOrientation());

		camera->update();
	}

	void Camera2D::updateInternal(float elapsedTime)
	{
		if (Engine::instance()->getConfig().m_isGame)
		{
			Camera* camera = NodeTree::instance()->get2dCamera();
			if (camera)
				syncDataToCamera(camera);
		}
	}
}
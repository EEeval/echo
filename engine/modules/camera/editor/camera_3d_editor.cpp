#include "camera_3d_editor.h"
#include "engine/core/main/Engine.h"
#include "engine/core/scene/node_tree.h"
#include "engine/core/main/game_settings.h"

namespace Echo
{
#ifdef ECHO_EDITOR_MODE
	Camera3DEditor::Camera3DEditor(Object* object)
		: ObjectEditor(object)
		, m_isSelect(false)
	{
		m_gizmo = ECHO_DOWN_CAST<Echo::Gizmos*>(Echo::Class::create("Gizmos"));
		m_gizmo->setName(StringUtil::Format("gizmo_obj_%d", m_object->getId()));
		m_gizmo->setRenderType("3d");
	}

	Camera3DEditor::~Camera3DEditor()
	{
		EchoSafeDelete(m_gizmo, Gizmos);
	}

	ImagePtr Camera3DEditor::getThumbnail() const
	{
		return Image::loadFromFile(Engine::instance()->getRootPath() + "engine/modules/camera/editor/icon/camera3d.png");
	}

	void Camera3DEditor::editor_update_self()
	{
		if (m_isSelect)
		{
			Camera camera = *NodeTree::instance()->get3dCamera();
			if (!Engine::instance()->getConfig().m_isGame)
			{
				Camera3D* camera3D = ECHO_DOWN_CAST<Camera3D*>(m_object);
				camera3D->syncDataToCamera(&camera);

				camera.update();

				// vertices
				const Vector3* eightVertices = camera.getFrustum().getVertexs();

				m_gizmo->clear();
				m_gizmo->drawLine(eightVertices[0], eightVertices[1], Color::BLUE);
				m_gizmo->drawLine(eightVertices[1], eightVertices[2], Color::BLUE);
				m_gizmo->drawLine(eightVertices[2], eightVertices[3], Color::BLUE);
				m_gizmo->drawLine(eightVertices[3], eightVertices[0], Color::BLUE);
				
				m_gizmo->drawLine(eightVertices[1], eightVertices[5], Color::BLUE);
				m_gizmo->drawLine(eightVertices[2], eightVertices[6], Color::BLUE);
				m_gizmo->drawLine(eightVertices[3], eightVertices[7], Color::BLUE);
				m_gizmo->drawLine(eightVertices[0], eightVertices[4], Color::BLUE);

				m_gizmo->drawLine(eightVertices[4], eightVertices[5], Color::BLUE);
				m_gizmo->drawLine(eightVertices[5], eightVertices[6], Color::BLUE);
				m_gizmo->drawLine(eightVertices[6], eightVertices[7], Color::BLUE);
				m_gizmo->drawLine(eightVertices[7], eightVertices[4], Color::BLUE);

				m_gizmo->update(Engine::instance()->getFrameTime(), true);
			}
		}
	}
#endif
}
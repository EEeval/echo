#include "EchoEngine.h"
#include <QFileInfo>
#include <QString>
#include <array>
#include <engine/core/main/Engine.h>
#include <base/buffer/frame_buffer.h>
#include "engine/core/thread/Threading.h"
#include "Studio.h"
#include "RenderWindow.h"
#include <base/renderer.h>
#include <string>
#include "Studio.h"
#include "base/state/render_state.h"
#include <engine/core/util/PathUtil.h>
#include <engine/core/util/TimeProfiler.h>
#include <engine/core/util/hash_generator.h>
#include <engine/core/io/IO.h>
#include "editor_general_settings.h"

namespace Studio
{
	std::string	EchoEngine::m_projectFile;
	RenderWindow* EchoEngine::m_renderWindow = NULL;

	EchoEngine::EchoEngine()
	{
	}

	EchoEngine::~EchoEngine()
	{
        if(m_invisibleNodeForEditor)
            m_invisibleNodeForEditor->queueFree();
	}

	EchoEngine* EchoEngine::instance()
	{
		static EchoEngine* inst = EchoNew(EchoEngine);
		return inst;
	}

	bool EchoEngine::Initialize(size_t hwnd)
	{
		auto calcEngineRootPath = []()
		{
			Echo::String appPath = Echo::PathUtil::GetCurrentDir();
			Echo::PathUtil::FormatPath(appPath, false);

			// calculate root path
#ifdef ECHO_PLATFORM_WINDOWS
			Echo::String rootPath = appPath + "../../../../";
#elif defined ECHO_PLATFORM_MAC
			Echo::String rootPath = appPath + "../echo/";
#else
			Echo::String rootPath = appPath + "../../../../";
#endif
			Echo::PathUtil::FormatPath(rootPath, false);
			Echo::PathUtil::FormatPathAbsolut(rootPath, false);

			return rootPath;
		};

        // init
        Echo::initRender( hwnd);

		Echo::Engine::Config rootcfg;
		rootcfg.m_projectFile = m_projectFile;
		rootcfg.m_isGame = false;
		rootcfg.m_userPath = Echo::PathUtil::GetCurrentDir() + "/user/" + Echo::StringUtil::Format("u%d/", Echo::BKDRHash(m_projectFile.c_str()));
		rootcfg.m_rootPath = calcEngineRootPath();
		rootcfg.m_engineResPath = rootcfg.m_rootPath + "engine/resources/";
		Echo::PathUtil::FormatPath(rootcfg.m_userPath);
		Echo::Engine::instance()->initialize(rootcfg);

		TIME_PROFILE
		(
			// editor node
			m_invisibleNodeForEditor = ECHO_DOWN_CAST<Echo::Node*>(Echo::Class::create("Node"));

			// gizmos node
			m_gizmosNodeGrid3d = ECHO_DOWN_CAST<Echo::Gizmos*>(Echo::Class::create("Gizmos"));
			m_gizmosNodeGrid3d->setName("Gizmos 3d grid");
			m_gizmosNodeGrid3d->setParent(m_invisibleNodeForEditor);

			// gizmos node 2d
			m_gizmosNodeGrid2d = ECHO_DOWN_CAST<Echo::Gizmos*>(Echo::Class::create("Gizmos"));
			m_gizmosNodeGrid2d->setName("Gizmos 2d grid");
			m_gizmosNodeGrid2d->setParent(m_invisibleNodeForEditor);

			// init back grid
			InitializeBackGrid();
		);

		m_currentEditNode = nullptr;

		return true;
	}

	void EchoEngine::Render(float elapsedTime, bool isRenderWindowVisible)
	{
		// update back grid
		resizeBackGrid2d();
		resizeBackGrid3d();

		if (m_invisibleNodeForEditor)
			m_invisibleNodeForEditor->update(elapsedTime, true);

		Echo::Engine::instance()->tick(elapsedTime);
	}

	void EchoEngine::Resize(int cx, int cy)
	{
		Echo::Engine::instance()->onSize(cx, cy);

		m_renderWindow->getInputController()->onSizeCamera(Echo::Renderer::instance()->getWindowWidth(), Echo::Renderer::instance()->getWindowHeight());
	}

	bool EchoEngine::SetProject(const char* projectFile)
	{
		m_projectFile = projectFile;

		// initialize render window
		m_renderWindow = static_cast<RenderWindow*>(AStudio::instance()->getRenderWindow());

		return true;
	}

	void EchoEngine::setCurrentEditNodeSavePath(const Echo::String& savePath) 
	{ 
		m_currentEditNodeSavePath = savePath;
		if (!m_currentEditNodeSavePath.empty())
		{
			AStudio::instance()->getConfigMgr()->setValue("last_edit_node_tree", m_currentEditNodeSavePath.c_str());
		}
	}

	void EchoEngine::newEditNodeTree()
	{
		setCurrentEditNodeSavePath("");
		if (m_currentEditNode)
		{
			m_currentEditNode->queueFree();
			m_currentEditNode = nullptr;
		}
	}

	bool EchoEngine::onOpenNodeTree(const Echo::String& resPath)
	{
		Echo::Node* node = Echo::Node::loadLink(resPath, false);

		setCurrentEditNode(node);
		setCurrentEditNodeSavePath(resPath);

		// recove input controller parameters
		AStudio::instance()->getRenderWindow()->getInputController2D()->onOpenNodeTree(resPath);
		AStudio::instance()->getRenderWindow()->getInputController3D()->onOpenNodeTree(resPath);

		return node ? true : false;
	}

	void EchoEngine::setCurrentEditNode(Echo::Node* node) 
	{
        // clear
        if(m_currentEditNode)
        {
            m_currentEditNode->queueFree();
            m_currentEditNode = nullptr;
        }
        
        // set new
		m_currentEditNode = node;
        if(m_currentEditNode)
        {
            m_currentEditNode->setParent(m_invisibleNodeForEditor);
        }
	}

	void EchoEngine::saveCurrentEditNodeTree()
	{
		if (m_currentEditNode && !m_currentEditNodeSavePath.empty())
		{
			saveBranchAsScene( m_currentEditNodeSavePath, m_currentEditNode);
		}
	}

	void EchoEngine::saveCurrentEditNodeTreeAs(const Echo::String& savePath)
	{
		saveBranchAsScene(savePath, m_currentEditNode);
	}

	void EchoEngine::saveBranchAsScene(const Echo::String& savePath, Echo::Node* node)
	{
		if (node && !savePath.empty())
		{
			node->save(savePath);
		}
	}

	void EchoEngine::InitializeBackGrid()
	{
		resizeBackGrid2d();
		resizeBackGrid3d();
	}

	void EchoEngine::resizeBackGrid3d()
	{	
		static int xOffsetBefore = 0.f;
		static int zOffsetBefore = 0.f;

		// center pos
		Echo::Vector3 centerPos = Echo::NodeTree::instance()->get3dCamera()->getPosition();
		int xOffset = centerPos.x;
		int yOffset = abs(centerPos.y);
		int zOffset = centerPos.z;
			
		m_gizmosNodeGrid3d->setVisible(Echo::EditorGeneralSettings::instance()->isShow3dGrid());

		if (xOffset != xOffsetBefore || zOffset != zOffsetBefore)
		{
			// calc y alpha scale
			float startGrayFadeDistance = 10.f;
			float endGrayFadeDistance = 35.f;
			float yGrayAlphaScale = 1.f - Echo::Math::Clamp((float)abs(yOffset), startGrayFadeDistance, endGrayFadeDistance) / (endGrayFadeDistance - startGrayFadeDistance);

			float startBlueFadeDistance = 10.f;
			float endBlueFadeDistance = 200.f;
			float yBlueAlphaScale = 1.f - Echo::Math::Clamp((float)abs(yOffset), startBlueFadeDistance, endBlueFadeDistance) / (endBlueFadeDistance - startBlueFadeDistance);

			m_gizmosNodeGrid3d->clear();
			m_gizmosNodeGrid3d->setRenderType("3d");

			// gray line
			if (yOffset < endGrayFadeDistance)
			{
				int lineNum = (80 + abs(yOffset)) / 10 * 10;
				for (int i = -lineNum; i <= lineNum; i++)
				{
					Echo::Color color = Echo::Color(0.5f, 0.5f, 0.5f, 0.4f * yGrayAlphaScale);

					// xaxis
					int xAxis = xOffset + i;
					if (xAxis % 10 != 0)
						m_gizmosNodeGrid3d->drawLine(Echo::Vector3(xAxis, 0.f, -lineNum + zOffset), Echo::Vector3(xAxis, 0.f, lineNum + zOffset), color);

					int zAxis = zOffset + i;
					if (zAxis % 10 != 0)
						m_gizmosNodeGrid3d->drawLine(Echo::Vector3(-lineNum + xOffset, 0.f, zAxis), Echo::Vector3(lineNum + xOffset, 0.f, zAxis), color);
				}
			}

			// blue line
			int xOffset10 = xOffset / 10;
			int zOffset10 = zOffset / 10;
			int lineNum = (80 + abs(yOffset * 5)) / 10;
			for (int i = -lineNum; i <= lineNum; i++)
			{
				// xaxis
				int xAxis = xOffset10 + i;
				Echo::Color color = Echo::Color(0.8f, 0.5, 0.5f, 0.5f * yBlueAlphaScale);
				m_gizmosNodeGrid3d->drawLine(Echo::Vector3(xAxis * 10.f, 0.f, (-lineNum + zOffset10)*10.f), Echo::Vector3(xAxis * 10.f, 0.f, (lineNum + zOffset10)*10.f), color);

				int zAxis = zOffset10 + i;
				m_gizmosNodeGrid3d->drawLine(Echo::Vector3((-lineNum + xOffset10)*10.f, 0.f, zAxis * 10.f), Echo::Vector3((lineNum + xOffset10)*10.f, 0.f, zAxis*10.f), color);
			}

			xOffsetBefore = xOffset;
			zOffsetBefore = zOffset;
		}
	}

	void EchoEngine::resizeBackGrid2d()
	{
		static Echo::i32 curWindowHalfWidth = -1;
		static Echo::i32 curWindowHalfHeight = -1;
		Echo::i32 windowHalfWidth = Echo::GameSettings::instance()->getDesignWidth() / 2;
		Echo::i32 windowHalfHeight = Echo::GameSettings::instance()->getDesignHeight() / 2;
		if (curWindowHalfWidth != windowHalfWidth || curWindowHalfHeight != windowHalfHeight)
		{
			m_gizmosNodeGrid2d->clear();
			m_gizmosNodeGrid2d->setRenderType("2d");
			m_gizmosNodeGrid2d->drawLine(Echo::Vector3(-windowHalfWidth, -windowHalfHeight, 0.0), Echo::Vector3(windowHalfWidth, -windowHalfHeight, 0.0), Echo::Color::RED);
			m_gizmosNodeGrid2d->drawLine(Echo::Vector3(-windowHalfWidth, -windowHalfHeight, 0.0), Echo::Vector3(-windowHalfWidth, windowHalfHeight, 0.0), Echo::Color::RED);
			m_gizmosNodeGrid2d->drawLine(Echo::Vector3(windowHalfWidth, windowHalfHeight, 0.0), Echo::Vector3(windowHalfWidth, -windowHalfHeight, 0.0), Echo::Color::RED);
			m_gizmosNodeGrid2d->drawLine(Echo::Vector3(windowHalfWidth, windowHalfHeight, 0.0), Echo::Vector3(-windowHalfWidth, windowHalfHeight, 0.0), Echo::Color::RED);

			curWindowHalfWidth = windowHalfWidth;
			curWindowHalfHeight = windowHalfHeight;
		}
	}

	void EchoEngine::SaveSceneThumbnail(bool setCam)
	{
		//Echo::RenderTarget* defaultBackBuffer = Echo::RenderTargetManager::Instance()->getRenderTargetByID(Echo::RTI_DefaultBackBuffer);
		//if (defaultBackBuffer)
		//{
		//	Echo::String sceneFullPath = EchoResourceManager->getFileLocation(EchoSceneManager->getCurrentScene()->getSceneName() + ".scene");
		//	Echo::String sceneLocation = Echo::PathUtil::GetFileDirPath(sceneFullPath);
		//	Echo::String bmpSavePath = Echo::PathUtil::GetRenameExtFile(sceneFullPath, ".bmp");
		//	if (setCam)
		//		defaultBackBuffer->saveTo((std::string(bmpSavePath.c_str())).c_str());
		//	else
		//		defaultBackBuffer->saveTo((std::string(sceneLocation.c_str()) + "/map.bmp").c_str());
		//}
	}	
}

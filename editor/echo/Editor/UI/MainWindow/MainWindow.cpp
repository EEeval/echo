#include "MainWindow.h"
#include "Studio.h"
#include "UndoHistory.h"
#include "LogPanel.h"
#include "ResPanel.h"
#include "NodeTreePanel.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopservices>
#include <QShortcut>
#include <QMdiArea>
#include <QComboBox>
#include <QStatusBar>
#include "DebuggerPanel.h"
#include "EchoEngine.h"
#include "QResSelect.h"
#include "ResChooseDialog.h"
#include "TextEditor.h"
#include "TextEditorArea.h"
#include "ShaderEditor.h"
#include "ProjectWnd.h"
#include "PathChooseDialog.h"
#include "RenderWindow.h"
#include "MacHelper.h"
#include "BuildWindow.h"
#include "TerminalDialog.h"
#include "Document.h"
#include "About.h"
#include <QTimer>
#include <engine/core/util/PathUtil.h>
#include <engine/core/io/IO.h>
#include <engine/core/main/module.h>
#include <engine/core/scene/render_node.h>
#include <engine/core/util/Timer.h>
#include "editor_render_settings.h"

namespace Studio
{
	static MainWindow* g_inst = nullptr;

	MainWindow::MainWindow(QMainWindow* parent/*=0*/)
		: QMainWindow(parent)
	{
		setupUi(this);

#ifdef ECHO_PLATFORM_WINDOWS
		// hide window hwnd
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
#elif defined(ECHO_PLATFORM_MAC)
		// set title bar color
		//macChangeTitleBarColor(winId(), 0.f, 0.f, 0.f);
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
		menubar->setNativeMenuBar(false);
#endif

		// set menu icon
		menubar->setTopLeftCornerIcon(":/icon/Icon/icon.png");

		// project operate
		QObject::connect(m_actionNewProject, SIGNAL(triggered(bool)), this, SLOT(onNewAnotherProject()));
		QObject::connect(m_actionOpenProject, SIGNAL(triggered(bool)), this, SLOT(onOpenAnotherProject()));
		QObject::connect(m_actionSaveProject, SIGNAL(triggered(bool)), this, SLOT(onSaveProject()));
		QObject::connect(m_actionSaveAsProject, SIGNAL(triggered(bool)), this, SLOT(onSaveasProject()));
        QObject::connect(m_actionBuild, SIGNAL(triggered(bool)), this, SLOT(onBuildProject()));

		// connect scene operate signal slot
		QObject::connect(m_actionNewScene, SIGNAL(triggered(bool)), this, SLOT(onNewScene()));
		QObject::connect(m_actionSaveScene, SIGNAL(triggered(bool)), this, SLOT(onSaveScene()));
		QObject::connect(m_actionSaveAsScene, SIGNAL(triggered(bool)), this, SLOT(onSaveAsScene()));
        
        // redo|undo
        QObject::connect(m_actionUndo, SIGNAL(triggered(bool)), this, SLOT(onUndo()));
        QObject::connect(m_actionRedo, SIGNAL(triggered(bool)), this, SLOT(onRedo()));

		// window
		QObject::connect(m_actionTerminal, SIGNAL(triggered(bool)), this, SLOT(onOpenWindow()));

		// editor
		QObject::connect(m_actionThemeDark, SIGNAL(triggered(bool)), this, SLOT(onChooseThemeDark()));
		QObject::connect(m_actionThemeChoose, SIGNAL(triggered(bool)), this, SLOT(onChooseTheme()));

		// connect signal slot
		QObject::connect(m_actionPlayGame, SIGNAL(triggered(bool)), this, SLOT(onPlayGame()));
		QObject::connect(m_actionStopGame, SIGNAL(triggered(bool)), &m_gameProcess, SLOT(terminate()));
		QObject::connect(m_actionExitEditor, SIGNAL(triggered(bool)), this, SLOT(close()));
		QObject::connect(m_actionApi, SIGNAL(triggered(bool)), this, SLOT(onOpenHelpDialog()));
		QObject::connect(m_actionDocumentation, SIGNAL(triggered(bool)), this, SLOT(onOpenWiki()));
		QObject::connect(m_actionAbout, SIGNAL(triggered(bool)), this, SLOT(onAbout()));

		// add combox, switch 2D,3D,Script etc.
		m_subEditComboBox = new QComboBox(m_toolBar);
		m_subEditComboBox->addItem("2D");
		m_subEditComboBox->addItem("3D");
		m_toolBar->addWidget(m_subEditComboBox);
		QObject::connect(m_subEditComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onSubEditChanged(const QString&)));

		m_toolBar->addAction(m_actionPlayGame);
		m_toolBar->addAction(m_actionStopGame);

		// Camera settings
		//m_toolBar->addAction(m_actionEditorCameraSettings);
		//QObject::connect(m_actionEditorCameraSettings, SIGNAL(triggered(bool)), this, SLOT(onClickEditorCameraSettings()));

		// show status message
		QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(onShowStatusMessage()));
		m_timer.start(100);

		EchoAssert(!g_inst);
		g_inst = this;
	}

	MainWindow::~MainWindow()
	{
		m_gameProcess.terminate();
		m_gameProcess.waitForFinished();

		EchoSafeDelete(m_scriptEditorMdiArea, TextEditorArea);
		EchoSafeDelete(m_shaderEditorPanel, ShaderEditor);
		EchoSafeDelete(m_documentPanel, DocumentPanel);
		EchoSafeDelete(m_debuggerPanel, DebuggerPanel);
        EchoSafeDelete(m_scenePanel, NodeTreePanel);
        EchoSafeDelete(m_resPanel, ResPanel);
        EchoSafeDelete(m_renderPanel, QDockWidget);
	}

	MainWindow* MainWindow::instance()
	{
		return g_inst;
	}

	void MainWindow::onOpenProject()
	{
		//setCentralWidget(nullptr);
		centralWidget()->setMaximumHeight(0);

		m_renderPanel = EchoNew(QDockWidget(this));
		m_resPanel = EchoNew(ResPanel(this));
		m_scenePanel = EchoNew(NodeTreePanel(this));
		m_scriptEditorMdiArea = EchoNew(TextEditorArea);
		m_shaderEditorPanel = EchoNew(ShaderEditor(this));
		m_logPanel = AStudio::instance()->getLogPanel();
		m_documentPanel = EchoNew(DocumentPanel(this));
		m_debuggerPanel = EchoNew(DebuggerPanel(this));
		m_scriptEditorMdiArea->setVisible(false);
		m_shaderEditorPanel->setVisible(false);

		// add renderWindow to RenderDockWidget
		QWidget* renderWindow = AStudio::instance()->getRenderWindow();
		m_renderPanel->setWidget(renderWindow);
		m_renderPanel->setWindowTitle("Render");

		this->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
		this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
		this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
		this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

		this->addDockWidget(Qt::TopDockWidgetArea, m_scriptEditorMdiArea);
		this->addDockWidget(Qt::TopDockWidgetArea, m_shaderEditorPanel);
		this->addDockWidget(Qt::TopDockWidgetArea, m_renderPanel);
		this->addDockWidget(Qt::LeftDockWidgetArea, m_resPanel);
		this->addDockWidget(Qt::RightDockWidgetArea, m_scenePanel);
		this->addDockWidget(Qt::BottomDockWidgetArea, m_logPanel);
		this->addDockWidget(Qt::BottomDockWidgetArea, m_documentPanel);
		this->addDockWidget(Qt::BottomDockWidgetArea, m_debuggerPanel);

		this->tabifyDockWidget(m_scriptEditorMdiArea, m_shaderEditorPanel);
		this->tabifyDockWidget(m_logPanel, m_documentPanel);
		this->tabifyDockWidget(m_documentPanel, m_debuggerPanel);

		m_resPanel->onOpenProject();

		// menu [Project(P)]
		updateRencentProjectsDisplay();
		updateSettingDisplay();

		// settings of echo
		QTimer::singleShot(100, this, SLOT(recoverEditSettings()));

		// signals & slots
		QObject::connect(m_actionSaveProject, SIGNAL(triggered(bool)), this, SLOT(onSave()));
		QObject::connect(m_scriptEditorMdiArea, SIGNAL(visibilityChanged(bool)), this, SLOT(onScriptEditVisibilityChanged()));
		QObject::connect(m_shaderEditorPanel, SIGNAL(visibilityChanged(bool)), this, SLOT(onCenterDockWidgetVisibilityChanged()));
		QObject::connect(m_renderPanel, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(onDockWidgetLocationChanged()));
	}

	void MainWindow::addCenterPanel(QDockWidget* panel, float widthRatio)
	{
		if (std::find(m_centerPanels.begin(), m_centerPanels.end(), panel) == m_centerPanels.end())
		{
			QDockWidget* tabifyPanel = m_centerPanels.size() > 0 ? m_centerPanels.back() : m_scriptEditorMdiArea;

			this->addDockWidget(Qt::TopDockWidgetArea, panel);
			this->tabifyDockWidget(tabifyPanel, panel);

			// https://www.qtcentre.org/threads/47927-How-to-set-focus-(select)-a-tabbed-QDockWidget
			Echo::Time::instance()->addDelayTask(100, [panel]()
			{
				panel->raise();
			});

			QObject::connect(panel, SIGNAL(visibilityChanged(bool)), this, SLOT(onExternalCenterDockWidgetVisibilityChanged()));
			resizeDocks({ panel, m_renderPanel }, { int(widthRatio * 100.f) , int((1.f-widthRatio) * 100.f) }, Qt::Horizontal);

			m_centerPanels.push_back(panel);
		}
		else
		{
			panel->setVisible(true);
		}
	}

	void MainWindow::removeCenterPanel(QDockWidget* panel)
	{
		if (panel)
		{
			//for (auto it = m_centerPanels.begin(); it != m_centerPanels.end();)
			//{
			//	if (*it == panel)
			//	{
			//		this->removeDockWidget(panel);
			//		//it = m_centerPanels.erase(it);
			//	}
			//	else
			//	{
			//		it++;
			//	}
			//}
		}
	}

	void MainWindow::addBottomPanel(Echo::EditorDockPanel* panel)
	{
		if (std::find(m_bottomPanels.begin(), m_bottomPanels.end(), panel) == m_bottomPanels.end())
		{
			QDockWidget* tabifyPanel = m_bottomPanels.size() > 0 ? m_bottomPanels.back()->getUiPtr() : m_logPanel;

			this->addDockWidget(Qt::BottomDockWidgetArea, panel->getUiPtr());
			this->tabifyDockWidget(tabifyPanel, panel->getUiPtr());

			// https://www.qtcentre.org/threads/47927-How-to-set-focus-(select)-a-tabbed-QDockWidget
			Echo::Time::instance()->addDelayTask(100, [panel]()
			{
				panel->getUiPtr()->raise();
			});

			m_bottomPanels.push_back(panel);
		}
		else
		{
			panel->getUiPtr()->setVisible(true);
		}
	}

	void MainWindow::removeBottomPanel(Echo::EditorDockPanel* panel)
	{

	}
    
    void MainWindow::onPrepareQuit()
    {
        EchoSafeDelete(m_scriptEditorMdiArea, TextEditorArea);
		EchoSafeDelete(m_shaderEditorPanel, ShaderEditor);
    }

	void MainWindow::recoverEditSettings()
	{
		// open last edit node tree
		Echo::String lastNodeTreePath = AStudio::instance()->getConfigMgr()->getValue("last_edit_node_tree");
		if (Echo::IO::instance()->isExist(lastNodeTreePath))
		{
			openNodeTree(lastNodeTreePath);
		}

		// recover last edit type 
		Echo::String subEditType = AStudio::instance()->getConfigMgr()->getValue("main_window_sub_edit_type");
		if (!subEditType.empty())
		{
			m_subEditComboBox->setCurrentText(subEditType.c_str());
		}

		// recover last edit script
		Echo::String isScriptEditVisible = AStudio::instance()->getConfigMgr()->getValue("luascripteditor_visible");
		if (isScriptEditVisible == "true")
		{
            m_scriptEditorMdiArea->recoverScriptOpenStates();
		}

		// save config
		m_resPanel->recoverEditSettings();
	}

    void MainWindow::updateRenderWindowTitle()
    {
        Echo::String scenePath = EchoEngine::instance()->getCurrentEditNodeSavePath();
        Echo::String title     = !scenePath.empty() ? scenePath.c_str() : "Render";
        m_renderPanel->setWindowTitle(title.c_str());
    }

	void MainWindow::onNewScene()
	{
		onSaveProject();
		m_scenePanel->clear();
		EchoEngine::instance()->newEditNodeTree();
        
        updateRenderWindowTitle();
	}

	void MainWindow::onSaveScene()
	{
		onSaveProject();
	}

	void MainWindow::onSaveAsScene()
	{
		Echo::String savePath = PathChooseDialog::getExistingPathName(this, ".scene", "Save").toStdString().c_str();
		if (!savePath.empty() && !Echo::PathUtil::IsDir(savePath))
		{
			Echo::String resPath;
			if (Echo::IO::instance()->convertFullPathToResPath(savePath, resPath))
				EchoEngine::instance()->saveCurrentEditNodeTreeAs(resPath.c_str());

			// refresh respanel display
			m_resPanel->reslectCurrentDir();
		}
	}

	void MainWindow::onNewAnotherProject()
	{
		Echo::String newProjectPathName = AStudio::instance()->getProjectWindow()->newProject();
		if (!newProjectPathName.empty())
		{
			openAnotherProject(newProjectPathName);
		}
	}

	void MainWindow::onOpenAnotherProject()
	{
		QString projectName = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("(*.echo)"));
		if (!projectName.isEmpty())
		{
			openAnotherProject(projectName.toStdString().c_str());
		}
	}

	void MainWindow::openAnotherProject(const Echo::String& fullPathName)
	{
		Echo::String app = QCoreApplication::applicationFilePath().toStdString().c_str();
		Echo::String cmd = Echo::StringUtil::Format("%s %s", app.c_str(), fullPathName.c_str());

		QProcess process;
		process.startDetached(cmd.c_str());

		// exit
		close();
	}

	void MainWindow::onSaveasProject()
	{
		Echo::String projectName = QFileDialog::getSaveFileName(this, tr("Save as Project"), "", tr("(*.echo)")).toStdString().c_str();
		if (!projectName.empty())
		{
			// 0.confirm path and file name
			Echo::String fullPath = projectName;
			Echo::String filePath = Echo::PathUtil::GetFileDirPath(fullPath);
			Echo::String fileName = Echo::PathUtil::GetPureFilename(fullPath, false);
			Echo::String saveasPath = filePath + fileName + "/";

			if (!Echo::PathUtil::IsDirExist(saveasPath))
			{
				Echo::PathUtil::CreateDir(saveasPath);

				Echo::String currentProject = Echo::IO::instance()->convertResPathToFullPath(Echo::GameSettings::instance()->getPath());
				Echo::String currentPath = Echo::PathUtil::GetFileDirPath(currentProject);
				Echo::String currentName = Echo::PathUtil::GetPureFilename(currentProject, true);

				// copy resource
				Echo::PathUtil::CopyDir(currentPath, saveasPath);

				// rename
				Echo::String currentPathname = saveasPath + currentName;
				Echo::String destPathName = saveasPath + fileName + ".echo";
				Echo::PathUtil::RenameFile(currentPathname, destPathName);

				// open dest folder
				QString qSaveasPath = saveasPath.c_str();
				QDesktopServices::openUrl(qSaveasPath);
			}
			else
			{
				EchoLogError("Save as directory [%s] isn't null", saveasPath.c_str());
			}
		}
	}

	void MainWindow::onOpenRencentProject()
	{
		QAction* action = qobject_cast<QAction*>(sender());
		if (action)
		{
			Echo::String text = action->text().toStdString().c_str();
			if (Echo::PathUtil::IsFileExist(text))
				openAnotherProject(text);
			else
				EchoLogError("Project file [%s] not exist.", text.c_str());
		}
	}

	void MainWindow::onEditSingletonSettings()
	{
		QAction* action = qobject_cast<QAction*>(sender());
		if (action)
		{
			Echo::String text = action->text().toStdString().c_str();
			Echo::Object* obj = Echo::Class::create(text);
			if (obj)
				Studio::NodeTreePanel::instance()->setNextEditObject(obj);
		}
	}
    
    void MainWindow::onBuildProject()
    {
        BuildWindow* buildWindow = EchoNew(BuildWindow(nullptr));
        buildWindow->exec();
    }

	void MainWindow::setSubEdit(const char* subEditName)
	{
		m_subEditComboBox->setCurrentText(subEditName);
	}

	void MainWindow::onSubEditChanged(const QString& subeditName)
	{
		Echo::String renderType = subeditName.toStdString().c_str();
		if (renderType == "2D")
		{
			Echo::Render::setRenderTypes(Echo::Render::Type_2D | Echo::Render::Type_Ui);

			RenderWindow* renderWindow = ECHO_DOWN_CAST<RenderWindow*>(AStudio::instance()->getRenderWindow());
			if (renderWindow)
				renderWindow->switchToController2d();

			AStudio::instance()->getConfigMgr()->setValue("main_window_sub_edit_type", subeditName.toStdString().c_str());
		}
		else if (renderType == "3D")
		{
			Echo::Render::setRenderTypes(Echo::Render::Type_3D);

			RenderWindow* renderWindow = ECHO_DOWN_CAST<RenderWindow*>(AStudio::instance()->getRenderWindow());
			if (renderWindow)
				renderWindow->switchToController3d();

			AStudio::instance()->getConfigMgr()->setValue("main_window_sub_edit_type", subeditName.toStdString().c_str());
		}
	}

	void MainWindow::onSaveProject()
	{
		// if path isn't exist. choose a save directory
		if (EchoEngine::instance()->getCurrentEditNode() && EchoEngine::instance()->getCurrentEditNodeSavePath().empty())
		{
			Echo::String savePath = PathChooseDialog::getExistingPathName(this, ".scene", "Save").toStdString().c_str();
			if (!savePath.empty() && !Echo::PathUtil::IsDir(savePath))
			{
				Echo::String resPath;
				if (Echo::IO::instance()->convertFullPathToResPath(savePath, resPath))
					EchoEngine::instance()->setCurrentEditNodeSavePath(resPath.c_str());
			}
		}

		// save settings
		Echo::Engine::instance()->saveSettings();

		// save current edit node
		EchoEngine::instance()->saveCurrentEditNodeTree();

		// save current edit res
		m_scenePanel->saveCurrentEditRes();

		// refresh respanel display
		m_resPanel->reslectCurrentDir();
        
        // update title
        updateRenderWindowTitle();
	}

	void MainWindow::setTheme(const char* theme)
	{
		QFile qssFile(theme);
		qssFile.open(QFile::ReadOnly);
		if (qssFile.isOpen())
		{
			QString qss = QLatin1String(qssFile.readAll());
			qApp->setStyleSheet(qss);

			qssFile.close();
		}
	}

	void MainWindow::onChooseTheme()
	{
		QString qssFile = QFileDialog::getOpenFileName(this, tr("Choose Theme"), "", tr("(*.qss)"));
		if (!qssFile.isEmpty())
		{
			setTheme(qssFile.toStdString().c_str());
			Studio::EditorConfig::instance()->setValue("CurrentTheme", qssFile.toStdString().c_str());
		}
	}

	void MainWindow::onChooseThemeDark()
	{
		setTheme(":/Qss/Qss/Ps.qss");
		Studio::EditorConfig::instance()->setValue("CurrentTheme", ":/Qss/Qss/Ps.qss");
	}

	void MainWindow::onPlayGame()
	{
		// if launch scene not exist, set it
		Echo::GameSettings* projSettings = Echo::GameSettings::instance();
		if (projSettings)
		{
			const Echo::String& launchScene = projSettings->getLaunchScene().getPath();
			if (launchScene.empty())
			{
				if (QMessageBox::Yes == QMessageBox(QMessageBox::Information, "Warning", "Launch Scene is empty, Would you set it now?", QMessageBox::Yes | QMessageBox::No).exec())
				{
					Echo::String  scene = ResChooseDialog::getSelectingFile(this, ".scene");
					if (!scene.empty())
					{
						projSettings->setLaunchScene(Echo::ResourcePath(scene));
					}
					else
					{
						return;
					}
				}
				else
				{
					return;
				}
			}
		}

		// save project
		onSaveProject();

		// start game
		if (!projSettings->getLaunchScene().getPath().empty())
		{
			Echo::String app = QCoreApplication::applicationFilePath().toStdString().c_str();
			Echo::String project = Echo::Engine::instance()->getConfig().m_projectFile;
			Echo::String cmd = Echo::StringUtil::Format("play %s", project.c_str());

			m_gameProcess.terminate();
			m_gameProcess.waitForFinished();

			m_gameProcess.start((app + " " + cmd).c_str());

			QObject::connect(&m_gameProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onGameProcessFinished(int, QProcess::ExitStatus)));
			QObject::connect(&m_gameProcess, SIGNAL(readyRead()), this, SLOT(onReadMsgFromGame()));

			EchoLogWarning("%s", cmd.c_str());
		}
	}

	void MainWindow::OpenProject(const char* projectName)
	{
		AStudio::instance()->OpenProject(projectName);

		// init render window
		AStudio::instance()->getRenderWindow();
	}

	void MainWindow::openNodeTree(const Echo::String& resPath)
	{
		// clear
		onNewScene();

		Studio::EchoEngine::instance()->onOpenNodeTree(resPath);

		NodeTreePanel::instance()->refreshNodeTreeDisplay();
        
        updateRenderWindowTitle();
	}

	void MainWindow::openTextEditor(const Echo::String& fileName)
	{
		m_scriptEditorMdiArea->open(fileName);
	}

    void MainWindow::openShaderEditor(const Echo::String& resPath)
    {
        if(m_shaderEditorPanel)
        {
            m_shaderEditorPanel->open(resPath);
        }
    }

	void MainWindow::onCenterDockWidgetVisibilityChanged()
	{
		QDockWidget* panel = qobject_cast<QDockWidget*>(sender());
		if (panel)
		{
			if (panel->isVisible())
				resizeDocks({ panel, m_renderPanel }, { 70 , 30 }, Qt::Horizontal);
		}
	}

	void MainWindow::onExternalCenterDockWidgetVisibilityChanged()
	{
		QDockWidget* panel = qobject_cast<QDockWidget*>(sender());
		removeCenterPanel(panel);
	}

	void MainWindow::onScriptEditVisibilityChanged()
	{
		if (m_scriptEditorMdiArea->isVisible())
			resizeDocks({ m_scriptEditorMdiArea, m_renderPanel }, { 70 , 30 }, Qt::Horizontal);

		AStudio::instance()->getConfigMgr()->setValue("luascripteditor_visible", m_scriptEditorMdiArea->isVisible() ? "true" : "false");
	}

	void MainWindow::onDockWidgetLocationChanged()
	{
		//centralWidget()->setMaximumHeight(200);
		//centralWidget()->setMinimumHeight(200);
	}

	void MainWindow::onClickEditorCameraSettings()
	{
		EchoSafeDelete(m_cameraSettingsMenu, QMenu);
		m_cameraSettingsMenu = EchoNew(QMenu);

		m_cameraSettingsMenu->addAction(m_actionEditorCameraSettings);

		m_cameraSettingsMenu->exec(QCursor::pos());
	}

	void MainWindow::closeEvent(QCloseEvent *event)
	{
		AStudio::instance()->getLogPanel()->close();

		// disconnect connections
		m_scriptEditorMdiArea->disconnect();
	}

	void MainWindow::onGameProcessFinished(int id, QProcess::ExitStatus status)
	{
		EchoLogWarning("stop game debug");
	}

	void MainWindow::onOpenHelpDialog()
	{
		m_logPanel->raise();
	}

	void MainWindow::onOpenWiki()
	{
		QString link = "https://github.com/blab-liuliang/echo/wiki";
		QDesktopServices::openUrl(QUrl(link));
	}

	void MainWindow::onAbout()
	{
		if (!m_aboutWindow)
		{
			m_aboutWindow = new AboutWindow(this);
			m_aboutWindow->setWindowModality(Qt::ApplicationModal);
		}

		m_aboutWindow->setVisible(true);
	}

	void MainWindow::onReadMsgFromGame()
	{
		Echo::String msg = m_gameProcess.readAllStandardOutput().toStdString().c_str();
		if (!msg.empty())
		{
			Echo::StringArray msgArray = Echo::StringUtil::Split(msg, "@@");
			
			int i = 0;
			int argc = static_cast<int>(msgArray.size());
			while (i < argc)
			{
				Echo::String command = msgArray[i++];
				if (command == "-log")
				{
					int    logLevel     = Echo::StringUtil::ParseInt(msgArray[i++]);
					Echo::String logMsg = msgArray[i++];

					Echo::Log::instance()->logMessage(Echo::LogOutput::Level(logLevel), logMsg.c_str());
				}
			}
		}
	}

	void MainWindow::updateRencentProjectsDisplay()
	{
		// clear
		m_menuRecents->clear();

		EditorConfig* configMgr = AStudio::instance()->getConfigMgr();

		Echo::list<Echo::String>::type recentProjects;
		configMgr->getAllRecentProject(recentProjects);

		Echo::list<Echo::String>::iterator iter = recentProjects.begin();
		for (; iter != recentProjects.end(); ++iter)
		{
			Echo::String projectFullPath = (*iter).c_str();
			if (!projectFullPath.empty() && Echo::PathUtil::IsFileExist(projectFullPath))
			{
				Echo::String icon = Echo::PathUtil::GetFileDirPath(projectFullPath) + "icon.png";
				if (Echo::PathUtil::IsFileExist(icon))
				{
					QAction* action = new QAction(this);
					action->setText(projectFullPath.c_str());
					action->setIcon(QIcon(icon.c_str()));

					m_menuRecents->addAction(action);

					QObject::connect(action, SIGNAL(triggered()), this, SLOT(onOpenRencentProject()));
				}
			}
		}
	}

    QMenu* getParentMenu(Echo::map<Echo::String, QMenu*>::type& allMenus, const Echo::String& className)
    {
        Echo::String parentClass;
        if(Echo::Class::getParentClass(parentClass, className))
        {
            auto it = allMenus.find(parentClass);
            if(it!=allMenus.end())
            {
                return it->second;
            }
            else
            {
                QMenu* parentMenu = getParentMenu(allMenus, parentClass);
                QMenu* curMenu = parentMenu->addMenu(parentClass.c_str());
                
                allMenus[parentClass] = curMenu;
                
                return curMenu;
            }
        }
        
        return allMenus["Object"];
    }

	void MainWindow::updateSettingDisplay()
	{
        Echo::map<Echo::String, QMenu*>::type allMenus;
        allMenus["Object"] = m_menuSettings;
        
		m_menuSettings->clear();

		Echo::StringArray classes;
		Echo::Class::getAllClasses(classes);
		for (Echo::String& className : classes)
		{
			if (Echo::Class::isSingleton(className))
			{
                Echo::Object* obj = Echo::Class::create(className);
                
                Echo::PropertyInfos propertys;
                if(Echo::Class::getPropertys(className, obj, propertys, Echo::PropertyInfo::All, true) > 0)
                {
                    QAction* action = new QAction(this);
                    action->setText(className.c_str());
					
					// check state
					Echo::Module* module = dynamic_cast<Echo::Module*>(obj);
					if (module && !module->isEditorOnly())
					{
						action->setCheckable(true);
						action->setChecked(module->isEnable());
					}
                    
                    QMenu* menu = getParentMenu(allMenus, className);
                    menu->addAction(action);
                    
                    QObject::connect(action, SIGNAL(triggered()), this, SLOT(onEditSingletonSettings()));
                }
			}
		}
	}

	void MainWindow::onOpenWindow()
	{
		QAction* action = qobject_cast<QAction*>(sender());
		if (action)
		{
			Echo::String windowName = action->text().toStdString().c_str();
			if (Echo::StringUtil::Equal(windowName, "Terminal"))
			{
				TerminalDialog* inst = TerminalDialog::instance();
				inst->setVisible(true);
				if (inst->exec() == QDialog::Accepted)
				{
				}
			}
		}
	}

    void MainWindow::onUndo()
    {
        UndoHistory::instance()->undo();
    }

    void MainWindow::onRedo()
    {
        UndoHistory::instance()->redo();
    }

	void MainWindow::onSave()
	{
		m_scriptEditorMdiArea->save();
		m_shaderEditorPanel->save();

		for (Echo::EditorDockPanel* panel : m_bottomPanels)
			panel->save();
	}

	void MainWindow::onShowStatusMessage()
	{
		Echo::i32 fps = Echo::FrameState::instance()->getFps();
		Echo::i32 drawcall = Echo::FrameState::instance()->getDrawCalls();
		statusBar()->showMessage(Echo::StringUtil::Format("Fps:%d Drawcall:%d", fps, drawcall).c_str());

		hideWhiteLineOfQTabBar();
	}

	void MainWindow::hideWhiteLineOfQTabBar()
	{
		// https://stackoverflow.com/questions/42746408/how-to-get-rid-of-strange-white-line-under-qtabbar-while-customzing-tabified-qdo
		QList<QTabBar*> tabBars = findChildren<QTabBar*>("", Qt::FindDirectChildrenOnly);
		for (QTabBar* tabBar : tabBars)
		{
			tabBar->setDrawBase(false);
		}
	}
}

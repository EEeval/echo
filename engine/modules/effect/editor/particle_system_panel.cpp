#include "particle_system_panel.h"
#include "engine/core/editor/editor.h"
#include "engine/core/editor/qt/QWidgets.h"
#include "engine/core/base/class_method_bind.h"
#include "engine/core/util/PathUtil.h"
#include "engine/core/util/StringUtil.h"
#include "engine/core/io/memory_reader.h"
#include "engine/core/util/Buffer.h"
#include "engine/core/io/IO.h"
#include "engine/core/main/Engine.h"
#include "engine/core/render/base/image/image.h"
#include "engine/core/render/base/texture/texture_atlas.h"

namespace Echo
{
#ifdef ECHO_EDITOR_MODE
	ParticleSystemPanel::ParticleSystemPanel(Object* obj)
	{
		m_particleSystem = ECHO_DOWN_CAST<ParticleSystem*>(obj);

		m_ui = (QDockWidget*)EditorApi.qLoadUi("engine/modules/effect/editor/particle_system_panel.ui");

		QSplitter* splitter = m_ui->findChild<QSplitter*>("m_splitter");
		if (splitter)
		{
			splitter->setStretchFactor(0, 0);
			splitter->setStretchFactor(1, 1);
		}

		// Tool button icons
		m_ui->findChild<QToolButton*>("m_import")->setIcon(QIcon((Engine::instance()->getRootPath() + "engine/core/render/base/editor/icon/import.png").c_str()));

		// connect signal slots
		EditorApi.qConnectWidget(m_ui->findChild<QWidget*>("m_import"), QSIGNAL(clicked()), this, createMethodBind(&ParticleSystemPanel::onImport));
		EditorApi.qConnectWidget(m_ui->findChild<QWidget*>("m_nodeTreeWidget"), QSIGNAL(itemClicked(QTreeWidgetItem*, int)), this, createMethodBind(&ParticleSystemPanel::onSelectItem));
		EditorApi.qConnectWidget(m_ui->findChild<QWidget*>("m_nodeTreeWidget"), QSIGNAL(itemChanged(QTreeWidgetItem*, int)), this, createMethodBind(&ParticleSystemPanel::onChangedAtlaName));

		// create QGraphicsScene
		m_graphicsView = m_ui->findChild<QGraphicsView*>("m_graphicsView");
		m_graphicsScene = EditorApi.qGraphicsSceneNew();
		m_graphicsView->setScene(m_graphicsScene);

		refreshUiDisplay();
	}

	ParticleSystemPanel::~ParticleSystemPanel()
	{
		clearImageItemAndBorder();
	}

	void ParticleSystemPanel::update()
	{
	}

	void ParticleSystemPanel::onNewAtla()
	{
	}

	void ParticleSystemPanel::onImport()
	{
		if (!m_importMenu)
		{
			m_importMenu = EchoNew(QMenu(m_ui));
			
			m_importMenu->addAction( m_ui->findChild<QAction*>("m_actionAddNewOne"));
			m_importMenu->addSeparator();
			m_importMenu->addAction(m_ui->findChild<QAction*>("m_actionBuildFromGrid"));
			m_importMenu->addAction(m_ui->findChild<QAction*>("m_actionImportFromImages"));

			EditorApi.qConnectAction(m_ui->findChild<QAction*>("m_actionAddNewOne"), QSIGNAL(triggered()), this, createMethodBind(&ParticleSystemPanel::onNewAtla));
			EditorApi.qConnectAction(m_ui->findChild<QAction*>("m_actionImportFromImages"), QSIGNAL(triggered()), this, createMethodBind(&ParticleSystemPanel::onImportFromImages));
			EditorApi.qConnectAction(m_ui->findChild<QAction*>("m_actionBuildFromGrid"), QSIGNAL(triggered()), this, createMethodBind(&ParticleSystemPanel::onSplit));
		}

		m_importMenu->exec(QCursor::pos());
	}

	void ParticleSystemPanel::onImportFromImages()
	{

	}

	void ParticleSystemPanel::onSplit()
	{
	}

	void ParticleSystemPanel::refreshUiDisplay()
	{
	}

	void ParticleSystemPanel::refreshAtlaList()
	{
	}

	void ParticleSystemPanel::clearImageItemAndBorder()
	{
		if (m_imageItem)
		{
			m_graphicsScene->removeItem(m_imageItem);
			m_imageItem = nullptr;

			// because m_imageBorder is a child of m_imageItem.
			// so m_imageBorder will be delete too.
		}
	}

	void ParticleSystemPanel::refreshImageDisplay()
	{

	}

	void ParticleSystemPanel::onSelectItem()
	{

	}

	void ParticleSystemPanel::onChangedAtlaName()
	{

	}
#endif
}

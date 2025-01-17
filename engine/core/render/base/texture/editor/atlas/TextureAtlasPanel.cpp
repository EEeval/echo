#include "TextureAtlasPanel.h"
#include "engine/core/editor/editor.h"
#include "engine/core/editor/qt/QWidgets.h"
#include "engine/core/base/class_method_bind.h"
#include "engine/core/util/PathUtil.h"
#include "engine/core/util/StringUtil.h"
#include "engine/core/io/memory_reader.h"
#include "engine/core/util/Buffer.h"
#include "engine/core/io/io.h"
#include "engine/core/render/base/image/image.h"
#include "engine/core/render/base/texture/texture_atlas.h"
#include "TextureAtlasPackage.h"
#include "engine/core/main/Engine.h"
#include "engine/core/editor/editor.h"

namespace Echo
{
#ifdef ECHO_EDITOR_MODE
	TextureAtlasPanel::TextureAtlasPanel(Object* obj)
	{
		m_textureAtlas = ECHO_DOWN_CAST<TextureAtlas*>(obj);

		m_ui = (QDockWidget*)EditorApi.qLoadUi("engine/core/render/base/texture/editor/atlas/TextureAtlasPanel.ui");
		m_nodeTreeWidget = m_ui->findChild<QTreeWidget*>("m_nodeTreeWidget");

		m_splitDialog = (QDialog*)EditorApi.qLoadUi("engine/core/render/base/texture/editor/atlas/TextureAtlasSplitDialog.ui");

		QSplitter* splitter = m_ui->findChild<QSplitter*>("m_splitter");
		if (splitter)
		{
			splitter->setStretchFactor(0, 0);
			splitter->setStretchFactor(1, 1);
		}

		// Tool button icons
		QToolButton* importButton = m_ui->findChild<QToolButton*>("m_import");
		if (importButton)
		{
			importButton->setIcon(QIcon((Engine::instance()->getRootPath() + "engine/core/render/base/texture/editor/icon/import.png").c_str()));
		}

		// connect signal slots
		EditorApi.qConnectWidget(m_ui->findChild<QWidget*>("m_import"), QSIGNAL(clicked()), this, createMethodBind(&TextureAtlasPanel::onImport));
		EditorApi.qConnectWidget(m_ui->findChild<QWidget*>("m_nodeTreeWidget"), QSIGNAL(itemClicked(QTreeWidgetItem*, int)), this, createMethodBind(&TextureAtlasPanel::onSelectItem));
		EditorApi.qConnectWidget(m_ui->findChild<QWidget*>("m_nodeTreeWidget"), QSIGNAL(itemChanged(QTreeWidgetItem*, int)), this, createMethodBind(&TextureAtlasPanel::onChangedAtlaName));

		// create QGraphicsScene
		m_graphicsView = m_ui->findChild<QGraphicsView*>("m_graphicsView");
		m_graphicsScene = EditorApi.qGraphicsSceneNew();
		m_graphicsView->setScene(m_graphicsScene);

		refreshUiDisplay();
	}

	TextureAtlasPanel::~TextureAtlasPanel()
	{
		clearImageItemAndBorder();
	}

	void TextureAtlasPanel::update()
	{
	}

	void TextureAtlasPanel::onNewAtla()
	{
		if (m_textureAtlas && m_textureAtlas->getTexture())
		{
			String atlaName = StringUtil::Format("NewAtla");
			m_textureAtlas->addAtla(atlaName, Vector4(0, 0, 128, 128));

			refreshUiDisplay();
		}
	}

	void TextureAtlasPanel::onImport()
	{
		if (!m_importMenu)
		{
			m_importMenu = EchoNew(QMenu(m_ui));
			
			m_importMenu->addAction(m_ui->findChild<QAction*>("m_actionAddNewOne"));
			m_importMenu->addSeparator();
			m_importMenu->addAction(m_ui->findChild<QAction*>("m_actionBuildFromGrid"));
			m_importMenu->addAction(m_ui->findChild<QAction*>("m_actionImportFromImages"));

			EditorApi.qConnectAction(m_ui->findChild<QAction*>("m_actionAddNewOne"), QSIGNAL(triggered()), this, createMethodBind(&TextureAtlasPanel::onNewAtla));
			EditorApi.qConnectAction(m_ui->findChild<QAction*>("m_actionImportFromImages"), QSIGNAL(triggered()), this, createMethodBind(&TextureAtlasPanel::onImportFromImages));
			EditorApi.qConnectAction(m_ui->findChild<QAction*>("m_actionBuildFromGrid"), QSIGNAL(triggered()), this, createMethodBind(&TextureAtlasPanel::onSplit));
		}

		m_importMenu->exec(QCursor::pos());
	}

	void TextureAtlasPanel::onImportFromImages()
	{
		QStringList files = QFileDialog::getOpenFileNames(nullptr, "Select Images", "", "*.png");
		if (!files.empty())
		{
			// load images
			multimap<float, Image*, std::greater<float>>::type images;
			for (QString& file : files)
			{
				Image* image = Image::loadFromFile(file.toStdString().c_str());
				if (image)
					images.insert(std::pair<float, Image*>(float(image->getWidth()*image->getHeight()), image));
			}

			// built atlas
			array<i32, 7> sizes = { 64, 128, 256, 512, 1024, 2048, 4096 };
			for (size_t i = 0; i < sizes.size(); i++)
			{
				bool isSpaceEnough = true;
				TextureAtlasPackage atlasPackage(sizes[i], sizes[i]);
				for (auto& it : images)
				{
					Image* image = it.second;
					String atlaName = PathUtil::GetPureFilename(image->getFilePath(), false);
					if (image)
					{
						vector<Color>::type colors = image->getColors();
						int id = atlasPackage.insert(colors.data(), image->getWidth(), image->getHeight(), atlaName);
						if (id == -1)
						{
							isSpaceEnough = false;
							break;
						}
					}
				}

				if (isSpaceEnough)
				{
					m_textureAtlas->clear();
					for (const TextureAtlasPackage::Node& node : atlasPackage.getAllNodes())
					{
						if (node.m_id != -1)
						{
							String name = any_cast<String>(node.m_userData);
							Vector4 viewPort = atlasPackage.getViewport(node.m_id);

							m_textureAtlas->addAtla(name, viewPort);
						}
					}

					// save png
					{
						String resPath = m_textureAtlas->getPath();
						String fullPath = IO::instance()->convertResPathToFullPath(resPath);
						fullPath = Echo::PathUtil::GetRenameExtFile(fullPath, ".png");

						atlasPackage.saveImage(fullPath);

						if(IO::instance()->convertFullPathToResPath(fullPath, resPath))
							m_textureAtlas->setTextureRes(resPath);
					}

					m_textureAtlas->save();

					break;
				}
			}

			EchoSafeDeleteMap(images, Image);
		}

		refreshUiDisplay();
	}

	void TextureAtlasPanel::onSplit()
	{
		if (m_splitDialog->exec())
		{
			ui32 rows = m_splitDialog->findChild<QSpinBox*>("m_spinBoxRows")->value();
			ui32 columns = m_splitDialog->findChild<QSpinBox*>("m_spinBoxColumns")->value();
			String prefix = m_splitDialog->findChild<QLineEdit*>("m_prefix")->text().toStdString().c_str();

			TexturePtr texture = m_textureAtlas->getTexture();
			if (texture)
			{
				m_textureAtlas->clear();

				float stepWidth = texture->getWidth() / columns;
				float stepHeight = texture->getHeight() / rows;

				for (ui32 row = 0; row < rows; row++)
				{
					for (ui32 column = 0; column < columns; column++)
					{
						float left = column * stepWidth;
						float top = row * stepHeight;
						String atlaName = StringUtil::Format("%s_%d_%d", prefix.c_str(), row, column);
						m_textureAtlas->addAtla(atlaName, Vector4(left, top, stepWidth, stepHeight));
					}
				}

				m_textureAtlas->save();

				refreshUiDisplay();
			}
		}
	}

	void TextureAtlasPanel::refreshUiDisplay()
	{
		refreshAtlaList();
		refreshImageDisplay();
	}

	void TextureAtlasPanel::refreshAtlaList()
	{
		QTreeWidget* nodeTreeWidget = m_ui->findChild<QTreeWidget*>("m_nodeTreeWidget");
		if (nodeTreeWidget)
		{
			nodeTreeWidget->clear();

			QTreeWidgetItem* rootItem = nodeTreeWidget->invisibleRootItem();
			if (rootItem)
			{
				for (const TextureAtlas::Atla atla : m_textureAtlas->getAllAtlas())
				{
					QTreeWidgetItem* objetcItem = new QTreeWidgetItem;
					objetcItem->setText( 0, atla.m_name.c_str());
					objetcItem->setData( 0, Qt::UserRole, atla.m_name.c_str());
					objetcItem->setFlags(objetcItem->flags() | Qt::ItemIsEditable);
					//EditorApi.qTreeWidgetItemSetIcon(objetcItem, 0, Editor::instance()->getNodeIcon(node).c_str());
					rootItem->addChild(objetcItem);
				}
			}
		}
	}

	void TextureAtlasPanel::clearImageItemAndBorder()
	{
		if (m_imageItem)
		{
			m_graphicsScene->removeItem(m_imageItem);
			m_imageItem = nullptr;

			// because m_imageBorder is a child of m_imageItem.
			// so m_imageBorder will be delete too.
		}
	}

	void TextureAtlasPanel::refreshImageDisplay()
	{
		QGraphicsView* graphicsView = m_ui->findChild<QGraphicsView*>("m_graphicsView");
		if (graphicsView)
		{
			clearImageItemAndBorder();

			String resPath = m_textureAtlas->getTextureRes().getPath();
			String fullPath = IO::instance()->convertResPathToFullPath(resPath);

			m_imageItem = EditorApi.qGraphicsSceneAddPixmap(m_graphicsScene, fullPath.c_str());
			m_imageItem->setFlag(QGraphicsItem::ItemIsMovable, true);

			// image border
			QRectF rect = m_imageItem->sceneBoundingRect();

			Color color;
			color.setRGBA(56, 56, 56, 255);

			vector<Vector2>::type paths;
			paths.emplace_back(0.f, 0.f);
			paths.emplace_back(rect.width(), 0.f);
			paths.emplace_back(rect.width(), rect.height());
			paths.emplace_back(0.f, rect.height());
			paths.emplace_back(0.f, 0.f);
			m_imageBorder = EditorApi.qGraphicsSceneAddPath(m_graphicsScene, paths, 1.f, color);
			m_imageBorder->setParentItem(m_imageItem);

			// fit in view
			//EditorApi.qGraphicsViewFitInView(EditorApi.qFindChild(m_ui, "m_graphicsView"), rect);
		}
	}

	void TextureAtlasPanel::onSelectItem()
	{
		QTreeWidgetItem* item = m_nodeTreeWidget->currentItem();
		if (item && m_textureAtlas->getTexture())
		{
			Vector4 viewPort;
			String userData = item->data(0, Qt::UserRole).toString().toStdString().c_str();
			if (m_textureAtlas->getViewport(userData, viewPort))
			{
				if (m_atlaBorder)
				{
					m_graphicsScene->removeItem(m_atlaBorder);
					m_atlaBorder = nullptr;
				}

				// calculate paths
				vector<Vector2>::type paths;
				{
					Real atlaWidth = viewPort.z;
					Real atlaHeight = viewPort.w;
					Real atlaPosX = viewPort.x;
					Real atlaPosY = viewPort.y;

					paths.emplace_back(atlaPosX, atlaPosY);
					paths.emplace_back(atlaPosX+atlaWidth, atlaPosY);
					paths.emplace_back(atlaPosX + atlaWidth, atlaPosY + atlaHeight);
					paths.emplace_back(atlaPosX, atlaPosY + atlaHeight);
					paths.emplace_back(atlaPosX, atlaPosY);
				}

				// create qGraphicsItem
				m_atlaBorder = EditorApi.qGraphicsSceneAddPath(m_graphicsScene, paths, 1.f, Color::RED);
				m_atlaBorder->setParentItem(m_imageItem);
			}
		}
	}

	void TextureAtlasPanel::onChangedAtlaName()
	{
		QTreeWidgetItem* item = m_nodeTreeWidget->currentItem();
		if (item)
		{
			Vector4 viewPort;

			String newName = item->text(0).toStdString().c_str();
			String oldName = item->data(0, Qt::UserRole).toString().toStdString().c_str();
			if (m_textureAtlas->getViewport(oldName, viewPort))
			{
				m_textureAtlas->removeAtla(oldName);
				m_textureAtlas->addAtla(newName, viewPort);

				item->setData( 0, Qt::UserRole, newName.c_str());
			}
		}
	}
#endif
}

#include "Studio.h"
#include "NewNodeDialog.h"
#include "EchoEngine.h"
#include "NodeTreePanel.h"
#include <engine/core/base/class.h>
#include <engine/core/main/module.h>

namespace Studio
{
	static bool isModuleEnable(const Echo::String& nodeName)
	{
		Echo::ClassInfo* cinfo = Echo::Class::getClassInfo(nodeName);
		if (!cinfo->m_module.empty())
		{
			Echo::Object* obj = Echo::Class::create(cinfo->m_module);
			if (obj)
			{
				Echo::Module* module = dynamic_cast<Echo::Module*>(obj);
				if (module)
					return module->isEnable();
			}
		}

		return true;
	}

	NewNodeDialog::NewNodeDialog(QWidget* parent)
		: QDialog(parent)
		, m_viewNodeByModule(true)
	{
		setupUi(this);

		// hide default window title
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

		// sort proxy model
		m_standardModel = new QStandardItemModel(m_treeView);
		m_filterProxyModel = new QSortFilterProxyModel(m_treeView);
		m_filterProxyModel->setSourceModel(m_standardModel);
		m_filterProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
		//m_filterProxyModel->setFilterKeyColumn(0);
		m_treeView->setModel(m_filterProxyModel);
        m_treeView->setAttribute(Qt::WA_MacShowFocusRect,0);
        m_searchLineEdit->setAttribute(Qt::WA_MacShowFocusRect,0);

		// connect signal slot
		QObject::connect(m_treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onSelectNode(QModelIndex)));
		QObject::connect(m_confirm, SIGNAL(clicked()), this, SLOT(onConfirmNode()));
		QObject::connect(m_cancel, SIGNAL(clicked()), this, SLOT(reject()));
		QObject::connect(m_viewNodeButton, SIGNAL(clicked()), this, SLOT(onSwitchNodeVeiwType()));
		QObject::connect(m_treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onConfirmNode()));
		QObject::connect(m_searchLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onSearchTextChanged()));

		recoverEditSettings();
	}

	NewNodeDialog::~NewNodeDialog()
	{

	}

	NewNodeDialog* NewNodeDialog::instance()
	{
		NewNodeDialog* inst = new NewNodeDialog();
		return inst;
	}

	Echo::String NewNodeDialog::getSelectedNodeType()
	{
		NewNodeDialog* inst = instance();
		inst->setVisible(true);
		if (inst->exec() == QDialog::Accepted)
		{
			return inst->m_result;
		}

		return "";
	}

	void NewNodeDialog::initNodeDisplayByModule()
	{
		m_standardModel->clear();

		addNode( "Node");

		m_treeView->expandAll();
	}

	QStandardItem* NewNodeDialog::getModuleItem(const Echo::String& nodeName)
	{
		Echo::ClassInfo* cinfo = Echo::Class::getClassInfo(nodeName);
		Echo::String moduleName = (cinfo && !cinfo->m_module.empty()) ? cinfo->m_module : "Core";
		Echo::String moduleDisplayName = Echo::StringUtil::Replace(moduleName, "Module", "");

		QStandardItem* rootItem = m_standardModel->invisibleRootItem();
		for (int i = 0; i < rootItem->rowCount(); i++)
		{
			QStandardItem* moduleItem = rootItem->child(i, 0);
			if (moduleItem->text().toStdString().c_str() == moduleDisplayName)
			{
				return moduleItem;
			}
		}

		// create module item
		QStandardItem* moduleItem = new QStandardItem;
		moduleItem->setText( moduleDisplayName.c_str());
		moduleItem->setData( "module", Qt::UserRole);
		rootItem->setChild(rootItem->rowCount(), moduleItem);

		// sort
		rootItem->sortChildren(0);

		return getModuleItem( nodeName);
	}

	QStandardItem* NewNodeDialog::createQTreeWidgetItemByNodeName(const Echo::String& nodeName, QStandardItem* parent, bool isCreateWhenNodeIsVirtual)
	{
		bool isNodeVirtual = Echo::Class::isVirtual(nodeName);
		if (isNodeVirtual && !isCreateWhenNodeIsVirtual)
			return nullptr;

		QStandardItem* nodeItem = new QStandardItem();
		parent->setChild(parent->rowCount(), nodeItem);
		nodeItem->setText( nodeName.c_str());
		nodeItem->setData( "node", Qt::UserRole);
		if (!isNodeVirtual)
		{
			Echo::Node* node = (Echo::Node*)Echo::Class::create(nodeName);
			if (node)
			{
				node->setName(nodeName);
				Echo::ImagePtr thumbnail = node->getEditor() ? node->getEditor()->getThumbnail() : nullptr;
				EchoSafeDelete(node, Node);
				if (thumbnail)
				{
					QImage image(thumbnail->getData(), thumbnail->getWidth(), thumbnail->getHeight(), QImage::Format_RGBA8888);
					nodeItem->setIcon(QIcon(QPixmap::fromImage(image)));
				}
				else
				{
					// get icon path by node name
					Echo::String lowerCaseNodeName = nodeName;
					Echo::StringUtil::LowerCase(lowerCaseNodeName);
					Echo::String qIconPath = Echo::StringUtil::Format(":/icon/node/%s.png", lowerCaseNodeName.c_str());

					nodeItem->setIcon(QIcon(qIconPath.c_str()));
				}
			}
		}

		return nodeItem;
	}

	void NewNodeDialog::addNode(const Echo::String& nodeName)
	{
		if (isModuleEnable(nodeName))
		{
			// use module item as parent
			QStandardItem* moduleParent = getModuleItem(nodeName);
			createQTreeWidgetItemByNodeName(nodeName, moduleParent, false);

			// recursive all children
			Echo::StringArray childNodes;
			if (Echo::Class::getChildClasses(childNodes, nodeName.c_str(), false))
			{
				for (const Echo::String& childNode : childNodes)
				{
					addNode(childNode);
				}
			}
		}
	}

	void NewNodeDialog::initNodeDisplayByInherite()
	{
		m_standardModel->clear();

		// begin with "Node"
		addNode("Node", m_standardModel->invisibleRootItem());

		// expand all items
		m_treeView->expandAll();
	}

	void NewNodeDialog::addNode(const Echo::String& nodeName, QStandardItem* parent)
	{
		if (isModuleEnable(nodeName))
		{
			// create by node name
			QStandardItem* nodeItem = createQTreeWidgetItemByNodeName(nodeName, parent, true);

			// recursive all children
			Echo::StringArray childNodes;
			if (Echo::Class::getChildClasses(childNodes, nodeName.c_str(), false))
			{
				for (const Echo::String& childNode : childNodes)
				{
					addNode(childNode, nodeItem);
				}
			}
		}
	}

	void NewNodeDialog::onSelectNode(QModelIndex index)
	{
		Echo::String text = m_filterProxyModel->data(index, Qt::DisplayRole).toString().toStdString().c_str();
		Echo::String userData = m_filterProxyModel->data(index, Qt::UserRole).toString().toStdString().c_str();
		m_confirm->setEnabled( userData == "node" && !Echo::Class::isVirtual(text) && isModuleEnable(text));
	}

	void NewNodeDialog::onConfirmNode()
	{
		Echo::String text = m_filterProxyModel->data( m_treeView->currentIndex(), Qt::DisplayRole).toString().toStdString().c_str();
		if (!Echo::Class::isVirtual( text))
		{
			m_result = text;

			accept();

			hide();
		}
	}

	void NewNodeDialog::onSwitchNodeVeiwType()
	{
		m_viewNodeByModule = !m_viewNodeByModule;
		if (m_viewNodeByModule)
		{
			initNodeDisplayByModule();

			m_viewNodeButton->setIcon(QIcon(":/icon/Icon/res/view_type_list.png"));
			m_viewNodeButton->setToolTip("Display node by Module");
		}
		else
		{
			initNodeDisplayByInherite();

			m_viewNodeButton->setIcon(QIcon(":/icon/Icon/res/view_type_grid.png"));
			m_viewNodeButton->setToolTip("Display node by inheritance relationships");
		}

		// save config
		AStudio::instance()->getConfigMgr()->setValue("NewNodeDialog_NodeViewType", m_viewNodeByModule ? "Module" : "Inherit");
	}

	void NewNodeDialog::recoverEditSettings()
	{
		Echo::String viewType = AStudio::instance()->getConfigMgr()->getValue("NewNodeDialog_NodeViewType");
		if (!viewType.empty() && m_viewNodeByModule && viewType != "Module")
		{
			onSwitchNodeVeiwType();
		}
		else
		{
			initNodeDisplayByModule();
		}
	}

	void NewNodeDialog::onSearchTextChanged()
	{
		Echo::String pattern = m_searchLineEdit->text().toStdString().c_str();

		QRegExp regExp(pattern.c_str(), Qt::CaseInsensitive);
		m_filterProxyModel->setFilterRegExp(regExp);

		m_treeView->expandAll();
	}
}

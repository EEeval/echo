#include "QChannelEditor.h"
#include "QCheckBoxEditor.h"
#include <QColorDialog>
#include <engine/core/util/StringUtil.h>
#include "ChannelExpressionDialog.h"

namespace QT_UI
{
	QChannelEditor::QChannelEditor(class QPropertyModel* model, QString propertyName, QWidget* parent)
		: QPushButton( parent)
		, m_propertyModel(model)
		, m_propertyName(propertyName)
	{ 
		connect( this, SIGNAL(clicked()), this, SLOT(onDisplayExpression()));
	}

	void QChannelEditor::onDisplayExpression()
	{
		m_displayExpression = !m_displayExpression;
	}

	void QChannelEditor::mouseDoubleClickEvent(QMouseEvent* event)
	{

	}

	void QChannelEditor::setInfo( const string& info)
	{ 
		m_info = info.c_str();
	}

	const string& QChannelEditor::getInfo()
	{
		return m_info;
	}

	bool QChannelEditor::ItemDelegatePaint( QPainter *painter, const QRect& rect, const string& val)
	{
		return ItemDelegatePaintExpression(painter, rect, val, false);
	}

	bool QChannelEditor::ItemDelegatePaintExpression(QPainter *painter, const QRect& rect, const string& val, bool isRenderExpressionOnly)
	{
		Echo::StringArray	dataArray = Echo::StringUtil::Split(val.c_str(), "#");
		if (dataArray.size() == 3)
		{
			Echo::String		expression = dataArray[0];
			Echo::String		value = dataArray[1];
			Echo::Variant::Type type = Echo::Variant::Type(Echo::StringUtil::ParseI32(dataArray[2]));

			// color rect
			QRect tRect = QRect(rect.left() + 1, rect.top() + 1, rect.width() - 2, rect.height() - 2);
			painter->setBrush(QColor(70, 140, 70));
			painter->drawRect(tRect);
			painter->setPen(QColor(0, 0, 0));
			painter->drawRect(QRect(rect.left(), rect.top(), rect.width() - 1, rect.height() - 1));

			if (!isRenderExpressionOnly)
			{
				if (type == Echo::Variant::Type::Bool)
				{
					QCheckBoxEditor::ItemDelegatePaint(painter, rect, value);
				}
				else
				{
					Echo::String text = value;
					QRect textRect(rect.left() + 6, rect.top() + 3, rect.width() - 6, rect.height() - 6);
					QFont font = painter->font(); font.setBold(false);
					painter->setFont(font);
					painter->setPen(QColor(232, 232, 232));
					painter->drawText(textRect, Qt::AlignLeft, text.c_str());
				}
			}
			else
			{
				// text
				Echo::String text = expression.c_str();
				QRect textRect(rect.left() + 6, rect.top() + 3, rect.width() - 6, rect.height() - 6);
				QFont font = painter->font(); font.setBold(false);
				painter->setFont(font);
				painter->setPen(QColor(232, 232, 232));
				painter->drawText(textRect, Qt::AlignLeft, text.c_str());
			}

			return true;
		}

		return false;
	}

	QSize QChannelEditor::sizeHint() const
	{
		return size();
	}

	void QChannelEditor::paintEvent( QPaintEvent* event)
	{
		QPainter painter( this);

		ItemDelegatePaintExpression( &painter, rect(), m_info.c_str(), m_displayExpression);
	}
}

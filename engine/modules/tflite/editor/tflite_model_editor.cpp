#include "tflite_model_editor.h"
#include "engine/core/editor/editor.h"
#include "engine/core/math/Curve.h"
#include "engine/core/main/Engine.h"

namespace Echo
{
#ifdef ECHO_EDITOR_MODE
	TFLiteModelEditor::TFLiteModelEditor(Object* object)
		: ObjectEditor(object)
	{

	}

	TFLiteModelEditor::~TFLiteModelEditor()
	{

	}

	ImagePtr TFLiteModelEditor::getThumbnail() const
	{
		return Image::loadFromFile(Engine::instance()->getRootPath() + "engine/modules/tflite/editor/icon/tflite-model.png");
	}

	void TFLiteModelEditor::onEditorSelectThisNode()
	{
	}

	void TFLiteModelEditor::postEditorCreateObject()
	{

	}

	void TFLiteModelEditor::editor_update_self()
	{

	}
#endif
}


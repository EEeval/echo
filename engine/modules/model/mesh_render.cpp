#include "mesh_render.h"
#include "engine/core/io/memory_reader.h"

namespace Echo
{
	MeshRender::MeshRender()
	{
		setRenderType("3d");
	}

	MeshRender::~MeshRender()
	{

	}

	void MeshRender::bindMethods()
	{
		CLASS_BIND_METHOD(MeshRender, getMesh);
		CLASS_BIND_METHOD(MeshRender, setMesh);
		CLASS_BIND_METHOD(MeshRender, getMaterial);
		CLASS_BIND_METHOD(MeshRender, setMaterial);

		CLASS_REGISTER_PROPERTY(MeshRender, "Mesh", Variant::Type::Object, getMesh, setMesh);
		CLASS_REGISTER_PROPERTY_HINT(MeshRender, "Mesh", PropertyHintType::ObjectType, "Mesh");
		CLASS_REGISTER_PROPERTY(MeshRender, "Material", Variant::Type::Object, getMaterial, setMaterial);
		CLASS_REGISTER_PROPERTY_HINT(MeshRender, "Material", PropertyHintType::ObjectType, "Material");
	}

	void MeshRender::setMesh(Object* mesh)
	{
		m_mesh = (Mesh*)mesh;

		m_isRenderableDirty = true;
	}

	void MeshRender::setMaterial(Object* material)
	{
		m_material = (Material*)material;

		m_isRenderableDirty = true;
	}

	void MeshRender::updateInternal(float elapsedTime)
	{
		buildRenderable();

		if (m_renderable)
		{
			m_renderable->setSubmitToRenderQueue(isNeedRender());
		}
	}

	void MeshRender::buildRenderable()
	{
		if (m_isRenderableDirty)
		{
			clearRenderable();

			// make sure one material is valid
			if (!m_material && m_mesh)
			{
				StringArray macros;
				if(m_mesh->getVertexData().getFormat().m_isUseNormal)
					macros.emplace_back("HAS_NORMALS");

				ShaderProgramPtr shader = ShaderProgram::getDefault3D(macros);

				// material
				m_material = ECHO_CREATE_RES(Material);
				m_material->setShaderPath(shader->getPath());
			}

			// mesh
			if (m_mesh)
			{
				// create renderable
				m_renderable = RenderProxy::create(m_mesh, m_material, this, true);
			}

			m_isRenderableDirty = false;
		}
	}

	void MeshRender::clearRenderable()
	{
		m_renderable.reset();
	}
}

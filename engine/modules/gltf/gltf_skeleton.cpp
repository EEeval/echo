#include "gltf_skeleton.h"
#include "engine/core/log/Log.h"
#include "engine/core/main/Engine.h"
#include "engine/core/util/magic_enum.hpp"

namespace Echo
{
	GltfSkeleton::GltfSkeleton()
		: m_animations("")
	{
	}

	GltfSkeleton::~GltfSkeleton()
	{
	}

	void GltfSkeleton::bindMethods()
	{
		CLASS_BIND_METHOD(GltfSkeleton, getGltfRes);
		CLASS_BIND_METHOD(GltfSkeleton, setGltfRes);
		CLASS_BIND_METHOD(GltfSkeleton, getAnim);
		CLASS_BIND_METHOD(GltfSkeleton, setAnim);

		CLASS_REGISTER_PROPERTY(GltfSkeleton, "Gltf", Variant::Type::ResourcePath, getGltfRes, setGltfRes);
		CLASS_REGISTER_PROPERTY(GltfSkeleton, "Anim", Variant::Type::StringOption, getAnim, setAnim);
	}

	// set gltf resource
	void GltfSkeleton::setGltfRes(const ResourcePath& path)
	{
		if (m_assetPath.setPath(path.getPath()))
		{
			m_asset = (GltfRes*)Res::get(m_assetPath);	
			if (m_asset)
			{
				for (GltfAnimInfo& gltfAnim : m_asset->m_animations)
				{
					AnimClip* clip = gltfAnim.m_clip;
					if (clip)
					{
						if (clip->m_name.empty())
							generateUniqueName(clip->m_name);

						m_clips.emplace_back(clip);
						m_animations.addOption(clip->m_name);
					}
				}

				if (m_asset->m_animations.size() && m_asset->m_nodes.size())
				{
					if (m_nodeTransforms.empty())
						m_nodeTransforms.resize(m_asset->m_nodes.size());
				}
			}
		}
	}

	// play anim
	void GltfSkeleton::setAnim(const StringOption& animName)
	{
		m_animations.setValue(animName.getValue());
	}

	// get current anim clip
	const AnimClip* GltfSkeleton::getAnimClip()
	{
		return m_animations.isValid() ? m_clips[m_animations.getIdx()] : nullptr;
	}

	void GltfSkeleton::updateInternal(float elapsedTime)
	{
		if (m_animations.isValid())
		{
			ui32 deltaTime = Engine::instance()->getFrameTimeMS();
			AnimClip* clip = m_clips[m_animations.getIdx()];
			if (clip)
			{
				clip->update(deltaTime);

				extractClipData(clip);
			}
		}
	}

	bool GltfSkeleton::isAnimExist(const char* animName)
	{
		for (AnimClip* clip : m_clips)
		{
			if (clip->m_name == animName)
				return true;
		}

		return false;
	}

	void GltfSkeleton::generateUniqueName(String& oName)
	{
		char name[128] = "anim_";
		for (i32 i = 0; i < 65535; i++)
		{
			sprintf( name+5, "%d", i);
			if (!isAnimExist(name))
			{
				oName = name;
				break;
			}
		}
	}

	void GltfSkeleton::extractClipData(AnimClip* clip)
	{
		if (clip)
		{
			// reset
			for (Transform& transform : m_nodeTransforms)
			{
				transform.reset();
			}

			for (AnimObject* animNode : clip->m_objects)
			{
				// copy all properties results of this node
				i32 nodeIdx = any_cast<i32>(animNode->m_userData);
				for (AnimProperty* property : animNode->m_properties)
				{
					GltfAnimChannel::Path channelPath = magic_enum::enum_cast<GltfAnimChannel::Path>(property->m_name.c_str()).value_or(GltfAnimChannel::Path::Translation);
					switch (channelPath)
					{
					case GltfAnimChannel::Path::Translation:	m_nodeTransforms[nodeIdx].m_pos = ((AnimPropertyVec3*)property)->getValue(); break;
					case GltfAnimChannel::Path::Rotation:		m_nodeTransforms[nodeIdx].m_quat = ((AnimPropertyQuat*)property)->getValue(); break;
					case GltfAnimChannel::Path::Scale:			m_nodeTransforms[nodeIdx].m_scale = ((AnimPropertyVec3*)property)->getValue(); break;
					//case GltfAnimChannel::Path::Weights:		m_nodeTransforms[nodeIdx].m_pos = ((AnimPropertyVec3*)property)->getValue(); break;
					default: EchoLogError("Unprocessed gltf anim data form gltf skeleton");	break;
					}
				}
			}

			for (GltfSceneInfo& scene : m_asset->m_scenes)
			{
				for (ui32 rootNodeIdx : scene.m_nodes)
				{
					jointInhertParentTransform(rootNodeIdx);
				}
			}
		}
	}

	void GltfSkeleton::jointInhertParentTransform(i32 nodeIdx)
	{
		const GltfNodeInfo& nodeInfo = m_asset->m_nodes[nodeIdx];
		if (nodeInfo.m_parent != -1)
		{
			m_nodeTransforms[nodeIdx] = m_nodeTransforms[nodeInfo.m_parent] * m_nodeTransforms[nodeIdx];
		}

		for (ui32 child : nodeInfo.m_children)
		{
			jointInhertParentTransform(child);
		}
	}

	bool GltfSkeleton::getGltfNodeTransform(Transform& transform, size_t nodeIdx)
	{
		if (nodeIdx < m_nodeTransforms.size())
		{
			transform = m_nodeTransforms[nodeIdx];
			return true;
		}

		return false;
	}
}

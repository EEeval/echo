#include "object.h"
#include "engine/core/resource/Res.h"
#include "engine/core/log/Log.h"
#include <thirdparty/pugixml/pugixml.hpp>
#include <thirdparty/pugixml/pugiconfig.hpp>

namespace Echo
{
	static std::unordered_map<i32, Object*> g_objs;

	Object::Object()
	{
#ifdef ECHO_EDITOR_MODE
		m_objectEditor = nullptr;
#endif

		// begin with 1
		static i32 id = 1;
		m_id = id++;

		g_objs[m_id] = this;
	}

	Object::~Object()
	{
#ifdef ECHO_EDITOR_MODE
		EchoSafeDelete(m_objectEditor, ObjectEditor);
#endif

		clearPropertys();

        unregisterFromScript();
		unregisterChannels();

		auto it = g_objs.find(m_id);
		if (it != g_objs.end())
		{
			g_objs.erase(it);
		}
		else
		{
			EchoLogError("Object isn't exist. destruct failed.");
		}
	}

	void Object::bindMethods()
	{
		BIND_METHOD(Object::connect, DEF_METHOD("Object.connect"));
		BIND_METHOD(Object::disconnect, DEF_METHOD("Object.disconnect"));

		CLASS_BIND_METHOD(Object, getId);
		CLASS_BIND_METHOD(Object, isChannelExist);
	}

	bool Object::isValid()
	{
		Object* obj = getById(m_id);
		return obj && obj==this ? true : false;
	}

	Object* Object::getById(i32 id)
	{
		auto it = g_objs.find(id);
		if (it!=g_objs.end())
		{
			return it->second;
		}

		return nullptr;
	}

	bool Object::isObject(const void* ptr)
	{
		return getById(((Object*)ptr)->getId()) == ptr;
	}

	const String& Object::getClassName() const
	{
		static String className = "Object";
		return className;
	}

    void Object::registerToScript() 
    {
        if (!m_registeredToScript)
        {
            String globalTableName = StringUtil::Format("objs._%d", this->getId());
            LuaBinder::instance()->registerObject(getClassName(), globalTableName.c_str(), this);

            m_registeredToScript = true;
        }
    }

    void Object::unregisterFromScript()
    {
        if (m_registeredToScript)
        {
            String luaStr = StringUtil::Format("objs._%d = nil", getId());
            LuaBinder::instance()->execString(luaStr);
        }
    }

	bool Object::connect(Object* from, const char* signalName, Object* to, const char* luaFunName)
	{
		Signal* signal = Class::getSignal(from, signalName);
		if (signal)
		{
			return signal->connectLuaMethod(to, luaFunName);
		}
		else
		{
			EchoLogError("Signal [%s] not found in class [%s]", signalName, from->getClassName().c_str());
			return false;
		}
	}

	bool Object::disconnect(Object* from, const char* signalName, Object* to, const char* luaFunName)
	{
		Signal* signal = Class::getSignal(from, signalName);
		if (signal)
		{
			signal->disconnectLuaMethod(to, luaFunName);
			return true;
		}
		else
		{
			EchoLogError("Signal [%s] not found in class [%s]", signalName, from->getClassName().c_str());
			return false;
		}
	}
    
    void Object::unregisterChannel(const String& propertyName)
    {
		if (m_chanels)
		{
			for (std::vector<Channel*>::iterator it = (*m_chanels).begin(); it != (*m_chanels).end(); it++)
			{
				if ((*it)->getName() == propertyName)
				{
					EchoSafeDelete(*it, Channel);
					(*m_chanels).erase(it);
					break;
				}
			}
		}
    }
    
    void Object::unregisterChannels()
    {
		if (m_chanels)
		{
			EchoSafeDeleteContainer((*m_chanels), Channel);
            delete m_chanels; m_chanels = nullptr;
		}
    }
    
    bool Object::registerChannel(const String& propertyName, const String& expression)
    {
        // channel depends on lua
        registerToScript();
        
		if (!m_chanels)
			m_chanels = new std::vector<Channel*>;
		else
			unregisterChannel(propertyName);

        Channel* channel = EchoNew(Channel(this, propertyName, expression));
        m_chanels->push_back(channel);
        
        return true;
    }

	Channel* Object::getChannel(const String& propertyName)
	{
		if (m_chanels)
		{
			for (Channel* channel : *m_chanels)
			{
				if (channel->getName() == propertyName)
					return channel;
			}
		}

		return nullptr;
	}
    
    bool Object::isChannelExist(const String& propertyName)
    {
        if(!m_chanels)
            return false;
        
        for(Channel* channel : *m_chanels)
        {
            if(channel->getName() == propertyName)
                return true;
        }
        
        return false;
    }

	const PropertyInfos& Object::getPropertys() 
	{ 
		return m_propertys;
	}

	void Object::clearPropertys()
	{
		EchoSafeDeleteContainer(m_propertys, PropertyInfo);
	}

	bool Object::registerProperty(const String& className, const String& propertyName, const Variant::Type type, const PropertyHintArray& hints)
	{
		PropertyInfoDynamic* info = EchoNew(PropertyInfoDynamic);
		info->m_name = propertyName;
		info->m_type = type;
		info->m_className = className;
		info->m_hints = hints;

		m_propertys.push_back(info);

		return true;
	}

	Object* Object::instanceObject(void* pugiNode)
	{
		pugi::xml_node* xmlNode = (pugi::xml_node*)pugiNode;

		Echo::String className = xmlNode->attribute("class").value();
		Object* res = Echo::Class::create<Object*>(className);
		if (res)
		{
			loadPropertyRecursive(pugiNode, res, className);
            loadSignalSlotConnects(xmlNode, res, className);
            loadChannels(xmlNode, res);

			return res;
		}
		else
		{
			EchoLogError("Class::create failed. Class [%s] not exist", className.c_str());
		}

		return  nullptr;
	}

	void Object::loadPropertyRecursive(void* pugiNode, Echo::Object* classPtr, const Echo::String& className)
	{
		// load parent property first
		Echo::String parentClassName;
		if (Echo::Class::getParentClass(parentClassName, className))
		{
			// don't display property of object
			if (parentClassName != "Object")
				loadPropertyRecursive(pugiNode, classPtr, parentClassName);
		}

		// load property
		loadPropertyValue(pugiNode, classPtr, className, PropertyInfo::Static);
		loadPropertyValue(pugiNode, classPtr, className, PropertyInfo::Dynamic);
	}

	void Object::loadPropertyValue(void* pugiNode, Echo::Object* classPtr, const Echo::String& className, i32 flag)
	{
		pugi::xml_node* xmlNode = (pugi::xml_node*)pugiNode;

		Echo::PropertyInfos propertys;
		Echo::Class::getPropertys(className, classPtr, propertys, flag);

		// iterator
		for (const Echo::PropertyInfo* prop : propertys)
		{
			if (prop->m_type == Variant::Type::Object)
			{
				for (pugi::xml_node propertyNode = xmlNode->child("property"); propertyNode; propertyNode = propertyNode.next_sibling("property"))
				{
					String propertyName = propertyNode.attribute("name").as_string();
					if (propertyName == prop->m_name)
					{
						String path = propertyNode.attribute("path").as_string();
						if (!path.empty())
						{
							Res* res = Res::get(path);
							Class::setPropertyValue(classPtr, prop->m_name, res);
						}
						else
						{
							pugi::xml_node objNode = propertyNode.child("obj");
							Object* obj = instanceObject(&objNode);
							Class::setPropertyValue(classPtr, prop->m_name, obj);
						}

						break;
					}
				}
			}
			else if (prop->m_type == Variant::Type::String && prop->IsHaveHint(PropertyHintType::XmlCData))
			{
				for (pugi::xml_node propertyNode = xmlNode->child("property"); propertyNode; propertyNode = propertyNode.next_sibling("property"))
				{
					String propertyName = propertyNode.attribute("name").as_string();
					if (propertyName == prop->m_name)
					{
						Echo::Variant var;

						// https://pugixml.org/docs/manual.html
						// child_value() returns the value of the first child with type node_pcdata or node_cdata;
						String valueStr = propertyNode.child_value();
						if (!valueStr.empty())
						{
							var.fromString(prop->m_type, valueStr);
							Class::setPropertyValue(classPtr, prop->m_name, var);
						}

						break;
					}
				}
			}
			else
			{
				Echo::Variant var;
				String valueStr = xmlNode->attribute(prop->m_name.c_str()).value();
				if (!valueStr.empty())
				{
					var.fromString(prop->m_type, valueStr);
					Class::setPropertyValue(classPtr, prop->m_name, var);
				}
			}
		}
	}

	void Object::savePropertyRecursive(void* pugiNode, Echo::Object* classPtr, const Echo::String& className)
	{
		pugi::xml_node* xmlNode = (pugi::xml_node*)pugiNode;

		// save parent property first
		Echo::String parentClassName;
		if (Echo::Class::getParentClass(parentClassName, className))
		{
			// don't display property of object
			if (parentClassName == "Object")
            {
                xmlNode->append_attribute("class").set_value(classPtr->getClassName().c_str());
            }
            else
            {
                savePropertyRecursive(pugiNode, classPtr, parentClassName);
            }
		}

		Echo::PropertyInfos propertys;
		Echo::Class::getPropertys(className, classPtr, propertys);
		for (Echo::PropertyInfo* prop : propertys)
		{
			if (Echo::Class::getPropertyFlag(classPtr, prop->m_name) & PropertyFlag::Save)
			{
				Echo::Variant var;
				Echo::Class::getPropertyValue(classPtr, prop->m_name, var);
				if (var.getType() == Variant::Type::Object)
				{
					Object* obj = var.toObj();
					if (obj)
					{
						pugi::xml_node propertyNode = xmlNode->append_child("property");
						propertyNode.append_attribute("name").set_value(prop->m_name.c_str());
						if (!obj->getPath().empty())
						{
							propertyNode.append_attribute("path").set_value(obj->getPath().c_str());
						}
						else
						{
							pugi::xml_node objNode = propertyNode.append_child("obj");
							savePropertyRecursive(&objNode, obj, obj->getClassName());
						}
					}
				}
				else if (var.getType() == Variant::Type::String && prop->IsHaveHint(PropertyHintType::XmlCData))
				{
					Echo::String varStr = var.toString();
					pugi::xml_node propertyNode = xmlNode->append_child("property");
					propertyNode.append_attribute("name").set_value(prop->m_name.c_str());

					propertyNode.append_child(pugi::node_cdata).set_value(varStr.c_str());
				}
				else
				{
					Echo::String varStr = var.toString();
					xmlNode->append_attribute(prop->m_name.c_str()).set_value(varStr.c_str());
				}
			}
		}
	}
    
    void Object::loadSignalSlotConnects(void* pugiNode, Echo::Object* classPtr, const Echo::String& className)
    {
        for (pugi::xml_node signalNode = ((pugi::xml_node*)pugiNode)->child("signal"); signalNode; signalNode = signalNode.next_sibling("signal"))
        {
            // get signal by class name
            String signalName = signalNode.attribute("name").as_string();
            Signal* signal = Class::getSignal(classPtr, signalName);
            if(signal)
                signal->load(&signalNode);
        }
    }
    
    void Object::saveSignalSlotConnects(void* pugiNode, Echo::Object* classPtr, const Echo::String& className)
    {
        pugi::xml_node* xmlNode = (pugi::xml_node*)pugiNode;
        
        // save parent property first
        Echo::String parentClassName;
        if (Echo::Class::getParentClass(parentClassName, className))
        {
            // don't display property of object
            if (parentClassName != "Object")
                saveSignalSlotConnects(pugiNode, classPtr, parentClassName);
        }
        
        Echo::ClassInfo* classInfo = Echo::Class::getClassInfo(className);
        if(classInfo && !classInfo->m_signals.empty())
        {
            // iterate signals
            for(auto it : classInfo->m_signals)
            {
                Variant::CallError error;
                Signal* signal = it.second->call(classPtr, nullptr, 0, error);
                if(signal && signal->isHaveConnects())
                {
                    pugi::xml_node signalNode = xmlNode->append_child("signal");
                    signalNode.append_attribute("name").set_value(it.first.c_str());
                    
                    // connects
                    signal->save(&signalNode);
                }
            }
        }
    }
    
    void Object::loadChannels(void* pugiNode, Echo::Object* classPtr)
    {
        for (pugi::xml_node channelNode = ((pugi::xml_node*)pugiNode)->child("channel"); channelNode; channelNode = channelNode.next_sibling("channel"))
        {
            // get signal by class name
            String name = channelNode.attribute("name").as_string();
            String expression = channelNode.attribute("expression").as_string();
            
            classPtr->registerChannel( name, expression);
        }
    }
    
    void Object::saveChannels(void* pugiNode, Echo::Object* classPtr)
    {
        ChannelsPtr channels = classPtr->getChannels();
        if(channels)
        {
            pugi::xml_node* xmlNode = (pugi::xml_node*)pugiNode;
            for(Channel* channel : *channels)
            {
                pugi::xml_node channelNode = xmlNode->append_child("channel");
                channelNode.append_attribute("name").set_value(channel->getName().c_str());
                channelNode.append_attribute("expression").set_value(channel->getExpression().c_str());
            }
        }
    }
}

#include "windows_build_settings.h"
#include <engine/core/io/IO.h>
#include <engine/core/util/PathUtil.h>
#include <engine/core/main/Engine.h>
#include <engine/core/main/module.h>
#include <engine/core/io/stream/FileHandleDataStream.h>

namespace Echo
{
	WindowsBuildSettings::WindowsBuildSettings()
	{

	}

	WindowsBuildSettings::~WindowsBuildSettings()
	{

	}

	WindowsBuildSettings* WindowsBuildSettings::instance()
	{
		static WindowsBuildSettings* inst = EchoNew(WindowsBuildSettings);
		return inst;
	}

	void WindowsBuildSettings::bindMethods()
	{
		CLASS_BIND_METHOD(WindowsBuildSettings, getIconRes);
		CLASS_BIND_METHOD(WindowsBuildSettings, setIconRes);

		CLASS_REGISTER_PROPERTY(WindowsBuildSettings, "Icon", Variant::Type::ResourcePath, getIconRes, setIconRes);
	}

	ImagePtr WindowsBuildSettings::getPlatformThumbnail() const
	{
		return Image::loadFromFile(Engine::instance()->getRootPath() + "editor/echo/Editor/Modules/build/editor/icon/windows.png");
	}

	void WindowsBuildSettings::setOutputDir(const String& outputDir)
	{
		m_outputDir = outputDir;
		PathUtil::FormatPath(m_outputDir, false);
	}

	bool WindowsBuildSettings::prepare()
	{
		m_rootDir = Engine::instance()->getRootPath();
		m_projectDir = Engine::instance()->getResPath();
		m_outputDir = m_outputDir.empty() ? PathUtil::GetCurrentDir() + "/build/windows/" : m_outputDir;

		// create dir
		if (!PathUtil::IsDirExist(m_outputDir))
		{
			log("Create output directory : [%s]", m_outputDir.c_str());
			PathUtil::CreateDir(m_outputDir);
		}

		return true;
	}

	void WindowsBuildSettings::setIconRes(const ResourcePath& path)
	{

	}

	void WindowsBuildSettings::copySrc()
	{
		log("Copy Engine Source Code ...");

		// copy app
		PathUtil::CopyDir(m_rootDir + "app/windows/", m_outputDir + "app/windows/");

		// copy engine
		PathUtil::CopyDir(m_rootDir + "engine/", m_outputDir + "engine/");

		// copy thirdparty
		PathUtil::CopyDir(m_rootDir + "thirdparty/", m_outputDir + "thirdparty/");

		// copy CMakeLists.txt
		PathUtil::CopyFilePath(m_rootDir + "CMakeLists.txt", m_outputDir + "CMakeLists.txt");

		// copy build script
		PathUtil::CopyFilePath(m_rootDir + "tool/build/windows/cmake.bat", m_outputDir + "cmake.bat");
	}

	void WindowsBuildSettings::copyRes()
	{
		log("Convert Project File ...");

		// copy release used dlls
		PathUtil::CopyDir(m_rootDir + "bin/app/Win64/Release/", m_outputDir + "bin/app/win64/Release/");
		PathUtil::CopyDir(m_rootDir + "bin/app/Win64/Debug/", m_outputDir + "bin/app/win64/Debug/");

		// copy res
		PathUtil::CopyDir(m_projectDir, m_outputDir + "bin/app/win64/Release/data/");
		packageRes(m_outputDir + "bin/app/win64/Release/data/");

		// rename
		String projectFile = PathUtil::GetPureFilename(Engine::instance()->getConfig().m_projectFile);
		PathUtil::RenameFile(m_outputDir + "bin/app/win64/Release/data/" + projectFile, m_outputDir + "bin/app/win64/Release/data/app.echo");
	}

	void WindowsBuildSettings::cmake()
	{

	}

	void WindowsBuildSettings::compile()
	{

	}

	void WindowsBuildSettings::build()
	{
		log("Build App for Windows x64 platform.");

		m_listener->onBegin();

		if (prepare())
		{
			copySrc();
			copyRes();

			writeModuleConfig();

			// cmake();

			// compile();
		}

		m_listener->onEnd();
	}

	String WindowsBuildSettings::getFinalResultPath()
	{
		return m_outputDir;
	}

	void WindowsBuildSettings::writeModuleConfig()
	{
		String  moduleSrc;

		// include
		writeLine(moduleSrc, "#include <engine/core/main/module.h>\n");

		// namespace
		writeLine(moduleSrc, "namespace Echo\n{");
		writeLine(moduleSrc, "\tvoid registerModules()");
		writeLine(moduleSrc, "\t{");
		vector<Module*>::type* allModules = Module::getAllModules();
		if (allModules)
		{
			for (Module* module : *allModules)
			{
				if (module->isEnable() && !module->isEditorOnly())
					writeLine(moduleSrc, StringUtil::Format("\t\tREGISTER_MODULE(%s)", module->getClassName().c_str()));
			}
		}

		// end namespace
		writeLine(moduleSrc, "\t}\n}\n");

		// Write to file
		String savePath = m_outputDir + "app/windows/Config/ModuleConfig.cpp";
		FileHandleDataStream stream(savePath, DataStream::WRITE);
		if (!stream.fail())
		{
			stream.write(moduleSrc.data(), moduleSrc.size());
			stream.close();
		}
	}
}

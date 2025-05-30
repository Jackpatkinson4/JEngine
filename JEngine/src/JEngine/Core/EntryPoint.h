#pragma once
#include "JEngine/Core/Core.h"

#ifdef JE_PLATFORM_WINDOWS

extern JEngine::Application* JEngine::CreateApplication();

int main(int argc, char** argv)
{
	JEngine::Log::Init();

	JE_PROFILE_BEGIN_SESSION("Startup", "JEngineProfile-Startup.json");
	auto app = JEngine::CreateApplication();
	JE_PROFILE_END_SESSION();

	JE_PROFILE_BEGIN_SESSION("Runtime", "JEngineProfile-Runtime.json");
	app->Run();
	JE_PROFILE_END_SESSION();

	JE_PROFILE_BEGIN_SESSION("Shutdown", "JEngineProfile-Shutdown.json");
	delete app;
	JE_PROFILE_END_SESSION();
}

#endif
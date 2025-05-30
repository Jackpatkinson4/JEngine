#pragma once

#ifdef JE_PLATFORM_WINDOWS

extern JEngine::Application* JEngine::CreateApplication();

int main(int argc, char** argv)
{
	JEngine::Log::Init();
	JE_CORE_WARN("Initialized Log");

	auto app = JEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif
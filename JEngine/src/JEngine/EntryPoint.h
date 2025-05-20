#pragma once

#ifdef JE_PLATFORM_WINDOWS

extern JEngine::Application* JEngine::CreateApplication();

int main(int argc, char** argv)
{
	JEngine::Log::Init();
	JE_CORE_WARN("Initialized Log");
	int a = 8;
	JE_INFO("Initialized Log Var={0}", a);

	auto app = JEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif
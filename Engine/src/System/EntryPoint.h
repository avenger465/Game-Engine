#pragma once
#include "Log.h"

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();

	auto app = Engine::CreateApplication();
	app->run();
	delete app;

	ENGINE_CORE_INFO("Closing down Engine");

	return 0;
}

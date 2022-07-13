#pragma once
#include "Log.h"
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();

	auto app = Engine::CreateApplication();
	app->run();
	delete app;

	E_CORE_INFO("Closing down Engine");
}

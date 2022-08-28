#include <Engine.h>

namespace Engine
{
	class Main : public Application
	{
	public:
		Main()
		{

		}
		~Main()
		{
		
		}
	};

	Application* CreateApplication()
	{
		APP_CORE_INFO("Creating Application");
		return new Main();
	}
}

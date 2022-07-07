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
		return new Main();
	}
}

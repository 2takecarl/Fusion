#pragma once
#include "Application.h"

namespace Fusion
{
	class Entrypoint
	{
	public:
		static void Main()
		{
			Application* app = new Application;
			app->run();
		}
	};
}
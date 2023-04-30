#pragma once
#include <glm.hpp>
#include <string>

namespace Fusion
{
	class GameObject
	{
	public:
		struct Object
		{
			glm::vec3 Position;
			glm::vec3 Rotation;
			glm::vec3 Scale;
			int objIndex;
			bool selected;
			const char* name;
		};
	};
}
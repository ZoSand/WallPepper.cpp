//
// Created by ZoSand on 09/01/2023.
//

#ifndef PEPPER_SHARED_VERTEX_H
#define PEPPER_SHARED_VERTEX_H

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace Pepper::Core
{

	class Vertex
	{
	private:
		friend class Drawable;
		//TODO: add OpenGLEngine friend class
		friend class VulkanEngine;

		glm::vec2 position;
		glm::vec3 color;

	public:
		Vertex(
				glm::vec2 _position,
				glm::vec3 _color
		      );
	};

} // Pepper::Core

#endif //PEPPER_SHARED_VERTEX_H

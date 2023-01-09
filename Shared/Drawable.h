//
// Created by ZoSand on 09/01/2023.
//

#ifndef PEPPER_SHARED_DRAWABLE_H
#define PEPPER_SHARED_DRAWABLE_H

#include "ConstantDefinitions.h"
#include "Vertex.h"

#include <vector>

namespace Pepper::Core
{

	class Drawable
	{
	private:
		std::vector<Vertex> m_vertices;
	public:
		Drawable();
		NO_DISCARD_UNUSED std::vector<Vertex> GetVertices() const;
	};

} // Pepper::Shared

#endif //PEPPER_SHARED_DRAWABLE_H

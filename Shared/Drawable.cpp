//
// Created by ZoSand on 09/01/2023.
//

#include "Drawable.h"

Pepper::Core::Drawable::Drawable()
		: m_vertices({
				             {{ 0.0f,  -0.5f }, { 1.0f, 1.0f, 1.0f }},
				             {{ 0.5f,  0.5f },  { 1.0f, 1.0f, 1.0f }},
				             {{ -0.5f, 0.5f },  { 0.0f, 0.0f, 1.0f }}
		             })
{

}

std::vector<Pepper::Core::Vertex> Pepper::Core::Drawable::GetVertices() const
{
	return m_vertices;
}

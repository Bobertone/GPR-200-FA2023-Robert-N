#pragma once
#include "../ew/mesh.h"
namespace bob 
{
	ew::MeshData createPlane(float width, float height, int subdivisions);
	ew::MeshData createSphere(float radius, int numSegments);
	ew::MeshData createCylinder(float height, float radius, int numSegments);
}

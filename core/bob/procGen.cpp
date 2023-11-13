#include "procGen.h"

namespace bob {
	ew::MeshData createPlane(float width, float height, int subdivisions)
	{
		ew::Vertex v;
		ew::MeshData meshData;
		//unsigned int startVertex = meshData->vertices.size();
		for (int row = 0; row <= subdivisions; row++)
		{
			for (int col = 0; col <= subdivisions; col++)
			{
				v.pos.x = width * ((float)col / subdivisions);
				v.pos.z = -height * ((float)row / subdivisions);
				v.pos.y = 0;
				v.normal = ew::Vec3(0, 1, 0);
				v.uv = ew::Vec2(((float)col / subdivisions), ((float)row / subdivisions));
				meshData.vertices.push_back(v);
			}
		}

		int columns = subdivisions + 1;
		for (int row = 0; row < subdivisions; row++)
		{
			for (int col = 0; col < subdivisions; col++)
			{
				int start = row * columns + col;
				//Bottom right triangle
				meshData.indices.push_back(start);
				meshData.indices.push_back(start + 1);
				meshData.indices.push_back(start + columns + 1);
				//Top left triangle
				meshData.indices.push_back(start);
				meshData.indices.push_back(start + columns + 1);
				meshData.indices.push_back(start + columns);
			}
		}
		return meshData;
	}

	ew::MeshData createSphere(float radius, int numSegments)
	{
		//SPHERE VERTICES
		ew::Vertex v;
		ew::MeshData meshData;
		float thetaStep = 2*ew::PI / numSegments;
		float phiStep = ew::PI / numSegments;
		for (int row = 0; row <= numSegments; row++) 
		{
			//First and last row converge at poles
			float phi = row * phiStep;
			for (int col = 0; col <= numSegments; col++) //Duplicate column for each row
			{
				float theta = col * thetaStep;
				v.pos.x = radius * cos(theta) * sin(phi);
				v.pos.y = radius * cos(phi);
				v.pos.z = radius * sin(theta) * sin(phi);
				v.normal = ew::Normalize(ew::Vec3(cos(theta) * sin(phi), cos(phi), sin(theta) * sin(phi)));
				v.uv = ew::Vec2(((float)col / numSegments), ((1 - (float)row / numSegments)));
				meshData.vertices.push_back(v);
			} 
		}
		//SPHERE INDICES
		int columns = numSegments + 1;
		for (int row = 0; row < numSegments; row++)
		{
			for (int col = 0; col < numSegments; col++)
			{
				int start = row * columns + col;
				//Triangle 1
				meshData.indices.push_back(start);
				meshData.indices.push_back(start + 1);
				meshData.indices.push_back(start + columns);
				//Triangle 2
				meshData.indices.push_back(start + columns);
				meshData.indices.push_back(start + 1);
				meshData.indices.push_back(start + columns + 1);
			}
		}
		return meshData;
	}

	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		ew::Vertex v;
		ew::MeshData meshData;
		float thetaStep = 2 * ew::PI / numSegments;

		//CYLINDER VERTICES

		float topY = height / 2; //y = 0 is centered
		float bottomY = -topY;
		
		#pragma region ring 1 top vertices
			//Top center - Add top vertex(0, topY, 0);
			v.pos = ew::Vec3(0, topY, 0);
			v.normal = ew::Vec3(0, 1, 0); //up
			v.uv = ew::Vec2(0.5f, 0.5f);
			meshData.vertices.push_back(v);
			//Top ring - Add top ring vertices
			for (int i = 0; i <= numSegments; i++)
			{
				float theta = i * thetaStep;
				float x = cos(theta) * radius;
				float y = topY;
				float z = sin(theta) * radius;
				v.pos = ew::Vec3(x, y, z);
				v.normal = ew::Vec3(0, 1, 0); //up
				v.uv = ew::Vec2((x+1)/2,(z+1)/2);
				meshData.vertices.push_back(v);
			}
			float sideStart = meshData.vertices.size();
		#pragma endregion

		#pragma region ring 1 side vertices
			//Top ring - Add top ring vertices
			for (int i = 0; i <= numSegments; i++)
			{
				float theta = i * thetaStep;
				float x = cos(theta) * radius;
				float y = topY;
				float z = sin(theta) * radius;
				v.pos = ew::Vec3(x, y, z);
				v.normal = ew::Normalize(ew::Vec3(cos(theta), 0, sin(theta))); //out
				v.uv = ew::Vec2(((float)i / numSegments), 1);
				meshData.vertices.push_back(v);
			}
		#pragma endregion

		#pragma region ring 2 side vertices
			//Bottom Ring - Add bottom ring vertices
			for (int i = 0; i <= numSegments; i++)
			{
				float theta = i * thetaStep;
				float x = cos(theta) * radius;
				float y = bottomY;
				float z = sin(theta) * radius;
				v.pos = ew::Vec3(x, y, z);
				v.normal = ew::Normalize(ew::Vec3(cos(theta), 0, sin(theta))); //out
				v.uv = ew::Vec2(((float)i / numSegments), 0);
				meshData.vertices.push_back(v);
			}
			float sideEnd = meshData.vertices.size();
		#pragma endregion

		#pragma region ring 2 bottom vertices
			//Bottom Ring - Add bottom ring vertices
			for (int i = 0; i <= numSegments; i++)
			{
				float theta = i * thetaStep;
				float x = cos(theta) * radius;
				float y = bottomY;
				float z = sin(theta) * radius;
				v.pos = ew::Vec3(x, y, z);
				v.normal = ew::Vec3(0, 1, 0); //up
				v.uv = ew::Vec2((x + 1) / 2, (z + 1) / 2);
				meshData.vertices.push_back(v);
			}
			//Bottom Center - Add bottom vertex(0, bottomY, 0)
			v.pos = ew::Vec3(0, bottomY, 0);
			v.normal = ew::Vec3(0, -1, 0); //down
			v.uv = ew::Vec2(0.5f, 0.5f);
			meshData.vertices.push_back(v);
		#pragma endregion

		//CYLINDER INDICES
		
		#pragma region ring 1 top indices
			//Uses a triangle fan to connect ring to center
			float start = 1;//Index of first ring vertex
			float center = 0;//Index of center vertex
			for (float i = 0; i <= numSegments; i++) 
			{
				meshData.indices.push_back(start + i);
				meshData.indices.push_back(center);
				meshData.indices.push_back(start + i + 1);
			}
		#pragma endregion

		#pragma region side indices
			//Assumes bottom ring vertices have been added immediately after top ring
			float columns = numSegments + 1;
			for (int i = 0; i < columns; i++) 
			{
				start = sideStart + i;
				//Triangle 1
				meshData.indices.push_back(start);
				meshData.indices.push_back(start + 1);
				meshData.indices.push_back(start + columns);
				//Triangle 2
				meshData.indices.push_back(start + 1);
				meshData.indices.push_back(start + columns + 1);
				meshData.indices.push_back(start + columns);
			}
		#pragma endregion

		#pragma region ring 2 bottom indices
			//Uses a triangle fan to connect ring to center
			start = sideEnd;//Index of first ring vertex
			center = meshData.vertices.size() - 1;//Index of center vertex
			for (float i = 0; i <= numSegments; i++)
			{
				meshData.indices.push_back(center);
				meshData.indices.push_back(start + i);
				meshData.indices.push_back(start + i + 1);
			}
		#pragma endregion

		return meshData;
	}
}
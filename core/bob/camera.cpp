#include "camera.h"
#include "../bob/transformations.h"
namespace bob {

	Camera::Camera(float ratio)
	{
		position = ew::Vec3(0, 0, 0); //Camera body position
		target = ew::Vec3(0, 0, 0); //Position to look at
		fov = 60; //Vertical field of view in degrees
		aspectRatio = ratio; //Screen width / Screen height
		nearPlane = .1f; //Near plane distance (+Z)
		farPlane = 100.0f; //Far plane distance (+Z)
		orthographic = false; //Perspective or orthographic?
		orthoSize = 6.0f; //Height of orthographic frustum
	}

	ew::Mat4 Camera::ViewMatrix()
	{
		return LookAt(position, target, ew::Vec3(0, 1, 0));
	}

	ew::Mat4 Camera::ProjectionMatrix()
	{
		if (orthographic)
		{
			 return Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
		}
		else
		{
			float fovRadians = fov * (3.1415f / 180.0f);
			return Perspective(fovRadians, aspectRatio, nearPlane, farPlane);
		}
	}

}
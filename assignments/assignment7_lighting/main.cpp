#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

const int MAX_LIGHTS = 4;
int lightCount = MAX_LIGHTS;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
	ew::Vec3 position; //World space
	ew::Vec3 color; //RGB
};

struct Material {
	float ambientK; //Ambient coefficient (0-1)
	float diffuseK; //Diffuse coefficient (0-1)
	float specularK; //Specular coefficient (0-1)
	float shininess; //Shininess
};


int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");

	Material material {
		material.ambientK = 1.0f, //Ambient coefficient (0-1)
		material.diffuseK = 1.0f, //Diffuse coefficient (0-1)
		material.specularK = 1.0f, //Specular coefficient (0-1)
		material.shininess = 15.0f //Shininess
	};

	//Create Shapes
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	//Create Material
	Material mat;

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	resetCamera(camera,cameraController);

	ew::Mesh lightSphereMesh = ew::createSphere(0.5f, 20);

	ew::Transform lightTransforms[MAX_LIGHTS];

	//Create lights
	Light lights[MAX_LIGHTS]
	{
		lights[0].position = ew::Vec3(5.0f, 2.0f,7.0f),
		lights[0].color = ew::Vec3(0.5f, 0.0f, 0.0f),

		lights[1].position = ew::Vec3(5.0f, 2.0f,-7.0f),
		lights[1].color = ew::Vec3(0.0f, 0.5f, 0.0f),

		lights[2].position = ew::Vec3(-5.0f, 2.0f,0.0f),
		lights[2].color = ew::Vec3(0.0f, 0.0f, 0.5f),

		lights[3].position = ew::Vec3(0.0f, 6.0f,0.0f),
		lights[3].color = ew::Vec3(0.15f, 0.15f, 0.15f)
	};

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();

		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		
		shader.setFloat("_ambientK", material.ambientK);
		shader.setFloat("_diffuseK", material.diffuseK);
		shader.setFloat("_specularK", material.specularK);
		shader.setFloat("_shininess", material.shininess);
		shader.setVec3("_camPosition", camera.position);
		shader.setInt("_lightCount", lightCount);
		
		//Set light pos & color
		shader.setVec3("_Lights[0].position", lights[0].position);
		shader.setVec3("_Lights[0].color", lights[0].color);

		shader.setVec3("_Lights[1].position", lights[1].position);
		shader.setVec3("_Lights[1].color", lights[1].color);

		shader.setVec3("_Lights[2].position", lights[2].position);
		shader.setVec3("_Lights[2].color", lights[2].color);

		shader.setVec3("_Lights[3].position", lights[3].position);
		shader.setVec3("_Lights[3].color", lights[3].color);

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//Render point lights
		for(int i = 0; i < lightCount; i++)
		{
			unlitShader.use();
			unlitShader.setVec3("_Color", lights[i].color);
			unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
			
			lightTransforms[i].position = lights[i].position;
			unlitShader.setMat4("_Model", lightTransforms[i].getModelMatrix());
			lightSphereMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
			
			ImGui::SliderInt("Number of Lights", &lightCount, 0, MAX_LIGHTS);
			/*
			Material material{
				material.ambientK = 1.0f, //Ambient coefficient (0-1)
				material.diffuseK = 1.0f, //Diffuse coefficient (0-1)
				material.specularK = 1.0f, //Specular coefficient (0-1)
				material.shininess = 15.0f //Shininess
			};
			*/
			ImGui::SliderFloat("Ambient", &material.ambientK, 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse", &material.diffuseK, 0.0f, 1.0f);
			ImGui::SliderFloat("Specular", &material.specularK, 0.0f, 1.0f);
			ImGui::SliderFloat("Shininess", &material.shininess, 0.0f, 255.0f);
			
			if (ImGui::CollapsingHeader("Lights")) {
				for (size_t i = 0; i < MAX_LIGHTS; i++)
				{
					ImGui::PushID(i);
					if (ImGui::CollapsingHeader(("Light "+std::to_string(i+1)).c_str())) {
						ImGui::DragFloat3("Position", &lights[i].position.x, 0.05f);
						ImGui::ColorEdit3("Color", &lights[i].color.x);
					}
					ImGui::PopID();
				}
			};

			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}



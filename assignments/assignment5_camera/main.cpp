#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <bob/camera.h>

//Camera aiming related variables
struct CameraControls {
	double prevMouseX, prevMouseY; //Mouse position from previous frame
	float yaw = 0, pitch = 0; //Degrees
	float mouseSensitivity = 0.1f; //How fast to turn with mouse
	bool firstMouse = true; //Flag to store initial mouse position
	float moveSpeed = 5.0f; //How fast to move with arrow keys (M/S)
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void moveCamera(GLFWwindow* window, bob::Camera* camera, CameraControls* controls, float dt);
double clamp(double value, double min, double max);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 8;
const float NUM_CUBES_F = 8.0f;
ew::Transform cubeTransforms[NUM_CUBES];
bool zPositive;
bool xPositive;

ew::Vec3 main_position = ew::Vec3(0,0,5); //Camera body position
ew::Vec3 main_target = ew::Vec3(0, 0, 0); //Position to look at
float main_fov = 60; //Vertical field of view in degrees
float main_aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT; //Screen width / Screen height
float main_nearPlane = .1f; //Near plane distance (+Z)
float main_farPlane = 100.0f; //Far plane distance (+Z)
bool main_orthographic = false; //Perspective or orthographic?
float main_orthoSize = 6.0f; //Height of orthographic frustum

float sensitivity = 1.0f;

float prevTime; //Timestamp of previous frame

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	CameraControls ccontrols;

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));
	//camera creation
	bob::Camera cam((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		//cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		//cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
		if (i % 2 == 0) 
		{
			if (zPositive){ 
				cubeTransforms[i].position.z = 3; 
				zPositive = false; }
			else{ 
				cubeTransforms[i].position.z = -3; 
				zPositive = true;}
		}
		else 
		{
			if (xPositive) {
				cubeTransforms[i].position.x = 3;
				xPositive = false;
			}
			else {
				cubeTransforms[i].position.x = -3;
				xPositive = true;
			}
		}
		if(i < NUM_CUBES/2.0f)
		{
			cubeTransforms[i].position.y = i/(NUM_CUBES_F/2.0f) +.75f;
		}
		else
		{
			cubeTransforms[i].position.y = -1*(((i - (NUM_CUBES_F / 2))/ (NUM_CUBES_F/2.0f))+.75f);
		}
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//Calculate deltaTime
		float time = (float)glfwGetTime(); //Timestamp of current frame
		float deltaTime = time - prevTime;
		prevTime = time;


		moveCamera(window, &cam, &ccontrols, deltaTime);
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		shader.use();

		//Update Camera Variables
		//cam.position = main_position;
		//cam.target = main_target;
		cam.fov = main_fov;
		cam.aspectRatio = main_aspectRatio;
		cam.nearPlane = main_nearPlane;
		cam.farPlane = main_farPlane;
		cam.orthographic = main_orthographic;
		cam.orthoSize = main_orthoSize;

		//Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());

			//Cam Projection
			shader.setMat4("_View", cam.ViewMatrix());
			shader.setMat4("_Projection", cam.ProjectionMatrix());

			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if(ImGui::CollapsingHeader("Cubes")) {
				for (size_t i = 0; i < NUM_CUBES; i++)
				{
					ImGui::PushID(i);
					if (ImGui::CollapsingHeader("Transform")) {
						ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
						ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
						ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
					}
					ImGui::PopID();
				}
			};
			
			//Was not able to do all the imgui features described in Asn5 doc. Accessing my cams transform was the main issue.
			
			ImGui::Text("Camera");
			//ImGui::DragFloat3("Position", &main_position.x, 0.05f);
			//ImGui::DragFloat3("Target", &main_target.x, 0.05f);
			//ImGui::Text("Vector Values: %.2f, %.2f, %.2f", vectorToReset.x, vectorToReset.y, vectorToReset.z);
			ImGui::Checkbox("Orthographic", &main_orthographic);
			ImGui::DragFloat("FOV", &main_fov, 0.05f);
			ImGui::DragFloat("Near Plane", &main_nearPlane, 0.05f);
			ImGui::DragFloat("Far Plane", &main_farPlane, 0.05f);
			ImGui::DragFloat("Sensitivity", &sensitivity, 0.001f);
			//if (ImGui::Button("Reset Cam Pos")) { &cam->position = ew::Vec3(0, 0, 0); }
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
}

void moveCamera(GLFWwindow* window, bob::Camera* camera, CameraControls* controls, float dt) 
{
	//If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}
	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	//If we just started right clicking, set prevMouse values to current position.
	//This prevents a bug where the camera moves as soon as we click.
	if (controls->firstMouse) {
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	//DONE: Get mouse position delta for this frame
	double mXdelta = mouseX - controls->prevMouseX;
	double mYdelta = mouseY - controls->prevMouseY;

	//DONE: Add to yaw and pitch
	controls->yaw += mXdelta*sensitivity;
	controls->pitch += mYdelta* sensitivity;

	//DONE: Clamp pitch between -89 and 89 degrees
	controls->pitch = clamp(controls->pitch, -89, 89);

	//Remember previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	//Construct forward vector using yaw and pitch. Don't forget to convert to radians!
	
	ew::Vec3 forward(
		cos(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch)),
		-sin(ew::Radians(controls->pitch)),
		sin(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch))
	);
	/*
	ew::Vec3 forward(
		sin(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch)),
		-sin(ew::Radians(controls->pitch)),
		-cos(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch))
	);
	*/
	//TODO: Using camera forward and world up (0,1,0), construct camera right and up vectors. Graham-schmidt process!
	ew::Vec3 right = ew::Normalize(ew::Cross(ew::Vec3(0,1,0), forward));
	ew::Vec3 up = ew::Cross(forward, right);
	
	//TODO: Keyboard controls for moving along forward, back, right, left, up, and down. See Requirements for key mappings.
	//EXAMPLE: Moving along forward axis if W is held.
	//Note that this is framerate dependent, and will be very fast until you scale by deltaTime. See the next section.
	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera->position += forward * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->position -= forward * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->position += right * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->position -= right * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera->position += up * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera->position -= up * controls->moveSpeed * dt;
	}
	//Setting camera.target should be done after changing position. Otherwise, it will use camera.position from the previous frame and lag behind
	camera->target = camera->position + forward;
}

double clamp(double value, double min, double max)
{
	if (value < min) { value = min; }
	if (value > max) { value = max; }
	return value;
}
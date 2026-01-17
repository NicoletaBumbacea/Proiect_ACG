#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "SkyBox.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"
#include "imgui\imgui_impl_opengl3.h"

void processKeyboardInput();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
Mesh generateWaterGrid(int size, float spacing, std::vector<Texture> tex);
Mesh generateCircularRiver(float radius, float width, int segments, std::vector<Texture> tex);
float lerpAngle(float start, float end, float speed, float dt);

Window window("Game Engine", 800, 800);
Camera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


bool firstMouse = true;
float lastX = 400, lastY = 400;

bool isCursorLocked = true;
bool showTaskWindow = true;

const float MODEL_CORRECTION_ANGLE = -90.0f;
glm::vec3 catPosition = glm::vec3(0.0f, -10.0f, 0.0f);
float catRotationY = 180.0f;
float targetCatRotation = 180.0f;

bool hasFishingRod = false;
bool eKeyPressedLastFrame = false;
glm::vec3 rodWorldPos = glm::vec3(190.0f, -20.0f, -195.0f);

bool isDoorOpen = false;
glm::vec3 cabinPos = glm::vec3(205.0f, -20.0f, -70.0f);

glm::vec3 interactionPoint = glm::vec3(205.0f, -15.0f, -70.0f);
float interactionRadius = 15.0f;
bool isMenuOpen = false;
GLFWwindow* extraWindow = nullptr;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

bool isFishing = false;

bool isTransitioning = false;
bool isZoomingIn = false; 
float transitionTimer = 0.0f;
float transitionDuration = 1.0f; 
glm::vec3 startTransitionPos;

// fish structure
struct FishData {
	glm::vec3 position;
	glm::vec3 target;
	float speed;
};


float lerpAngle(float current, float target, float speed, float dt) {
	float diff = target - current;
	while (diff > 180.0f) diff -= 360.0f;
	while (diff < -180.0f) diff += 360.0f;
	return current + diff * speed * dt;
}

bool checkNearWater(glm::vec3 pos) {
	glm::vec3 riverCenter(-120.0f, -18.6f, -85.0f);
	if (glm::distance(pos, riverCenter) < 95.0f && glm::distance(pos, riverCenter) > 10.0f) return true;

	glm::vec3 oceanCenter(200.0f, -18.60f, 120.0f);
	if (glm::distance(pos, oceanCenter) < 70.0f) return true;

	return false;
}

glm::vec3 getRandomRiverPoint() {
	glm::vec3 center = glm::vec3(-120.0f, -18.6f, -85.0f);
	float minRadius = 15.0f;
	float maxRadius = 90.0f;
	float angle = (rand() % 360) * (3.14159f / 180.0f);
	float radius = minRadius + (rand() % (int)(maxRadius - minRadius));
	float x = center.x + cos(angle) * radius;
	float z = center.z + sin(angle) * radius;
	float y = center.y - 2.0f;
	return glm::vec3(x, y, z);
}

glm::vec3 getRandomOceanPoint() {
	glm::vec3 center = glm::vec3(200.0f, -18.60f, 120.0f);
	float minRadius = 10.0f;
	float maxRadius = 65.0f;
	float angle = (rand() % 360) * (3.14159f / 180.0f);
	float radius = minRadius + (rand() % (int)(maxRadius - minRadius));
	float x = center.x + cos(angle) * radius;
	float z = center.z + sin(angle) * radius;
	float y = center.y - 2.0f;
	return glm::vec3(x, y, z);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!isCursorLocked) return;
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.processMouseInput(xoffset, yoffset);
}

int main()
{
	glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window.getWindow(), mouse_callback);

	glClearColor(0.2f, 0.8f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 10.0f;
	style.FrameRounding = 5.0f;
	style.PopupRounding = 5.0f;
	style.ScrollbarRounding = 10.0f;
	style.GrabRounding = 5.0f;

	// colors GUI screen
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.15f, 0.85f); 
	colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.4f, 1.0f);   
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.6f, 1.0f);   
	colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.2f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.7f, 0.3f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.4f, 0.1f, 1.0f);
	colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Load shaders
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
	Shader waterShader("Shaders/water_vertex_shader.glsl", "Shaders/water_fragment_shader.glsl");
	Shader riverShader("Shaders/river_vertex_shader.glsl", "Shaders/river_fragment_shader.glsl");
	Shader skyboxShader("Shaders/skybox_vertex.glsl", "Shaders/skybox_fragment.glsl");

	// skybox setup
	Skybox mySkybox;
	mySkybox.setup();
	std::vector<std::string> faces = {
		"Resources/Textures/Skybox/px.png",
		"Resources/Textures/Skybox/nx.png",
		"Resources/Textures/Skybox/py.png",
		"Resources/Textures/Skybox/ny.png",
		"Resources/Textures/Skybox/nz.png",
		"Resources/Textures/Skybox/pz.png"
	};
	mySkybox.textureID = mySkybox.loadCubemap(faces);
	skyboxShader.use();
	glUniform1i(glGetUniformLocation(skyboxShader.getId(), "skybox"), 0);

	// Load textures
	GLuint tex = loadBMP("Resources/Textures/wood.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/watah.bmp");
	GLuint tex3 = loadBMP("Resources/Textures/orange.bmp");
	GLuint tex4 = loadBMP("Resources/Textures/cat.bmp");
	GLuint tex5 = loadBMP("Resources/Textures/sand.bmp");
	GLuint skyTexID = loadBMP("Resources/Textures/sky.bmp");
	GLuint boatTexID = loadBMP("Resources/Textures/boat_color.bmp");
	GLuint reedTexID = loadBMP("Resources/Textures/reed.bmp");
	GLuint treeTexID = loadBMP("Resources/Textures/forrest.bmp");
	GLuint fishTexID = loadBMP("Resources/Textures/fih.bmp");
	GLuint hammockTex = loadBMP("Resources/Textures/hammock_tex.bmp");
	GLuint hammockTreeTex = loadBMP("Resources/Textures/hammockTree_tex.bmp");
	GLuint fishingRodTex = loadBMP("Resources/Textures/fishingRod.bmp");
	GLuint cabinBodyTex = loadBMP("Resources/Textures/brightwood.bmp");
	GLuint cabinRoofTex = loadBMP("Resources/Textures/darkwood.bmp");
	GLuint cabinFoundationTex = loadBMP("Resources/Textures/rock_cabin.bmp");
	GLuint cabinDoorTex = loadBMP("Resources/Textures/brightwood.bmp");
	GLuint frogTex = loadBMP("Resources/Textures/masterfrog.bmp");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture vectors
	std::vector<Texture> textures2; textures2.push_back({ tex2, "texture_diffuse" });
	std::vector<Texture> textures3; textures3.push_back({ tex3, "texture_diffuse" });
	std::vector<Texture> textures4; textures4.push_back({ tex4, "texture_diffuse" });
	std::vector<Texture> textures5; textures5.push_back({ tex5, "texture_diffuse" });
	std::vector<Texture> boatTextures; boatTextures.push_back({ boatTexID, "texture_diffuse" });
	std::vector<Texture> reedTextures; reedTextures.push_back({ reedTexID, "texture_diffuse" });
	std::vector<Texture> treeTextures; treeTextures.push_back({ treeTexID, "texture_diffuse" });
	std::vector<Texture> fishTextures; fishTextures.push_back({ fishTexID, "texture_diffuse" });
	std::vector<Texture> hammockTexture; hammockTexture.push_back({ hammockTex, "texture_diffuse" });
	std::vector<Texture> hammockWoodTexture; hammockWoodTexture.push_back({ hammockTreeTex, "texture_diffuse" });
	std::vector<Texture> fishingRodTextures; fishingRodTextures.push_back({ fishingRodTex, "texture_diffuse" });
	std::vector<Texture> cabinTexture; cabinTexture.push_back({ cabinBodyTex, "texture_diffuse" });
	std::vector<Texture> cabinFoundationTexture; cabinFoundationTexture.push_back({ cabinFoundationTex, "texture_diffuse" });
	std::vector<Texture> cabinRoofTexture; cabinRoofTexture.push_back({ cabinRoofTex, "texture_diffuse" });
	std::vector<Texture> cabinDoorTexture; cabinDoorTexture.push_back({ cabinDoorTex, "texture_diffuse" });
	std::vector<Texture> frogTexture; frogTexture.push_back({ frogTex, "texture_diffuse" });


	// Load models
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj", textures3);
	Mesh plane = loader.loadObj("Resources/Models/plane.obj", textures5);
	Mesh cat = loader.loadObj("Resources/Models/cat.obj", textures4);
	Mesh waterMesh = generateWaterGrid(120, 1.0f, textures2);
	Mesh riverMesh = generateCircularRiver(50.0f, 100.0f, 100, textures2);
	Mesh boat = loader.loadObj("Resources/Models/boat.obj", boatTextures);
	Mesh reed = loader.loadObj("Resources/Models/reed.obj", reedTextures);
	Mesh tree = loader.loadObj("Resources/Models/bigtree.obj", treeTextures);
	Mesh fish = loader.loadObj("Resources/Models/fih.obj", fishTextures);
	Mesh hammock = loader.loadObj("Resources/Models/hammond.obj", hammockTexture);
	Mesh hammockTrees = loader.loadObj("Resources/Models/cocotierul_vincent.obj", hammockWoodTexture);
	Mesh fishingRod = loader.loadObj("Resources/Models/fishingRod.obj", fishingRodTextures);
	Mesh cabin = loader.loadObj("Resources/Models/cabin.obj", cabinTexture);
	Mesh cabinFoundation = loader.loadObj("Resources/Models/foundation.obj", cabinFoundationTexture);
	Mesh cabinRoof = loader.loadObj("Resources/Models/roof.obj", cabinRoofTexture);
	Mesh cabinDoor = loader.loadObj("Resources/Models/door.obj", cabinDoorTexture);
	Mesh frog = loader.loadObj("Resources/Models/masterfrog.obj", frogTexture);


	std::vector<FishData> schoolOfFish;
	for (int i = 0; i < 5; i++) {
		FishData f;
		f.position = getRandomRiverPoint();
		f.target = getRandomRiverPoint();
		f.speed = 10.0f + (rand() % 10);
		schoolOfFish.push_back(f);
	}
	std::vector<FishData> schoolOfOceanFish;
	for (int i = 0; i < 10; i++) {
		FishData f;
		f.position = getRandomOceanPoint();
		f.target = getRandomOceanPoint();
		f.speed = 8.0f + (rand() % 5);
		schoolOfOceanFish.push_back(f);
	}

	float titleUpdateTimer = 0.0f;

	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showTaskWindow)
		{
			ImGui::Begin("Quest Log");
			ImGui::Text("Main Story: The Cat's Adventure");
			if (ImGui::Button("Close Menu")) {
				showTaskWindow = false;
			}
			ImGui::End();
		}

		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processKeyboardInput();
		catRotationY = lerpAngle(catRotationY, targetCatRotation, 10.0f, deltaTime);

		glm::vec3 firstPersonPos = catPosition + glm::vec3(0.0f, 8.0f, 0.0f);

		camera.updateCameraPosition(catPosition);
		glm::vec3 thirdPersonPos = camera.getCameraPosition();

		if (isTransitioning) {
			transitionTimer += deltaTime;
			float t = transitionTimer / transitionDuration;

			t = t * t * (3.0f - 2.0f * t);

			if (transitionTimer >= transitionDuration) {
				isTransitioning = false;
				isFishing = isZoomingIn;

				if (isFishing) camera.setCameraPosition(firstPersonPos);
				else camera.updateCameraPosition(catPosition);
			}
			else {
				glm::vec3 currentPos;

				if (isZoomingIn) {
					currentPos = glm::mix(startTransitionPos, firstPersonPos, t);
				}
				else {
					currentPos = glm::mix(startTransitionPos, thirdPersonPos, t);
				}

				camera.setCameraPosition(currentPos);
			}
		}
		else if (isFishing) {
			camera.setCameraPosition(firstPersonPos);
		}

		glm::mat4 ViewMatrix = camera.getViewMatrix();
		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);

		// tracker logic
		titleUpdateTimer += deltaTime;
		if (titleUpdateTimer > 0.1f) {
			glm::vec3 pos = camera.getCameraPosition();
			float distToRod = glm::length(catPosition - rodWorldPos);
			std::stringstream ss;
			ss << std::fixed << std::setprecision(1);
			ss << "Pos: " << (int)pos.x << "," << (int)pos.z
				<< " | Rod Dist: " << distToRod
				<< " | Holding: " << (hasFishingRod ? "YES" : "NO");
			glfwSetWindowTitle(window.getWindow(), ss.str().c_str());
			titleUpdateTimer = 0.0f;
		}

		// draw water
		waterShader.use();
		glm::vec3 waterPosition = glm::vec3(200.0f, -20.0f, 120.0f);
		glm::mat4 WaterModel = glm::translate(glm::mat4(1.0), waterPosition);
		glm::mat4 WaterMVP = ProjectionMatrix * ViewMatrix * WaterModel;
		glUniformMatrix4fv(glGetUniformLocation(waterShader.getId(), "MVP"), 1, GL_FALSE, &WaterMVP[0][0]);
		glUniform1f(glGetUniformLocation(waterShader.getId(), "time"), (float)glfwGetTime());
		glUniform3f(glGetUniformLocation(waterShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(waterShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(waterShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		waterMesh.draw(waterShader);

		// draw river
		riverShader.use();
		glm::vec3 riverPos = glm::vec3(-120.0f, -19.5f, -85.0f);
		glm::mat4 RiverModel = glm::translate(glm::mat4(1.0), riverPos);
		glm::mat4 RiverMVP = ProjectionMatrix * ViewMatrix * RiverModel;
		glUniformMatrix4fv(glGetUniformLocation(riverShader.getId(), "MVP"), 1, GL_FALSE, &RiverMVP[0][0]);
		glUniform1f(glGetUniformLocation(riverShader.getId(), "time"), (float)glfwGetTime());
		glUniform3f(glGetUniformLocation(riverShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(riverShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(riverShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		riverMesh.draw(riverShader);


		shader.use();
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

		// draw plane
		glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -20.0f, 0.0f));
		float planeScale = 3.0f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(planeScale, planeScale, planeScale));
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		plane.draw(shader);


		// draw masterfrog
		 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -20.0f, -50.0f));
		float frogScale = 40.0f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(frogScale, frogScale, frogScale));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		frog.draw(shader);

		// draw boat
		ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-28.0f, -20.0f, -56.0f));
		float boatScale = 0.1f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(boatScale, boatScale, boatScale));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		boat.draw(shader);

		// fishing rod
		ModelMatrix = glm::mat4(1.0);
		if (hasFishingRod) {
			if (isFishing && !isTransitioning) {
				// first person
				glm::mat4 invView = glm::inverse(ViewMatrix);
				ModelMatrix = invView;
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.5f, -1.5f, -2.5f));
				ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-10.0f), glm::vec3(1, 0, 0));
				ModelMatrix = glm::rotate(ModelMatrix, glm::radians(170.0f), glm::vec3(0, 1, 0));
			}
			else {
				// thrid person
				ModelMatrix = glm::translate(ModelMatrix, catPosition);
				ModelMatrix = glm::rotate(ModelMatrix, catRotationY, glm::vec3(0.0f, 1.0f, 0.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.0f, 10.0f, 1.5f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -10.0f, -5.0f));
			}
		}
		else {
			// on ground logic
			ModelMatrix = glm::translate(ModelMatrix, rodWorldPos);
			float spinSpeed = 50.0f;
			float spinAngle = (float)glfwGetTime() * spinSpeed;
			ModelMatrix = glm::rotate(ModelMatrix, (spinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
			float floatHeight = sin((float)glfwGetTime() * 2.0f) * 1.0f;
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, floatHeight + 3.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		float fishingRodScale = 1.5f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(fishingRodScale, fishingRodScale, fishingRodScale));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		fishingRod.draw(shader);

		// draw reeds
		std::vector<glm::vec3> reedPositions = {
			glm::vec3(-25.0f, -20.0f, -80.0f),
			glm::vec3(-30.0f, -20.0f, -75.0f),
			glm::vec3(-20.0f, -20.0f, -82.0f),
		};
		for (auto& pos : reedPositions) {
			ModelMatrix = glm::translate(glm::mat4(1.0), pos);
			float reedScale = 2.0f;
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(reedScale, reedScale, reedScale));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			reed.draw(shader);
		}

		// draw trees
		std::vector<glm::vec3> treePositions = {
			glm::vec3(225.0f, -20.0f, -162.0f),
			glm::vec3(205.0f, -20.0f, -175.0f),
			glm::vec3(170.0f, -20.0f, -155.0f),
		};
		for (auto& pos : treePositions) {
			ModelMatrix = glm::translate(glm::mat4(1.0), pos);
			float treeScale = 3.0f;
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(treeScale, treeScale, treeScale));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			tree.draw(shader);
		}

		// draw river fish
		for (auto& f : schoolOfFish) {
			glm::vec3 direction = f.target - f.position;
			if (glm::length(direction) < 1.0f) {
				f.target = getRandomRiverPoint();
			}
			else {
				f.position += glm::normalize(direction) * f.speed * deltaTime;
			}
			ModelMatrix = glm::translate(glm::mat4(1.0), f.position);
			float angle = glm::degrees(atan2(direction.x, direction.z));
			ModelMatrix = glm::rotate(ModelMatrix, angle + 90.0f, glm::vec3(0, 1, 0));
			float fishScale = 3.0f;
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(fishScale, fishScale, fishScale));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			fish.draw(shader);
		}

		// draw ocean fish
		for (auto& f : schoolOfOceanFish) {
			glm::vec3 direction = f.target - f.position;
			if (glm::length(direction) < 1.0f) {
				f.target = getRandomOceanPoint();
			}
			else {
				f.position += glm::normalize(direction) * f.speed * deltaTime;
			}
			ModelMatrix = glm::translate(glm::mat4(1.0), f.position);
			float angle = glm::degrees(atan2(direction.x, direction.z));
			ModelMatrix = glm::rotate(ModelMatrix, angle + 90.0f, glm::vec3(0, 1, 0));
			float fishScale = 3.0f;
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(fishScale, fishScale, fishScale));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			fish.draw(shader);
		}

		// draw cat
		bool shouldDrawCat = true;

		if (isFishing && !isTransitioning) shouldDrawCat = false;
		if (isTransitioning && isZoomingIn && (transitionTimer / transitionDuration > 0.8f)) shouldDrawCat = false;

		if (shouldDrawCat) {
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, catPosition);
			ModelMatrix = glm::rotate(ModelMatrix, catRotationY, glm::vec3(0.0f, 1.0f, 0.0f));
			float catScale = 5.0f;
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(catScale, catScale, catScale));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			cat.draw(shader);
		}

		// hammock
		glm::mat4 BaseMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, -20.0f, 25.0f));
		BaseMatrix = glm::scale(BaseMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
		BaseMatrix = glm::rotate(BaseMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MVP = ProjectionMatrix * ViewMatrix * BaseMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &BaseMatrix[0][0]);
		hammock.draw(shader);

		glm::mat4 TreeMatrix = glm::translate(BaseMatrix, glm::vec3(0.4f, 0.0f, -4.5f));
		MVP = ProjectionMatrix * ViewMatrix * TreeMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &TreeMatrix[0][0]);
		hammockTrees.draw(shader);

		// cabin
		glm::mat4 CabinMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(205.0f, -20.0f, -70.0f));
		CabinMatrix = glm::scale(CabinMatrix, glm::vec3(4.0f, 4.0f, 4.0f));
		CabinMatrix = glm::rotate(CabinMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 CabinMVP = ProjectionMatrix * ViewMatrix * CabinMatrix;

		// foundation and body and roof
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &CabinMVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &CabinMatrix[0][0]);
		cabinFoundation.draw(shader);
		cabin.draw(shader);
		cabinRoof.draw(shader);

		// door Animation
		static float currentDoorSlide = 0.0f;
		float targetSlide = isDoorOpen ? 1.2f : 0.0f;
		float slideSpeed = 2.0f * deltaTime;

		if (currentDoorSlide < targetSlide) {
			currentDoorSlide += slideSpeed;
			if (currentDoorSlide > targetSlide) currentDoorSlide = targetSlide;
		}
		else if (currentDoorSlide > targetSlide) {
			currentDoorSlide -= slideSpeed;
			if (currentDoorSlide < targetSlide) currentDoorSlide = targetSlide;
		}

		glm::mat4 DoorMatrix = glm::translate(CabinMatrix, glm::vec3(-0.6f, 0.0f, 1.35f + currentDoorSlide));
		glm::mat4 DoorMVP = ProjectionMatrix * ViewMatrix * DoorMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &DoorMVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &DoorMatrix[0][0]);
		cabinDoor.draw(shader);

		//  draw sun
		sunShader.use();
		ModelMatrix = glm::translate(glm::mat4(1.0), lightPos);
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(sunShader.getId(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
		sun.draw(sunShader);

		//  draw skybox 
		mySkybox.draw(skyboxShader, ViewMatrix, ProjectionMatrix);

		// menu logic
		if (!isMenuOpen) {
			shader.use();
			glm::mat4 MarkerMatrix = glm::translate(glm::mat4(1.0f), interactionPoint);
			glm::mat4 MarkerMVP = ProjectionMatrix * ViewMatrix * MarkerMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MarkerMVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &MarkerMatrix[0][0]);
			sun.draw(shader);
		}

		if (extraWindow != nullptr) {
			if (glfwWindowShouldClose(extraWindow)) {
				glfwDestroyWindow(extraWindow);
				extraWindow = nullptr;
				isMenuOpen = false;
			}
			else {
				glfwMakeContextCurrent(extraWindow);
				glViewport(0, 0, 400, 300);
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glfwSwapBuffers(extraWindow);
				glfwMakeContextCurrent(window.getWindow());
				glViewport(0, 0, window.getWidth(), window.getHeight());
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.update();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void processKeyboardInput()
{
	static bool enterPressedLastFrame = false;
	if (window.isPressed(GLFW_KEY_ENTER)) {
		if (!enterPressedLastFrame) {
			if (!isFishing && !isTransitioning && hasFishingRod && checkNearWater(catPosition)) {
				isTransitioning = true;
				isZoomingIn = true;
				transitionTimer = 0.0f;
				startTransitionPos = camera.getCameraPosition();

				isCursorLocked = true;
				glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
		enterPressedLastFrame = true;
	}
	else {
		enterPressedLastFrame = false;
	}

	if (!isFishing) {
		float catSpeed = 20.0f * deltaTime;
		bool moved = false;
		glm::vec3 direction(0.0f);

		glm::vec3 camForward = camera.getCameraViewDirection();
		camForward.y = 0.0f;
		if (glm::length(camForward) > 0.01f) camForward = glm::normalize(camForward);
		glm::vec3 camRight = glm::cross(camForward, glm::vec3(0, 1, 0));
		if (glm::length(camRight) > 0.01f) camRight = glm::normalize(camRight);

		if (window.isPressed(GLFW_KEY_W)) { direction += camForward; moved = true; }
		if (window.isPressed(GLFW_KEY_S)) { direction -= camForward; moved = true; }
		if (window.isPressed(GLFW_KEY_A)) { direction -= camRight; moved = true; }
		if (window.isPressed(GLFW_KEY_D)) { direction += camRight; moved = true; }

		if (moved && glm::length(direction) > 0.01f) {
			direction = glm::normalize(direction);
			catPosition += direction * catSpeed;
			float movementAngle = glm::degrees(atan2(direction.x, direction.z));
			targetCatRotation = movementAngle + MODEL_CORRECTION_ANGLE;
		}
	}

	static bool wasComboDown = false;
	bool ctrlDown = glfwGetKey(window.getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
	bool rightClick = glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	bool comboNow = ctrlDown && rightClick;

	if (comboNow && !wasComboDown) {
		isCursorLocked = !isCursorLocked;
		if (isCursorLocked) {
			glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			firstMouse = true;
		}
		else {
			glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	wasComboDown = comboNow;

	if (window.isPressed(GLFW_KEY_E))
	{
		if (!eKeyPressedLastFrame)
		{
			if (isFishing && !isTransitioning) {
				isTransitioning = true;
				isZoomingIn = false;
				transitionTimer = 0.0f;
				startTransitionPos = camera.getCameraPosition();
				std::cout << "Starting Zoom Out..." << std::endl;
			}
			else {
				float distToRod = glm::length(catPosition - rodWorldPos);
				float distToDoor = glm::length(catPosition - cabinPos);
				float distToInteraction = glm::length(catPosition - interactionPoint);

				if (hasFishingRod || distToRod < 50.0f) {
					if (!hasFishingRod) {
						hasFishingRod = true;
					}
					else {
						hasFishingRod = false;
						float dropDist = 10.0f;
						float yawRad = glm::radians(catRotationY);
						rodWorldPos.x = catPosition.x - (sin(yawRad) * dropDist);
						rodWorldPos.z = catPosition.z - (cos(yawRad) * dropDist);
						rodWorldPos.y = -20.0f;
					}
				}
				else if (distToInteraction < interactionRadius) {
					if (extraWindow == nullptr) {
						extraWindow = glfwCreateWindow(400, 300, "Interaction Menu", NULL, window.getWindow());
						isMenuOpen = true;
					}
					else {
						glfwSetWindowShouldClose(extraWindow, true);
					}
				}
				else if (distToDoor < 40.0f) {
					isDoorOpen = !isDoorOpen;
				}
			}
			eKeyPressedLastFrame = true;
		}
	}
	else {
		eKeyPressedLastFrame = false;
	}
}

Mesh generateWaterGrid(int size, float spacing, std::vector<Texture> tex) {
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	float offset = (size * spacing) / 2.0f;
	for (int z = 0; z <= size; ++z) {
		for (int x = 0; x <= size; ++x) {
			Vertex v;
			v.pos = glm::vec3((x * spacing) - offset, 0.0f, (z * spacing) - offset);
			v.normals = glm::vec3(0.0f, 1.0f, 0.0f);
			v.textureCoords = glm::vec2((float)x / size, (float)z / size);
			vertices.push_back(v);
		}
	}
	for (int z = 0; z < size; ++z) {
		for (int x = 0; x < size; ++x) {
			int tl = (z * (size + 1)) + x;
			int tr = tl + 1;
			int bl = ((z + 1) * (size + 1)) + x;
			int br = bl + 1;
			indices.insert(indices.end(), { tl, bl, tr, tr, bl, br });
		}
	}
	return Mesh(vertices, indices, tex);
}

Mesh generateCircularRiver(float radius, float width, int segments, std::vector<Texture> tex) {
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	float innerRadius = radius - (width / 2.0f);
	int widthSegments = 18;
	for (int i = 0; i <= segments; ++i) {
		float theta = (float)i / segments * 2.0f * 3.1415926f;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);
		for (int j = 0; j <= widthSegments; ++j) {
			float r = innerRadius + ((float)j / widthSegments) * width;
			Vertex v;
			v.pos = glm::vec3(r * cosTheta, 0.0f, r * sinTheta);
			v.normals = glm::vec3(0.0f, 1.0f, 0.0f);
			v.textureCoords = glm::vec2(((float)i / segments) * 20.0f, (float)j / widthSegments);
			vertices.push_back(v);
		}
	}
	for (int i = 0; i < segments; ++i) {
		for (int j = 0; j < widthSegments; ++j) {
			int tl = (i * (widthSegments + 1)) + j;
			int tr = tl + 1;
			int bl = ((i + 1) * (widthSegments + 1)) + j;
			int br = bl + 1;
			indices.insert(indices.end(), { tl, bl, tr, tr, bl, br });
		}
	}
	return Mesh(vertices, indices, tex);
}
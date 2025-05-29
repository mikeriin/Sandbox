

#include <iostream>
#include <array>
#include <vector>
#include <cstdint>
#include <functional>
#include <chrono>
#include <utility>
#include <format>
#include <random>

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "globals.h"
#include "window.h"
#include "event_dispatcher.h"
#include "renderer.h"
#include "shader.h"
#include "voxel.h"
#include "buffer.h"
#include "vertex_array.h"
#include "camera.h"
#include "frustum.h"
#include "voxel_mesh.h"
#include "voxel_chunk.h"
#include "world_generator.h"


int mk::seed = 0;


int main(int argc, char* argv[])
{
	/* ---------- WINDOW ---------- */
	mk::WindowCreateInfo windowInfo{};
	windowInfo.name = "SANDBOX";
	windowInfo.width = 1600;
	windowInfo.height = 900;
	windowInfo.gfxApi = mk::GfxApi::OpenGL;

	mk::Window window;
	window.Create(windowInfo);
	/* ---------- !WINDOW ---------- */


	/* ---------- RENDERER ---------- */
	mk::RendererCreateInfo rendererInfo{};
	rendererInfo.pWindow = &window;

	mk::Renderer renderer;
	renderer.Create(rendererInfo);
	/* ---------- !RENDERER ---------- */


	glClearColor(0.459f, 0.416f, 0.714f, 1.0f);

	/* ---------- SHADERS ---------- */
	std::array<mk::ShaderInfo, 2> shaders{};
	shaders[0] = { "assets/shaders/voxel.vert", GL_VERTEX_SHADER };
	shaders[1] = { "assets/shaders/voxel.frag", GL_FRAGMENT_SHADER };

	mk::ShaderProgramCreateInfo programInfo{};
	programInfo.shaderCount = static_cast<uint32_t>(shaders.size());
	programInfo.pShaders = shaders.data();

	mk::ShaderProgram program;
	program.Create(programInfo);
	program.Use();
	/* ---------- !SHADERS ---------- */


	/* ---------- WORLD GENERATION ---------- */
	std::mt19937 gen(static_cast<uint32_t>(std::random_device{}()));
	std::uniform_int_distribution<> dist(1337, 1234567890);
	mk::seed = dist(gen);

	mk::WorldGenerator worldGen;
	/* ---------- !WORLD GENERATION ---------- */



	/* ---------- CAMERA ---------- */
	mk::CameraCreateInfo camInfo{};
	camInfo.position = glm::vec3(0.0);
	//camInfo.position = glm::vec3(0.0, (float)MK_VOXEL_CHUNK_MAX_GEN_HEIGHT, 0.0);
	camInfo.fov = 75.0f;
	camInfo.aspectRatio = window.GetExtent().x / window.GetExtent().y;
	camInfo.nearPlane = 0.01f;
	camInfo.farPlane = 500.0f;
	camInfo.yaw = 0.0f;
	camInfo.pitch = 0.0f;
	camInfo.cameraMode = MK_CAMERA_MODE_FREE_LOOK;

	mk::Camera mainCam;
	mainCam.Create(camInfo);

	mk::BufferCreateInfo camBufferInfo{};
	camBufferInfo.isMutable = false;
	camBufferInfo.size = sizeof(mk::CameraUniform);
	camBufferInfo.pData = nullptr;
	camBufferInfo.flags = GL_DYNAMIC_STORAGE_BIT;
	camBufferInfo.binding = 0;
	camBufferInfo.target = GL_UNIFORM_BUFFER;

	mk::Buffer camBuffer;
	camBuffer.Create(camBufferInfo);

	mk::Frustum mainCamFrustum;
	/* ---------- !CAMERA ---------- */


	/* ---------- EVENTS ---------- */
	mk::EventDispatcher& dispatcher = mk::EventDispatcher::GetInstance();
	dispatcher.RegisterHandler(mk::Event::Type::Quit, std::bind(&mk::Window::OnQuitEvent, &window, std::placeholders::_1));
	dispatcher.RegisterHandler(mk::Event::Type::WindowResized, std::bind(&mk::Window::OnWindowResizedEvent, &window, std::placeholders::_1));
	dispatcher.RegisterHandler(mk::Event::Type::WindowResized,
		std::bind(&mk::Renderer::OnWindowResizedEvent, &renderer, std::placeholders::_1));
	dispatcher.RegisterHandler(mk::Event::Type::KeyDown, std::bind(&mk::Renderer::OnKeyDownEvent, &renderer, std::placeholders::_1));
	dispatcher.RegisterHandler(
		mk::Event::Type::WindowResized, std::bind(&mk::Camera::OnWindowResizedEvent, &mainCam, std::placeholders::_1));
	dispatcher.RegisterHandler(mk::Event::Type::MouseMotion, std::bind(&mk::Camera::OnMouseMotionEvent, &mainCam, std::placeholders::_1));
	dispatcher.RegisterHandler(mk::Event::Type::KeyDown, std::bind(&mk::Camera::OnKeyDownEvent, &mainCam, std::placeholders::_1));
	dispatcher.RegisterHandler(mk::Event::Type::KeyUp, std::bind(&mk::Camera::OnKeyUpEvent, &mainCam, std::placeholders::_1));
	/* ---------- !EVENTS ---------- */


	auto lastFrameTime = std::chrono::high_resolution_clock::now();

	constexpr float fixedDeltaTime = 1.0f / 60.0f;
	float fixedDTTimer = 0.0f;
	bool shouldFixedUpdate = false;


	while (window.IsOpen())
	{
		/* ---------- TIME UPDATE ---------- */
		auto currentFrameTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		fixedDTTimer += deltaTime;
		if (fixedDTTimer >= fixedDeltaTime)
		{
			fixedDTTimer = 0.0f;
			shouldFixedUpdate = true;
		}
		/* ---------- !TIME UPDATE ---------- */

		window.PollEvents(mk::EventDispatcher::GetInstance());

		/* ---------- CAMERA UPDATE ---------- */
		mainCam.Update(deltaTime);

		mk::CameraUniform camUniform = mainCam.GetCameraUniform();

		mk::BufferUpdateInfo updateInfo{};
		updateInfo.offset = 0;
		updateInfo.size = sizeof(mk::CameraUniform);
		updateInfo.pData = &camUniform;
		camBuffer.Update(updateInfo);

		mainCamFrustum.Update(camUniform.viewProjection);
		/* ---------- !CAMERA UPDATE ---------- */


		/* ---------- WORLD GEN UPDATE ---------- */
		worldGen.UpdateCameraPosition(mainCam.GetCameraPosition());
		worldGen.Tick();

		static mk::ChunkMeshes chunkMeshesToRender;

		if (shouldFixedUpdate)
		{
			chunkMeshesToRender.clear();

			for (const auto& [pos, mesh] : worldGen.GetChunks())
			{
				glm::vec3 min = glm::vec3(pos) * (float)MK_VOXEL_CHUNK_SIZE;
				glm::vec3 max = min + glm::vec3((float)MK_VOXEL_CHUNK_SIZE);

				if (!mainCamFrustum.IntersectsAABB(min, max)) continue;

				chunkMeshesToRender.insert_or_assign(pos, mesh);
			}
		}
		/* ---------- !WORLD GEN UPDATE ---------- */


		/* ---------- DRAW ---------- */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		program.Use();

		for (const auto& [pos, mesh] : chunkMeshesToRender)
		{
			program.SetVec3("u_ChunkPosition", pos);
			mesh->Draw();
		}
		
		/* ---------- !DRAW ---------- */

		
		if (shouldFixedUpdate) shouldFixedUpdate = false;


		window.SwapBuffers();
	}


	return 0;

}
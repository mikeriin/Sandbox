#ifndef MK_CAMERA_H
#define MK_CAMERA_H



#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>
#include <gtx/compatibility.hpp>

#include "event.h"
#include "globals.h"



namespace mk
{


	struct CameraCreateInfo
	{
		float fov;
		float aspectRatio;
		float nearPlane;
		float farPlane;
		glm::vec3 position;
		float yaw;
		float pitch;
		uint32_t cameraMode = MK_CAMERA_MODE_FREE_LOOK;
	};


	struct CameraUniform
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 viewProjection;
	};


	class Camera
	{
	private:
		glm::vec3 m_position = {};
		glm::vec3 m_front = {};
		glm::vec3 m_right = {};
		glm::vec3 m_up = {};
		mk::CameraUniform m_uniform = {};
		float m_yaw = 0.0f;
		float m_pitch = 0.0f;
		float m_fov = 0.0f;
		float m_near = 0.0f;
		float m_far = 0.0f;
		glm::vec3 m_direction = {};
		glm::quat m_orientation = {};
		glm::quat m_desiredOrientation = {};
		uint32_t m_cameraMode = 0;


	public:
		Camera() = default;
		~Camera() = default;


		bool Create(const mk::CameraCreateInfo& camInfo);


		void Update(float dt);


		inline const mk::CameraUniform& GetCameraUniform() const { return m_uniform; }
		inline glm::ivec3 GetCameraPosition() const { return m_position; }

		
		void OnWindowResizedEvent(const mk::Event& e);
		void OnMouseMotionEvent(const mk::Event& e);
		void OnKeyDownEvent(const mk::Event& e);
		void OnKeyUpEvent(const mk::Event& e);


	private:
		void updateCameraVectors();
		void updateCameraPosition(float dt);
		glm::mat4 computeViewMatrix() const;
	};



}



#endif // !MK_CAMERA_H

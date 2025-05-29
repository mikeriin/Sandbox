#include "camera.h"


#include <iostream>
#include <cmath>

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keycode.h>



namespace mk
{






	bool Camera::Create(const mk::CameraCreateInfo& camInfo)
	{
		
		m_position = camInfo.position;
		m_yaw = camInfo.yaw;
		m_pitch = camInfo.pitch;

		m_fov = camInfo.fov;
		m_near = camInfo.nearPlane;
		m_far = camInfo.farPlane;

		m_cameraMode = camInfo.cameraMode;

		
		if (m_cameraMode == MK_CAMERA_MODE_FREE_LOOK)
		{
			m_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			m_desiredOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		}
		else if (m_cameraMode == MK_CAMERA_MODE_CLASSIC)
			updateCameraVectors();

		m_uniform.projection = glm::perspective(glm::radians(m_fov), camInfo.aspectRatio, m_near, m_far);
		m_uniform.view = computeViewMatrix();
		m_uniform.viewProjection = m_uniform.projection * m_uniform.view;

		return true;
	}

	void Camera::Update(float dt)
	{
		if (m_cameraMode == MK_CAMERA_MODE_FREE_LOOK)
		{
			m_orientation = glm::slerp(m_orientation, m_desiredOrientation, glm::clamp(dt * MK_CAMERA_ROTATION_SMOOTHING_SPEED, 0.0f, 1.0f));
			m_uniform.view = computeViewMatrix();
			m_uniform.viewProjection = m_uniform.projection * m_uniform.view;
		}

		updateCameraPosition(dt);
	}

	void Camera::OnWindowResizedEvent(const mk::Event& e)
	{
		const mk::WindowResizedEvent& wndResizedEvent = static_cast<const mk::WindowResizedEvent&>(e);

		m_uniform.projection = glm::perspective(glm::radians(m_fov), wndResizedEvent.width / (float)wndResizedEvent.height, m_near, m_far);
		m_uniform.viewProjection = m_uniform.projection * m_uniform.view;
	}

	void Camera::OnMouseMotionEvent(const mk::Event& e)
	{
		const mk::MouseMotionEvent& mouseMotionEvent = static_cast<const mk::MouseMotionEvent&>(e);

		if ((mouseMotionEvent.state & SDL_BUTTON_LEFT) == SDL_BUTTON_LEFT)
		{
			if (m_cameraMode == MK_CAMERA_MODE_FREE_LOOK)
			{
				float yaw = -mouseMotionEvent.xRel * MK_CAMERA_SENSITIVITY_X;
				float pitch = -mouseMotionEvent.yRel * MK_CAMERA_SENSITIVITY_Y;

				glm::vec3 camUp = m_desiredOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
				float dot = glm::dot(camUp, glm::vec3(0.0f, 1.0f, 0.0f));

				glm::vec3 upAxis = (dot >= 0.0f)
					? glm::vec3(0.0f, 1.0f, 0.0f)
					: glm::vec3(0.0f, -1.0f, 0.0f);

				/*float t = glm::clamp(dot, -1.0f, 1.0f);
				glm::vec3 upAxis = glm::normalize(glm::mix(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), (t + 1.0f) / 2.0f));*/

				glm::quat qYaw = glm::angleAxis(glm::radians(yaw), upAxis);

				glm::vec3 camRight = m_desiredOrientation * glm::vec3(1.0f, 0.0f, 0.0f);
				glm::quat qPitch = glm::angleAxis(glm::radians(pitch), camRight);

				m_desiredOrientation = glm::normalize(qYaw * qPitch * m_desiredOrientation);

				if (!glm::all(glm::isfinite(
					glm::vec4(m_desiredOrientation.x, m_desiredOrientation.y, m_desiredOrientation.z, m_desiredOrientation.w))))
				{
					m_desiredOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
				}
			}
			else if (m_cameraMode == MK_CAMERA_MODE_CLASSIC)
			{
				m_yaw -= mouseMotionEvent.xRel * MK_CAMERA_SENSITIVITY_X;
				m_yaw = std::fmodf(m_yaw, 360.0f);

				m_pitch -= mouseMotionEvent.yRel * MK_CAMERA_SENSITIVITY_Y;
				m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);


				updateCameraVectors();

				m_uniform.view = computeViewMatrix();
				m_uniform.viewProjection = m_uniform.projection * m_uniform.view;
			}
		}
	}

	void Camera::OnKeyDownEvent(const mk::Event& e)
	{
		const mk::KeyDownEvent& keyDownEvent = static_cast<const mk::KeyDownEvent&>(e);

		switch (keyDownEvent.keycode)
		{
		case SDLK_Z:
			m_direction.z = -1.0f;
			break;

		case SDLK_Q:
			m_direction.x = -1.0f;
			break;

		case SDLK_S:
			m_direction.z =  1.0f;
			break;

		case SDLK_D:
			m_direction.x =  1.0f;
			break;

		case SDLK_SPACE:
			m_direction.y =  1.0f;
			break;

		case SDLK_LSHIFT:
			m_direction.y = -1.0f;
			break;

		default:
			break;
		}
	}

	void Camera::OnKeyUpEvent(const mk::Event& e)
	{
		const mk::KeyUpEvent& keyUpEvent = static_cast<const mk::KeyUpEvent&>(e);

		switch (keyUpEvent.keycode)
		{
		case SDLK_Z:
		case SDLK_S:
			m_direction.z = 0.0f;
			break;

		case SDLK_Q:
		case SDLK_D:
			m_direction.x = 0.0f;
			break;

		case SDLK_SPACE:
		case SDLK_LSHIFT:
			m_direction.y = 0.0f;
			break;

		default:
			break;
		}
	}

	void Camera::updateCameraVectors()
	{
		glm::quat qPitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat qYaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat qOrientation = qYaw * qPitch;
		glm::vec3 newFront = qOrientation * glm::vec3(0.0f, 0.0f, -1.0f);

		m_front = glm::normalize(newFront);
		m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0)));
		m_up = glm::normalize(glm::cross(m_right, m_front));

		m_uniform.view = computeViewMatrix();
		m_uniform.viewProjection = m_uniform.projection * m_uniform.view;
	}

	void Camera::updateCameraPosition(float dt)
	{
		if (glm::length(m_direction) > 0.0f)
		{
			glm::vec3 desiredDirection = glm::normalize(m_direction);
			
			if (m_cameraMode == MK_CAMERA_MODE_FREE_LOOK)
			{
				glm::vec3 worldMovement = glm::normalize(m_orientation * m_direction);
				m_position += worldMovement * dt * MK_CAMERA_SPEED;
			}
			else if (m_cameraMode == MK_CAMERA_MODE_CLASSIC)
			{
				glm::vec3 forward = -glm::normalize(glm::vec3(m_front.x, 0.0f, m_front.z));
				m_position += (desiredDirection.z * forward 
					+ desiredDirection.x * m_right 
					+ desiredDirection.y * glm::vec3(0.0f, 1.0f, 0.0f)) * dt * MK_CAMERA_SPEED;
			}

			m_uniform.view = computeViewMatrix();
			m_uniform.viewProjection = m_uniform.projection * m_uniform.view;
		}
	}

	glm::mat4 Camera::computeViewMatrix() const
	{
		if (m_cameraMode == MK_CAMERA_MODE_FREE_LOOK)
		{
			glm::mat4 rotation = glm::toMat4(glm::conjugate(m_orientation));
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_position);

			return rotation * translation;
		}

		return glm::lookAt(
			m_position,
			m_position + m_front,
			m_up
		);
	}

}
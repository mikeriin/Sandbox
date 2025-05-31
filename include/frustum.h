#ifndef MK_FRUSTUM_H
#define MK_FRUSTUM_H



#include <array>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_access.hpp>



namespace mk
{


	struct Plane
	{
		glm::vec3 normal;
		float distance;
	};


	class Frustum
	{
	public:
		enum Side { Left = 0, Right, Bottom, Top, Near, Far };
		
		std::array<mk::Plane, 6> planes;


		inline void Update(const glm::mat4& projView)
		{
			glm::vec4 row0 = glm::row(projView, 0);
			glm::vec4 row1 = glm::row(projView, 1);
			glm::vec4 row2 = glm::row(projView, 2);
			glm::vec4 row3 = glm::row(projView, 3);


			glm::vec4 combined[6];
			combined[Side::Left] = row3 + row0;
			combined[Side::Right] = row3 - row0;
			combined[Side::Bottom] = row3 + row1;
			combined[Side::Top] = row3 - row1;
			combined[Side::Near] = row3 + row2;
			combined[Side::Far] = row3 - row2;


			for (int i = 0; i < 6; i++)
			{
				float length = glm::length(glm::vec3(combined[i]));

				planes[i].normal = glm::vec3(combined[i]) / length;
				planes[i].distance = combined[i].w / length;
			}
		}


		inline bool ContainsPoint(const glm::vec3& point) const
		{
			for (const auto& plane : planes)
			{
				if (glm::dot(plane.normal, point) + plane.distance < 0.0f) return false;
			}
			return true;
		}


		inline bool IntersectsAABB(const glm::vec3& min, const glm::vec3& max)
		{
			for (const auto& plane : planes)
			{
				glm::vec3 positiveVertex(
					plane.normal.x > 0.0f ? max.x : min.x,
					plane.normal.y > 0.0f ? max.y : min.y,
					plane.normal.z > 0.0f ? max.z : min.z
					);

				if (glm::dot(plane.normal, positiveVertex) + plane.distance < 0.0f) return false;
			}
			return true;
		}

	};


}




#endif // !MK_FRUSTUM_H

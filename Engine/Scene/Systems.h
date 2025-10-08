#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"

#include "Components.h"

namespace Iaonnis
{
	class System
	{
	public:
		System(entt::registry* reg)
			:registery(reg)
		{

		}
		virtual ~System() {}

		virtual void OnUpdate(float dt) = 0;
	protected:
		entt::registry* registery;
	};

	class TransformSystem : public System
	{
	public:
		TransformSystem(entt::registry* reg)
			:System(reg)
		{

		}
		~TransformSystem() {}

		static void TransformPosition(glm::mat4 mat, glm::vec4& position)
		{
			position = mat * position;
		}

		virtual void OnUpdate(float dt) override
		{
			for (auto entt : registery->view<TransformComponent>())
			{
				auto& transform = registery->get<TransformComponent>(entt);

				transform.model = glm::translate(glm::mat4(1.0f), transform.position);

				transform.model = glm::rotate(transform.model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0)); 
				transform.model = glm::rotate(transform.model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0)); 
				transform.model = glm::rotate(transform.model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));

				transform.model = glm::scale(transform.model, transform.scale);
			}
		}

	private:

	};
}
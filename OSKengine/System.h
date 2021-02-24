#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ComponentArray.h"
#include "Component.h"

#include <set>
#include <vulkan/vulkan.h>

namespace OSK {
	class EntityComponentSystem;
}

namespace OSK::ECS {

	//Sistema del ECS.
	class System {

	public:

		virtual ~System() = default;

		virtual void OnCreate() {}
		virtual void OnTick(deltaTime_t deltaTime) {}
		virtual void OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) {}
		virtual void OnRemove() {}

		virtual Signature GetSystemSignature() {
			Signature signature{};

			return signature;
		}

		std::set<GameObjectID> Objects;

		EntityComponentSystem* ECSsystem = nullptr;

	};

	class SystemManager {

	public:

		void OnTick(deltaTime_t deltaTime) {
			for (auto& i : systems) {
				i.second->OnTick(deltaTime);
			}
		}

		void OnDraw(VkCommandBuffer cmdBuffer, uint32_t ix) {
			for (auto& i : systems) {
				i.second->OnDraw(cmdBuffer, ix);
			}
		}

		template <typename T> T* CreateSystem() {
			const char* name = typeid(T).name();

			T* sistema = new T();
			systems.insert({ name, (System*)sistema });

			return sistema;
		}

		template <typename T> void SetSignature(Signature signature) {
			const char* name = typeid(T).name();

			signatures.insert({ name, signature });
		}

		void GameObjectDestroyed(GameObjectID obj) {
			for (auto& i : systems) {
				i.second->Objects.erase(obj);
			}
		}

		void GameObjectSignatureChanged(GameObjectID object, Signature signature) {
			for (auto& i : systems)
			{
				const char* type = i.first;
				System* system = i.second;
				Signature systemSignature = signatures[type];

				// Entity signature matches system signature - insert into set
				if ((signature & systemSignature) == systemSignature)
				{
					system->Objects.insert(object);
				}
				// Entity signature does not match system signature - erase from set
				else
				{
					system->Objects.erase(object);
				}
			}
		}

	private:

		std::unordered_map<const char*, Signature> signatures;
		std::unordered_map<const char*, System*> systems;

	};

}
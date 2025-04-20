#pragma once

#include "../components/Animatable.hpp"
#include "../components/Collectable.hpp"
#include "../components/Collider.hpp"
#include "../components/Consumable.hpp"
#include "../components/Controllable.hpp"
#include "../components/Interactable.hpp"
#include "../components/Inventory.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../components/SoundEmitter.hpp"
#include "../components/Stats.hpp"
#include "../components/Tombstone.hpp"
#include "../constants.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <easys/easys.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

class SaveGameManager {
  public:
	SaveGameManager(Easys::ECS &ecs) : ecs_(ecs)
	{
	}

	void save(const std::string path)
	{
		std::cout << "saving to " << path << std::endl;
		std::ofstream os(path, std::ios::binary);
		if (!os) {
			std::cerr << "Error: Unable to open file." << std::endl;
			return;
		}
		cereal::JSONOutputArchive archive(os);

		archive(ecs_.getEntities());

		forEachComponentType<COMPONENT_TYPES>([&](auto dummy) {
			using T = decltype(dummy);
			serializeComponentsByType<T>(archive);
		});
	}

	// This function is a bit convoluted, because it works around the constraint, that the ECS manages entity ids.
	// The user can specify entity ids only once in the constructor of Easys::ECS. So we need to create a
	// new Easys::ECS instance, whenever we load.
	void load(const std::string path)
	{
		std::cout << "loading from " << path << std::endl;
		if (!std::filesystem::exists(path)) {
			std::cerr << "Error: File does not exist." << std::endl;
			return;
		}
		std::ifstream is(path, std::ios::binary);
		if (!is) {
			std::cerr << "Error: Unable to open file." << std::endl;
			return;
		}
		cereal::JSONInputArchive archive(is);

		// initialise fresh ecs with entities from save file
		std::set<Easys::Entity> entities;
		archive(entities);
		ecs_ = Easys::ECS(entities);

		forEachComponentType<COMPONENT_TYPES>([&](auto dummy) {
			using T = decltype(dummy);
			deserializeComponentsByType<T>(archive);
		});
	}

  private:
	template <typename... Types, typename Func>
	void forEachComponentType(Func f)
	{
		(f(Types{}), ...);
	}

	template <typename T, class Archive>
	void serializeComponentsByType(Archive &archive)
	{
		std::vector<std::tuple<Easys::Entity, std::string, T>> arr;

		for (auto &e : ecs_.getEntities()) {
			if (ecs_.hasComponent<T>(e)) {
				arr.push_back(std::tuple(e, typeid(T).name(), ecs_.getComponent<T>(e)));
			}
		}

		archive(arr);
	}

	template <typename T, class Archive>
	void deserializeComponentsByType(Archive &archive)
	{
		std::vector<std::tuple<Easys::Entity, std::string, T>> arr;

		archive(arr);

		for (auto &[e, t, c] : arr) {
			if (!ecs_.getEntities().contains(e)) {
				std::cout << "COULD NOT FIND ENTITY <" << e << "> WHILE SERIALISING COMPONENT <" << t << ">."
				          << std::endl;
			}
			ecs_.addComponent<T>(e, c);
		}
	}

	Easys::ECS &ecs_;
};

#pragma once

#include "../ai/nodes/IsEnemyVisible.hpp"
#include "../ai/nodes/IsInState.hpp"
#include "../ai/nodes/MoveTo.hpp"
#include "../ai/nodes/PatrolTo.hpp"
#include "../ai/nodes/RandomSelector.hpp"
#include "../ai/nodes/TurnTo.hpp"
#include "../ai/nodes/WaitFor.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include <unordered_map>

class BTManager {
  public:
	explicit BTManager(ECSManager &ecs)
	{
		registerNodes(ecs);
		registerTreesFromDirectory(BT_DIRECTORY);
	}

	void createTreeForEntity(const Entity &entity, const std::string &treeName)
	{
		// TODO?: if entity has tree
		auto tree = factory.createTree(treeName);
		tree.rootBlackboard()->set("entity", entity);
		tree.rootBlackboard()->set("deltaTime", 0.0);
		trees[entity] = std::move(tree);
	}

	void tickTree(Entity entity) { trees[entity].tickOnce(); }

	// Set a value globally for every tree.
	template <typename T>
	void setGlobalTreeValue(const std::string &key, const T &value)
	{
		for (auto &[Entity, tree] : trees) {
			tree.rootBlackboard()->set(key, value); // propagate deltaTime to all nodes
		}
	}

  private:
	void registerNodes(ECSManager &ecs)
	{
		factory.registerNodeType<RandomSelector>("RandomSelector");

		factory.registerNodeType<IsEnemyVisible>("IsEnemyVisible", std::ref(ecs));
		factory.registerNodeType<TurnTo>("TurnTo", std::ref(ecs));
		factory.registerNodeType<WaitFor>("WaitFor", std::ref(ecs));
		factory.registerNodeType<MoveTo>("MoveTo", std::ref(ecs));
		factory.registerNodeType<PatrolTo>("PatrolTo", std::ref(ecs));
		factory.registerNodeType<IsInState>("IsInState", std::ref(ecs));
	}

	// Find all the XML files in a folder and register all of them.
	void registerTreesFromDirectory(const std::string &directory)
	{
		using std::filesystem::directory_iterator;
		for (auto const &entry : directory_iterator(directory)) {
			if (entry.path().extension() == ".xml") {
				std::cout << entry.path().string() << "\n";
				factory.registerBehaviorTreeFromFile(entry.path().string());
			}
		}
	}

	BT::BehaviorTreeFactory factory;
	std::unordered_map<Entity, BT::Tree> trees;
	// It might be preferable to store the tree in a component (dedicated or else) so we keep all game state within the
	// ecs. This would make saving and loading (more) straight forward. Otherwise we would have to manually handle
	// loading and saving the btmanager and pass a ref to some places (menus, savegamemanager).
};
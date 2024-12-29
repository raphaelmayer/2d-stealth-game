#pragma once

#include "../ai/nodes/IsEnemyVisible.hpp"
#include "../ai/nodes/MoveTo.hpp"
#include "../ai/nodes/TurnTo.hpp"
#include "../ai/nodes/WaitFor.hpp"
#include "../ecs/ECSManager.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include <unordered_map>

class BTManager {
  public:
	explicit BTManager(ECSManager &ecs)
	{
		factory.registerNodeType<IsEnemyVisible>("IsEnemyVisible", std::ref(ecs));
		factory.registerNodeType<TurnTo>("TurnTo", std::ref(ecs));
		factory.registerNodeType<WaitFor>("WaitFor", std::ref(ecs));
		factory.registerNodeType<MoveTo>("MoveTo", std::ref(ecs));
	}

	void createTreeForEntity(Entity entity, std::string filepath)
	{
		// TODO?: if entity has tree
		auto tree = createTree(filepath);
		tree.rootBlackboard()->set("entity", entity);
		tree.rootBlackboard()->set("deltaTime", 0.0);
		trees[entity] = std::move(tree);
	}

	void tickTree(Entity entity) { trees[entity].tickOnce(); }

	template <typename T>
	void setGlobalTreeValue(std::string key, T value)
	{
		for (auto &[Entity, tree] : trees) {
			tree.rootBlackboard()->set(key, value); // propagate deltaTime to all nodes
		}
	}

  private:
	BT::Tree createTree(const std::string path)
	{
		if (!std::filesystem::exists(path)) {
			throw std::runtime_error("File does not exist at: " + path);
		}

		auto tree = factory.createTreeFromFile(path);

		return tree;
	}

	BT::BehaviorTreeFactory factory;
	std::unordered_map<Entity, BT::Tree> trees;
	// It might be preferable to store the tree in a component (dedicated or else) so we keep all game state within the
	// ecs. This would make saving and loading (more) straight forward. Otherwise we would have to manually handle
	// loading and saving the btmanager and pass a ref to some places (menus, savegamemanager).
};
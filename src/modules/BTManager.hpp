#pragma once

#include "../ai/nodes/IsEnemyVisible.hpp"
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
	}

	BT::Tree &getTree(Entity entity) { return trees[entity]; }

	BT::Tree createTree(const std::string path)
	{
		if (!std::filesystem::exists(path)) {
			throw std::runtime_error("File does not exist at: " + path);
		}

		auto tree = factory.createTreeFromFile(path);

		return tree;
	}

	void createTreeForEntity(Entity entity)
	{
		// TODO?: if entity has tree
		auto tree = createTree("../assets/ai/trees/tree.xml");
		tree.rootBlackboard()->set("entity", entity);
		tree.rootBlackboard()->set("deltaTime", 0.0);
		trees[entity] = std::move(tree);
	}

	void tickTree(Entity entity) { trees[entity].tickOnce(); }

	void tickTrees()
	{
		for (auto &[Entity, tree] : trees) {
			// TODO
		}
	}

	template <typename T>
	void setGlobalTreeValue(std::string key, T value)
	{
		for (auto &[Entity, tree] : trees) {
			tree.rootBlackboard()->set(key, value); // propagate deltaTime to all nodes
		}
	}

  private:
	BT::BehaviorTreeFactory factory;
	std::unordered_map<Entity, BT::Tree> trees;
	// It might be preferable to store the tree in a component (dedicated or else) so we keep all game state within the
	// ecs. This would make saving and loading (more) straight forward. Otherwise we would have to manually handle
	// loading and saving the btmanager and pass a ref to some places (menus, savegamemanager).
};
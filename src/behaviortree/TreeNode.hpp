#pragma once

#include "BehaviorStatus.hpp"
#include "../ai/Blackboard.hpp"

class TreeNode {
  public:
	virtual ~TreeNode() = default;

	void updateStatus(BehaviorStatus newStatus) { 
		prevStatus = currentStatus;
		currentStatus = newStatus;
	}

	void activate() { 
		if (!active) {
			active = true;
			//onActivate(blackboard);
		}
	}

	void deactivate() { 
		if (active) {
			active = false;
			//onDeactivate(blackboard);
		}
	}
	
	bool isActive() { return active; }

	BehaviorStatus internalTick(Blackboard &blackboard) { 
		BehaviorStatus newState = tick(blackboard);
		
		if (newState != currentStatus) {
			updateStatus(newState);
			(newState == BehaviorStatus::FAILURE) ? deactivate() : activate();
		}

		return newState;
	}

	virtual TreeNodeType type() const = 0;

	void haltNode() { 
		halt(); // call user halt
	}

	BehaviorStatus status() const { return currentStatus; }

	void resetStatus() {
		updateStatus(BehaviorStatus::IDLE);
	}

  protected:
	virtual BehaviorStatus tick(Blackboard &blackboard) = 0;

  private:
	virtual void halt() = 0;

	int id;
	BehaviorStatus currentStatus;
	BehaviorStatus prevStatus;
	bool active = false;
};
#pragma once

enum class StateID { IDLE = 0, AIMING, READY, FIRING, RELOADING };

class StateMachine;

class StateNode {
  public:
	virtual void onEnter(StateMachine &sm) = 0;
	virtual void onUpdate(StateMachine &sm, double deltaTime) = 0;
	virtual void onExit(StateMachine &sm) = 0;
};

class StateMachine {
  public:
	StateMachine(std::unique_ptr<StateNode> initialState) 
	{
		switchState(std::move(initialState));
	}

	void switchState(std::unique_ptr<StateNode> newState)
	{
		if (currentState) {
			currentState->onExit(*this);
		}

		currentState = std::move(newState);
		
		if (currentState) {
			currentState->onEnter(*this);
		}
	}

	void update(double deltaTime)
	{
		if (currentState) {
			currentState->onUpdate(*this, deltaTime);
		}
	}

  private:
	std::unique_ptr<StateNode> currentState;
};


// These nodes are not used. They are used to try using a FSM but we still need a robust FSM implementation. I tend to
// implementing one ourselves, but we might just use some library.

//class IdleNode;
//class AimingNode;
//class ReadyNode;
//class FiringNode;
//class ReloadingNode;

//class IdleNode : public StateNode {
//  public:
//	IdleNode(EquippedWeapon &weapon) : ew(weapon), wdata(WeaponDatabase::getInstance().get(weapon.weaponId)) {}
//
//	void onEnter(StateMachine &sm) override {}
//
//	void onUpdate(StateMachine &sm, double deltaTime) override {}
//
//	void onExit(StateMachine &sm) override {}
//
//  private:
//	const WeaponMetadata &wdata;
//	EquippedWeapon &ew;
//};
//
//class AimingNode : public StateNode {
//  public:
//	AimingNode(EquippedWeapon &weapon) : ew(weapon), wdata(WeaponDatabase::getInstance().get(weapon.weaponId)) {}
//
//	void onEnter(StateMachine &sm) override { ew.warmupAccumulator = 0; }
//
//	void onUpdate(StateMachine &sm, double deltaTime) override
//	{
//		ew.warmupAccumulator += deltaTime;
//		if (ew.warmupAccumulator >= wdata.warmup) {
//			sm.switchState(std::make_unique<ReadyNode>(ew));
//		}
//	}
//
//	void onExit(StateMachine &sm) override { ew.warmupAccumulator = 0; }
//
//  private:
//	const WeaponMetadata &wdata;
//	EquippedWeapon &ew;
//};
//
//class ReadyNode : public StateNode {
//  public:
//	ReadyNode(EquippedWeapon &weapon) : ew(weapon), wdata(WeaponDatabase::getInstance().get(weapon.weaponId)) {}
//
//	void onEnter(StateMachine &sm) override {}
//
//	void onUpdate(StateMachine &sm, double deltaTime) override { sm.switchState(std::make_unique<FiringNode>(ew)); }
//
//	void onExit(StateMachine &sm) override {}
//
//  private:
//	const WeaponMetadata &wdata;
//	EquippedWeapon &ew;
//};
//
//class FiringNode : public StateNode {
//  public:
//	FiringNode(EquippedWeapon &weapon) : ew(weapon), wdata(WeaponDatabase::getInstance().get(weapon.weaponId)) {}
//
//	void onEnter(StateMachine &sm) override { ew.reloadTimeAccumulator = 0; }
//
//	void onUpdate(StateMachine &sm, double deltaTime) override
//	{
//		ew.firerateAccumulator += deltaTime;
//		if (ew.firerateAccumulator < wdata.firerate) {
//			return;
//		}
//
//		if (ew.magazineSize != wdata.magazineSize) {
//			sm.switchState(std::make_unique<ReloadingNode>(ew));
//			return;
//		}
//
//		// shoot
//		ew.firerateAccumulator += static_cast<float>(deltaTime);
//		--ew.magazineSize;
//
//		// Vec2f start = ecs.getComponent<Positionable>(entity).position;
//		// Vec2f mouseScreenPos = Utils::toFloat(engine_.getMousePosition());
//		// Vec2f mouseWorldPos = camera_.screenToWorld(mouseScreenPos, engine_.getRenderScale());
//		// spawnProjectile(ecs, start, mouseWorldPos, entity, ew.weaponId);
//	}
//
//	void onExit(StateMachine &sm) override { ew.reloadTimeAccumulator = 0; }
//
//  private:
//	const WeaponMetadata &wdata;
//	EquippedWeapon &ew;
//};
//
//class ReloadingNode : public StateNode {
//  public:
//	ReloadingNode(EquippedWeapon &weapon) : ew(weapon), wdata(WeaponDatabase::getInstance().get(weapon.weaponId)) {}
//
//	void onEnter(StateMachine &sm) override {}
//
//	void onUpdate(StateMachine &sm, double deltaTime) override {}
//
//	void onExit(StateMachine &sm) override {}
//
//  private:
//	const WeaponMetadata &wdata;
//	EquippedWeapon &ew;
//};
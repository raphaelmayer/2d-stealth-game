#include "../../src/modules/SaveGameManager.hpp"
#include "../../src/components/Positionable.hpp"
#include "../../src/ecs/ECSManager.hpp"
#include <catch2/catch.hpp>
#include <filesystem>

#define PATH "test-savefile.json"

TEST_CASE("SaveGameManager Tests", "[SaveGameManager]")
{
	SECTION("load")
	{
		ECSManager ecs;
		SaveGameManager sgm(ecs);

		Entity entity = ecs.addEntity();
		Positionable comp = {{0, 0}};
		ecs.addComponent<Positionable>(entity, comp);
		sgm.save(PATH);

		ECSManager ecs2;
		SaveGameManager sgm2(ecs2);
		sgm2.load(PATH);

		REQUIRE(ecs2.hasComponent<Positionable>(entity));
	}

	SECTION("save")
	{
		ECSManager ecs;
		SaveGameManager sgm(ecs);

		Entity entity = ecs.addEntity();
		Positionable comp = {{0, 0}};
		ecs.addComponent<Positionable>(entity, comp);

		sgm.save(PATH);

		REQUIRE(std::filesystem::exists(PATH));
	}
}
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "../../src/components/Positionable.hpp"
#include "../../src/components/RigidBody.hpp"
#include "../../src/ecs/ECSManager.hpp"
#include "../../src/systems/System.hpp"
#include <chrono>

#define NUM_ENT 10000 // number of entities
#define NUM_COM 1     // number of components per entity

// CATCH_CONFIG_RUNNER tells catch2, that we will implement our own main function to config the test runner.
int main(int argc, char *argv[])
{
	Catch::Session session;

	// Set the configuration to show all test results, including successful tests
	session.configData().showSuccessfulTests = true;

	// Set the reporter to 'console'
	session.configData().reporterName = "compact";

	// You can add additional configuration here if needed

	// Run the Catch2 session
	return session.run(argc, argv);
}

std::string formatEntCompInfo(const std::string functionName, const int numEntities, const int numComponents)
{
	std::ostringstream oss;
	oss << functionName << "(): e: " << numEntities << ", c: " << numComponents * numEntities << ", c/e: " << numComponents;
	return oss.str();
}

// This function is a helper to run benchmarks. It  that takes a lambda function as an argument.
// This lambda function will contain the code to benchmark.
template <typename Func>
void benchmarkSection(Func func, const std::string &sectionName)
{
	auto start = std::chrono::high_resolution_clock::now();

	func(); // Execute the lambda function

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;

	// CAPTURE(sectionName, elapsed.count());
	SUCCEED("Benchmark completed for " + sectionName + ": " + std::to_string(elapsed.count()) + " ms");
}

TEST_CASE("ECS Benchmark", "[ECS]")
{
	struct TestComponent {
		int value;
	};

	struct AnotherComponent {
		float value;
	};

	class MockSystem : public System {
	  public:
		bool updateCalled = false;

		void update(Easys::ECS &ecs, double deltaTime) override { updateCalled = true; }
	};

	SECTION("Benchmarking Entity Addition")
	{
		Easys::ECS ecs;

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.addEntity();
			    }
		    },
		    formatEntCompInfo("addEntity", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Entity Removal")
	{
		Easys::ECS ecs;

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.removeEntity(i);
			    }
		    },
		    formatEntCompInfo("removeEntity", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Addition")
	{
		Easys::ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.addComponent<TestComponent>(i, c);
			    }
		    },
		    formatEntCompInfo("addComponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Addition 2")
	{
		Easys::ECS ecs;
		TestComponent c = TestComponent{};
		AnotherComponent a = AnotherComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.addComponent<TestComponent>(i, c);
				    ecs.addComponent<AnotherComponent>(i, a);
			    }
		    },
		    formatEntCompInfo("addComponent", NUM_ENT, NUM_COM*2));
	}

	SECTION("Benchmarking Component Removal")
	{
		Easys::ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<TestComponent>(e, c);
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.removeComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("removeComponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Retrieval")
	{
		Easys::ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<TestComponent>(e, c);
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.getComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("getComponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Existence Check")
	{
		Easys::ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
			ecs.addComponent<TestComponent>(e, c);
		}

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.hasComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("hascomponent", NUM_ENT, NUM_COM));
	}

	SECTION("Benchmarking Component Existence Check")
	{
		Easys::ECS ecs;
		TestComponent c = TestComponent{};

		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity();
		}
		ecs.addComponent<TestComponent>(0, c);

		benchmarkSection(
		    [&] {
			    for (int i = 0; i < NUM_ENT; i++) {
				    ecs.hasComponent<TestComponent>(i);
			    }
		    },
		    formatEntCompInfo("hasComponent", NUM_ENT, 0));
	}

	SECTION("Benchmarking simulation with multiple components per entity and systems")
	{
		Easys::ECS ecs;
		Positionable p = Positionable{};
		RigidBody r = RigidBody{};

		struct TestPhysicsSystem : public System {
			void update(Easys::ECS &ecs, double deltaTime)
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Positionable>(entity)) {
						auto &position = ecs.getComponent<Positionable>(entity).position;
						auto &rigidBody = ecs.getComponent<RigidBody>(entity);
						rigidBody.nextPosition = position + 1;
					}
				}
			}
		};

		struct TestUpdateSystem : public System {
			void update(Easys::ECS &ecs, double deltaTime)
			{
				for (const Entity &entity : ecs.getEntities()) {
					if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Positionable>(entity)) {
						auto &position = ecs.getComponent<Positionable>(entity).position;
						auto &rigidBody = ecs.getComponent<RigidBody>(entity);
						position = rigidBody.nextPosition;
					}
				}
			}
		};

		TestPhysicsSystem testPhysicsSystem = TestPhysicsSystem();
		TestUpdateSystem testUpdateSystem = TestUpdateSystem();
		double deltaTime = 0.0;
		
		for (int i = 0; i < NUM_ENT; i++) {
			Entity e = ecs.addEntity(); 
			ecs.addComponent<Positionable>(e, p);
			ecs.addComponent<RigidBody>(e, r);
		}

		benchmarkSection(
		    [&] {
			    testPhysicsSystem.update(ecs, deltaTime);
			    testUpdateSystem.update(ecs, deltaTime);
			    testPhysicsSystem.update(ecs, deltaTime);
			    testUpdateSystem.update(ecs, deltaTime);
			    testPhysicsSystem.update(ecs, deltaTime);
			    testUpdateSystem.update(ecs, deltaTime);
		    },
		    formatEntCompInfo("im, 2 systems, 3 updates", NUM_ENT, NUM_COM*2));
	}
}

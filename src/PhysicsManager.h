#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <btBulletDynamicsCommon.h>
#include <vector>
#include <raylib.h>

// Physics manager using Bullet
class PhysicsManager {
public:
    PhysicsManager();
    ~PhysicsManager();

    // Initialize world
    void Init();

    // Add static body for BSP geometry (simplified as boxes)
    void AddStaticBody(const Vector3& position, const Vector3& size);

    // Add player body
    btRigidBody* AddPlayerBody(const Vector3& position);

    // Update physics simulation
    void Update(float deltaTime);

    // Get player position from physics
    Vector3 GetPlayerPosition(btRigidBody* playerBody);

    // Set player velocity
    void SetPlayerVelocity(btRigidBody* playerBody, const Vector3& velocity);

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
};

#endif // PHYSICS_MANAGER_H
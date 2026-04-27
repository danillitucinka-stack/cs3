#include "PhysicsManager.h"

// Constructor
PhysicsManager::PhysicsManager() :
    collisionConfiguration(nullptr),
    dispatcher(nullptr),
    overlappingPairCache(nullptr),
    solver(nullptr),
    dynamicsWorld(nullptr) {
}

// Destructor
PhysicsManager::~PhysicsManager() {
    if (dynamicsWorld) {
        delete dynamicsWorld;
    }
    if (solver) {
        delete solver;
    }
    if (overlappingPairCache) {
        delete overlappingPairCache;
    }
    if (dispatcher) {
        delete dispatcher;
    }
    if (collisionConfiguration) {
        delete collisionConfiguration;
    }
}

// Initialize world
void PhysicsManager::Init() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
}

// Add static body for BSP geometry
void PhysicsManager::AddStaticBody(const Vector3& position, const Vector3& size) {
    btCollisionShape* groundShape = new btBoxShape(btVector3(size.x / 2, size.y / 2, size.z / 2));
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
}

// Add player body
btRigidBody* PhysicsManager::AddPlayerBody(const Vector3& position) {
    btCollisionShape* playerShape = new btCapsuleShape(0.5f, 1.8f); // Capsule for player
    btDefaultMotionState* playerMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));
    btScalar mass = 1.0f;
    btVector3 playerInertia(0, 0, 0);
    playerShape->calculateLocalInertia(mass, playerInertia);
    btRigidBody::btRigidBodyConstructionInfo playerRigidBodyCI(mass, playerMotionState, playerShape, playerInertia);
    btRigidBody* playerRigidBody = new btRigidBody(playerRigidBodyCI);
    dynamicsWorld->addRigidBody(playerRigidBody);
    return playerRigidBody;
}

// Update physics simulation
void PhysicsManager::Update(float deltaTime) {
    dynamicsWorld->stepSimulation(deltaTime, 10);
}

// Get player position from physics
Vector3 PhysicsManager::GetPlayerPosition(btRigidBody* playerBody) {
    btTransform trans;
    playerBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return {pos.getX(), pos.getY(), pos.getZ()};
}

// Set player velocity
void PhysicsManager::SetPlayerVelocity(btRigidBody* playerBody, const Vector3& velocity) {
    playerBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2008 Erwin Coumans  http://bulletphysics.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

// #include <stdio.h>
#include "LinearMath/btIDebugDraw.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btMultiSphereShape.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "LinearMath/btDefaultMotionState.h"
#include "hbrKinematicCharacterController.h"

// static helper method
static btVector3
getNormalizedVector(const btVector3 &v)
{
	btVector3 n(0, 0, 0);

	if (v.length() > SIMD_EPSILON)
	{
		n = v.normalized();
	}
	return n;
}

///@todo Interact with dynamic objects,
///Ride kinematicly animated platforms properly
///More realistic (or maybe just a config option) falling
/// -> Should integrate falling velocity manually and use that in stepDown()
///Support jumping
///Support ducking
class btKinematicClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
	btKinematicClosestNotMeRayResultCallback(btCollisionObject *me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_me)
			return 1.0;

		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

protected:
	btCollisionObject *m_me;
};

class btKinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	btKinematicClosestNotMeConvexResultCallback(btCollisionObject *me, const btVector3 &up, btScalar minSlopeDot)
		: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)), m_me(me), m_up(up), m_minSlopeDot(minSlopeDot)
	{
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return btScalar(1.0);

		if (!convexResult.m_hitCollisionObject->hasContactResponse())
			return btScalar(1.0);

		btVector3 hitNormalWorld;
		if (normalInWorldSpace)
		{
			hitNormalWorld = convexResult.m_hitNormalLocal;
		}
		else
		{
			///need to transform normal into worldspace
			hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;
		}

		btScalar dotUp = m_up.dot(hitNormalWorld);
		if (dotUp < m_minSlopeDot)
		{
			return btScalar(1.0);
		}

		return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
	}

protected:
	btCollisionObject *m_me;
	const btVector3 m_up;
	btScalar m_minSlopeDot;
};

class btKinematicClosestNotMeConvexResultCallback2 : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	btKinematicClosestNotMeConvexResultCallback2(btCollisionObject *me, const btVector3 &up, btScalar minSlopeDot)
		: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)), m_me(me), m_up(up), m_minSlopeDot(minSlopeDot)
	{
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return btScalar(1.0);

		if (!convexResult.m_hitCollisionObject->hasContactResponse())
			return btScalar(1.0);

		btVector3 hitNormalWorld;
		if (normalInWorldSpace)
		{
			hitNormalWorld = convexResult.m_hitNormalLocal;
		}
		else
		{
			///need to transform normal into worldspace
			hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;
		}

		return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
	}

protected:
	btCollisionObject *m_me;
	const btVector3 m_up;
	btScalar m_minSlopeDot;
};

/*
 * Returns the reflection direction of a ray going 'direction' hitting a surface with normal 'normal'
 *
 * from: http://www-cs-students.stanford.edu/~adityagp/final/node3.html
 */
btVector3 hbrKinematicCharacterController::computeReflectionDirection(const btVector3 &direction, const btVector3 &normal)
{
	return direction - (btScalar(2.0) * direction.dot(normal)) * normal;
}

/*
 * Returns the portion of 'direction' that is parallel to 'normal'
 */
btVector3 hbrKinematicCharacterController::parallelComponent(const btVector3 &direction, const btVector3 &normal)
{
	btScalar magnitude = direction.dot(normal);
	return normal * magnitude;
}

/*
 * Returns the portion of 'direction' that is perpindicular to 'normal'
 */
btVector3 hbrKinematicCharacterController::perpindicularComponent(const btVector3 &direction, const btVector3 &normal)
{
	return direction - parallelComponent(direction, normal);
}

hbrKinematicCharacterController::hbrKinematicCharacterController(btPairCachingGhostObject *ghostObject, btConvexShape *convexShape, btScalar stepHeight, const btVector3 &up)
{
	m_timeSinceGrounded = 0.0f;
	m_jumpOffset = 0.15;
	m_standingCollisionObject = NULL;
	m_standingPoint.setValue(0.0f, 1.0f, 0.0f);
	m_ghostObject = ghostObject;
	m_up.setValue(0.0f, 0.0f, 1.0f);
	m_jumpAxis.setValue(0.0f, 0.0f, 1.0f);
	m_addedMargin = 0.02;
	m_walkDirection.setValue(0.0f, 0.0f, 0.0f);
	m_AngVel.setValue(0.0, 0.0, 0.0);
	m_velocity.setValue(0.0, 0.0, 0.0);
	m_useGhostObjectSweepTest = true;
	m_turnAngle = btScalar(0.0);
	m_convexShape = convexShape;
	m_useWalkDirection = false; // use walk direction by default, legacy behavior
	m_velocityTimeInterval = 0.0;
	m_verticalVelocity = 0.0;
	m_verticalOffset = 0.0;
	m_gravity = 9.8 * 3.0; // 3G acceleration.
	m_fallSpeed = 55.0;	// Terminal velocity of a sky diver in m/s.
	m_jumpSpeed = 10.0;	// ?
	m_SetjumpSpeed = m_jumpSpeed;
	m_wasOnGround = false;
	m_wasJumping = false;
	m_interpolateUp = true;
	m_currentStepOffset = 0.0;
	m_maxPenetrationDepth = 0.2;
	full_drop = false;
	bounce_fix = false;
	m_linearDamping = btScalar(0.0);
	m_angularDamping = btScalar(0.0);

	m_prevVelocity.setValue(0.0, 0.0, 0.0);

	m_localVelocity.setValue(0.0, 0.0, 0.0);
	m_externalVelocity.setValue(0.0, 0.0, 0.0);
	m_velocity.setValue(0.0, 0.0, 0.0);
	m_moveOffset.setValue(0.0, 0.0, 0.0);
	m_acceleration.setValue(0.0, 0.0, 0.0);

	m_groundNormal.setValue(0.0, 1.0, 0.0);

	m_walkMaxSpeed = btScalar(5.0);
	m_runMaxSpeed = btScalar(5.0);
	m_airMaxSpeed = btScalar(5.0);
	m_flyMaxSpeed = btScalar(5.0);

	m_walkAcceleration = btScalar(5.0);
	m_runAcceleration = btScalar(5.0);
	m_airAcceleration = btScalar(5.0);
	m_flyAcceleration = btScalar(5.0);

	m_friction = 0.1;
	m_drag = 0.01;
	m_currentSpeed = 0.0;
	m_isAirWalking = false;
	m_speedModifier = 1.0;
	m_deAccelerationMultiplier = 0.9;

	setUp(up);
	setStepHeight(stepHeight);
	setMaxSlope(btRadians(45.0));
}

hbrKinematicCharacterController::~hbrKinematicCharacterController()
{
}

btPairCachingGhostObject *hbrKinematicCharacterController::getGhostObject()
{
	return m_ghostObject;
}

bool hbrKinematicCharacterController::recoverFromPenetration(btCollisionWorld *collisionWorld)
{
	// Here we must refresh the overlapping paircache as the penetrating movement itself or the
	// previous recovery iteration might have used setWorldTransform and pushed us into an object
	// that is not in the previous cache contents from the last timestep, as will happen if we
	// are pushed into a new AABB overlap. Unhandled this means the next convex sweep gets stuck.
	//
	// Do this by calling the broadphase's setAabb with the moved AABB, this will update the broadphase
	// paircache and the ghostobject's internal paircache at the same time.    /BW

	btVector3 minAabb, maxAabb;
	m_convexShape->getAabb(m_ghostObject->getWorldTransform(), minAabb, maxAabb);
	collisionWorld->getBroadphase()->setAabb(m_ghostObject->getBroadphaseHandle(),
											 minAabb,
											 maxAabb,
											 collisionWorld->getDispatcher());

	bool penetration = false;

	collisionWorld->getDispatcher()->dispatchAllCollisionPairs(m_ghostObject->getOverlappingPairCache(), collisionWorld->getDispatchInfo(), collisionWorld->getDispatcher());

	m_currentPosition = m_ghostObject->getWorldTransform().getOrigin();

	//	btScalar maxPen = btScalar(0.0);
	for (int i = 0; i < m_ghostObject->getOverlappingPairCache()->getNumOverlappingPairs(); i++)
	{
		m_manifoldArray.resize(0);

		btBroadphasePair *collisionPair = &m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray()[i];

		btCollisionObject *obj0 = static_cast<btCollisionObject *>(collisionPair->m_pProxy0->m_clientObject);
		btCollisionObject *obj1 = static_cast<btCollisionObject *>(collisionPair->m_pProxy1->m_clientObject);

		if ((obj0 && !obj0->hasContactResponse()) || (obj1 && !obj1->hasContactResponse()))
			continue;

		if (!needsCollision(obj0, obj1))
			continue;

		if (collisionPair->m_algorithm)
			collisionPair->m_algorithm->getAllContactManifolds(m_manifoldArray);

		for (int j = 0; j < m_manifoldArray.size(); j++)
		{
			btPersistentManifold *manifold = m_manifoldArray[j];
			btScalar directionSign = manifold->getBody0() == m_ghostObject ? btScalar(-1.0) : btScalar(1.0);
			for (int p = 0; p < manifold->getNumContacts(); p++)
			{
				const btManifoldPoint &pt = manifold->getContactPoint(p);

				btScalar dist = pt.getDistance();

				if (dist < -m_maxPenetrationDepth)
				{
					// TODO: cause problems on slopes, not sure if it is needed
					//if (dist < maxPen)
					//{
					//	maxPen = dist;
					//	m_touchingNormal = pt.m_normalWorldOnB * directionSign;//??

					//}
					m_currentPosition += pt.m_normalWorldOnB * directionSign * dist * btScalar(0.2);
					penetration = true;
				}
				else
				{
					//printf("touching %f\n", dist);
				}
			}

			//manifold->clearManifold();
		}
	}
	btTransform newTrans = m_ghostObject->getWorldTransform();
	newTrans.setOrigin(m_currentPosition);
	m_ghostObject->setWorldTransform(newTrans);
	//	printf("m_touchingNormal = %f,%f,%f\n",m_touchingNormal[0],m_touchingNormal[1],m_touchingNormal[2]);
	return penetration;
}

void hbrKinematicCharacterController::stepUp(btCollisionWorld *world)
{
	btScalar stepHeight = 0.0f;
	if (m_verticalVelocity < 0.0f)
		stepHeight = m_stepHeight;

	// phase 1: up
	btTransform start, end;

	start.setIdentity();
	end.setIdentity();

	/* FIXME: Handle penetration properly */
	start.setOrigin(m_currentPosition);

	m_targetPosition = m_currentPosition + m_up * stepHeight; // + m_jumpAxis * ((m_verticalOffset > 0.f ? m_verticalOffset : 0.f));

	end.setOrigin(m_targetPosition);

	start.setRotation(m_currentOrientation);
	end.setRotation(m_targetOrientation);

	btKinematicClosestNotMeConvexResultCallback callback(m_ghostObject, -m_up, m_maxSlopeCosine);
	callback.m_collisionFilterGroup = getGhostObject()->getBroadphaseHandle()->m_collisionFilterGroup;
	callback.m_collisionFilterMask = getGhostObject()->getBroadphaseHandle()->m_collisionFilterMask;

	if (m_useGhostObjectSweepTest)
	{
		m_ghostObject->convexSweepTest(m_convexShape, start, end, callback, world->getDispatchInfo().m_allowedCcdPenetration);
	}
	else
	{
		world->convexSweepTest(m_convexShape, start, end, callback, world->getDispatchInfo().m_allowedCcdPenetration);
	}

	if (callback.hasHit() && m_ghostObject->hasContactResponse() && needsCollision(m_ghostObject, callback.m_hitCollisionObject))
	{
		// Only modify the position if the hit was a slope and not a wall or ceiling.
		// we moved up only a fraction of the step height
		m_currentStepOffset = stepHeight * callback.m_closestHitFraction;
		m_currentPosition.setInterpolate3(m_currentPosition, m_targetPosition, callback.m_closestHitFraction);

		btTransform &xform = m_ghostObject->getWorldTransform();
		xform.setOrigin(m_currentPosition);
		m_ghostObject->setWorldTransform(xform);

		// fix penetration if we hit a ceiling for example
		int numPenetrationLoops = 0;
		m_touchingContact = false;
		while (recoverFromPenetration(world))
		{
			numPenetrationLoops++;
			m_touchingContact = true;
			if (numPenetrationLoops > 4)
			{
				//printf("character could not recover from penetration = %d\n", numPenetrationLoops);
				break;
			}
		}
		m_targetPosition = m_ghostObject->getWorldTransform().getOrigin();
		m_currentPosition = m_targetPosition;

		if (m_verticalOffset > 0)
		{
			m_verticalOffset = 0.0;
			m_verticalVelocity = 0.0;
			// m_currentStepOffset = m_stepHeight;
		}
	}
	else
	{
		m_currentStepOffset = stepHeight;
		m_currentPosition = m_targetPosition;
	}
}

bool hbrKinematicCharacterController::needsCollision(const btCollisionObject *body0, const btCollisionObject *body1)
{
	bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
	collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);
	return collides;
}

void hbrKinematicCharacterController::updateTargetPositionBasedOnCollision(const btVector3 &hitNormal, btScalar tangentMag, btScalar normalMag)
{
	btVector3 movementDirection = m_targetPosition - m_currentPosition;
	btScalar movementLength = movementDirection.length();
	if (movementLength > SIMD_EPSILON)
	{
		movementDirection.normalize();

		btVector3 reflectDir = computeReflectionDirection(movementDirection, hitNormal);
		reflectDir.normalize();

		btVector3 parallelDir, perpindicularDir;

		parallelDir = parallelComponent(reflectDir, hitNormal);
		perpindicularDir = perpindicularComponent(reflectDir, hitNormal);

		m_targetPosition = m_currentPosition;
		if (0) //tangentMag != 0.0)
		{
			btVector3 parComponent = parallelDir * btScalar(tangentMag * movementLength);
			//			printf("parComponent=%f,%f,%f\n",parComponent[0],parComponent[1],parComponent[2]);
			m_targetPosition += parComponent;
		}

		if (normalMag != 0.0)
		{
			btVector3 perpComponent = perpindicularDir * btScalar(normalMag * movementLength);
			// printf("moveOffset=%f,%f,%f\n",m_moveOffset[0],m_moveOffset[1],m_moveOffset[2]);
			// printf("perpComponent=%f,%f,%f\n",perpComponent[0],perpComponent[1],perpComponent[2]);
			m_targetPosition += perpComponent;
		}
	}
	else
	{
		//		printf("movementLength don't normalize a zero vector\n");
	}
}

void hbrKinematicCharacterController::stepForwardAndStrafe(btCollisionWorld *collisionWorld, const btVector3 &walkMove)
{
	// printf("m_normalizedDirection=%f,%f,%f\n",
	// 	m_normalizedDirection[0],m_normalizedDirection[1],m_normalizedDirection[2]);
	// phase 2: forward and strafe
	btTransform start, end;

	m_targetPosition = m_currentPosition + walkMove;

	start.setIdentity();
	end.setIdentity();

	btScalar fraction = 1.0;
	btScalar distance2 = (m_currentPosition - m_targetPosition).length2();
	//	printf("distance2=%f\n",distance2);

	int maxIter = 10;

	bool updatePosition = false;

	while (fraction > btScalar(0.01) && maxIter-- > 0)
	{
		start.setOrigin(m_currentPosition);
		end.setOrigin(m_targetPosition);
		btVector3 sweepDirNegative(m_currentPosition - m_targetPosition);

		start.setRotation(m_currentOrientation);
		end.setRotation(m_targetOrientation);

		btKinematicClosestNotMeConvexResultCallback callback(m_ghostObject, sweepDirNegative, btScalar(0.0));
		callback.m_collisionFilterGroup = getGhostObject()->getBroadphaseHandle()->m_collisionFilterGroup;
		callback.m_collisionFilterMask = getGhostObject()->getBroadphaseHandle()->m_collisionFilterMask;

		btScalar margin = m_convexShape->getMargin();
		m_convexShape->setMargin(margin + m_addedMargin);

		if (!(start == end))
		{
			if (m_useGhostObjectSweepTest)
			{
				m_ghostObject->convexSweepTest(m_convexShape, start, end, callback, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);
			}
			else
			{
				collisionWorld->convexSweepTest(m_convexShape, start, end, callback, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);
			}
		}
		m_convexShape->setMargin(margin);

		fraction -= callback.m_closestHitFraction;

		updatePosition = true;
		if (callback.hasHit() && m_ghostObject->hasContactResponse() && needsCollision(m_ghostObject, callback.m_hitCollisionObject))
		{
			// we moved only a fraction
			//btScalar hitDistance;
			//hitDistance = (callback.m_hitPointWorld - m_currentPosition).length();

			//			m_currentPosition.setInterpolate3 (m_currentPosition, m_targetPosition, callback.m_closestHitFraction);

			// printf("HasHit(%s)\n", callback.hasHit() ? "true" : "false");
			// printf("m_hitNormalWorld(%f,%f,%f)\n", callback.m_hitNormalWorld[0], callback.m_hitNormalWorld[1], callback.m_hitNormalWorld[2]);
			// printf("Dot(%f)\n", callback.m_hitNormalWorld.dot(m_up));

			updateTargetPositionBasedOnCollision(callback.m_hitNormalWorld);
			btVector3 currentDir = m_targetPosition - m_currentPosition;
			distance2 = currentDir.length2();

			// printf("dir(%f,%f,%f)\n", currentDir[0],currentDir[1],currentDir[2]);

			if (distance2 > SIMD_EPSILON)
			{
				currentDir.normalize();
				/* See Quake2: "If velocity is against original velocity, stop ead to avoid tiny oscilations in sloping corners." */
				if (currentDir.dot(m_normalizedDirection) <= btScalar(0.0))
				{
					// updatePosition = false;
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			m_currentPosition = m_targetPosition;
		}
	}

	if (updatePosition)
	{
		m_currentPosition = m_targetPosition;
	}
}

void hbrKinematicCharacterController::stepDown(btCollisionWorld *collisionWorld, btScalar dt)
{
	// btTransform start, end, end_double;
	// btVector3 orig_position = m_targetPosition;
	// btScalar downVelocity = (m_verticalVelocity < 0.f ? -m_verticalVelocity : 0.f) * dt;

	// printf("m_currentStepOffset: %f\n", m_currentStepOffset);

	btTransform start, end, end_double;
	bool runonce = false;

	// phase 3: down
	/*btScalar additionalDownStep = (m_wasOnGround && !onGround()) ? m_stepHeight : 0.0;
	btVector3 step_drop = m_up * (m_currentStepOffset + additionalDownStep);
	btScalar downVelocity = (additionalDownStep == 0.0 && m_verticalVelocity<0.0?-m_verticalVelocity:0.0) * dt;
	btVector3 gravity_drop = m_up * downVelocity; 
	m_targetPosition -= (step_drop + gravity_drop);*/

	m_targetPosition = m_currentPosition;

	btVector3 orig_position = m_targetPosition;

	btScalar downVelocity = (m_verticalVelocity < 0.f ? -m_verticalVelocity : 0.f) * dt;

	if (m_verticalVelocity > 0.0)
	{
		btKinematicClosestNotMeConvexResultCallback callback(m_ghostObject, m_up, m_maxSlopeCosine);
		callback.m_collisionFilterGroup = getGhostObject()->getBroadphaseHandle()->m_collisionFilterGroup;
		callback.m_collisionFilterMask = getGhostObject()->getBroadphaseHandle()->m_collisionFilterMask;

		btVector3 step_drop = m_up * dt;
		m_targetPosition -= step_drop;

		start.setIdentity();
		end.setIdentity();

		start.setOrigin(m_currentPosition);
		end.setOrigin(m_targetPosition);

		m_ghostObject->convexSweepTest(m_convexShape, start, end, callback, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);

		if (callback.hasHit() && m_ghostObject->hasContactResponse())
		{
			// printf("m_hitNormalWorld(%f,%f,%f)\n", callback.m_hitNormalWorld[0], callback.m_hitNormalWorld[1], callback.m_hitNormalWorld[2]);
			m_groundNormal = callback.m_hitNormalWorld;

			m_wasJumping = false;
			m_onGround = true;
		}

		return;
	}

	if (downVelocity > 0.0 && downVelocity > m_fallSpeed && (m_wasOnGround || !m_wasJumping))
		downVelocity = m_fallSpeed;

	btVector3 step_drop = m_up * (m_currentStepOffset + downVelocity);
	m_targetPosition -= step_drop;

	btKinematicClosestNotMeConvexResultCallback callback(m_ghostObject, m_up, m_maxSlopeCosine);
	callback.m_collisionFilterGroup = getGhostObject()->getBroadphaseHandle()->m_collisionFilterGroup;
	callback.m_collisionFilterMask = getGhostObject()->getBroadphaseHandle()->m_collisionFilterMask;

	btKinematicClosestNotMeConvexResultCallback callback2(m_ghostObject, m_up, m_maxSlopeCosine);
	callback2.m_collisionFilterGroup = getGhostObject()->getBroadphaseHandle()->m_collisionFilterGroup;
	callback2.m_collisionFilterMask = getGhostObject()->getBroadphaseHandle()->m_collisionFilterMask;

	while (1)
	{
		start.setIdentity();
		end.setIdentity();

		end_double.setIdentity();

		start.setOrigin(m_currentPosition);
		end.setOrigin(m_targetPosition);

		start.setRotation(m_currentOrientation);
		end.setRotation(m_targetOrientation);

		//set double test for 2x the step drop, to check for a large drop vs small drop
		end_double.setOrigin(m_targetPosition - step_drop);

		if (1 == 0 && m_useGhostObjectSweepTest)
		{
			m_ghostObject->convexSweepTest(m_convexShape, start, end, callback, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);

			if (!callback.hasHit() && m_ghostObject->hasContactResponse())
			{
				//test a double fall height, to see if the character should interpolate it's fall (full) or not (partial)
				m_ghostObject->convexSweepTest(m_convexShape, start, end_double, callback2, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);
			}
		}
		else
		{
			collisionWorld->convexSweepTest(m_convexShape, start, end, callback, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);

			if (!callback.hasHit() && m_ghostObject->hasContactResponse())
			{
				//test a double fall height, to see if the character should interpolate it's fall (large) or not (small)
				collisionWorld->convexSweepTest(m_convexShape, start, end_double, callback2, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);
			}
		}

		btScalar downVelocity2 = (m_verticalVelocity < 0.f ? -m_verticalVelocity : 0.f) * dt;
		bool has_hit;
		if (bounce_fix == true)
			has_hit = (callback.hasHit() || callback2.hasHit()) && m_ghostObject->hasContactResponse() && needsCollision(m_ghostObject, callback.m_hitCollisionObject);
		else
			has_hit = callback2.hasHit() && m_ghostObject->hasContactResponse() && needsCollision(m_ghostObject, callback2.m_hitCollisionObject);

		btScalar stepHeight = 0.0f;
		if (m_verticalVelocity < 0.0)
			stepHeight = m_stepHeight;

		if (downVelocity2 > 0.0 && downVelocity2 < stepHeight && has_hit == true && runonce == false && (m_wasOnGround || !m_wasJumping))
		{
			//redo the velocity calculation when falling a small amount, for fast stairs motion
			//for larger falls, use the smoother/slower interpolated movement by not touching the target position

			m_targetPosition = orig_position;
			downVelocity = stepHeight;

			step_drop = m_up * (m_currentStepOffset + downVelocity);
			m_targetPosition -= step_drop;
			runonce = true;
			continue; //re-run previous tests
		}
		break;
	}

	if ((m_ghostObject->hasContactResponse() && (callback.hasHit() && needsCollision(m_ghostObject, callback.m_hitCollisionObject))) || runonce == true)
	{
		// we dropped a fraction of the height -> hit floor
		btScalar fraction = (m_currentPosition.getY() - callback.m_hitPointWorld.getY()) / 2;

		//printf("hitpoint: %g - pos %g\n", callback.m_hitPointWorld.getY(), m_currentPosition.getY());

		if (bounce_fix == true)
		{
			if (full_drop == true)
				m_currentPosition.setInterpolate3(m_currentPosition, m_targetPosition, callback.m_closestHitFraction);
			else
				//due to errors in the closestHitFraction variable when used with large polygons, calculate the hit fraction manually
				m_currentPosition.setInterpolate3(m_currentPosition, m_targetPosition, fraction);
		}
		else
			m_currentPosition.setInterpolate3(m_currentPosition, m_targetPosition, callback.m_closestHitFraction);

		full_drop = false;

		m_verticalVelocity = 0.0;
		m_verticalOffset = 0.0;
		m_wasJumping = false;
		m_onGround = true;
		m_standingCollisionObject = callback.m_hitCollisionObject;
		m_standingPoint = callback.m_hitPointWorld;
		m_groundNormal = callback.m_hitNormalWorld;
	}
	else
	{
		// we dropped the full height

		full_drop = true;

		if (bounce_fix == true)
		{
			downVelocity = (m_verticalVelocity < 0.f ? -m_verticalVelocity : 0.f) * dt;
			if (downVelocity > m_fallSpeed && (m_wasOnGround || !m_wasJumping))
			{
				m_targetPosition += step_drop; //undo previous target change
				downVelocity = m_fallSpeed;
				step_drop = m_up * (m_currentStepOffset + downVelocity);
				m_targetPosition -= step_drop;
			}
		}
		// printf("full drop - %g, %g\n", m_currentPosition.getY(), m_targetPosition.getY());

		m_currentPosition = m_targetPosition;
	}
}

void hbrKinematicCharacterController::setWalkDirection(
	const btVector3 &walkDirection)
{
	m_useWalkDirection = false;
	m_walkDirection = walkDirection;
	m_normalizedDirection = getNormalizedVector(m_walkDirection);
}

void hbrKinematicCharacterController::setVelocityForTimeInterval(
	const btVector3 &velocity,
	btScalar timeInterval)
{
	//	printf("setVelocity!\n");
	//	printf("  interval: %f\n", timeInterval);
	//	printf("  velocity: (%f, %f, %f)\n",
	//		 velocity.x(), velocity.y(), velocity.z());

	m_useWalkDirection = false;
	m_walkDirection = velocity;
	m_normalizedDirection = getNormalizedVector(m_walkDirection);
	m_velocityTimeInterval += timeInterval;
}

void hbrKinematicCharacterController::setAngularVelocity(const btVector3 &velocity)
{
	m_AngVel = velocity;
}

const btVector3 &hbrKinematicCharacterController::getAngularVelocity() const
{
	return m_AngVel;
}

void hbrKinematicCharacterController::setLinearVelocity(const btVector3 &velocity)
{
	m_localVelocity = velocity;

	// m_walkDirection = velocity;

	// // HACK: if we are moving in the direction of the up, treat it as a jump :(
	// if (m_walkDirection.length2() > 0)
	// {
	// 	btVector3 w = velocity.normalized();
	// 	btScalar c = w.dot(m_up);
	// 	if (c != 0)
	// 	{
	// 		//there is a component in walkdirection for vertical velocity
	// 		btVector3 upComponent = m_up * (btSin(SIMD_HALF_PI - btAcos(c)) * m_walkDirection.length());
	// 		m_walkDirection -= upComponent;
	// 		m_verticalVelocity = (c < 0.0f ? -1 : 1) * upComponent.length();

	// 		if (c > 0.0f)
	// 		{
	// 			m_wasJumping = true;
	// 			m_jumpPosition = m_ghostObject->getWorldTransform().getOrigin();
	// 		}
	// 	}
	// }
	// else
	// 	m_verticalVelocity = 0.0f;
}

btVector3 hbrKinematicCharacterController::getLinearVelocity() const
{
	return m_localVelocity + m_externalVelocity;
}

btVector3 hbrKinematicCharacterController::getLocalLinearVelocity() const
{
	return m_localVelocity;
}

void hbrKinematicCharacterController::reset(btCollisionWorld *collisionWorld)
{
	m_verticalVelocity = 0.0;
	m_verticalOffset = 0.0;
	m_wasOnGround = false;
	m_wasJumping = false;
	m_walkDirection.setValue(0, 0, 0);
	m_velocityTimeInterval = 0.0;

	//clear pair cache
	btHashedOverlappingPairCache *cache = m_ghostObject->getOverlappingPairCache();
	while (cache->getOverlappingPairArray().size() > 0)
	{
		cache->removeOverlappingPair(cache->getOverlappingPairArray()[0].m_pProxy0, cache->getOverlappingPairArray()[0].m_pProxy1, collisionWorld->getDispatcher());
	}
}

void hbrKinematicCharacterController::warp(const btVector3 &origin)
{
	btTransform xform;
	xform.setIdentity();
	xform.setOrigin(origin);
	m_ghostObject->setWorldTransform(xform);
}

void hbrKinematicCharacterController::applyExternalVelocity()
{
	btScalar maxVelocity = m_externalVelocity.length();

	if (maxVelocity > 0.0)
	{
		// btVector3 externalDir = m_externalVelocity;
		// externalDir.normalize();

		// btScalar projVel = m_localVelocity.dot(externalDir);
		// btScalar accelVel = btMax(maxVelocity - projVel, 0.0f);

		// printf("ExternalVelocity(%f,%f,%f)\n", m_externalVelocity[0], m_externalVelocity[1], m_externalVelocity[2]);
		// printf("ExternalVelocity2(%f,%f,%f)\n", externalDir[0] * accelVel, externalDir[1] * accelVel, externalDir[2] * accelVel);

		m_localVelocity += m_externalVelocity; //externalDir * accelVel;
		m_externalVelocity.setZero();

		// printf("Velocity(%f,%f,%f)\n", m_localVelocity[0], m_localVelocity[1], m_localVelocity[2]);
		// printf("projVel(%f)\n", projVel);
		// printf("accelVel(%f)\n", accelVel);
		// printf("maxVelocity(%f)\n", maxVelocity);
	}
}

void hbrKinematicCharacterController::preStep(btCollisionWorld *collisionWorld)
{
	m_currentPosition = m_ghostObject->getWorldTransform().getOrigin();
	m_targetPosition = m_currentPosition;

	m_currentOrientation = m_ghostObject->getWorldTransform().getRotation();
	m_targetOrientation = m_currentOrientation;

	//	printf("m_targetPosition=%f,%f,%f\n",m_targetPosition[0],m_targetPosition[1],m_targetPosition[2]);
}

void hbrKinematicCharacterController::preUpdate(btCollisionWorld *collisionWorld, btScalar deltaTime)
{
}

void hbrKinematicCharacterController::playerStep(btCollisionWorld *collisionWorld, btScalar dt)
{
	//	printf("playerStep(): ");
	//	printf("  dt = %f", dt);

	if (m_AngVel.length2() > 0.0f)
	{
		m_AngVel *= btPow(btScalar(1) - m_angularDamping, dt);
	}

	// integrate for angular velocity
	if (m_AngVel.length2() > 0.0f)
	{
		btTransform xform;
		xform = m_ghostObject->getWorldTransform();

		btQuaternion rot(m_AngVel.normalized(), m_AngVel.length() * dt);

		btQuaternion orn = rot * xform.getRotation();

		xform.setRotation(orn);
		m_ghostObject->setWorldTransform(xform);

		m_currentPosition = m_ghostObject->getWorldTransform().getOrigin();
		m_targetPosition = m_currentPosition;
		m_currentOrientation = m_ghostObject->getWorldTransform().getRotation();
		m_targetOrientation = m_currentOrientation;
	}

	m_prevVelocity = m_localVelocity;
	m_wasOnGround = m_onGround;

	m_onGround = false;

	btScalar frictionMultiplier = 1.0;
	if (m_standingCollisionObject)
	{
		frictionMultiplier = m_standingCollisionObject->getFriction();
	}

	inheritVelocity(collisionWorld, dt);

	if (!m_wasOnGround && m_externalVelocity.length2() > 0.0)
	{
		if (m_wasJumping && m_externalVelocity.y() < 0.0)
		{
			m_externalVelocity.setY(0.0);
		}

		if (!m_wasJumping)
		{
			applyExternalVelocity();
		}

		m_externalVelocity.setZero();
	}

	m_localVelocity *= btPow(btScalar(1) - m_linearDamping, dt);

	// btScalar friction = (m_wasOnGround ? m_friction : m_drag);
	// btScalar frictionMagnitude = 0.0;

	btScalar speed = m_localVelocity.length();
	btScalar accelerate = m_speedModifier * (m_wasOnGround ? m_walkAcceleration : m_airAcceleration);
	btScalar maxVelocity = m_speedModifier * (m_wasOnGround ? m_walkMaxSpeed : m_airMaxSpeed);

	if (m_wasOnGround && !m_wasJumping && m_localVelocity.length2() > SIMD_EPSILON)
	{
		// btVector3 velDir = m_localVelocity.normalized();
		// velDir.setY(0.0);
		// btScalar velDot = velDir.dot(m_walkDirection);

		btVector3 groundFriction = -m_friction * m_localVelocity * frictionMultiplier;

		//Dont apply friction along walk direction
		// btVector3 groundFriction = m_localVelocity - m_localVelocity * btPow(btScalar(1.0) - m_friction, dt);
		groundFriction.setY(0.0f);

		btScalar dotProdWalk = m_walkDirection.dot(m_localVelocity);

		btVector3 frictionOffset(0.0f, 0.0f, 0.0f);
		if (m_walkDirection.length2() > SIMD_EPSILON)
		{
			frictionOffset = projectVectors(groundFriction, m_walkDirection);

			if (dotProdWalk < 0.0f)
			{
				frictionOffset *= 0.5f;
			}
			else if (speed > maxVelocity)
			{
				frictionOffset *= m_deAccelerationMultiplier;
			}
		}

		m_localVelocity += groundFriction - frictionOffset;

		// printf("friction(%f,%f,%f)\n", asd[0],asd[1],asd[2]);
		// frictionMagnitude = groundFriction.length();
		// printf("Dot=(%f)\n", velDot);
		// printf("Dot=(%f)\n", dotProdWalk);
	}
	else
	{
		btVector3 dragFriction = -m_drag * m_localVelocity;
		m_localVelocity += dragFriction;
	}

	btScalar projVel = m_localVelocity.dot(m_walkDirection);
	btScalar accelVel = accelerate * dt;

	if (projVel + accelVel > maxVelocity)
	{
		accelVel = btMax(maxVelocity - projVel, 0.0f);
	}

	btScalar gravity = m_isAirWalking ? 0.0 : m_gravity;

	m_acceleration += m_walkDirection * accelVel - gravity * m_up * dt;

	if (m_localVelocity[1] < 0.0 && btFabs(m_localVelocity[1]) > btFabs(m_fallSpeed))
	{
		m_acceleration.setY(0.0f);
	}

	m_localVelocity += m_acceleration;

	// if (m_localVelocity.y() > 0.0 && m_localVelocity.y() > m_jumpSpeed)
	// {
	// 	m_localVelocity.setY(m_jumpSpeed);
	// }

	// if (m_localVelocity.y() < 0.0 && btFabs(m_localVelocity.y()) > btFabs(m_fallSpeed))
	// {
	// 	m_localVelocity.setY(-btFabs(m_fallSpeed));
	// }

	m_moveOffset = m_localVelocity * dt + m_externalVelocity * dt;

	m_verticalVelocity = m_localVelocity.y();
	m_verticalOffset = m_moveOffset.y();

	// printf("m_externalVelocity(%f,%f,%f)\n", m_externalVelocity[0],m_externalVelocity[1],m_externalVelocity[2]);
	// printf("m_verticalVelocity=%f\n", m_verticalVelocity);

	btTransform xform;
	xform = m_ghostObject->getWorldTransform();

	m_jumpAxis.setValue(0.0, !m_isAirWalking, 0.0);

	m_currentSpeed = m_localVelocity.length();

	//	printf("walkDirection(%f,%f,%f)\n", m_walkDirection[0],m_walkDirection[1],m_walkDirection[2]);
	//	printf("walkSpeed=%f\n",walkSpeed);

	stepUp(collisionWorld);

	btVector3 currentPosition = m_currentPosition;

	btVector3 moveOffset = btVector3(m_moveOffset.x(), m_moveOffset.y(), m_moveOffset.z());
	stepForwardAndStrafe(collisionWorld, moveOffset);

	btVector3 deltaPosition = m_currentPosition - currentPosition;
	m_localVelocity = deltaPosition / dt - m_externalVelocity;

	// m_verticalVelocity = m_localVelocity[1];

	// printf("m_localVelocity(%f)\n", m_localVelocity[0]);

	// m_localVelocity.setX(deltaPosition.x() / dt - m_externalVelocity.x());
	// m_localVelocity.setZ(deltaPosition.z() / dt - m_externalVelocity.z());

	stepDown(collisionWorld, dt);

	if (m_onGround && m_localVelocity.y() < 0.0f)
	{
		m_localVelocity.setY(0.0);
	}

	if (!m_onGround)
	{
		m_timeSinceGrounded += dt;
	}
	else
	{
		m_timeSinceGrounded = 0.0f;
	}

	m_acceleration.setZero();

	// printf("moveOffset(%f,%f,%f)\n", m_moveOffset[0],m_moveOffset[1],m_moveOffset[2]);
	// printf("Delta(%f,%f,%f)\n", deltaPosition[0],deltaPosition[1],deltaPosition[2]);

	// printf("Velocity(%f,%f,%f)\n", m_localVelocity[0], m_localVelocity[1], m_localVelocity[2]);

	xform.setOrigin(m_currentPosition);
	m_ghostObject->setWorldTransform(xform);

	int numPenetrationLoops = 0;
	m_touchingContact = false;
	while (recoverFromPenetration(collisionWorld))
	{
		numPenetrationLoops++;
		m_touchingContact = true;
		if (numPenetrationLoops > 4)
		{
			//printf("character could not recover from penetration = %d\n", numPenetrationLoops);
			break;
		}
	}

	// testCollisions(collisionWorld);
}

void hbrKinematicCharacterController::inheritVelocity(btCollisionWorld *collisionWorld, btScalar dt)
{
	if (m_standingCollisionObject)
	{
		btVector3 worldPos = m_standingPoint + m_externalVelocity * dt * 0.5;

		btTransform transform = m_standingCollisionObject->getInterpolationWorldTransform();
		btVector3 linearVel = m_standingCollisionObject->getInterpolationLinearVelocity();
		btVector3 angularVelocity = m_standingCollisionObject->getInterpolationAngularVelocity();

		btVector3 localPosition = worldPos - transform.getOrigin();

		btVector3 newVelocity = angularVelocity.cross(localPosition) + linearVel;

		if (m_externalVelocity.y() - newVelocity.y() > m_fallSpeed)
		{
			// printf("FallSpeed = %f\n", m_fallSpeed);
			// printf("LimitExternal = %f\n", m_externalVelocity.y());
			// printf("VelIncreaseExternal = %f\n", m_externalVelocity.y() - newVelocity.y());
			newVelocity.setY(m_externalVelocity.y());
		}

		m_externalVelocity = newVelocity;

		m_standingCollisionObject = NULL;
	}
}

void hbrKinematicCharacterController::testCollisions(btCollisionWorld *collisionWorld)
{
	btManifoldArray manifoldArray;
	btBroadphasePairArray &pairArray = m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
	int numPairs = pairArray.size();

	for (int i = 0; i < numPairs; i++)
	{
		manifoldArray.clear();

		const btBroadphasePair &pair = pairArray[i];

		btBroadphasePair *collisionPair = collisionWorld->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

		if (collisionPair == NULL)
			continue;

		if (collisionPair->m_algorithm != NULL)
			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

		for (int j = 0; j < manifoldArray.size(); j++)
		{
			btPersistentManifold *pManifold = manifoldArray[j];

			// if(pManifold->getBody1() == m_ghostObject)
			// 	continue;

			if (btGhostObject::upcast(pManifold->getBody1()) != NULL)
				continue;

			bool isDynamic = !pManifold->getBody1()->isStaticOrKinematicObject();
			const btRigidBody *body = btRigidBody::upcast(pManifold->getBody0() == m_ghostObject ? pManifold->getBody1() : pManifold->getBody0());

			for (int p = 0; p < pManifold->getNumContacts(); p++)
			{
				const btManifoldPoint &point = pManifold->getContactPoint(p);

				if (point.getDistance() < 0.0f)
				{
					//const btVector3 &ptA = point.getPositionWorldOnA();
					const btVector3 &ptB = point.getPositionWorldOnB();

					//const btVector3 &normalOnB = point.m_normalWorldOnB;

					// If point is in rounded bottom region of capsule shape, it is on the ground

					// printf("Velocity(%f)\n", point.m_normalWorldOnB.dot(m_up));

					//point.m_normalWorldOnB.dot(m_up) > 0.0

					if (isDynamic && body)
					{
						btVector3 localPoint = pManifold->getBody0() == m_ghostObject ? point.m_localPointA : point.m_localPointB;

						btRigidBody *b = const_cast<btRigidBody *>(body);
						b->applyForce(m_currentSpeed * m_walkDirection * point.m_normalWorldOnB * -100.0, localPoint);

						// btVector3 velocityInPoint = b->getVelocityInLocalPoint(localPoint);

						// // m_acceleration += velocityInPoint / 60.0;

						// m_localVelocity += velocityInPoint / 2.0;
					}

					if (m_currentPosition.getY() - ptB.getY() > 0.9 - m_stepHeight)
					{
						// m_onGround = true;
					}
					else
					{
						//point.m_normalWorldOnB

						// btVector3 velInNormalDir = point.m_normalWorldOnB * m_localVelocity.dot(point.m_normalWorldOnB);

						// printf("normal(%f,%f,%f)\n", point.m_normalWorldOnB[0],point.m_normalWorldOnB[1],point.m_normalWorldOnB[2]);

						// m_localVelocity -= velInNormalDir * 1.05f;

						//m_localVelocity -= point.m_normalWorldOnB * m_localVelocity;

						// if(isDynamic){
						// 	btRigidBody* b = const_cast<btRigidBody*>(body);
						// 	b->applyForce(point.m_normalWorldOnB * 100.0, point.m_localPointB);
						// }
					}
				}
			}
		}
	}
}

void hbrKinematicCharacterController::setMaxWalkSpeed(btScalar speed)
{
	m_walkMaxSpeed = speed;
}
void hbrKinematicCharacterController::setMaxRunSpeed(btScalar speed)
{
	m_runMaxSpeed = speed;
}
void hbrKinematicCharacterController::setMaxAirSpeed(btScalar speed)
{
	m_airMaxSpeed = speed;
}
void hbrKinematicCharacterController::setMaxFlySpeed(btScalar speed)
{
	m_flyMaxSpeed = speed;
}

void hbrKinematicCharacterController::setWalkAcceleration(btScalar acceleration)
{
	m_walkAcceleration = acceleration;
}
void hbrKinematicCharacterController::setRunAcceleration(btScalar acceleration)
{
	m_runAcceleration = acceleration;
}
void hbrKinematicCharacterController::setAirAcceleration(btScalar acceleration)
{
	m_airAcceleration = acceleration;
}
void hbrKinematicCharacterController::setFlyAcceleration(btScalar acceleration)
{
	m_flyAcceleration = acceleration;
}

void hbrKinematicCharacterController::setFallSpeed(btScalar fallSpeed)
{
	m_fallSpeed = fallSpeed;
}

void hbrKinematicCharacterController::setJumpSpeed(btScalar jumpSpeed)
{
	m_jumpSpeed = jumpSpeed;
	m_SetjumpSpeed = m_jumpSpeed;
}

void hbrKinematicCharacterController::setMaxJumpHeight(btScalar maxJumpHeight)
{
	m_maxJumpHeight = maxJumpHeight;
}

bool hbrKinematicCharacterController::canJump() const
{
	return !m_wasJumping && m_jumpOffset >= m_timeSinceGrounded;
}

void hbrKinematicCharacterController::jump(const btVector3 &v)
{
	m_jumpSpeed = v.length2() == 0 ? m_SetjumpSpeed : v.length();
	m_verticalVelocity = m_jumpSpeed;
	m_wasJumping = true;

	m_jumpAxis = v.length2() == 0 ? m_up : v.normalized();

	m_jumpPosition = m_ghostObject->getWorldTransform().getOrigin();

	if (m_localVelocity.y() < 0.0)
	{
		m_localVelocity.setY(0.0);
	}

	// printf("m_hitNormalWorld(%f,%f,%f)\n", m_groundNormal[0], m_groundNormal[1], m_groundNormal[2]);

	m_externalVelocity.setY(btMax(0.0f, m_externalVelocity.y()));

	applyExternalVelocity();

	// btVector3 extraVelocity = projectVectors(m_prevVelocity, m_groundNormal);

	//btVector3 extraVelocity(0.0f, 0.0f, 0.0f);

	// && m_groundNormal.dot(m_localVelocity) >= 0.0f
	// if (m_localVelocity.length2() > 0.1f)
	// {
	// 	extraVelocity = m_prevVelocity.length() * m_groundNormal * 0.2f;
	// }

	// printf("m_prevVelocity(%f,%f,%f)\n", m_prevVelocity[0], m_prevVelocity[1], m_prevVelocity[2]);
	// printf("extraVelocity(%f,%f,%f)\n", extraVelocity[0], extraVelocity[1], extraVelocity[2]);
	// printf("m_localVelocity(%f,%f,%f)\n", m_localVelocity[0], m_localVelocity[1], m_localVelocity[2]);

	m_localVelocity += m_jumpAxis * m_verticalVelocity * m_speedModifier;// + extraVelocity;
#if 0
	currently no jumping.
	btTransform xform;
	m_rigidBody->getMotionState()->getWorldTransform (xform);
	btVector3 up = xform.getBasis()[1];
	up.normalize ();
	btScalar magnitude = (btScalar(1.0)/m_rigidBody->getInvMass()) * btScalar(8.0);
	m_rigidBody->applyCentralImpulse (up * magnitude);
#endif
}

void hbrKinematicCharacterController::setGravity(const btVector3 &gravity)
{
	if (gravity.length2() > 0)
		setUpVector(-gravity);

	m_gravity = gravity.length();
}

btVector3 hbrKinematicCharacterController::getGravity() const
{
	return -m_gravity * m_up;
}

void hbrKinematicCharacterController::setMaxSlope(btScalar slopeRadians)
{
	m_maxSlopeRadians = slopeRadians;
	m_maxSlopeCosine = btCos(slopeRadians);
}

btScalar hbrKinematicCharacterController::getMaxSlope() const
{
	return m_maxSlopeRadians;
}

void hbrKinematicCharacterController::setMaxPenetrationDepth(btScalar d)
{
	m_maxPenetrationDepth = d;
}

btScalar hbrKinematicCharacterController::getMaxPenetrationDepth() const
{
	return m_maxPenetrationDepth;
}

bool hbrKinematicCharacterController::onGround() const
{
	return m_onGround;
}

void hbrKinematicCharacterController::setStepHeight(btScalar h)
{
	m_stepHeight = h;
}

btVector3 *hbrKinematicCharacterController::getUpAxisDirections()
{
	static btVector3 sUpAxisDirection[3] = {btVector3(1.0f, 0.0f, 0.0f), btVector3(0.0f, 1.0f, 0.0f), btVector3(0.0f, 0.0f, 1.0f)};

	return sUpAxisDirection;
}

void hbrKinematicCharacterController::debugDraw(btIDebugDraw *debugDrawer)
{
}

void hbrKinematicCharacterController::setUpInterpolate(bool value)
{
	m_interpolateUp = value;
}

void hbrKinematicCharacterController::setUp(const btVector3 &up)
{
	if (up.length2() > 0 && m_gravity > 0.0f)
	{
		setGravity(-m_gravity * up.normalized());
		return;
	}

	setUpVector(up);
}

void hbrKinematicCharacterController::setUpVector(const btVector3 &up)
{
	if (m_up == up)
		return;

	btVector3 u = m_up;

	if (up.length2() > 0)
		m_up = up.normalized();
	else
		m_up = btVector3(0.0, 0.0, 0.0);

	if (!m_ghostObject)
		return;
	btQuaternion rot = getRotation(m_up, u);

	//set orientation with new up
	btTransform xform;
	xform = m_ghostObject->getWorldTransform();
	btQuaternion orn = rot.inverse() * xform.getRotation();
	xform.setRotation(orn);
	m_ghostObject->setWorldTransform(xform);
}

btQuaternion hbrKinematicCharacterController::getRotation(btVector3 &v0, btVector3 &v1) const
{
	if (v0.length2() == 0.0f || v1.length2() == 0.0f)
	{
		btQuaternion q;
		return q;
	}

	return shortestArcQuatNormalize2(v0, v1);
}

inline btVector3 hbrKinematicCharacterController::projectVectors(const btVector3 &v1, const btVector3 &v2) const
{
	float v2_ls = v2.length2();
	return v2 * (v2.dot(v1) / v2_ls);
}

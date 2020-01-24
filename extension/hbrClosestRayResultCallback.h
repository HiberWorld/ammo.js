
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

// #include <stdio.h>

struct hbrClosestRayResultCallback : public btCollisionWorld::RayResultCallback
	{
		hbrClosestRayResultCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld)
			: m_rayFromWorld(rayFromWorld),
			  m_rayToWorld(rayToWorld)
		{
		}

		btVector3 m_rayFromWorld;  //used to calculate hitPointWorld from hitFraction
		btVector3 m_rayToWorld;

		btVector3 m_hitNormalWorld;
		btVector3 m_hitPointWorld;

        

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
            printf("hitFraction %f\n", rayResult.m_hitFraction);

            if(rayResult.m_hitFraction > m_closestHitFraction){
                return m_closestHitFraction;
            }

			// m_closestHitFraction = rayResult.m_hitFraction;
			m_collisionObject = rayResult.m_collisionObject;
			if (normalInWorldSpace)
			{
				m_hitNormalWorld = rayResult.m_hitNormalLocal;
			}
			else
			{
				///need to transform normal into worldspace
				m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal;
			}
			m_hitPointWorld.setInterpolate3(m_rayFromWorld, m_rayToWorld, rayResult.m_hitFraction);

			return m_closestHitFraction;
		}
	};



    struct hbrAllHitsRayResultCallback : public btCollisionWorld::RayResultCallback
	{
		hbrAllHitsRayResultCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld)
			: m_rayFromWorld(rayFromWorld),
			  m_rayToWorld(rayToWorld),
              a_closestHitFraction(1.0)
		{
		}

		btVector3 m_rayFromWorld;  //used to calculate hitPointWorld from hitFraction
		btVector3 m_rayToWorld;

		btScalar a_closestHitFraction;
        btVector3 m_closestHitNormalWorld;
		btVector3 m_closestHitPointWorld;

        bool hasHit() const
		{
			return (a_closestHitFraction < btScalar(1.));
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			m_collisionObject = rayResult.m_collisionObject;
			btVector3 hitNormalWorld;
			if (normalInWorldSpace)
			{
				hitNormalWorld = rayResult.m_hitNormalLocal;
			}
			else
			{
				///need to transform normal into worldspace
				hitNormalWorld = m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal;
			}
			btVector3 hitPointWorld;
			hitPointWorld.setInterpolate3(m_rayFromWorld, m_rayToWorld, rayResult.m_hitFraction);

            if(a_closestHitFraction > rayResult.m_hitFraction){
                m_closestHitNormalWorld = hitNormalWorld;
                m_closestHitPointWorld = hitPointWorld;
                a_closestHitFraction = rayResult.m_hitFraction;
            }

			return m_closestHitFraction;
		}
	};
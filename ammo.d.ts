/* eslint-disable @typescript-eslint/no-explicit-any */
/* eslint-disable @typescript-eslint/no-use-before-define */
/* eslint-disable @typescript-eslint/camelcase */
/* eslint-disable @typescript-eslint/class-name-casing */

declare namespace Ammo {
function destroy(type: any): void;
function getPointer(obj: any): number;
type PHY_ScalarType = string;
type btConstraintParams = string;
export class btIDebugDraw {
	drawLine(from: btVector3, to: btVector3, color: btVector3): void;
	drawContactPoint(pointOnB: btVector3, normalOnB: btVector3, distance: number, lifeTime: number, color: btVector3): void;
	reportErrorWarning(warningString: string): void;
	draw3dText(location: btVector3, textString: string): void;
	setDebugMode(debugMode: number): void;
	getDebugMode(): number;
}

export class DebugDrawer {
	constructor();
	drawLine(from: btVector3, to: btVector3, color: btVector3): void;
	drawContactPoint(pointOnB: btVector3, normalOnB: btVector3, distance: number, lifeTime: number, color: btVector3): void;
	reportErrorWarning(warningString: string): void;
	draw3dText(location: btVector3, textString: string): void;
	setDebugMode(debugMode: number): void;
	getDebugMode(): number;
}

export class btVector3 {
	constructor();
	constructor(x: number, y: number, z: number);
	setZero(): void;
	isZero(): boolean;
	fuzzyZero(): boolean;
	distance(v: btVector3): number;
	distance2(v: btVector3): number;
	length(): number;
	length2(): number;
	x(): number;
	y(): number;
	z(): number;
	setX(x: number): void;
	setY(y: number): void;
	setZ(z: number): void;
	setValue(x: number, y: number, z: number): void;
	normalize(): void;
	cross(v: btVector3): btVector3;
	rotate(wAxis: btVector3, angle: number): btVector3;
	dot(v: btVector3): number;
	op_mul(x: number): btVector3;
	op_add(v: btVector3): btVector3;
	op_sub(v: btVector3): btVector3;
}

export class btVector4 extends btVector3  {
	constructor();
	constructor(x: number, y: number, z: number, w: number);
	w(): number;
	setValue(x: number, y: number, z: number, w: number): void;
}

export class btQuadWord {
	x(): number;
	y(): number;
	z(): number;
	w(): number;
	setX(x: number): void;
	setY(y: number): void;
	setZ(z: number): void;
	setW(w: number): void;
}

export class btQuaternion extends btQuadWord  {
	constructor(x: number, y: number, z: number, w: number);
	setValue(x: number, y: number, z: number, w: number): void;
	setEulerZYX(z: number, y: number, x: number): void;
	setRotation(axis: btVector3, angle: number): void;
	normalize(): void;
	length2(): number;
	length(): number;
	dot(q: btQuaternion): number;
	normalized(): btQuaternion;
	getAxis(): btVector3;
	inverse(): btQuaternion;
	getAngle(): number;
	getAngleShortestPath(): number;
	angle(q: btQuaternion): number;
	angleShortestPath(q: btQuaternion): number;
	op_add(q: btQuaternion): btQuaternion;
	op_sub(q: btQuaternion): btQuaternion;
	op_mul(s: number): btQuaternion;
	op_mulq(q: btQuaternion): btQuaternion;
	op_div(s: number): btQuaternion;
}

export class btMatrix3x3 {
	setEulerZYX(ex: number, ey: number, ez: number): void;
	getRotation(q: btQuaternion): void;
	getRow(y: number): btVector3;
}

export class btTransform {
	constructor();
	constructor(q: btQuaternion, v: btVector3);
	setIdentity(): void;
	setOrigin(origin: btVector3): void;
	setRotation(rotation: btQuaternion): void;
	getOrigin(): btVector3;
	getRotation(): btQuaternion;
	getBasis(): btMatrix3x3;
	inverse(): btTransform;
	op_mul(t: btTransform): btTransform;
}

export class btMotionState {
	getWorldTransform(worldTrans: btTransform): void;
	setWorldTransform(worldTrans: btTransform): void;
}

export class btDefaultMotionState extends btMotionState  {
	constructor(startTrans?: btTransform, centerOfMassOffset?: btTransform);
	get_m_graphicsWorldTrans(): btTransform;
	set_m_graphicsWorldTrans(value: btTransform): void;
}

export class btCollisionObject {
	setAnisotropicFriction(anisotropicFriction: btVector3, frictionMode: number): void;
	getCollisionShape(): btCollisionShape;
	setContactProcessingThreshold(contactProcessingThreshold: number): void;
	setActivationState(newState: number): void;
	forceActivationState(newState: number): void;
	activate(forceActivation?: boolean): void;
	isActive(): boolean;
	isKinematicObject(): boolean;
	isStaticObject(): boolean;
	isStaticOrKinematicObject(): boolean;
	setRestitution(rest: number): void;
	setFriction(frict: number): void;
	setRollingFriction(frict: number): void;
	getWorldTransform(): btTransform;
	getInterpolationWorldTransform(): btTransform;
	getInterpolationLinearVelocity(): btVector3;
	getInterpolationAngularVelocity(): btVector3;
	setInterpolationWorldTransform(worldTrans: btTransform): void;
	setInterpolationLinearVelocity(velocity: btVector3): void;
	setInterpolationAngularVelocity(velocity: btVector3): void;
	getCollisionFlags(): number;
	setCollisionFlags(flags: number): void;
	setWorldTransform(worldTrans: btTransform): void;
	setCollisionShape(collisionShape: btCollisionShape): void;
	setCcdMotionThreshold(ccdMotionThreshold: number): void;
	setCcdSweptSphereRadius(radius: number): void;
	getUserIndex(): number;
	setUserIndex(index: number): void;
	getUserPointer(): number;
	setUserPointer(userPointer: number): void;
}

export class btCollisionObjectWrapper {
	get_$__dummyprop__btCollisionObjectWrapper(): any;
	set_$__dummyprop__btCollisionObjectWrapper(value: any): void;
}

export class RayResultCallback {
	hasHit(): boolean;
	get_m_collisionFilterGroup(): number;
	set_m_collisionFilterGroup(value: number): void;
	get_m_collisionFilterMask(): number;
	set_m_collisionFilterMask(value: number): void;
	get_m_closestHitFraction(): number;
	set_m_closestHitFraction(value: number): void;
	get_m_collisionObject(): btCollisionObject;
	set_m_collisionObject(value: btCollisionObject): void;
}

export class hbrClosestRayResultCallback extends RayResultCallback  {
	constructor(from: btVector3, to: btVector3);
	get_m_flags(): number;
	set_m_flags(value: number): void;
	get_m_rayFromWorld(): btVector3;
	set_m_rayFromWorld(value: btVector3): void;
	get_m_rayToWorld(): btVector3;
	set_m_rayToWorld(value: btVector3): void;
	get_m_hitNormalWorld(): btVector3;
	set_m_hitNormalWorld(value: btVector3): void;
	get_m_hitPointWorld(): btVector3;
	set_m_hitPointWorld(value: btVector3): void;
}

export class ClosestRayResultCallback extends RayResultCallback  {
	constructor(from: btVector3, to: btVector3);
	get_m_flags(): number;
	set_m_flags(value: number): void;
	get_m_rayFromWorld(): btVector3;
	set_m_rayFromWorld(value: btVector3): void;
	get_m_rayToWorld(): btVector3;
	set_m_rayToWorld(value: btVector3): void;
	get_m_hitNormalWorld(): btVector3;
	set_m_hitNormalWorld(value: btVector3): void;
	get_m_hitPointWorld(): btVector3;
	set_m_hitPointWorld(value: btVector3): void;
}

export class btConstCollisionObjectArray {
	size(): number;
	at(n: number): btCollisionObject;
}

export class btScalarArray {
	size(): number;
	at(n: number): number;
}

export class AllHitsRayResultCallback {
	constructor(from: btVector3, to: btVector3);
	get_m_flags(): number;
	set_m_flags(value: number): void;
	get_m_collisionObjects(): btConstCollisionObjectArray;
	set_m_collisionObjects(value: btConstCollisionObjectArray): void;
	get_m_rayFromWorld(): btVector3;
	set_m_rayFromWorld(value: btVector3): void;
	get_m_rayToWorld(): btVector3;
	set_m_rayToWorld(value: btVector3): void;
	get_m_hitNormalWorld(): btVector3Array;
	set_m_hitNormalWorld(value: btVector3Array): void;
	get_m_hitPointWorld(): btVector3Array;
	set_m_hitPointWorld(value: btVector3Array): void;
	get_m_hitFractions(): btScalarArray;
	set_m_hitFractions(value: btScalarArray): void;
}

export class hbrAllHitsRayResultCallback extends RayResultCallback  {
	constructor(from: btVector3, to: btVector3);
	hasHit(): boolean;
	get_m_flags(): number;
	set_m_flags(value: number): void;
	get_a_closestHitFraction(): number;
	set_a_closestHitFraction(value: number): void;
	get_m_closestHitNormalWorld(): btVector3;
	set_m_closestHitNormalWorld(value: btVector3): void;
	get_m_closestHitPointWorld(): btVector3;
	set_m_closestHitPointWorld(value: btVector3): void;
	get_m_closestCollisionObject(): btCollisionObject;
	set_m_closestCollisionObject(value: btCollisionObject): void;
	get_m_rayFromWorld(): btVector3;
	set_m_rayFromWorld(value: btVector3): void;
	get_m_rayToWorld(): btVector3;
	set_m_rayToWorld(value: btVector3): void;
}

export class btManifoldPoint {
	getPositionWorldOnA(): btVector3;
	getPositionWorldOnB(): btVector3;
	getAppliedImpulse(): number;
	getDistance(): number;
	get_m_localPointA(): btVector3;
	set_m_localPointA(value: btVector3): void;
	get_m_localPointB(): btVector3;
	set_m_localPointB(value: btVector3): void;
	get_m_positionWorldOnB(): btVector3;
	set_m_positionWorldOnB(value: btVector3): void;
	get_m_positionWorldOnA(): btVector3;
	set_m_positionWorldOnA(value: btVector3): void;
	get_m_normalWorldOnB(): btVector3;
	set_m_normalWorldOnB(value: btVector3): void;
}

export class ContactResultCallback {
	addSingleResult(cp: btManifoldPoint, colObj0Wrap: btCollisionObjectWrapper, partId0: number, index0: number, colObj1Wrap: btCollisionObjectWrapper, partId1: number, index1: number): number;
}

export class ConcreteContactResultCallback {
	constructor();
	addSingleResult(cp: btManifoldPoint, colObj0Wrap: btCollisionObjectWrapper, partId0: number, index0: number, colObj1Wrap: btCollisionObjectWrapper, partId1: number, index1: number): number;
}

export class LocalShapeInfo {
	get_m_shapePart(): number;
	set_m_shapePart(value: number): void;
	get_m_triangleIndex(): number;
	set_m_triangleIndex(value: number): void;
}

export class LocalConvexResult {
	constructor(hitCollisionObject: btCollisionObject, localShapeInfo: LocalShapeInfo, hitNormalLocal: btVector3, hitPointLocal: btVector3, hitFraction: number);
	get_m_hitCollisionObject(): btCollisionObject;
	set_m_hitCollisionObject(value: btCollisionObject): void;
	get_m_localShapeInfo(): LocalShapeInfo;
	set_m_localShapeInfo(value: LocalShapeInfo): void;
	get_m_hitNormalLocal(): btVector3;
	set_m_hitNormalLocal(value: btVector3): void;
	get_m_hitPointLocal(): btVector3;
	set_m_hitPointLocal(value: btVector3): void;
	get_m_hitFraction(): number;
	set_m_hitFraction(value: number): void;
}

export class ConvexResultCallback {
	hasHit(): boolean;
	get_m_collisionFilterGroup(): number;
	set_m_collisionFilterGroup(value: number): void;
	get_m_collisionFilterMask(): number;
	set_m_collisionFilterMask(value: number): void;
	get_m_closestHitFraction(): number;
	set_m_closestHitFraction(value: number): void;
}

export class ClosestConvexResultCallback extends ConvexResultCallback  {
	constructor(convexFromWorld: btVector3, convexToWorld: btVector3);
	get_m_convexFromWorld(): btVector3;
	set_m_convexFromWorld(value: btVector3): void;
	get_m_convexToWorld(): btVector3;
	set_m_convexToWorld(value: btVector3): void;
	get_m_hitNormalWorld(): btVector3;
	set_m_hitNormalWorld(value: btVector3): void;
	get_m_hitPointWorld(): btVector3;
	set_m_hitPointWorld(value: btVector3): void;
}

export class btCollisionShape {
	setLocalScaling(scaling: btVector3): void;
	getLocalScaling(): btVector3;
	calculateLocalInertia(mass: number, inertia: btVector3): void;
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btConvexShape extends btCollisionShape  {
	get_$__dummyprop__btConvexShape(): any;
	set_$__dummyprop__btConvexShape(value: any): void;
}

export class btConvexTriangleMeshShape extends btConvexShape  {
	constructor(meshInterface: btStridingMeshInterface, calcAabb?: boolean);
}

export class btBoxShape extends btCollisionShape  {
	constructor(boxHalfExtents: btVector3);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btCapsuleShape extends btCollisionShape  {
	constructor(radius: number, height: number);
	setMargin(margin: number): void;
	getMargin(): number;
	getUpAxis(): number;
	getRadius(): number;
	getHalfHeight(): number;
}

export class btCapsuleShapeX extends btCapsuleShape  {
	constructor(radius: number, height: number);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btCapsuleShapeZ extends btCapsuleShape  {
	constructor(radius: number, height: number);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btCylinderShape extends btCollisionShape  {
	constructor(halfExtents: btVector3);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btCylinderShapeX extends btCylinderShape  {
	constructor(halfExtents: btVector3);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btCylinderShapeZ extends btCylinderShape  {
	constructor(halfExtents: btVector3);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btSphereShape extends btCollisionShape  {
	constructor(radius: number);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btConeShape extends btCollisionShape  {
	constructor(radius: number, height: number);
}

export class btIntArray {
	size(): number;
	at(n: number): number;
}

export class btFace {
	get_m_indices(): btIntArray;
	set_m_indices(value: btIntArray): void;
}

export class btVector3Array {
	size(): number;
	at(n: number): btVector3;
}

export class btFaceArray {
	size(): number;
	at(n: number): btFace;
}

export class btConvexPolyhedron {
	get_m_vertices(): btVector3Array;
	set_m_vertices(value: btVector3Array): void;
	get_m_faces(): btFaceArray;
	set_m_faces(value: btFaceArray): void;
}

export class btConvexHullShape extends btCollisionShape  {
	constructor();
	addPoint(point: btVector3, recalculateLocalAABB?: boolean): void;
	setMargin(margin: number): void;
	getMargin(): number;
	getNumVertices(): number;
	initializePolyhedralFeatures(shiftVerticesByMargin: number): boolean;
	recalcLocalAabb(): void;
	getConvexPolyhedron(): btConvexPolyhedron;
}

export class btShapeHull {
	constructor(shape: btConvexShape);
	buildHull(margin: number): boolean;
	numVertices(): number;
	getVertexPointer(): btVector3;
}

export class btConeShapeX extends btConeShape  {
	constructor(radius: number, height: number);
}

export class btConeShapeZ extends btConeShape  {
	constructor(radius: number, height: number);
}

export class btCompoundShape extends btCollisionShape  {
	constructor(enableDynamicAabbTree?: boolean);
	addChildShape(localTransform: btTransform, shape: btCollisionShape): void;
	removeChildShapeByIndex(childShapeindex: number): void;
	removeChildShape(shape: btCollisionShape): void;
	updateChildTransform(childShapeindex: number, newChildTransform: btTransform, shouldRecalculateLocalAabb: boolean): void;
	getChildTransform(childIndex: number): btTransform;
	getNumChildShapes(): number;
	getChildShape(index: number): btCollisionShape;
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btStridingMeshInterface {
	setScaling(scaling: btVector3): void;
}

export class btTriangleMesh extends btStridingMeshInterface  {
	constructor(use32bitIndices?: boolean, use4componentVertices?: boolean);
	addTriangle(vertex0: btVector3, vertex1: btVector3, vertex2: btVector3, removeDuplicateVertices?: boolean): void;
	findOrAddVertex(vertex: btVector3, removeDuplicateVertices: boolean): void;
	addTriangleIndices(index1: number, index2: number, index3: number): void;
}

export class btConcaveShape extends btCollisionShape  {
	get_$__dummyprop__btConcaveShape(): any;
	set_$__dummyprop__btConcaveShape(value: any): void;
}

export class btStaticPlaneShape extends btConcaveShape  {
	constructor(planeNormal: btVector3, planeConstant: number);
}

export class btTriangleMeshShape extends btConcaveShape  {
	get_$__dummyprop__btTriangleMeshShape(): any;
	set_$__dummyprop__btTriangleMeshShape(value: any): void;
}

export class btBvhTriangleMeshShape extends btTriangleMeshShape  {
	constructor(meshInterface: btStridingMeshInterface, useQuantizedAabbCompression: boolean, buildBvh?: boolean);
}

export class btHeightfieldTerrainShape extends btConcaveShape  {
	constructor(heightStickWidth: number, heightStickLength: number, heightfieldData: number, heightScale: number, minHeight: number, maxHeight: number, upAxis: number, hdt: PHY_ScalarType, flipQuadEdges: boolean);
	setMargin(margin: number): void;
	getMargin(): number;
}

export class btDefaultCollisionConstructionInfo {
	constructor();
}

export class btDefaultCollisionConfiguration extends btCollisionConfiguration  {
	constructor(info?: btDefaultCollisionConstructionInfo);
}

export class btPersistentManifold {
	constructor();
	getBody0(): btCollisionObject;
	getBody1(): btCollisionObject;
	getNumContacts(): number;
	getContactPoint(index: number): btManifoldPoint;
}

export class btDispatcher {
	getNumManifolds(): number;
	getManifoldByIndexInternal(index: number): btPersistentManifold;
	dispatchAllCollisionPairs(pairCache: btOverlappingPairCache, dispatchInfo: btDispatcherInfo, dispatcher: btDispatcher): void;
}

export class btCollisionDispatcher extends btDispatcher  {
	constructor(conf: btDefaultCollisionConfiguration);
}

export class btOverlappingPairCallback {
	get_$__dummyprop__btOverlappingPairCallback(): any;
	set_$__dummyprop__btOverlappingPairCallback(value: any): void;
}

export class btOverlappingPairCache {
	setInternalGhostPairCallback(ghostPairCallback: btOverlappingPairCallback): void;
}

export class btAxisSweep3 {
	constructor(worldAabbMin: btVector3, worldAabbMax: btVector3, maxHandles?: number, pairCache?: btOverlappingPairCache, disableRaycastAccelerator?: boolean);
}

export class btBroadphaseInterface {
	get_$__dummyprop__btBroadphaseInterface(): any;
	set_$__dummyprop__btBroadphaseInterface(value: any): void;
}

export class btCollisionConfiguration {
	get_$__dummyprop__btCollisionConfiguration(): any;
	set_$__dummyprop__btCollisionConfiguration(value: any): void;
}

export class btDbvtBroadphase extends btBroadphaseInterface  {
	constructor();
}

export class btBroadphaseProxy {
	get_m_collisionFilterGroup(): number;
	set_m_collisionFilterGroup(value: number): void;
	get_m_collisionFilterMask(): number;
	set_m_collisionFilterMask(value: number): void;
}

export class btRigidBodyConstructionInfo {
	constructor(mass: number, motionState: btMotionState, collisionShape: btCollisionShape, localInertia?: btVector3);
	get_m_linearDamping(): number;
	set_m_linearDamping(value: number): void;
	get_m_angularDamping(): number;
	set_m_angularDamping(value: number): void;
	get_m_friction(): number;
	set_m_friction(value: number): void;
	get_m_rollingFriction(): number;
	set_m_rollingFriction(value: number): void;
	get_m_restitution(): number;
	set_m_restitution(value: number): void;
	get_m_linearSleepingThreshold(): number;
	set_m_linearSleepingThreshold(value: number): void;
	get_m_angularSleepingThreshold(): number;
	set_m_angularSleepingThreshold(value: number): void;
	get_m_additionalDamping(): boolean;
	set_m_additionalDamping(value: boolean): void;
	get_m_additionalDampingFactor(): number;
	set_m_additionalDampingFactor(value: number): void;
	get_m_additionalLinearDampingThresholdSqr(): number;
	set_m_additionalLinearDampingThresholdSqr(value: number): void;
	get_m_additionalAngularDampingThresholdSqr(): number;
	set_m_additionalAngularDampingThresholdSqr(value: number): void;
	get_m_additionalAngularDampingFactor(): number;
	set_m_additionalAngularDampingFactor(value: number): void;
}

export class btRigidBody extends btCollisionObject  {
	constructor(constructionInfo: btRigidBodyConstructionInfo);
	getCenterOfMassTransform(): btTransform;
	setCenterOfMassTransform(xform: btTransform): void;
	setSleepingThresholds(linear: number, angular: number): void;
	setDamping(lin_damping: number, ang_damping: number): void;
	setMassProps(mass: number, inertia: btVector3): void;
	setLinearFactor(linearFactor: btVector3): void;
	applyTorque(torque: btVector3): void;
	applyForce(force: btVector3, rel_pos: btVector3): void;
	applyCentralForce(force: btVector3): void;
	applyTorqueImpulse(torque: btVector3): void;
	applyImpulse(impulse: btVector3, rel_pos: btVector3): void;
	applyCentralImpulse(impulse: btVector3): void;
	updateInertiaTensor(): void;
	getVelocityInLocalPoint(rel_pos: btVector3): btVector3;
	getCenterOfMassPosition(): btVector3;
	getLinearVelocity(): btVector3;
	getAngularVelocity(): btVector3;
	setLinearVelocity(lin_vel: btVector3): void;
	setAngularVelocity(ang_vel: btVector3): void;
	getMotionState(): btMotionState;
	setMotionState(motionState: btMotionState): void;
	setAngularFactor(angularFactor: btVector3): void;
	upcast(colObj: btCollisionObject): btRigidBody;
	getAabb(aabbMin: btVector3, aabbMax: btVector3): void;
	applyGravity(): void;
	getGravity(): btVector3;
	setGravity(acceleration: btVector3): void;
	getBroadphaseProxy(): btBroadphaseProxy;
}

export class btConstraintSetting {
	constructor();
	get_m_tau(): number;
	set_m_tau(value: number): void;
	get_m_damping(): number;
	set_m_damping(value: number): void;
	get_m_impulseClamp(): number;
	set_m_impulseClamp(value: number): void;
}

export class btTypedConstraint {
	enableFeedback(needsFeedback: boolean): void;
	getBreakingImpulseThreshold(): number;
	setBreakingImpulseThreshold(threshold: number): void;
	getParam(num: number, axis: number): number;
	setParam(num: number, value: number, axis: number): void;
}

export class btPoint2PointConstraint extends btTypedConstraint  {
	constructor(rbA: btRigidBody, rbB: btRigidBody, pivotInA: btVector3, pivotInB: btVector3);
	constructor(rbA: btRigidBody, pivotInA: btVector3);
	setPivotA(pivotA: btVector3): void;
	setPivotB(pivotB: btVector3): void;
	getPivotInA(): btVector3;
	getPivotInB(): btVector3;
	get_m_setting(): btConstraintSetting;
	set_m_setting(value: btConstraintSetting): void;
}

export class btGeneric6DofConstraint extends btTypedConstraint  {
	constructor(rbA: btRigidBody, rbB: btRigidBody, frameInA: btTransform, frameInB: btTransform, useLinearFrameReferenceFrameA: boolean);
	constructor(rbB: btRigidBody, frameInB: btTransform, useLinearFrameReferenceFrameB: boolean);
	setLinearLowerLimit(linearLower: btVector3): void;
	setLinearUpperLimit(linearUpper: btVector3): void;
	setAngularLowerLimit(angularLower: btVector3): void;
	setAngularUpperLimit(angularUpper: btVector3): void;
	getFrameOffsetA(): btTransform;
}

export class btGeneric6DofSpringConstraint extends btGeneric6DofConstraint  {
	constructor(rbA: btRigidBody, rbB: btRigidBody, frameInA: btTransform, frameInB: btTransform, useLinearFrameReferenceFrameA: boolean);
	constructor(rbB: btRigidBody, frameInB: btTransform, useLinearFrameReferenceFrameB: boolean);
	enableSpring(index: number, onOff: boolean): void;
	setStiffness(index: number, stiffness: number): void;
	setDamping(index: number, damping: number): void;
}

export class btSequentialImpulseConstraintSolver extends btConstraintSolver  {
	constructor();
}

export class btConeTwistConstraint extends btTypedConstraint  {
	constructor(rbA: btRigidBody, rbB: btRigidBody, rbAFrame: btTransform, rbBFrame: btTransform);
	constructor(rbA: btRigidBody, rbAFrame: btTransform);
	setLimit(limitIndex: number, limitValue: number): void;
	setAngularOnly(angularOnly: boolean): void;
	setDamping(damping: number): void;
	enableMotor(b: boolean): void;
	setMaxMotorImpulse(maxMotorImpulse: number): void;
	setMaxMotorImpulseNormalized(maxMotorImpulse: number): void;
	setMotorTarget(q: btQuaternion): void;
	setMotorTargetInConstraintSpace(q: btQuaternion): void;
}

export class btHingeConstraint extends btTypedConstraint  {
	constructor(rbA: btRigidBody, rbB: btRigidBody, pivotInA: btVector3, pivotInB: btVector3, axisInA: btVector3, axisInB: btVector3, useReferenceFrameA?: boolean);
	constructor(rbA: btRigidBody, rbB: btRigidBody, rbAFrame: btTransform, rbBFrame: btTransform, useReferenceFrameA?: boolean);
	constructor(rbA: btRigidBody, rbAFrame: btTransform, useReferenceFrameA?: boolean);
	setLimit(low: number, high: number, softness: number, biasFactor: number, relaxationFactor?: number): void;
	enableAngularMotor(enableMotor: boolean, targetVelocity: number, maxMotorImpulse: number): void;
	setAngularOnly(angularOnly: boolean): void;
	enableMotor(enableMotor: boolean): void;
	setMaxMotorImpulse(maxMotorImpulse: number): void;
	setMotorTarget(targetAngle: number, dt: number): void;
}

export class btSliderConstraint extends btTypedConstraint  {
	constructor(rbA: btRigidBody, rbB: btRigidBody, frameInA: btTransform, frameInB: btTransform, useLinearReferenceFrameA: boolean);
	constructor(rbB: btRigidBody, frameInB: btTransform, useLinearReferenceFrameA: boolean);
	setLowerLinLimit(lowerLimit: number): void;
	setUpperLinLimit(upperLimit: number): void;
	setLowerAngLimit(lowerAngLimit: number): void;
	setUpperAngLimit(upperAngLimit: number): void;
}

export class btFixedConstraint extends btTypedConstraint  {
	constructor(rbA: btRigidBody, rbB: btRigidBody, frameInA: btTransform, frameInB: btTransform);
}

export class btConstraintSolver {
	get_$__dummyprop__btConstraintSolver(): any;
	set_$__dummyprop__btConstraintSolver(value: any): void;
}

export class btDispatcherInfo {
	get_m_timeStep(): number;
	set_m_timeStep(value: number): void;
	get_m_stepCount(): number;
	set_m_stepCount(value: number): void;
	get_m_dispatchFunc(): number;
	set_m_dispatchFunc(value: number): void;
	get_m_timeOfImpact(): number;
	set_m_timeOfImpact(value: number): void;
	get_m_useContinuous(): boolean;
	set_m_useContinuous(value: boolean): void;
	get_m_enableSatConvex(): boolean;
	set_m_enableSatConvex(value: boolean): void;
	get_m_enableSPU(): boolean;
	set_m_enableSPU(value: boolean): void;
	get_m_useEpa(): boolean;
	set_m_useEpa(value: boolean): void;
	get_m_allowedCcdPenetration(): number;
	set_m_allowedCcdPenetration(value: number): void;
	get_m_useConvexConservativeDistanceUtil(): boolean;
	set_m_useConvexConservativeDistanceUtil(value: boolean): void;
	get_m_convexConservativeDistanceThreshold(): number;
	set_m_convexConservativeDistanceThreshold(value: number): void;
}

export class btCollisionWorld {
	getDispatcher(): btDispatcher;
	rayTest(rayFromWorld: btVector3, rayToWorld: btVector3, resultCallback: RayResultCallback): void;
	getPairCache(): btOverlappingPairCache;
	getDispatchInfo(): btDispatcherInfo;
	addCollisionObject(collisionObject: btCollisionObject, collisionFilterGroup?: number, collisionFilterMask?: number): void;
	removeCollisionObject(collisionObject: btCollisionObject): void;
	getBroadphase(): btBroadphaseInterface;
	convexSweepTest(castShape: btConvexShape, from: btTransform, to: btTransform, resultCallback: ConvexResultCallback, allowedCcdPenetration: number): void;
	contactPairTest(colObjA: btCollisionObject, colObjB: btCollisionObject, resultCallback: ContactResultCallback): void;
	contactTest(colObj: btCollisionObject, resultCallback: ContactResultCallback): void;
	updateSingleAabb(colObj: btCollisionObject): void;
	setDebugDrawer(debugDrawer: btIDebugDraw): void;
	getDebugDrawer(): btIDebugDraw;
	debugDrawWorld(): void;
	debugDrawObject(worldTransform: btTransform, shape: btCollisionShape, color: btVector3): void;
}

export class btContactSolverInfo {
	get_m_splitImpulse(): boolean;
	set_m_splitImpulse(value: boolean): void;
	get_m_splitImpulsePenetrationThreshold(): number;
	set_m_splitImpulsePenetrationThreshold(value: number): void;
	get_m_numIterations(): number;
	set_m_numIterations(value: number): void;
}

export class btDynamicsWorld extends btCollisionWorld  {
	addAction(action: btActionInterface): void;
	removeAction(action: btActionInterface): void;
	getSolverInfo(): btContactSolverInfo;
}

export class btDiscreteDynamicsWorld extends btDynamicsWorld  {
	constructor(dispatcher: btDispatcher, pairCache: btBroadphaseInterface, constraintSolver: btConstraintSolver, collisionConfiguration: btCollisionConfiguration);
	setGravity(gravity: btVector3): void;
	getGravity(): btVector3;
	addRigidBody(body: btRigidBody): void;
	addRigidBody(body: btRigidBody, group: number, mask: number): void;
	removeRigidBody(body: btRigidBody): void;
	addConstraint(constraint: btTypedConstraint, disableCollisionsBetweenLinkedBodies?: boolean): void;
	removeConstraint(constraint: btTypedConstraint): void;
	stepSimulation(timeStep: number, maxSubSteps?: number, fixedTimeStep?: number): number;
	setSynchronizeAllMotionStates(synchronizeAll: boolean): void;
}

export class btVehicleTuning {
	constructor();
	get_m_suspensionStiffness(): number;
	set_m_suspensionStiffness(value: number): void;
	get_m_suspensionCompression(): number;
	set_m_suspensionCompression(value: number): void;
	get_m_suspensionDamping(): number;
	set_m_suspensionDamping(value: number): void;
	get_m_maxSuspensionTravelCm(): number;
	set_m_maxSuspensionTravelCm(value: number): void;
	get_m_frictionSlip(): number;
	set_m_frictionSlip(value: number): void;
	get_m_maxSuspensionForce(): number;
	set_m_maxSuspensionForce(value: number): void;
}

export class btVehicleRaycasterResult {
	get_m_hitPointInWorld(): btVector3;
	set_m_hitPointInWorld(value: btVector3): void;
	get_m_hitNormalInWorld(): btVector3;
	set_m_hitNormalInWorld(value: btVector3): void;
	get_m_distFraction(): number;
	set_m_distFraction(value: number): void;
}

export class btVehicleRaycaster {
	castRay(from: btVector3, to: btVector3, result: btVehicleRaycasterResult): void;
}

export class btDefaultVehicleRaycaster extends btVehicleRaycaster  {
	constructor(world: btDynamicsWorld);
}

export class RaycastInfo {
	get_m_contactNormalWS(): btVector3;
	set_m_contactNormalWS(value: btVector3): void;
	get_m_contactPointWS(): btVector3;
	set_m_contactPointWS(value: btVector3): void;
	get_m_suspensionLength(): number;
	set_m_suspensionLength(value: number): void;
	get_m_hardPointWS(): btVector3;
	set_m_hardPointWS(value: btVector3): void;
	get_m_wheelDirectionWS(): btVector3;
	set_m_wheelDirectionWS(value: btVector3): void;
	get_m_wheelAxleWS(): btVector3;
	set_m_wheelAxleWS(value: btVector3): void;
	get_m_isInContact(): boolean;
	set_m_isInContact(value: boolean): void;
	get_m_groundObject(): any;
	set_m_groundObject(value: any): void;
}

export class btWheelInfoConstructionInfo {
	get_m_chassisConnectionCS(): btVector3;
	set_m_chassisConnectionCS(value: btVector3): void;
	get_m_wheelDirectionCS(): btVector3;
	set_m_wheelDirectionCS(value: btVector3): void;
	get_m_wheelAxleCS(): btVector3;
	set_m_wheelAxleCS(value: btVector3): void;
	get_m_suspensionRestLength(): number;
	set_m_suspensionRestLength(value: number): void;
	get_m_maxSuspensionTravelCm(): number;
	set_m_maxSuspensionTravelCm(value: number): void;
	get_m_wheelRadius(): number;
	set_m_wheelRadius(value: number): void;
	get_m_suspensionStiffness(): number;
	set_m_suspensionStiffness(value: number): void;
	get_m_wheelsDampingCompression(): number;
	set_m_wheelsDampingCompression(value: number): void;
	get_m_wheelsDampingRelaxation(): number;
	set_m_wheelsDampingRelaxation(value: number): void;
	get_m_frictionSlip(): number;
	set_m_frictionSlip(value: number): void;
	get_m_maxSuspensionForce(): number;
	set_m_maxSuspensionForce(value: number): void;
	get_m_bIsFrontWheel(): boolean;
	set_m_bIsFrontWheel(value: boolean): void;
}

export class btWheelInfo {
	constructor(ci: btWheelInfoConstructionInfo);
	getSuspensionRestLength(): number;
	updateWheel(chassis: btRigidBody, raycastInfo: RaycastInfo): void;
	get_m_suspensionStiffness(): number;
	set_m_suspensionStiffness(value: number): void;
	get_m_frictionSlip(): number;
	set_m_frictionSlip(value: number): void;
	get_m_engineForce(): number;
	set_m_engineForce(value: number): void;
	get_m_rollInfluence(): number;
	set_m_rollInfluence(value: number): void;
	get_m_suspensionRestLength1(): number;
	set_m_suspensionRestLength1(value: number): void;
	get_m_wheelsRadius(): number;
	set_m_wheelsRadius(value: number): void;
	get_m_wheelsDampingCompression(): number;
	set_m_wheelsDampingCompression(value: number): void;
	get_m_wheelsDampingRelaxation(): number;
	set_m_wheelsDampingRelaxation(value: number): void;
	get_m_steering(): number;
	set_m_steering(value: number): void;
	get_m_maxSuspensionForce(): number;
	set_m_maxSuspensionForce(value: number): void;
	get_m_maxSuspensionTravelCm(): number;
	set_m_maxSuspensionTravelCm(value: number): void;
	get_m_wheelsSuspensionForce(): number;
	set_m_wheelsSuspensionForce(value: number): void;
	get_m_bIsFrontWheel(): boolean;
	set_m_bIsFrontWheel(value: boolean): void;
	get_m_raycastInfo(): RaycastInfo;
	set_m_raycastInfo(value: RaycastInfo): void;
	get_m_chassisConnectionPointCS(): btVector3;
	set_m_chassisConnectionPointCS(value: btVector3): void;
	get_m_worldTransform(): btTransform;
	set_m_worldTransform(value: btTransform): void;
	get_m_wheelDirectionCS(): btVector3;
	set_m_wheelDirectionCS(value: btVector3): void;
	get_m_wheelAxleCS(): btVector3;
	set_m_wheelAxleCS(value: btVector3): void;
	get_m_rotation(): number;
	set_m_rotation(value: number): void;
	get_m_deltaRotation(): number;
	set_m_deltaRotation(value: number): void;
	get_m_brake(): number;
	set_m_brake(value: number): void;
	get_m_clippedInvContactDotSuspension(): number;
	set_m_clippedInvContactDotSuspension(value: number): void;
	get_m_suspensionRelativeVelocity(): number;
	set_m_suspensionRelativeVelocity(value: number): void;
	get_m_skidInfo(): number;
	set_m_skidInfo(value: number): void;
}

export class btActionInterface {
	updateAction(collisionWorld: btCollisionWorld, deltaTimeStep: number): void;
}

export class hbrKinematicCharacterController extends btActionInterface  {
	constructor(ghostObject: btPairCachingGhostObject, convexShape: btConvexShape, stepHeight: number, upAxis?: btVector3);
	setUp(up: btVector3): void;
	applyImpulse(v: btVector3): void;
	applyCentralImpulse(v: btVector3): void;
	applyCentralForce(v: btVector3): void;
	setWalkDirection(walkDirection: btVector3): void;
	reset(collisionWorld: btCollisionWorld): void;
	warp(origin: btVector3): void;
	preStep(collisionWorld: btCollisionWorld): void;
	playerStep(collisionWorld: btCollisionWorld, dt: number): void;
	preUpdate(collisionWorld: btCollisionWorld, dt: number): void;
	setFallSpeed(fallSpeed: number): void;
	setJumpSpeed(jumpSpeed: number): void;
	setMaxJumpHeight(maxJumpHeight: number): void;
	setMaxWalkSpeed(speed: number): void;
	setMaxRunSpeed(speed: number): void;
	setMaxAirSpeed(speed: number): void;
	setMaxFlySpeed(speed: number): void;
	setWalkAcceleration(acceleration: number): void;
	setRunAcceleration(acceleration: number): void;
	setAirAcceleration(acceleration: number): void;
	setFlyAcceleration(acceleration: number): void;
	setBunnyHopEnabled(enabled: boolean): void;
	setBunnyHopFactor(speed: number): void;
	setSpeedModifier(speed: number): void;
	setAirWalking(enabled: boolean): void;
	setFriction(friction: number): void;
	setDrag(drag: number): void;
	setJumpOffset(ms: number): void;
	setDeAccelerationMultiplier(multiplier: number): void;
	canJump(): boolean;
	jump(): void;
	setGravity(gravity: btVector3): void;
	getGravity(): btVector3;
	setMaxSlope(slopeRadians: number): void;
	getMaxSlope(): number;
	getGhostObject(): btPairCachingGhostObject;
	setUseGhostSweepTest(useGhostObjectSweepTest: boolean): void;
	onGround(): boolean;
	setLinearVelocity(velocity: btVector3): void;
	getLinearVelocity(): btVector3;
	getLocalLinearVelocity(): btVector3;
	setLinearDamping(d: number): void;
	getLinearDamping(): number;
	setAngularDamping(d: number): void;
	getAngularDamping(): number;
	setUpInterpolate(value: boolean): void;
}

export class btRaycastVehicle extends btActionInterface  {
	constructor(tuning: btVehicleTuning, chassis: btRigidBody, raycaster: btVehicleRaycaster);
	applyEngineForce(force: number, wheel: number): void;
	setSteeringValue(steering: number, wheel: number): void;
	getWheelTransformWS(wheelIndex: number): btTransform;
	updateWheelTransform(wheelIndex: number, interpolatedTransform: boolean): void;
	addWheel(connectionPointCS0: btVector3, wheelDirectionCS0: btVector3, wheelAxleCS: btVector3, suspensionRestLength: number, wheelRadius: number, tuning: btVehicleTuning, isFrontWheel: boolean): btWheelInfo;
	getNumWheels(): number;
	getRigidBody(): btRigidBody;
	getWheelInfo(index: number): btWheelInfo;
	setBrake(brake: number, wheelIndex: number): void;
	setCoordinateSystem(rightIndex: number, upIndex: number, forwardIndex: number): void;
	getCurrentSpeedKmHour(): number;
	getChassisWorldTransform(): btTransform;
	rayCast(wheel: btWheelInfo): number;
	updateVehicle(step: number): void;
	resetSuspension(): void;
	getSteeringValue(wheel: number): number;
	updateWheelTransformsWS(wheel: btWheelInfo, interpolatedTransform?: boolean): void;
	setPitchControl(pitch: number): void;
	updateSuspension(deltaTime: number): void;
	updateFriction(timeStep: number): void;
	getRightAxis(): number;
	getUpAxis(): number;
	getForwardAxis(): number;
	getForwardVector(): btVector3;
	getUserConstraintType(): number;
	setUserConstraintType(userConstraintType: number): void;
	setUserConstraintId(uid: number): void;
	getUserConstraintId(): number;
}

export class btGhostObject extends btCollisionObject  {
	constructor();
	getNumOverlappingObjects(): number;
	getOverlappingObject(index: number): btCollisionObject;
}

export class btPairCachingGhostObject extends btGhostObject  {
	constructor();
}

export class btGhostPairCallback extends btOverlappingPairCallback  {
	constructor();
}

}

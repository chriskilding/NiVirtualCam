/**
 * Copyright (c) 2013 Christopher Kilding
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "stdafx.h"
#include "ZigFuJsonWriter.h"
#include "HSO3.h"

/** Initialize the struct */
ZigFuJsonWriter::JointTuple ZigFuJsonWriter::jointMappings[] = {
	{ 1, "1", nite::JOINT_HEAD },
	{ 2, "2", nite::JOINT_NECK },
	{ 3, "3", nite::JOINT_TORSO },
	{ 6, "6", nite::JOINT_LEFT_SHOULDER },
	{ 7, "7", nite::JOINT_LEFT_ELBOW },
	{ 9, "9", nite::JOINT_LEFT_HAND },
	{ 12, "12", nite::JOINT_RIGHT_SHOULDER },
	{ 13, "13", nite::JOINT_RIGHT_ELBOW },
	{ 15, "15", nite::JOINT_RIGHT_HAND },
	{ 17, "17", nite::JOINT_LEFT_HIP },
	{ 18, "18", nite::JOINT_LEFT_KNEE },
	{ 20, "20", nite::JOINT_LEFT_FOOT },
	{ 21, "21", nite::JOINT_RIGHT_HIP },
	{ 22, "22", nite::JOINT_RIGHT_KNEE },
	{ 24, "24", nite::JOINT_RIGHT_FOOT }
};

/** Write a joint's worth of data */
cJSON* ZigFuJsonWriter::writeJoint(const nite::SkeletonJoint& joint, const int jointId) {
    cJSON *jointWritable = cJSON_CreateObject();
    
    // The joint ID - inside the joint JSON object, it is an int
    cJSON_AddNumberToObject(jointWritable, "id", jointId);
    
    // Position
    cJSON_AddItemToObject(jointWritable, "position", writePosition(joint));
    
    // Position confidence
    cJSON_AddNumberToObject(jointWritable, "positionconfidence", joint.getPositionConfidence());
    
    // Rotation (nite2 calls this orientation)
    cJSON_AddItemToObject(jointWritable, "rotation", writeOrientation(joint));
    
    // Rotation confidence
    cJSON_AddNumberToObject(jointWritable, "rotationconfidence", joint.getOrientationConfidence());
    
    return jointWritable;
}

/** Write the position of a joint in a 3-element array */
cJSON* ZigFuJsonWriter::writePosition(const nite::SkeletonJoint& joint) {
    const double posn [] = {
        joint.getPosition().x,
        joint.getPosition().y,
        joint.getPosition().z
    };
    
    return cJSON_CreateDoubleArray(posn, 3);
}

/** Write the rotation of a joint as a 3x3 rotation matrix */
cJSON* ZigFuJsonWriter::writeOrientation(const nite::SkeletonJoint& joint) {    
	// Get the value
	const nite::Quaternion niteQuat = joint.getOrientation();

	// Set out the returner value
	cJSON* returner;

	// FIRST TEST: ARE THEY ALL ZERO?
	// (If they are all zero, the rotation reading failed.)
	if ((niteQuat.w == 0) && (niteQuat.x == 0) && (niteQuat.y == 0) && (niteQuat.z == 0)) {
		// If all zero, return a rotation array of 9 zeroes
		const double rot[] = {
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
		};

		returner = cJSON_CreateDoubleArray(rot, 9);

	} else {
		// convert to a Boost quaternion
		// TODO does Boost want WXYZ or XYZW?
		const boost::math::quaternion<float> boostQuat(niteQuat.w,
			niteQuat.x, niteQuat.y, niteQuat.z);

		// Implemented using a Boost utility header file
		// which is not included in the Boost distribution itself.
		// Caution: this will throw exceptions if WXYZ is all zeroes
		const R3_matrix<float> boostRot = quaternion_to_R3_rotation(boostQuat);

		// Finally, pull the values out into a standard array
		const double rot[] = {
			boostRot.a11, boostRot.a12, boostRot.a13,
			boostRot.a21, boostRot.a22, boostRot.a23,
			boostRot.a31, boostRot.a32, boostRot.a33
		};

		returner = cJSON_CreateDoubleArray(rot, 9);
	}
    return returner;
}

/** Write a skeleton's worth of data */ 
cJSON* ZigFuJsonWriter::writeSkeleton(const nite::Skeleton& skeleton) {
    // The skeleton
    cJSON *skel = cJSON_CreateObject();
    
    // Add one by one; the structure is:
    // skeleton: { 1: { joint data }, 2: { joint data } ... }
    // can't iterate over the NiTE::JointType enum
    // so we cheat and define our own array of JointTuples
    //
	// Primitive arrays don't have a 'length' in C
	// so need to trick it out with sizeof
	// http://stackoverflow.com/a/204232
	size_t numElements = sizeof jointMappings / sizeof *jointMappings;

	for (int i(0); i < numElements; i++) {
		JointTuple tuple = jointMappings[i];

        cJSON_AddItemToObject(
            skel,
            tuple.stringId,
            writeJoint(skeleton.getJoint(tuple.type), tuple.id)
        );
    }        
        
    return skel;
}
    
cJSON* ZigFuJsonWriter::writeCenterOfMass(const nite::Point3f& center) {
    const double posn [] = {
        center.x,
        center.y,
        center.z
    };
        
    return cJSON_CreateDoubleArray(posn, 3);
}
    
cJSON* ZigFuJsonWriter::writeUser(const nite::UserData& user) {
    cJSON *root = cJSON_CreateObject();
    
    // User ID
    cJSON_AddNumberToObject(root, "id", user.getId());
        
    // Position tracked?
    cJSON_AddBoolToObject(root, "positionTracked", false);
        
    // Position (aka centre of mass)
    cJSON_AddItemToObject(root, "position", writeCenterOfMass(user.getCenterOfMass()));
        
    // Skeleton tracked?
    cJSON_AddBoolToObject(root, "skeletonTracked", true);
        
    // Skeleton
    cJSON_AddItemToObject(root, "skeleton", writeSkeleton(user.getSkeleton()));
    
    return root;      
}
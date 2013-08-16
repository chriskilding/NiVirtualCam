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

#pragma once

#include "JsonWriter.h"

using namespace std;
using namespace nite;

/**
 * An unholy mix of C and C++
 * for taking a NiTE user description
 * and turning it into ZigFu's JSON format.
 */
class ZigFuJsonWriter: public JsonWriter {
private:
	struct JointTuple {
		/** Integer ID of the joint. */
		int id;

		/**
		 * String version of the joint ID
		 * seems silly to include it
		 * but Zig needs it
		 * and it's faster to write it out here
		 * than to manually lexical_cast between string and int
		 * when we're actually building JSON at runtime
		 */
		char* stringId;

		/** The NITE joint type enum */
		nite::JointType type;
	};

	/** Maps Nite enums to Zig joint IDs. Note, this is initialized in the .cpp */
	static JointTuple jointMappings[];

public:
	ZigFuJsonWriter() {};

	~ZigFuJsonWriter() {};
    
	/** Write a joint's worth of data */
	cJSON* writeJoint(const nite::SkeletonJoint& joint, const int jointId);
    
	/** Write the position of a joint in a 3-element array */
	cJSON* writePosition(const nite::SkeletonJoint& joint);
    
	/** Write the rotation of a joint as a 3x3 rotation matrix */
	cJSON* writeOrientation(const nite::SkeletonJoint& joint);
    
	/** Write a skeleton's worth of data */ 
	cJSON* writeSkeleton(const nite::Skeleton& skeleton);
    
	/** Write a whole user's worth of data */
	cJSON* writeUser(const nite::UserData& user);
    
	/** Turn the center of mass into an overall ZigFu 'position' */
	cJSON* writeCenterOfMass(const nite::Point3f& center);
};
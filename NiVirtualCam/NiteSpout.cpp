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
#include "NiteSpout.h"

NiteSpout::NiteSpout(const Chanout<UserData>& _out)
    : out(_out)
{}
    
NiteSpout::NiteSpout(const Chanout<UserData>& _out, openni::Device& _device)
    : out(_out)
{
    // Use the device to create the userTracker
	Status niteRc;

	NiTE::initialize();

	niteRc = userTracker.create(&_device);
	if (niteRc != STATUS_OK) {
		std::cerr << "Couldn't create user tracker\n";
        exit(-1);
	}
}

NiteSpout::~NiteSpout() {
    NiTE::shutdown();
}
   
void NiteSpout::run() {
	Status niteRc;
    
	std::cout << "Start moving around to get detected...\n";
    std::cout << "PSI pose may be required for skeleton calibration, depending on the configuration\n";

	UserTrackerFrameRef userTrackerFrame;
    
	while (true) {
		niteRc = userTracker.readFrame(&userTrackerFrame);
        
		if (niteRc == STATUS_OK) {
			const Array<UserData>& users = userTrackerFrame.getUsers();
        
			// For now, we're forcing it to one user only
			if (users.getSize() > 0) {
				const UserData& user = users[0];
			
				if (user.isNew()) {
					userTracker.startSkeletonTracking(user.getId());
				} else if (user.getSkeleton().getState() == SKELETON_TRACKED) {
					// emit the whole user + skeleton data
					out << user;				
				}
			}
		} else {
			std::cerr << "Get next frame failed\n";
		}

	}
    
}
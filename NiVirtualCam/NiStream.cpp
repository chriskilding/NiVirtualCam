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
#include "NiStream.h"


NiStream::NiStream(const Device& _device, const openni::SensorType& _streamType) {
	stream = new openni::VideoStream();
	
	Status rc = stream->create(_device, _streamType);
        
    if (rc != openni::STATUS_OK) {
		std::cerr << "NiStream: Couldn't start stream:\n";
        std::cerr << openni::OpenNI::getExtendedError();
		openni::OpenNI::shutdown();
	}
}

void NiStream::start() {
	stream->start();
}

openni::Status NiStream::readFrame(VideoFrameRef& frame) {	
	Status rc = stream->readFrame(&frame);
	return rc;
}

NiStream::~NiStream() {
	stream->stop();
	stream->destroy();
	delete stream;
}
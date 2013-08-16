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
#include "NiDirectShowStream.h"

NiDirectShowStream::NiDirectShowStream(const int _width, const int _height) 
	: width(_width), height(_height), fileSize((1280 * 1024 * 3) + 3), dataSize(width * height * 3)
{
	sink.open("OpenNiVirtualCamFrameData", fileSize);
}

NiDirectShowStream::~NiDirectShowStream() {
	sink.close();
}

void NiDirectShowStream::sendBitmap(void * data) {
	// sizeof(data)
		
	// Final flags (C++ chars are 1 byte in size, so we use them as bytes):
	// Get an iterator pointing at the END of the file
	// then work backwards
	boost::iostreams::mapped_file_sink::iterator it = sink.end();

	// Write hasServer to fileSize - 1 (the end of file)
	const char hasServer = 0;
	*it = hasServer;

	// Move the iterator
	--it;

	// Write hasClient to fileSize - 2
	const char hasClient = 1;
	*it = hasClient;

	// Move the iterator
	--it;

	// Write res mode to fileSize - 3
	// 0 = some kind of 4:3 resolution
	const char resolutionMode = 0;
	*it = resolutionMode;

	// Now copy the data in, at the FRONT of the file
	memcpy(sink.data(), data, dataSize);
}
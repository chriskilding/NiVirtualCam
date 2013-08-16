/**
 * Copyright (c) 2013 Christopher Kilding and Soroush Falahati
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
#include "BitmapBroadcaster.h"

//static const int fileSize = (1280 * 1024 * 3) + 3;

BitmapBroadcaster::BitmapBroadcaster() {
	_hasClient = 0;
	lastChecked = getTimeStamp();
	fileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"OpenNiVirtualCamFrameData");
	if (fileHandle == NULL) {
		fileHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, fileSize, L"OpenNiVirtualCamFrameData");
	}
	file = MapViewOfFile(fileHandle, FILE_MAP_ALL_ACCESS, 0, 0, fileSize);  
	if (hasServer()) {
		throw "Only one server is allowed.";
	}
}

bool BitmapBroadcaster::SendBitmap(const openni::DepthPixel* image, const int w, const int h) {	
	// Determine the aspect ratio and resolution
	bool SD = w == 640 && h == 480;
    bool HD54 = w == 1280 && h == 1024;
    bool HD43 = w == 1280 && h == 960;
    
	// Determine the image data size (NOT the MMF size)
	int Size = w * h * 3;
    if (!SD && !HD54 && !HD43) {
		throw "Bad image size";
	}

	// Set the aspect ratio byte
	char *resByte = ((char*)file + fileSize - 3);
    if (HD54) {
        *resByte = 1;
	}
    else if (HD43) {
        *resByte = 2;
	}
    else {
        *resByte = 0;
	}
	
	/*
	 * We have uint16_t (DepthPixel) coming in.
	 * We need to write them in a space of 3 * uint8_t (BGR),
	 * condensing 16 bit values into 8 bit channels.
	 * TODO improve this by straddling the full uint16_t across multiple channels
	 */
	uint8_t bgr[3];
	for (int i(0); i < (w * h); i++)
	{
		// Get the current depth pixel from the image
		// Remember, a DepthPixel is a number stored in a uint16_t.
		// FIXME sometimes get access violations here!
		const openni::DepthPixel depth = image[i];

		// To avoid the terrible banding artifacts you get out of the box
		// (see the Depth Basics example in Kinect SDK)
		// smooth scaling must be applied;
		// the max value newer Kinects produce is 4096.
		// see http://stackoverflow.com/a/13193875
		uint8_t scaledVal = depth == 0 || depth > 4095 ? 0 : 255 - (uint8_t)(((float)depth / 4095.0f) * 255.0f);

		// Now fill in the color bytes, accounting for the colorspace conversion.
		bgr[0] = scaledVal;
		bgr[1] = scaledVal;
		bgr[2] = scaledVal;

		// An offset with a stride of 3 (bytes)
		// used for writing to the MMF
		// which, of course, is a bunch of 3 * uint8_ts
		const int fileOffset = (i * 3);

		// Copy the DepthPixel to the MMF
		// (size is going to be the 3 bytes of the bgr array)
		memcpy((char*)file + fileOffset, &bgr, 3);
	}

	// Finally set the server byte
	char *serverByte = ((char*)file + fileSize - 1);
	*serverByte = 1;
    return true;
}

bool BitmapBroadcaster::hasClient() {
    if (fileHandle != NULL && file != NULL)
    {
		char *clientByte = ((char*)file + fileSize - 2);
        if (_hasClient == 0 || getTimeStamp() - lastChecked > 2000)
        {
            *_hasClient = *clientByte == 1;
			*clientByte = 0;
        }
        return *_hasClient;
    }
    return false;
}

bool BitmapBroadcaster::hasServer() {
	if (fileHandle != NULL && file != NULL)
    {
		char *serverByte = ((char*)file + fileSize - 1);
		*serverByte = 0;
        int tried = 0;
        while (tried < 10)
        {
            if (*serverByte != 0)
                return true;
            Sleep(100);
            tried++;
        }
    }
	return false;
}
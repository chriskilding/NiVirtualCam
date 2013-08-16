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

#include "stdafx.h"

#include <boost/iostreams/device/mapped_file.hpp>

/**
 * A server to the DirectShow filter using Boost memory mapped files for portability.
 * Doesn't work yet!
 */
class NiDirectShowStream {
private:
	boost::iostreams::mapped_file_sink sink;
	const int width;
    const int height;

	/** Total size of the memory-mapped file, including headers and data. */
	const int fileSize;

	/** Size of the image data alone. Normally 640 x 480 x 3. */
	const int dataSize;

public:                  
	NiDirectShowStream(const int _width, const int _height);

	virtual ~NiDirectShowStream();
    
	void sendBitmap(void * data);
};
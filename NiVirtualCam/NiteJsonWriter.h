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
#include "JsonWriter.h"

using namespace nite;
using namespace csp;

/**
 * Takes a piece of nite user data,
 * converts to a JSON object
 */
class NiteJsonWriter: public CSProcess {
private:
    Chanin<UserData> in;
    Chanout<std::string> out;
	JsonWriter& writer;
    
protected:
    void run();

public:
	/**
	 * A Nite JSON writer takes an input channel, an output channel,
	 * and a JsonWriter subclass that will crank through writing the JSON
	 * in some particular structure.
	 */
    NiteJsonWriter(const Chanin<UserData>& _in, const Chanout<std::string>& _out, JsonWriter& _writer);
    ~NiteJsonWriter();
};
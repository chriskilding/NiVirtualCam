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
#include <set>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

typedef client::connection_ptr connection_ptr;

using namespace csp;

// Alias this common function
using websocketpp::lib::bind;

/**
 * A WebSocket client implemented as a CSP terminating process.
 * This client only sends data, it ignores any received data.
 */
class WebSocketClient: public CSProcess {
private:
    /** The channel upon which message strings are received from the previous CSProcess */
    Chanin<std::string> in;
    
    /** The WS client */
    client m_endpoint;

	/** The (one and only) connection handle. */
    connection_hdl handle;

    /** URI of the WS server. */
    std::string uri;

	void sendMessage(const std::string& msg);

protected:
    /** This is just the ASIO blocking event loop */
    void run();
    
public:
    WebSocketClient(const Chanin<std::string>& _in, const std::string& uri);
    
    ~WebSocketClient();
    
    void on_open(connection_hdl hdl);

    void on_close(connection_hdl hdl);

};
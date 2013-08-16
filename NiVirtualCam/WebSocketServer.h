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
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using namespace std;
using namespace boost;
using namespace csp;

/**
 * A slight adaptation of zaphoyd's broadcast_server
 * to let the server send of its own accord
 */
class WebSocketServer: public CSProcess {
private:
    /** The channel upon which message strings are received from the previous CSProcess */
    Chanin<string> in;
    
    /** The WS server */
    server m_server;
    
    /** The connection pool, modded so it does not use a C++11 comparator */
    set<connection_hdl> m_connections;

    /** A timer, to hook into the event loop and give us a way to send server-initiated messages */
    server::timer_ptr m_timer;
    
    /** Port upon which to communicate with The Outside */
    int port;
    
    /**
     * Delay in milliseconds between invocations of "nextMessage".
     * Since CSP handles the delays when you read from the Chanin,
     * this is to give the server a chance to deal with other client events. 
     */
    int delay;
    
protected:
    /** This is just the ASIO blocking event loop */
    void run();
    
public:
    WebSocketServer(const Chanin<string>& _in, const int _port);
    
    ~WebSocketServer();
    
    void on_open(connection_hdl hdl);

    void on_close(connection_hdl hdl);
    
    /**
     * An extra on top of zaphoyd's broadcast server
     * which allows the server to send strings
     * of its own accord
     */
    void nextMessage();
};
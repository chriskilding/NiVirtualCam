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

#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(const Chanin<string>& _in, const int _port)
    : in(_in), port(_port), delay(3)
{
    cout << "Init broadcast server" << endl;
    m_server.init_asio();
    
    m_server.set_open_handler(boost::bind(&WebSocketServer::on_open,this,::_1));
    m_server.set_close_handler(boost::bind(&WebSocketServer::on_close,this,::_1));
    
    cout << "Setting up message polling method" << endl;
    m_timer = m_server.set_timer(delay, websocketpp::lib::bind(&WebSocketServer::nextMessage, this));
}

WebSocketServer::~WebSocketServer() {
	cout << "Shutting down WebSocketServer" << endl;
}

void WebSocketServer::on_open(connection_hdl hdl) {
    cout << "Client joined broadcast server" << endl;
    m_connections.insert(hdl);
}

void WebSocketServer::on_close(connection_hdl hdl) {
    cout << "Client left broadcast server" << endl;
    m_connections.erase(hdl);
}

void WebSocketServer::run() {
    cout << "Run broadcast server" << endl;
    m_server.listen(port);
    cout << "Listening on port " << port << endl;
    m_server.start_accept();
    cout << "Broadcast server is now accepting connections" << endl;
    
    // This is the Boost asio service run loop
    // You need this or nothing will work
    // but it blocks!
    m_server.run();
}

void WebSocketServer::nextMessage() {
    string msg;
    in >> msg;
    
    cout << "Sending message initiated by server" << endl;
    
    for (set<connection_hdl>::iterator it = m_connections.begin(); it != m_connections.end(); ++it) {
        // when sending a string
        // must also indicate message type
        m_server.send(*it, msg, websocketpp::frame::opcode::TEXT);
    }
    
    
    m_timer = m_server.set_timer(delay, websocketpp::lib::bind(&WebSocketServer::nextMessage, this));
}
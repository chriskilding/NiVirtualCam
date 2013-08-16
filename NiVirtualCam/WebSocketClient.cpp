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
#include "WebSocketClient.h"

WebSocketClient::WebSocketClient(const Chanin<std::string>& _in, const std::string& _uri)
	: in(_in), uri(_uri)
{
	// set up access channels to only log connects and disconnects
	// (data is so large that the logging eats too much CPU time)
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.set_access_channels(websocketpp::log::alevel::connect);
    m_endpoint.set_access_channels(websocketpp::log::alevel::disconnect);

	// Initialize ASIO
    m_endpoint.init_asio();

	// Register our handlers
    //m_endpoint.set_message_handler(bind(&type::on_message,this,::_1,::_2));
    m_endpoint.set_close_handler(bind(&WebSocketClient::on_close, this, ::_1));
}

WebSocketClient::~WebSocketClient() {
	m_endpoint.stop();
}

void WebSocketClient::run() {    
    websocketpp::lib::error_code ec;
	client::connection_ptr con = m_endpoint.get_connection(uri, ec);

	if (ec) {
		std::cerr << ec.message() << std::endl;
	}
	else {
		// Store thread safe handle to the connection
		handle = m_endpoint.connect(con);

		// Fire up the ASIO io_service event loop
		// You need this or nothing will work
		// but because it blocks, we run it in a separate thread
		websocketpp::lib::thread asio_thread(&client::run, &m_endpoint);
    
		asio_thread.detach();

		// Now we hit the actual while loop for the CSProcess
		while (true) {
			std::string msg;
			in >> msg;
			sendMessage(msg);
		}
	}
}

void WebSocketClient::on_close(websocketpp::connection_hdl hdl) {
	std::cout << "WebSocketClient connection closed" << std::endl;
	// TODO clean up handle
}

void WebSocketClient::sendMessage(const std::string& msg) {
	//Check handle has not expired before sending
	if (!handle.expired()) {
		
		// when sending a string
		// must also indicate message type
		m_endpoint.send(handle, msg, websocketpp::frame::opcode::text);
	}
}
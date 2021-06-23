//////////#include <iostream>
//////////#include <wheel/http_server.hpp>
//////////#include <iguana/json.hpp>
//////////#include <sstream>
//////////
//////////namespace client
//////////{
//////////	struct person
//////////	{
//////////		std::string	name;
//////////		int64_t		 age;
//////////	};
//////////
//////////	REFLECTION(person, name, age);
//////////}
//////////
//////////struct service_proxy_configure
//////////{
//////////	int log_per_size;
//////////	int log_keep_num;
//////////	int log_level;
//////////};
//////////
//////////REFLECTION(service_proxy_configure,log_per_size, log_keep_num, log_level);
//////////
//////////using namespace wheel::http_servers;
//////////wheel::http_servers::http_server server;
//////////int main()
//////////{
//////////	client::person p1 = { "tom", 20 };
//////////	client::person p2 = { "jack", 19 };
//////////	client::person p3 = { "mike", 21 };
//////////
//////////	std::vector<client::person> v{ p1, p2, p3 };
//////////	iguana::string_stream ss;
//////////	iguana::json::to_json(ss, v);
//////////	auto json_str = ss.str();
//////////	std::cout << json_str << std::endl;
//////////
//////////	std::vector<client::person> v1;
//////////	iguana::json::from_json(v1, json_str.data(), json_str.length());
//////////
//////////	//service_proxy_configure t;
//////////	//t.log_per_size = 1;
//////////	//t.log_keep_num = 2;
//////////	//t.log_level = 3;
//////////
//////////	//std::string str = "{\"log_per_size\":200,\"log_keep_num\":1,\"log_level\":20}";
//////////	//bool r = iguana::json::from_json0(t, str.data(), str.length());
//////////
//////////	//std::stringstream ss;
//////////	//iguana::json::to_json(ss, t);
//////////	//auto json_str = ss.str();
//////////
//////////	//wheel::reflector::for_each_tuple_front(t, [](const auto& tp, const auto index) {
//////////	//	//auto value = wheel::reflector::get<index>(tp);
//////////	//	});
//////////
//////////	//auto tp = wheel::reflector::get_array<service_proxy_configure>();
//////////	server.listen(8084);
//////////
//////////	server.set_http_handler<GET, POST, OPTIONS, PUT>("/test", [](request& req, response& res) {
//////////		res.render_string("12345678");
//////////		});
//////////
//////////	server.run();
//////////}
////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio.hpp>
////////
////////
////////
////////
//////////#include <chrono>
//////////#include <cstdlib>
//////////#include <ctime>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <boost/beast/http/parser.hpp>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
//////////namespace my_program_state
//////////{
//////////	std::size_t
//////////		request_count()
//////////	{
//////////		static std::size_t count = 0;
//////////		return ++count;
//////////	}
//////////
//////////	std::time_t
//////////		now()
//////////	{
//////////		return std::time(0);
//////////	}
//////////}
//////////
//////////class http_connection : public std::enable_shared_from_this<http_connection>
//////////{
//////////public:
//////////	http_connection(tcp::socket socket)
//////////		: socket_(std::move(socket))
//////////	{
//////////	}
//////////
//////////	// Initiate the asynchronous operations associated with the connection.
//////////	void
//////////		start()
//////////	{
//////////		read_request();
//////////		check_deadline();
//////////	}
//////////
//////////private:
//////////	// The socket for the currently connected client.
//////////	tcp::socket socket_;
//////////
//////////	// The buffer for performing reads.
//////////	beast::flat_buffer buffer_{ 8192 };
//////////
//////////	// The request message.
//////////	http::request<http::dynamic_body> request_;
//////////
//////////	// The response message.
//////////	http::response<http::dynamic_body> response_;
//////////
//////////	// The timer for putting a deadline on connection processing.
//////////	net::steady_timer deadline_{
//////////		socket_.get_executor(), std::chrono::seconds(60) };
//////////
//////////	// Asynchronously receive a complete request message.
//////////	void
//////////		read_request()
//////////	{
//////////		auto self = shared_from_this();
//////////	
//////////		http::parser ps;
//////////		parser.on_chunk_header(callback);
//////////		http::async_read(
//////////			socket_,
//////////			buffer_,
//////////			request_,
//////////			[self](beast::error_code ec,
//////////				std::size_t bytes_transferred)
//////////			{
//////////				boost::ignore_unused(bytes_transferred);
//////////				if (!ec)
//////////					self->process_request();
//////////			});
//////////
//////////
//////////	}
//////////
//////////	// Determine what needs to be done with the request message.
//////////	void
//////////		process_request()
//////////	{
//////////		response_.version(request_.version());
//////////		response_.keep_alive(false);
//////////
//////////		switch (request_.method())
//////////		{
//////////		case http::verb::get:
//////////			response_.result(http::status::ok);
//////////			response_.set(http::field::server, "Beast");
//////////			create_response();
//////////			break;
//////////
//////////		default:
//////////			// We return responses indicating an error if
//////////			// we do not recognize the request method.
//////////			response_.result(http::status::bad_request);
//////////			response_.set(http::field::content_type, "text/plain");
//////////			beast::ostream(response_.body())
//////////				<< "Invalid request-method '"
//////////				<< std::string(request_.method_string())
//////////				<< "'";
//////////			break;
//////////		}
//////////
//////////		write_response();
//////////	}
//////////
//////////	// Construct a response message based on the program state.
//////////	void
//////////		create_response()
//////////	{
//////////		if (request_.target() == "/count")
//////////		{
//////////			response_.set(http::field::content_type, "text/html");
//////////			beast::ostream(response_.body())
//////////				<< "<html>\n"
//////////				<< "<head><title>Request count</title></head>\n"
//////////				<< "<body>\n"
//////////				<< "<h1>Request count</h1>\n"
//////////				<< "<p>There have been "
//////////				<< my_program_state::request_count()
//////////				<< " requests so far.</p>\n"
//////////				<< "</body>\n"
//////////				<< "</html>\n";
//////////		}
//////////		else if (request_.target() == "/time")
//////////		{
//////////			response_.set(http::field::content_type, "text/html");
//////////			beast::ostream(response_.body())
//////////				<< "<html>\n"
//////////				<< "<head><title>Current time</title></head>\n"
//////////				<< "<body>\n"
//////////				<< "<h1>Current time</h1>\n"
//////////				<< "<p>The current time is "
//////////				<< my_program_state::now()
//////////				<< " seconds since the epoch.</p>\n"
//////////				<< "</body>\n"
//////////				<< "</html>\n";
//////////		}
//////////		else
//////////		{
//////////			response_.result(http::status::not_found);
//////////			response_.set(http::field::content_type, "text/plain");
//////////			beast::ostream(response_.body()) << "File not found\r\n";
//////////		}
//////////	}
//////////
//////////	// Asynchronously transmit the response message.
//////////	void
//////////		write_response()
//////////	{
//////////		auto self = shared_from_this();
//////////
//////////		response_.set(http::field::content_length, response_.body().size());
//////////
//////////		http::async_write(
//////////			socket_,
//////////			response_,
//////////			[self](beast::error_code ec, std::size_t)
//////////			{
//////////				self->socket_.shutdown(tcp::socket::shutdown_send, ec);
//////////				self->deadline_.cancel();
//////////			});
//////////	}
//////////
//////////	// Check whether we have spent enough time on this connection.
//////////	void
//////////		check_deadline()
//////////	{
//////////		auto self = shared_from_this();
//////////
//////////		deadline_.async_wait(
//////////			[self](beast::error_code ec)
//////////			{
//////////				if (!ec)
//////////				{
//////////					// Close socket to cancel any outstanding operation.
//////////					self->socket_.close(ec);
//////////				}
//////////			});
//////////	}
//////////};
//////////
//////////// "Loop" forever accepting new connections.
//////////void
//////////http_server(tcp::acceptor& acceptor, tcp::socket& socket)
//////////{
//////////	acceptor.async_accept(socket,
//////////		[&](beast::error_code ec)
//////////		{
//////////			if (!ec)
//////////				std::make_shared<http_connection>(std::move(socket))->start();
//////////			http_server(acceptor, socket);
//////////		});
//////////}
//////////
//////////int
//////////main()
//////////{
//////////	try
//////////	{
//////////
//////////
//////////		auto const address = net::ip::make_address("127.0.0.1");
//////////		unsigned short port = static_cast<unsigned short>(std::atoi("8083"));
//////////
//////////		net::io_context ioc{ 1 };
//////////
//////////		tcp::acceptor acceptor{ ioc, {address, port} };
//////////		tcp::socket socket{ ioc };
//////////		http_server(acceptor, socket);
//////////
//////////		ioc.run();
//////////	}
//////////	catch (std::exception const& e)
//////////	{
//////////		std::cerr << "Error: " << e.what() << std::endl;
//////////		return EXIT_FAILURE;
//////////	}
//////////}
////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/connect.hpp>
//////////#include <boost/asio/ip/tcp.hpp>
//////////#include <cstdlib>
//////////#include <iostream>
//////////#include <string>
//////////
//////////namespace beast = boost::beast;     // from <boost/beast.hpp>
//////////namespace http = beast::http;       // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;        // from <boost/asio.hpp>
//////////using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
//////////
//////////				  // Performs an HTTP GET and prints the response
//////////int main()
//////////{
//////////	try
//////////	{
//////////		//const char* host = "47.111.173.190";
//////////		//const short port = 8111;
//////////		const std::string host = "127.0.0.1";
//////////		const short port = 8084;
//////////		//std::string target = "api/web/node/server/config";
//////////		std::string target = "/test";
//////////		int version = 11;
//////////
//////////		// The io_context is required for all I/O
//////////		net::io_context ioc;
//////////
//////////		// These objects perform our I/O
//////////		tcp::resolver resolver(ioc);
//////////		beast::tcp_stream stream(ioc);
//////////
//////////		// Look up the domain name
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		stream.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
//////////
//////////		// Set up an HTTP GET request message
//////////		http::request<http::string_body> req{ http::verb::get, target, version };
//////////		req.set("Token", "07728ee9faea4f619a563fbb7c15e390");
//////////		req.set("serverId", "257");
//////////
//////////		// Send the HTTP request to the remote host
//////////		http::write(stream, req);
//////////
//////////		// This buffer is used for reading and must be persisted
//////////		beast::flat_buffer buffer;
//////////
//////////		// Declare a container to hold the response
//////////		http::response<http::dynamic_body> res;
//////////
//////////		// Receive the HTTP response
//////////		http::read(stream, buffer, res);
//////////
//////////		// Write the message to standard out
//////////		std::cout << res << std::endl;
//////////
//////////		// Gracefully close the socket
//////////		beast::error_code ec;
//////////		stream.socket().shutdown(tcp::socket::shutdown_both, ec);
//////////
//////////		// not_connected happens sometimes
//////////		// so don't bother reporting it.
//////////		//
//////////		if (ec && ec != beast::errc::not_connected)
//////////			throw beast::system_error{ ec };
//////////
//////////		// If we get here then the connection is closed gracefully
//////////	}
//////////	catch (std::exception const& e)
//////////	{
//////////		std::cerr << "Error: " << e.what() << std::endl;
//////////		return EXIT_FAILURE;
//////////	}
//////////	return EXIT_SUCCESS;
//////////}
////////
////////////#define WHEEL_ENABLE_SSL
//////////#include <iostream>
//////////#include <wheel/http_server.hpp>
//////////#include <iguana/json.hpp>
//////////#include <sstream>
//////////#include <string>
//////////
//////////using namespace wheel::http_servers;
//////////wheel::http_servers::http_server server;
//////////int main()
//////////{
//////////	server.listen(8083);
//////////	//server.set_ssl_conf({"./user.api.chaojidun.com.crt","./user.api.chaojidun.com.key"});
//////////
//////////	//server.listen(9090);
//////////	server.set_http_handler<GET, POST, OPTIONS, PUT>("/test", [](request& req, response& res) {
//////////		res.render_string("123456783");
//////////		});
//////////
//////////	server.run();
//////////}
////////
//////////
//////////#include <iostream>
//////////#include "proxyEngine.h"
//////////#include <wheel/websocket_server.hpp>
//////////#include <unordered_map>
//////////#include <wheel/unit.hpp>
//////////
//////////
//////////
//////////int main()
//////////{
//////////	std::random_device rd;  // 将用于为随机数引擎获得种子
//////////	std::mt19937 gen(rd()); // 以播种标准 mersenne_twister_engine
//////////	std::uniform_int_distribution<> dis(0, 3);
//////////	uint32_t ret = dis(gen);
//////////
//////////	std::unordered_map<std::string, bool>distributed_ips_;
//////////	std::string ip = "127.0.0.1";
//////////	distributed_ips_.emplace(std::move(ip), true);
//////////	for (const auto info : distributed_ips_) {
//////////		std::cout << info.first << std::endl;
//////////	}
//////////
//////////	ProxyEngine eng;
//////////
//////////	std::shared_ptr<wheel::websocket::websocket_server> ptr = std::make_shared<wheel::websocket::websocket_server>(std::bind(&ProxyEngine::OnMessage1, &eng, std::placeholders::_1, std::placeholders::_2));
//////////	ptr->init(8084, 1);
//////////	//ptr->init(9090, 1);
//////////	ptr->run();
//////////
//////////}
//////////#define WHEEL_ENABLE_SSL
//////////#include <iostream>
//////////#include <wheel/http_server.hpp>
//////////#include <sstream>
//////////#include <string>
//////////
//////////using namespace std;
//////////
//////////using namespace wheel::http_servers;
//////////wheel::http_servers::http_server server;
//////////
//////////int main()
//////////{
//////////	server.set_ssl_conf({ "./crt.crt","./key.key" });
//////////
//////////	//server.listen(8443,32);//现在这个版本的http
//////////	//server.listen(8443,3000);//https test
//////////	//server.listen(8443);//https test
//////////	//server.listen(8443,1900);//https local
//////////	//server.listen(8443,1900);//http
//////////	//server.listen(8443, 2100);//http
//////////
//////////	server.listen(8443,1);
//////////
//////////	server.set_http_handler<GET,POST>("/", [](request& req, response& res) {
//////////		res.render_string("hello world");
//////////		});
//////////
//////////	server.run();
//////////}
//////////
////////
//////////#include <mutex>
//////////
//////////std::mutex m;
//////////
//////////
//////////int main()
//////////{
//////////	//std::unique_lock<std::mutex> lock1(m, std::defer_lock);
//////////	//std::unique_lock<std::mutex> lock2(m, std::defer_lock);
//////////
//////////	std::unique_lock<std::mutex> lock1(m);
//////////	std::unique_lock<std::mutex> lock2(m);
//////////}
////////
////////
//////////#include <iostream>
//////////#include <atomic>
//////////
//////////auto g_objenum = std::make_shared<std::atomic<long>>();
//////////
//////////long getobjenum()
//////////{
//////////	return (g_objenum.use_count() - 1);
//////////}
//////////
//////////class A {
//////////public:
//////////	A() {
//////////		m_objnum = g_objenum;
//////////		std::cout << getobjenum() << std::endl;
//////////	}
//////////	~A() {
//////////		m_objnum.reset();
//////////		std::cout << getobjenum() << std::endl;
//////////
//////////	}
//////////private:
//////////	std::shared_ptr<std::atomic<long>> m_objnum;
//////////};
//////////
//////////
//////////int main()
//////////{
//////////	int i1 = getobjenum();
//////////	  for (int i =0;i<100;++i){
//////////		  A a;
//////////		  std::cout << getobjenum() << std::endl;
//////////		  A b;	
//////////		  std::cout << getobjenum() << std::endl;
//////////		  A c;
//////////		  std::cout << getobjenum() << std::endl;
//////////		  A d;
//////////		  std::cout << getobjenum() << std::endl;
//////////		  A e;
//////////		  std::cout << getobjenum() << std::endl;
//////////		  A f;
//////////		  std::cout << getobjenum() << std::endl;
//////////	  }
//////////
//////////
//////////	  //std::cout << "11" << std::endl;
//////////	  int i =getobjenum();
//////////}
////////
////////////
//////////// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
////////////
//////////// Distributed under the Boost Software License, Version 1.0. (See accompanying
//////////// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////
//////////// Official repository: https://github.com/boostorg/beast
////////////
//////////
////////////------------------------------------------------------------------------------
////////////
//////////// Example: WebSocket server, asynchronous
////////////
////////////------------------------------------------------------------------------------
//////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/websocket.hpp>
//////////#include <boost/asio/dispatch.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <algorithm>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <thread>
//////////#include <vector>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Echoes back all received WebSocket messages
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	websocket::stream<beast::tcp_stream> ws_;
//////////	beast::flat_buffer buffer_;
//////////
//////////public:
//////////	// Take ownership of the socket
//////////	explicit
//////////		session(tcp::socket&& socket)
//////////		: ws_(std::move(socket))
//////////	{
//////////	}
//////////
//////////	// Get on the correct executor
//////////	void
//////////		run()
//////////	{
//////////		// We need to be executing within a strand to perform async operations
//////////		// on the I/O objects in this session. Although not strictly necessary
//////////		// for single-threaded contexts, this example code is written to be
//////////		// thread-safe by default.
//////////		net::dispatch(ws_.get_executor(),
//////////			beast::bind_front_handler(
//////////				&session::on_run,
//////////				shared_from_this()));
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		on_run()
//////////	{
//////////		// Set suggested timeout settings for the websocket
//////////		//ws_.set_option(
//////////		//	websocket::stream_base::timeout::suggested(
//////////		//		beast::role_type::server));
//////////
//////////		//// Set a decorator to change the Server of the handshake
//////////		//ws_.set_option(websocket::stream_base::decorator(
//////////		//	[](websocket::response_type& res)
//////////		//	{
//////////		//		res.set(http::field::server,
//////////		//			std::string(BOOST_BEAST_VERSION_STRING) +
//////////		//			" websocket-server-async");
//////////		//	}));
//////////		// Accept the websocket handshake
//////////		ws_.async_accept(
//////////			beast::bind_front_handler(
//////////				&session::on_accept,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_accept(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "accept");
//////////
//////////		// Read a message
//////////		do_read();
//////////	}
//////////
//////////	void
//////////		do_read()
//////////	{
//////////		// Read a message into our buffer
//////////		ws_.async_read(
//////////			buffer_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		// This indicates that the session was closed
//////////		if (ec == websocket::error::closed)
//////////			return;
//////////
//////////		if (ec)
//////////			fail(ec, "read");
//////////
//////////		std::cout << "连接成功" << std::endl;
//////////		// Echo the message
//////////		ws_.text(ws_.got_text());
//////////		ws_.async_write(
//////////			buffer_.data(),
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		// Clear the buffer
//////////		buffer_.consume(buffer_.size());
//////////
//////////		// Do another read
//////////		do_read();
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Accepts incoming connections and launches the sessions
//////////class listener : public std::enable_shared_from_this<listener>
//////////{
//////////	net::io_context& ioc_;
//////////	tcp::acceptor acceptor_;
//////////
//////////public:
//////////	listener(
//////////		net::io_context& ioc,
//////////		tcp::endpoint endpoint)
//////////		: ioc_(ioc)
//////////		, acceptor_(ioc)
//////////	{
//////////		beast::error_code ec;
//////////
//////////		// Open the acceptor
//////////		acceptor_.open(endpoint.protocol(), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "open");
//////////			return;
//////////		}
//////////
//////////		// Allow address reuse
//////////		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "set_option");
//////////			return;
//////////		}
//////////
//////////		// Bind to the server address
//////////		acceptor_.bind(endpoint, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "bind");
//////////			return;
//////////		}
//////////
//////////		// Start listening for connections
//////////		acceptor_.listen(
//////////			net::socket_base::max_listen_connections, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "listen");
//////////			return;
//////////		}
//////////	}
//////////
//////////	// Start accepting incoming connections
//////////	void
//////////		run()
//////////	{
//////////		do_accept();
//////////	}
//////////
//////////private:
//////////	void
//////////		do_accept()
//////////	{
//////////		// The new connection gets its own strand
//////////		acceptor_.async_accept(
//////////			net::make_strand(ioc_),
//////////			beast::bind_front_handler(
//////////				&listener::on_accept,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_accept(beast::error_code ec, tcp::socket socket)
//////////	{
//////////		if (ec)
//////////		{
//////////			fail(ec, "accept");
//////////		}
//////////		else
//////////		{
//////////			// Create the session and run it
//////////			std::make_shared<session>(std::move(socket))->run();
//////////		}
//////////
//////////		// Accept another connection
//////////		do_accept();
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char* argv[])
//////////{
//////////	auto const address = net::ip::make_address("0.0.0.0");
//////////	auto const port = static_cast<unsigned short>(std::atoi("9090"));
//////////	auto const threads = std::max<int>(1,5);
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc{ threads };
//////////
//////////	// Create and launch a listening port
//////////	std::make_shared<listener>(ioc, tcp::endpoint{ address, port })->run();
//////////
//////////	// Run the I/O service on the requested number of threads
//////////	std::vector<std::thread> v;
//////////	v.reserve(threads - 1);
//////////	for (auto i = threads - 1; i > 0; --i)
//////////		v.emplace_back(
//////////			[&ioc]
//////////			{
//////////				ioc.run();
//////////			});
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
////////
////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/websocket.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/bind_executor.hpp>
//////////#include <boost/asio/dispatch.hpp>
//////////#include <boost/asio/signal_set.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <boost/make_unique.hpp>
//////////#include <boost/optional.hpp>
//////////#include <algorithm>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <thread>
//////////#include <vector>
//////////
//////////namespace beast = boost::beast;                 // from <boost/beast.hpp>
//////////namespace http = beast::http;                   // from <boost/beast/http.hpp>
//////////namespace websocket = beast::websocket;         // from <boost/beast/websocket.hpp>
//////////namespace net = boost::asio;                    // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
//////////
//////////// Return a reasonable mime type based on the extension of a file.
//////////beast::string_view
//////////mime_type(beast::string_view path)
//////////{
//////////	using beast::iequals;
//////////	auto const ext = [&path]
//////////	{
//////////		auto const pos = path.rfind(".");
//////////		if (pos == beast::string_view::npos)
//////////			return beast::string_view{};
//////////		return path.substr(pos);
//////////	}();
//////////	if (iequals(ext, ".htm"))  return "text/html";
//////////	if (iequals(ext, ".html")) return "text/html";
//////////	if (iequals(ext, ".php"))  return "text/html";
//////////	if (iequals(ext, ".css"))  return "text/css";
//////////	if (iequals(ext, ".txt"))  return "text/plain";
//////////	if (iequals(ext, ".js"))   return "application/javascript";
//////////	if (iequals(ext, ".json")) return "application/json";
//////////	if (iequals(ext, ".xml"))  return "application/xml";
//////////	if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
//////////	if (iequals(ext, ".flv"))  return "video/x-flv";
//////////	if (iequals(ext, ".png"))  return "image/png";
//////////	if (iequals(ext, ".jpe"))  return "image/jpeg";
//////////	if (iequals(ext, ".jpeg")) return "image/jpeg";
//////////	if (iequals(ext, ".jpg"))  return "image/jpeg";
//////////	if (iequals(ext, ".gif"))  return "image/gif";
//////////	if (iequals(ext, ".bmp"))  return "image/bmp";
//////////	if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
//////////	if (iequals(ext, ".tiff")) return "image/tiff";
//////////	if (iequals(ext, ".tif"))  return "image/tiff";
//////////	if (iequals(ext, ".svg"))  return "image/svg+xml";
//////////	if (iequals(ext, ".svgz")) return "image/svg+xml";
//////////	return "application/text";
//////////}
//////////
//////////// Append an HTTP rel-path to a local filesystem path.
//////////// The returned path is normalized for the platform.
//////////std::string
//////////path_cat(
//////////	beast::string_view base,
//////////	beast::string_view path)
//////////{
//////////	if (base.empty())
//////////		return std::string(path);
//////////	std::string result(base);
//////////#ifdef BOOST_MSVC
//////////	char constexpr path_separator = '\\';
//////////	if (result.back() == path_separator)
//////////		result.resize(result.size() - 1);
//////////	result.append(path.data(), path.size());
//////////	for (auto& c : result)
//////////		if (c == '/')
//////////			c = path_separator;
//////////#else
//////////	char constexpr path_separator = '/';
//////////	if (result.back() == path_separator)
//////////		result.resize(result.size() - 1);
//////////	result.append(path.data(), path.size());
//////////#endif
//////////	return result;
//////////}
//////////
//////////// This function produces an HTTP response for the given
//////////// request. The type of the response object depends on the
//////////// contents of the request, so the interface requires the
//////////// caller to pass a generic lambda for receiving the response.
//////////template<
//////////	class Body, class Allocator,
//////////	class Send>
//////////	void
//////////	handle_request(
//////////		beast::string_view doc_root,
//////////		http::request<Body, http::basic_fields<Allocator>>&& req,
//////////		Send&& send)
//////////{
//////////	// Returns a bad request response
//////////	auto const bad_request =
//////////		[&req](beast::string_view why)
//////////	{
//////////		http::response<http::string_body> res{ http::status::bad_request, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, "text/html");
//////////		res.keep_alive(req.keep_alive());
//////////		res.body() = std::string(why);
//////////		res.prepare_payload();
//////////		return res;
//////////	};
//////////
//////////	// Returns a not found response
//////////	auto const not_found =
//////////		[&req](beast::string_view target)
//////////	{
//////////		http::response<http::string_body> res{ http::status::not_found, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, "text/html");
//////////		res.keep_alive(req.keep_alive());
//////////		res.body() = "The resource '" + std::string(target) + "' was not found.";
//////////		res.prepare_payload();
//////////		return res;
//////////	};
//////////
//////////	// Returns a server error response
//////////	auto const server_error =
//////////		[&req](beast::string_view what)
//////////	{
//////////		http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, "text/html");
//////////		res.keep_alive(req.keep_alive());
//////////		res.body() = "An error occurred: '" + std::string(what) + "'";
//////////		res.prepare_payload();
//////////		return res;
//////////	};
//////////
//////////	// Make sure we can handle the method
//////////	if (req.method() != http::verb::get &&
//////////		req.method() != http::verb::head)
//////////		return send(bad_request("Unknown HTTP-method"));
//////////
//////////	// Request path must be absolute and not contain "..".
//////////	if (req.target().empty() ||
//////////		req.target()[0] != '/' ||
//////////		req.target().find("..") != beast::string_view::npos)
//////////		return send(bad_request("Illegal request-target"));
//////////
//////////	// Build the path to the requested file
//////////	std::string path = path_cat(doc_root, req.target());
//////////	if (req.target().back() == '/')
//////////		path.append("index.html");
//////////
//////////	// Attempt to open the file
//////////	beast::error_code ec;
//////////	http::file_body::value_type body;
//////////	body.open(path.c_str(), beast::file_mode::scan, ec);
//////////
//////////	// Handle the case where the file doesn't exist
//////////	if (ec == beast::errc::no_such_file_or_directory)
//////////		return send(not_found(req.target()));
//////////
//////////	// Handle an unknown error
//////////	if (ec)
//////////		return send(server_error(ec.message()));
//////////
//////////	// Cache the size since we need it after the move
//////////	auto const size = body.size();
//////////
//////////	// Respond to HEAD request
//////////	if (req.method() == http::verb::head)
//////////	{
//////////		http::response<http::empty_body> res{ http::status::ok, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, mime_type(path));
//////////		res.content_length(size);
//////////		res.keep_alive(req.keep_alive());
//////////		return send(std::move(res));
//////////	}
//////////
//////////	// Respond to GET request
//////////	http::response<http::file_body> res{
//////////		std::piecewise_construct,
//////////		std::make_tuple(std::move(body)),
//////////		std::make_tuple(http::status::ok, req.version()) };
//////////	res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////	res.set(http::field::content_type, mime_type(path));
//////////	res.content_length(size);
//////////	res.keep_alive(req.keep_alive());
//////////	return send(std::move(res));
//////////}
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Echoes back all received WebSocket messages
//////////class websocket_session : public std::enable_shared_from_this<websocket_session>
//////////{
//////////	websocket::stream<beast::tcp_stream> ws_;
//////////	beast::flat_buffer buffer_;
//////////
//////////public:
//////////	// Take ownership of the socket
//////////	explicit
//////////		websocket_session(tcp::socket&& socket)
//////////		: ws_(std::move(socket))
//////////	{
//////////	}
//////////
//////////	// Start the asynchronous accept operation
//////////	template<class Body, class Allocator>
//////////	void
//////////		do_accept(http::request<Body, http::basic_fields<Allocator>> req)
//////////	{
//////////		// Set suggested timeout settings for the websocket
//////////		ws_.set_option(
//////////			websocket::stream_base::timeout::suggested(
//////////				beast::role_type::server));
//////////
//////////		// Set a decorator to change the Server of the handshake
//////////		ws_.set_option(websocket::stream_base::decorator(
//////////			[](websocket::response_type& res)
//////////			{
//////////				res.set(http::field::server,
//////////					std::string(BOOST_BEAST_VERSION_STRING) +
//////////					" advanced-server");
//////////			}));
//////////
//////////		// Accept the websocket handshake
//////////		ws_.async_accept(
//////////			req,
//////////			beast::bind_front_handler(
//////////				&websocket_session::on_accept,
//////////				shared_from_this()));
//////////	}
//////////
//////////private:
//////////	void
//////////		on_accept(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "accept");
//////////
//////////		// Read a message
//////////		do_read();
//////////	}
//////////
//////////	void
//////////		do_read()
//////////	{
//////////		// Read a message into our buffer
//////////		ws_.async_read(
//////////			buffer_,
//////////			beast::bind_front_handler(
//////////				&websocket_session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		// This indicates that the websocket_session was closed
//////////		if (ec == websocket::error::closed)
//////////			return;
//////////
//////////		if (ec)
//////////			fail(ec, "read");
//////////
//////////		// Echo the message
//////////		ws_.text(ws_.got_text());
//////////		ws_.async_write(
//////////			buffer_.data(),
//////////			beast::bind_front_handler(
//////////				&websocket_session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		// Clear the buffer
//////////		buffer_.consume(buffer_.size());
//////////
//////////		// Do another read
//////////		do_read();
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Handles an HTTP server connection
//////////class http_session : public std::enable_shared_from_this<http_session>
//////////{
//////////	// This queue is used for HTTP pipelining.
//////////	class queue
//////////	{
//////////		enum
//////////		{
//////////			// Maximum number of responses we will queue
//////////			limit = 8
//////////		};
//////////
//////////		// The type-erased, saved work item
//////////		struct work
//////////		{
//////////			virtual ~work() = default;
//////////			virtual void operator()() = 0;
//////////		};
//////////
//////////		http_session& self_;
//////////		std::vector<std::unique_ptr<work>> items_;
//////////
//////////	public:
//////////		explicit
//////////			queue(http_session& self)
//////////			: self_(self)
//////////		{
//////////			static_assert(limit > 0, "queue limit must be positive");
//////////			items_.reserve(limit);
//////////		}
//////////
//////////		// Returns `true` if we have reached the queue limit
//////////		bool
//////////			is_full() const
//////////		{
//////////			return items_.size() >= limit;
//////////		}
//////////
//////////		// Called when a message finishes sending
//////////		// Returns `true` if the caller should initiate a read
//////////		bool
//////////			on_write()
//////////		{
//////////			BOOST_ASSERT(!items_.empty());
//////////			auto const was_full = is_full();
//////////			items_.erase(items_.begin());
//////////			if (!items_.empty())
//////////				(*items_.front())();
//////////			return was_full;
//////////		}
//////////
//////////		// Called by the HTTP handler to send a response.
//////////		template<bool isRequest, class Body, class Fields>
//////////		void
//////////			operator()(http::message<isRequest, Body, Fields>&& msg)
//////////		{
//////////			// This holds a work item
//////////			struct work_impl : work
//////////			{
//////////				http_session& self_;
//////////				http::message<isRequest, Body, Fields> msg_;
//////////
//////////				work_impl(
//////////					http_session& self,
//////////					http::message<isRequest, Body, Fields>&& msg)
//////////					: self_(self)
//////////					, msg_(std::move(msg))
//////////				{
//////////				}
//////////
//////////				void
//////////					operator()()
//////////				{
//////////					http::async_write(
//////////						self_.stream_,
//////////						msg_,
//////////						beast::bind_front_handler(
//////////							&http_session::on_write,
//////////							self_.shared_from_this(),
//////////							msg_.need_eof()));
//////////				}
//////////			};
//////////
//////////			// Allocate and store the work
//////////			items_.push_back(
//////////				boost::make_unique<work_impl>(self_, std::move(msg)));
//////////
//////////			// If there was no previous work, start this one
//////////			if (items_.size() == 1)
//////////				(*items_.front())();
//////////		}
//////////	};
//////////
//////////	beast::tcp_stream stream_;
//////////	beast::flat_buffer buffer_;
//////////	std::shared_ptr<std::string const> doc_root_;
//////////	queue queue_;
//////////
//////////	// The parser is stored in an optional container so we can
//////////	// construct it from scratch it at the beginning of each new message.
//////////	boost::optional<http::request_parser<http::string_body>> parser_;
//////////
//////////public:
//////////	// Take ownership of the socket
//////////	http_session(
//////////		tcp::socket&& socket,
//////////		std::shared_ptr<std::string const> const& doc_root)
//////////		: stream_(std::move(socket))
//////////		, doc_root_(doc_root)
//////////		, queue_(*this)
//////////	{
//////////	}
//////////
//////////	// Start the session
//////////	void
//////////		run()
//////////	{
//////////		// We need to be executing within a strand to perform async operations
//////////		// on the I/O objects in this session. Although not strictly necessary
//////////		// for single-threaded contexts, this example code is written to be
//////////		// thread-safe by default.
//////////		net::dispatch(
//////////			stream_.get_executor(),
//////////			beast::bind_front_handler(
//////////				&http_session::do_read,
//////////				this->shared_from_this()));
//////////	}
//////////
//////////
//////////private:
//////////	void
//////////		do_read()
//////////	{
//////////		// Construct a new parser for each message
//////////		parser_.emplace();
//////////
//////////		// Apply a reasonable limit to the allowed size
//////////		// of the body in bytes to prevent abuse.
//////////		parser_->body_limit(10000);
//////////
//////////		// Set the timeout.
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Read a request using the parser-oriented interface
//////////		http::async_read(
//////////			stream_,
//////////			buffer_,
//////////			*parser_,
//////////			beast::bind_front_handler(
//////////				&http_session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(beast::error_code ec, std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		// This means they closed the connection
//////////		if (ec == http::error::end_of_stream)
//////////			return do_close();
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		// See if it is a WebSocket Upgrade
//////////		if (websocket::is_upgrade(parser_->get()))
//////////		{
//////////			// Create a websocket session, transferring ownership
//////////			// of both the socket and the HTTP request.
//////////			std::make_shared<websocket_session>(
//////////				stream_.release_socket())->do_accept(parser_->release());
//////////			return;
//////////		}
//////////
//////////		// Send the response
//////////		handle_request(*doc_root_, parser_->release(), queue_);
//////////
//////////		// If we aren't at the queue limit, try to pipeline another request
//////////		if (!queue_.is_full())
//////////			do_read();
//////////	}
//////////
//////////	void
//////////		on_write(bool close, beast::error_code ec, std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		if (close)
//////////		{
//////////			// This means we should close the connection, usually because
//////////			// the response indicated the "Connection: close" semantic.
//////////			return do_close();
//////////		}
//////////
//////////		// Inform the queue that a write completed
//////////		if (queue_.on_write())
//////////		{
//////////			// Read another request
//////////			do_read();
//////////		}
//////////	}
//////////
//////////	void
//////////		do_close()
//////////	{
//////////		// Send a TCP shutdown
//////////		beast::error_code ec;
//////////		stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
//////////
//////////		// At this point the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Accepts incoming connections and launches the sessions
//////////class listener : public std::enable_shared_from_this<listener>
//////////{
//////////	net::io_context& ioc_;
//////////	tcp::acceptor acceptor_;
//////////	std::shared_ptr<std::string const> doc_root_;
//////////
//////////public:
//////////	listener(
//////////		net::io_context& ioc,
//////////		tcp::endpoint endpoint,
//////////		std::shared_ptr<std::string const> const& doc_root)
//////////		: ioc_(ioc)
//////////		, acceptor_(net::make_strand(ioc))
//////////		, doc_root_(doc_root)
//////////	{
//////////		beast::error_code ec;
//////////
//////////		// Open the acceptor
//////////		acceptor_.open(endpoint.protocol(), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "open");
//////////			return;
//////////		}
//////////
//////////		// Allow address reuse
//////////		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "set_option");
//////////			return;
//////////		}
//////////
//////////		// Bind to the server address
//////////		acceptor_.bind(endpoint, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "bind");
//////////			return;
//////////		}
//////////
//////////		// Start listening for connections
//////////		acceptor_.listen(
//////////			net::socket_base::max_listen_connections, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "listen");
//////////			return;
//////////		}
//////////	}
//////////
//////////	// Start accepting incoming connections
//////////	void
//////////		run()
//////////	{
//////////		// We need to be executing within a strand to perform async operations
//////////		// on the I/O objects in this session. Although not strictly necessary
//////////		// for single-threaded contexts, this example code is written to be
//////////		// thread-safe by default.
//////////		net::dispatch(
//////////			acceptor_.get_executor(),
//////////			beast::bind_front_handler(
//////////				&listener::do_accept,
//////////				this->shared_from_this()));
//////////	}
//////////
//////////private:
//////////	void
//////////		do_accept()
//////////	{
//////////		// The new connection gets its own strand
//////////		acceptor_.async_accept(
//////////			net::make_strand(ioc_),
//////////			beast::bind_front_handler(
//////////				&listener::on_accept,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_accept(beast::error_code ec, tcp::socket socket)
//////////	{
//////////		if (ec)
//////////		{
//////////			fail(ec, "accept");
//////////		}
//////////		else
//////////		{
//////////			// Create the http session and run it
//////////			std::make_shared<http_session>(
//////////				std::move(socket),
//////////				doc_root_)->run();
//////////		}
//////////
//////////		// Accept another connection
//////////		do_accept();
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char* argv[])
//////////{
//////////	auto const address = net::ip::make_address("0.0.0.0");
//////////	auto const port = static_cast<unsigned short>(std::atoi("9090"));
//////////	auto const doc_root = std::make_shared<std::string>("/");
//////////	auto const threads = std::max<int>(1,5);
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc{ threads };
//////////
//////////	// Create and launch a listening port
//////////	std::make_shared<listener>(
//////////		ioc,
//////////		tcp::endpoint{ address, port },
//////////		doc_root)->run();
//////////
//////////	// Capture SIGINT and SIGTERM to perform a clean shutdown
//////////	net::signal_set signals(ioc, SIGINT, SIGTERM);
//////////	signals.async_wait(
//////////		[&](beast::error_code const&, int)
//////////		{
//////////			// Stop the `io_context`. This will cause `run()`
//////////			// to return immediately, eventually destroying the
//////////			// `io_context` and all of the sockets in it.
//////////			ioc.stop();
//////////		});
//////////
//////////	// Run the I/O service on the requested number of threads
//////////	std::vector<std::thread> v;
//////////	v.reserve(threads - 1);
//////////	for (auto i = threads - 1; i > 0; --i)
//////////		v.emplace_back(
//////////			[&ioc]
//////////			{
//////////				ioc.run();
//////////			});
//////////	ioc.run();
//////////
//////////	// (If we get here, it means we got a SIGINT or SIGTERM)
//////////
//////////	// Block until all the threads exit
//////////	for (auto& t : v)
//////////		t.join();
//////////
//////////	return EXIT_SUCCESS;
//////////}
//////////
////////////
//////////// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
////////////
//////////// Distributed under the Boost Software License, Version 1.0. (See accompanying
//////////// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////
//////////// Official repository: https://github.com/boostorg/beast
////////////
//////////
////////////------------------------------------------------------------------------------
////////////
//////////// Example: WebSocket SSL server, asynchronous
////////////
////////////------------------------------------------------------------------------------
//////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/ssl.hpp>
//////////#include <boost/beast/websocket.hpp>
//////////#include <boost/beast/websocket/ssl.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <boost/asio/dispatch.hpp>
//////////#include <algorithm>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <thread>
//////////#include <vector>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Echoes back all received WebSocket messages
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	websocket::stream<
//////////		beast::ssl_stream<beast::tcp_stream>> ws_;
//////////
//////////	beast::flat_buffer buffer_;
//////////
//////////public:
//////////	// Take ownership of the socket
//////////	session(tcp::socket&& socket, ssl::context& ctx)
//////////		: ws_(std::move(socket), ctx)
//////////	{
//////////	}
//////////
//////////	// Get on the correct executor
//////////	void
//////////		run()
//////////	{
//////////		// We need to be executing within a strand to perform async operations
//////////		// on the I/O objects in this session. Although not strictly necessary
//////////		// for single-threaded contexts, this example code is written to be
//////////		// thread-safe by default.
//////////		net::dispatch(ws_.get_executor(),
//////////			beast::bind_front_handler(
//////////				&session::on_run,
//////////				shared_from_this()));
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		on_run()
//////////	{
//////////		// Set the timeout.
//////////		beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
//////////
//////////		// Perform the SSL handshake
//////////		ws_.next_layer().async_handshake(
//////////			ssl::stream_base::server,
//////////			beast::bind_front_handler(
//////////				&session::on_handshake,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_handshake(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "handshake");
//////////
//////////		// Turn off the timeout on the tcp_stream, because
//////////		// the websocket stream has its own timeout system.
//////////		beast::get_lowest_layer(ws_).expires_never();
//////////
//////////		// Set suggested timeout settings for the websocket
//////////		ws_.set_option(
//////////			websocket::stream_base::timeout::suggested(
//////////				beast::role_type::server));
//////////
//////////		// Set a decorator to change the Server of the handshake
//////////		ws_.set_option(websocket::stream_base::decorator(
//////////			[](websocket::response_type& res)
//////////			{
//////////				res.set(http::field::server,
//////////					std::string(BOOST_BEAST_VERSION_STRING) +
//////////					" websocket-server-async-ssl");
//////////			}));
//////////
//////////		// Accept the websocket handshake
//////////		ws_.async_accept(
//////////			beast::bind_front_handler(
//////////				&session::on_accept,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_accept(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "accept");
//////////
//////////		// Read a message
//////////		do_read();
//////////	}
//////////
//////////	void
//////////		do_read()
//////////	{
//////////		// Read a message into our buffer
//////////		ws_.async_read(
//////////			buffer_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		// This indicates that the session was closed
//////////		if (ec == websocket::error::closed)
//////////			return;
//////////
//////////		if (ec)
//////////			fail(ec, "read");
//////////
//////////		// Echo the message
//////////		ws_.text(ws_.got_text());
//////////		ws_.async_write(
//////////			buffer_.data(),
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		// Clear the buffer
//////////		buffer_.consume(buffer_.size());
//////////
//////////		// Do another read
//////////		do_read();
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Accepts incoming connections and launches the sessions
//////////class listener : public std::enable_shared_from_this<listener>
//////////{
//////////	net::io_context& ioc_;
//////////	ssl::context& ctx_;
//////////	tcp::acceptor acceptor_;
//////////
//////////public:
//////////	listener(
//////////		net::io_context& ioc,
//////////		ssl::context& ctx,
//////////		tcp::endpoint endpoint)
//////////		: ioc_(ioc)
//////////		, ctx_(ctx)
//////////		, acceptor_(net::make_strand(ioc))
//////////	{
//////////		beast::error_code ec;
//////////
//////////		// Open the acceptor
//////////		acceptor_.open(endpoint.protocol(), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "open");
//////////			return;
//////////		}
//////////
//////////		// Allow address reuse
//////////		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "set_option");
//////////			return;
//////////		}
//////////
//////////		// Bind to the server address
//////////		acceptor_.bind(endpoint, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "bind");
//////////			return;
//////////		}
//////////
//////////		// Start listening for connections
//////////		acceptor_.listen(
//////////			net::socket_base::max_listen_connections, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "listen");
//////////			return;
//////////		}
//////////	}
//////////
//////////	// Start accepting incoming connections
//////////	void
//////////		run()
//////////	{
//////////		do_accept();
//////////	}
//////////
//////////private:
//////////	void
//////////		do_accept()
//////////	{
//////////		// The new connection gets its own strand
//////////		acceptor_.async_accept(
//////////			net::make_strand(ioc_),
//////////			beast::bind_front_handler(
//////////				&listener::on_accept,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_accept(beast::error_code ec, tcp::socket socket)
//////////	{
//////////		if (ec)
//////////		{
//////////			fail(ec, "accept");
//////////		}
//////////		else
//////////		{
//////////			// Create the session and run it
//////////			std::make_shared<session>(std::move(socket), ctx_)->run();
//////////		}
//////////
//////////		// Accept another connection
//////////		do_accept();
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char* argv[])
//////////{
//////////	auto const address = net::ip::make_address("0.0.0.0");
//////////	auto const port = 9090;
//////////	auto const threads = std::max<int>(1,5);
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc{ threads };
//////////
//////////	// The SSL context is required, and holds certificates
//////////	ssl::context ctx{ ssl::context::tlsv12 };
//////////
//////////	// This holds the self-signed certificate used by the server
//////////
//////////	// Create and launch a listening port
//////////	std::make_shared<listener>(ioc, ctx, tcp::endpoint{ address, port })->run();
//////////
//////////	// Run the I/O service on the requested number of threads
//////////	std::vector<std::thread> v;
//////////	v.reserve(threads - 1);
//////////	for (auto i = threads - 1; i > 0; --i)
//////////		v.emplace_back(
//////////			[&ioc]
//////////			{
//////////				ioc.run();
//////////			});
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
//////////
////////// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//////////
////////// Distributed under the Boost Software License, Version 1.0. (See accompanying
////////// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////
////////// Official repository: https://github.com/boostorg/beast
//////////
////////
//////////------------------------------------------------------------------------------
//////////
////////// Example: WebSocket client, asynchronous
//////////
//////////------------------------------------------------------------------------------
//////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/websocket.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Sends a WebSocket message and prints the response
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	tcp::resolver resolver_;
//////////	websocket::stream<beast::tcp_stream> ws_;
//////////	beast::flat_buffer buffer_;
//////////	std::string host_;
//////////	std::string text_;
//////////
//////////public:
//////////	// Resolver and socket require an io_context
//////////	explicit
//////////		session(net::io_context& ioc)
//////////		: resolver_(net::make_strand(ioc))
//////////		, ws_(net::make_strand(ioc))
//////////	{
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		run(
//////////			char const* host,
//////////			char const* port,
//////////			char const* text)
//////////	{
//////////		// Save these for later
//////////		host_ = host;
//////////		text_ = text;
//////////
//////////		// Look up the domain name
//////////		resolver_.async_resolve(
//////////			host,
//////////			port,
//////////			beast::bind_front_handler(
//////////				&session::on_resolve,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_resolve(
//////////			beast::error_code ec,
//////////			tcp::resolver::results_type results)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "resolve");
//////////
//////////		// Set the timeout for the operation
//////////		beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		beast::get_lowest_layer(ws_).async_connect(
//////////			results,
//////////			beast::bind_front_handler(
//////////				&session::on_connect,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "connect");
//////////
//////////		// Turn off the timeout on the tcp_stream, because
//////////		// the websocket stream has its own timeout system.
//////////		beast::get_lowest_layer(ws_).expires_never();
//////////
//////////		boost::system::error_code ec1;
//////////		static boost::asio::ip::tcp::no_delay no_delay(true);
//////////		ws_.next_layer().socket().set_option(no_delay, ec1);
//////////		ws_.next_layer().socket().set_option(boost::asio::socket_base::linger(true, 0), ec1);
//////////		ws_.next_layer().socket().set_option(boost::asio::socket_base::reuse_address(true), ec1);
//////////
//////////		// Set suggested timeout settings for the websocket
//////////		ws_.set_option(
//////////			websocket::stream_base::timeout::suggested(
//////////				beast::role_type::client));
//////////
//////////		// Set a decorator to change the User-Agent of the handshake
//////////		ws_.set_option(websocket::stream_base::decorator(
//////////			[](websocket::request_type& req)
//////////			{
//////////				req.set(http::field::user_agent,
//////////					std::string(BOOST_BEAST_VERSION_STRING) +
//////////					" websocket-client-async");
//////////			}));
//////////
//////////		// Update the host_ string. This will provide the value of the
//////////		// Host HTTP header during the WebSocket handshake.
//////////		// See https://tools.ietf.org/html/rfc7230#section-5.4
//////////		host_ += ':' + std::to_string(ep.port());
//////////
//////////		// Perform the websocket handshake
//////////		ws_.async_handshake(host_, "/",
//////////			beast::bind_front_handler(
//////////				&session::on_handshake,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_handshake(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "handshake");
//////////
//////////		// Send the message
//////////		ws_.async_write(
//////////			net::buffer(text_),
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		// Read a message into our buffer
//////////		ws_.async_read(
//////////			buffer_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////		std::cout << "总量" << bytes_transferred << std::endl;
//////////		std::string buffer_data	= boost::beast::buffers_to_string(buffer_.data());
//////////		std::cout << buffer_data << std::endl;
//////////		buffer_.consume(buffer_.size());
//////////
//////////		ws_.async_read(
//////////			buffer_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////		// Close the WebSocket connection
//////////		//ws_.async_close(websocket::close_code::normal,
//////////		//	beast::bind_front_handler(
//////////		//		&session::on_close,
//////////		//		shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_close(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "close");
//////////
//////////		// If we get here then the connection is closed gracefully
//////////
//////////		// The make_printable() function helps print a ConstBufferSequence
//////////		std::cout << beast::make_printable(buffer_.data()) << std::endl;
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char** argv)
//////////{
//////////	// Check command line arguments.
//////////
//////////	auto const host = "103.45.147.101";
//////////	auto const port = "7700";
//////////	auto const text = "{\"id\":1,\"act\":\"gameserver\"}";
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc;
//////////
//////////	// Launch the asynchronous operation
//////////	std::make_shared<session>(ioc)->run(host, port, text);
//////////
//////////	// Run the I/O service. The call will return when
//////////	// the socket is closed.
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
//////////
////////////
//////////// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
////////////
//////////// Distributed under the Boost Software License, Version 1.0. (See accompanying
//////////// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////
//////////// Official repository: https://github.com/boostorg/beast
////////////
//////////
////////////------------------------------------------------------------------------------
////////////
//////////// Example: Advanced server, flex (plain + SSL)
////////////
////////////------------------------------------------------------------------------------
//////////
//////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/ssl.hpp>
//////////#include <boost/beast/websocket.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/bind_executor.hpp>
//////////#include <boost/asio/dispatch.hpp>
//////////#include <boost/asio/signal_set.hpp>
//////////#include <boost/asio/steady_timer.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <boost/make_unique.hpp>
//////////#include <boost/optional.hpp>
//////////#include <algorithm>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <thread>
//////////#include <vector>
//////////
//////////namespace beast = boost::beast;                 // from <boost/beast.hpp>
//////////namespace http = beast::http;                   // from <boost/beast/http.hpp>
//////////namespace websocket = beast::websocket;         // from <boost/beast/websocket.hpp>
//////////namespace net = boost::asio;                    // from <boost/asio.hpp>
//////////namespace ssl = boost::asio::ssl;               // from <boost/asio/ssl.hpp>
//////////using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
//////////
//////////// Return a reasonable mime type based on the extension of a file.
//////////beast::string_view
//////////mime_type(beast::string_view path)
//////////{
//////////	using beast::iequals;
//////////	auto const ext = [&path]
//////////	{
//////////		auto const pos = path.rfind(".");
//////////		if (pos == beast::string_view::npos)
//////////			return beast::string_view{};
//////////		return path.substr(pos);
//////////	}();
//////////	if (iequals(ext, ".htm"))  return "text/html";
//////////	if (iequals(ext, ".html")) return "text/html";
//////////	if (iequals(ext, ".php"))  return "text/html";
//////////	if (iequals(ext, ".css"))  return "text/css";
//////////	if (iequals(ext, ".txt"))  return "text/plain";
//////////	if (iequals(ext, ".js"))   return "application/javascript";
//////////	if (iequals(ext, ".json")) return "application/json";
//////////	if (iequals(ext, ".xml"))  return "application/xml";
//////////	if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
//////////	if (iequals(ext, ".flv"))  return "video/x-flv";
//////////	if (iequals(ext, ".png"))  return "image/png";
//////////	if (iequals(ext, ".jpe"))  return "image/jpeg";
//////////	if (iequals(ext, ".jpeg")) return "image/jpeg";
//////////	if (iequals(ext, ".jpg"))  return "image/jpeg";
//////////	if (iequals(ext, ".gif"))  return "image/gif";
//////////	if (iequals(ext, ".bmp"))  return "image/bmp";
//////////	if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
//////////	if (iequals(ext, ".tiff")) return "image/tiff";
//////////	if (iequals(ext, ".tif"))  return "image/tiff";
//////////	if (iequals(ext, ".svg"))  return "image/svg+xml";
//////////	if (iequals(ext, ".svgz")) return "image/svg+xml";
//////////	return "application/text";
//////////}
//////////
//////////// Append an HTTP rel-path to a local filesystem path.
//////////// The returned path is normalized for the platform.
//////////std::string
//////////path_cat(
//////////	beast::string_view base,
//////////	beast::string_view path)
//////////{
//////////	if (base.empty())
//////////		return std::string(path);
//////////	std::string result(base);
//////////#ifdef BOOST_MSVC
//////////	char constexpr path_separator = '\\';
//////////	if (result.back() == path_separator)
//////////		result.resize(result.size() - 1);
//////////	result.append(path.data(), path.size());
//////////	for (auto& c : result)
//////////		if (c == '/')
//////////			c = path_separator;
//////////#else
//////////	char constexpr path_separator = '/';
//////////	if (result.back() == path_separator)
//////////		result.resize(result.size() - 1);
//////////	result.append(path.data(), path.size());
//////////#endif
//////////	return result;
//////////}
//////////
//////////// This function produces an HTTP response for the given
//////////// request. The type of the response object depends on the
//////////// contents of the request, so the interface requires the
//////////// caller to pass a generic lambda for receiving the response.
//////////template<
//////////	class Body, class Allocator,
//////////	class Send>
//////////	void
//////////	handle_request(
//////////		beast::string_view doc_root,
//////////		http::request<Body, http::basic_fields<Allocator>>&& req,
//////////		Send&& send)
//////////{
//////////	// Returns a bad request response
//////////	auto const bad_request =
//////////		[&req](beast::string_view why)
//////////	{
//////////		http::response<http::string_body> res{ http::status::bad_request, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, "text/html");
//////////		res.keep_alive(req.keep_alive());
//////////		res.body() = std::string(why);
//////////		res.prepare_payload();
//////////		return res;
//////////	};
//////////
//////////	// Returns a not found response
//////////	auto const not_found =
//////////		[&req](beast::string_view target)
//////////	{
//////////		http::response<http::string_body> res{ http::status::not_found, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, "text/html");
//////////		res.keep_alive(req.keep_alive());
//////////		res.body() = "The resource '" + std::string(target) + "' was not found.";
//////////		res.prepare_payload();
//////////		return res;
//////////	};
//////////
//////////	// Returns a server error response
//////////	auto const server_error =
//////////		[&req](beast::string_view what)
//////////	{
//////////		http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, "text/html");
//////////		res.keep_alive(req.keep_alive());
//////////		res.body() = "An error occurred: '" + std::string(what) + "'";
//////////		res.prepare_payload();
//////////		return res;
//////////	};
//////////
//////////	// Make sure we can handle the method
//////////	if (req.method() != http::verb::get &&
//////////		req.method() != http::verb::head)
//////////		return send(bad_request("Unknown HTTP-method"));
//////////
//////////	// Request path must be absolute and not contain "..".
//////////	if (req.target().empty() ||
//////////		req.target()[0] != '/' ||
//////////		req.target().find("..") != beast::string_view::npos)
//////////		return send(bad_request("Illegal request-target"));
//////////
//////////	// Build the path to the requested file
//////////	std::string path = path_cat(doc_root, req.target());
//////////	if (req.target().back() == '/')
//////////		path.append("index.html");
//////////
//////////	// Attempt to open the file
//////////	beast::error_code ec;
//////////	http::file_body::value_type body;
//////////	body.open(path.c_str(), beast::file_mode::scan, ec);
//////////
//////////	// Handle the case where the file doesn't exist
//////////	if (ec == beast::errc::no_such_file_or_directory)
//////////		return send(not_found(req.target()));
//////////
//////////	// Handle an unknown error
//////////	if (ec)
//////////		return send(server_error(ec.message()));
//////////
//////////	// Cache the size since we need it after the move
//////////	auto const size = body.size();
//////////
//////////	// Respond to HEAD request
//////////	if (req.method() == http::verb::head)
//////////	{
//////////		http::response<http::empty_body> res{ http::status::ok, req.version() };
//////////		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////		res.set(http::field::content_type, mime_type(path));
//////////		res.content_length(size);
//////////		res.keep_alive(req.keep_alive());
//////////		return send(std::move(res));
//////////	}
//////////
//////////	// Respond to GET request
//////////	http::response<http::file_body> res{
//////////		std::piecewise_construct,
//////////		std::make_tuple(std::move(body)),
//////////		std::make_tuple(http::status::ok, req.version()) };
//////////	res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//////////	res.set(http::field::content_type, mime_type(path));
//////////	res.content_length(size);
//////////	res.keep_alive(req.keep_alive());
//////////	return send(std::move(res));
//////////}
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	// ssl::error::stream_truncated, also known as an SSL "short read",
//////////	// indicates the peer closed the connection without performing the
//////////	// required closing handshake (for example, Google does this to
//////////	// improve performance). Generally this can be a security issue,
//////////	// but if your communication protocol is self-terminated (as
//////////	// it is with both HTTP and WebSocket) then you may simply
//////////	// ignore the lack of close_notify.
//////////	//
//////////	// https://github.com/boostorg/beast/issues/38
//////////	//
//////////	// https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
//////////	//
//////////	// When a short read would cut off the end of an HTTP message,
//////////	// Beast returns the error beast::http::error::partial_message.
//////////	// Therefore, if we see a short read here, it has occurred
//////////	// after the message has been completed, so it is safe to ignore it.
//////////
//////////	if (ec == net::ssl::error::stream_truncated)
//////////		return;
//////////
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Echoes back all received WebSocket messages.
//////////// This uses the Curiously Recurring Template Pattern so that
//////////// the same code works with both SSL streams and regular sockets.
//////////template<class Derived>
//////////class websocket_session
//////////{
//////////	// Access the derived class, this is part of
//////////	// the Curiously Recurring Template Pattern idiom.
//////////	Derived&
//////////		derived()
//////////	{
//////////		return static_cast<Derived&>(*this);
//////////	}
//////////
//////////	beast::flat_buffer buffer_;
//////////
//////////	// Start the asynchronous operation
//////////	template<class Body, class Allocator>
//////////	void
//////////		do_accept(http::request<Body, http::basic_fields<Allocator>> req)
//////////	{
//////////		// Set suggested timeout settings for the websocket
//////////		derived().ws().set_option(
//////////			websocket::stream_base::timeout::suggested(
//////////				beast::role_type::server));
//////////
//////////		// Set a decorator to change the Server of the handshake
//////////		derived().ws().set_option(
//////////			websocket::stream_base::decorator(
//////////				[](websocket::response_type& res)
//////////				{
//////////					res.set(http::field::server,
//////////						std::string(BOOST_BEAST_VERSION_STRING) +
//////////						" advanced-server-flex");
//////////				}));
//////////
//////////		// Accept the websocket handshake
//////////		derived().ws().async_accept(
//////////			req,
//////////			beast::bind_front_handler(
//////////				&websocket_session::on_accept,
//////////				derived().shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_accept(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "accept");
//////////
//////////		// Read a message
//////////		do_read();
//////////	}
//////////
//////////	void
//////////		do_read()
//////////	{
//////////		// Read a message into our buffer
//////////		derived().ws().async_read(
//////////			buffer_,
//////////			beast::bind_front_handler(
//////////				&websocket_session::on_read,
//////////				derived().shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		// This indicates that the websocket_session was closed
//////////		if (ec == websocket::error::closed)
//////////			return;
//////////
//////////		if (ec)
//////////			fail(ec, "read");
//////////
//////////		// Echo the message
//////////		derived().ws().text(derived().ws().got_text());
//////////		derived().ws().async_write(
//////////			buffer_.data(),
//////////			beast::bind_front_handler(
//////////				&websocket_session::on_write,
//////////				derived().shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		// Clear the buffer
//////////		buffer_.consume(buffer_.size());
//////////
//////////		// Do another read
//////////		do_read();
//////////	}
//////////
//////////public:
//////////	// Start the asynchronous operation
//////////	template<class Body, class Allocator>
//////////	void
//////////		run(http::request<Body, http::basic_fields<Allocator>> req)
//////////	{
//////////		// Accept the WebSocket upgrade request
//////////		do_accept(std::move(req));
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Handles a plain WebSocket connection
//////////class plain_websocket_session
//////////	: public websocket_session<plain_websocket_session>
//////////	, public std::enable_shared_from_this<plain_websocket_session>
//////////{
//////////	websocket::stream<beast::tcp_stream> ws_;
//////////
//////////public:
//////////	// Create the session
//////////	explicit
//////////		plain_websocket_session(
//////////			beast::tcp_stream&& stream)
//////////		: ws_(std::move(stream))
//////////	{
//////////	}
//////////
//////////	// Called by the base class
//////////	websocket::stream<beast::tcp_stream>&
//////////		ws()
//////////	{
//////////		return ws_;
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Handles an SSL WebSocket connection
//////////class ssl_websocket_session
//////////	: public websocket_session<ssl_websocket_session>
//////////	, public std::enable_shared_from_this<ssl_websocket_session>
//////////{
//////////	websocket::stream<
//////////		beast::ssl_stream<beast::tcp_stream>> ws_;
//////////
//////////public:
//////////	// Create the ssl_websocket_session
//////////	explicit
//////////		ssl_websocket_session(
//////////			beast::ssl_stream<beast::tcp_stream>&& stream)
//////////		: ws_(std::move(stream))
//////////	{
//////////	}
//////////
//////////	// Called by the base class
//////////	websocket::stream<
//////////		beast::ssl_stream<beast::tcp_stream>>&
//////////		ws()
//////////	{
//////////		return ws_;
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////template<class Body, class Allocator>
//////////void
//////////make_websocket_session(
//////////	beast::tcp_stream stream,
//////////	http::request<Body, http::basic_fields<Allocator>> req)
//////////{
//////////	std::make_shared<plain_websocket_session>(
//////////		std::move(stream))->run(std::move(req));
//////////}
//////////
//////////template<class Body, class Allocator>
//////////void
//////////make_websocket_session(
//////////	beast::ssl_stream<beast::tcp_stream> stream,
//////////	http::request<Body, http::basic_fields<Allocator>> req)
//////////{
//////////	std::make_shared<ssl_websocket_session>(
//////////		std::move(stream))->run(std::move(req));
//////////}
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Handles an HTTP server connection.
//////////// This uses the Curiously Recurring Template Pattern so that
//////////// the same code works with both SSL streams and regular sockets.
//////////template<class Derived>
//////////class http_session
//////////{
//////////	// Access the derived class, this is part of
//////////	// the Curiously Recurring Template Pattern idiom.
//////////	Derived&
//////////		derived()
//////////	{
//////////		return static_cast<Derived&>(*this);
//////////	}
//////////
//////////	// This queue is used for HTTP pipelining.
//////////	class queue
//////////	{
//////////		enum
//////////		{
//////////			// Maximum number of responses we will queue
//////////			limit = 8
//////////		};
//////////
//////////		// The type-erased, saved work item
//////////		struct work
//////////		{
//////////			virtual ~work() = default;
//////////			virtual void operator()() = 0;
//////////		};
//////////
//////////		http_session& self_;
//////////		std::vector<std::unique_ptr<work>> items_;
//////////
//////////	public:
//////////		explicit
//////////			queue(http_session& self)
//////////			: self_(self)
//////////		{
//////////			static_assert(limit > 0, "queue limit must be positive");
//////////			items_.reserve(limit);
//////////		}
//////////
//////////		// Returns `true` if we have reached the queue limit
//////////		bool
//////////			is_full() const
//////////		{
//////////			return items_.size() >= limit;
//////////		}
//////////
//////////		// Called when a message finishes sending
//////////		// Returns `true` if the caller should initiate a read
//////////		bool
//////////			on_write()
//////////		{
//////////			BOOST_ASSERT(!items_.empty());
//////////			auto const was_full = is_full();
//////////			items_.erase(items_.begin());
//////////			if (!items_.empty())
//////////				(*items_.front())();
//////////			return was_full;
//////////		}
//////////
//////////		// Called by the HTTP handler to send a response.
//////////		template<bool isRequest, class Body, class Fields>
//////////		void
//////////			operator()(http::message<isRequest, Body, Fields>&& msg)
//////////		{
//////////			// This holds a work item
//////////			struct work_impl : work
//////////			{
//////////				http_session& self_;
//////////				http::message<isRequest, Body, Fields> msg_;
//////////
//////////				work_impl(
//////////					http_session& self,
//////////					http::message<isRequest, Body, Fields>&& msg)
//////////					: self_(self)
//////////					, msg_(std::move(msg))
//////////				{
//////////				}
//////////
//////////				void
//////////					operator()()
//////////				{
//////////					http::async_write(
//////////						self_.derived().stream(),
//////////						msg_,
//////////						beast::bind_front_handler(
//////////							&http_session::on_write,
//////////							self_.derived().shared_from_this(),
//////////							msg_.need_eof()));
//////////				}
//////////			};
//////////
//////////			// Allocate and store the work
//////////			items_.push_back(
//////////				boost::make_unique<work_impl>(self_, std::move(msg)));
//////////
//////////			// If there was no previous work, start this one
//////////			if (items_.size() == 1)
//////////				(*items_.front())();
//////////		}
//////////	};
//////////
//////////	std::shared_ptr<std::string const> doc_root_;
//////////	queue queue_;
//////////
//////////	// The parser is stored in an optional container so we can
//////////	// construct it from scratch it at the beginning of each new message.
//////////	boost::optional<http::request_parser<http::string_body>> parser_;
//////////
//////////protected:
//////////	beast::flat_buffer buffer_;
//////////
//////////public:
//////////	// Construct the session
//////////	http_session(
//////////		beast::flat_buffer buffer,
//////////		std::shared_ptr<std::string const> const& doc_root)
//////////		: doc_root_(doc_root)
//////////		, queue_(*this)
//////////		, buffer_(std::move(buffer))
//////////	{
//////////	}
//////////
//////////	void
//////////		do_read()
//////////	{
//////////		// Construct a new parser for each message
//////////		parser_.emplace();
//////////
//////////		// Apply a reasonable limit to the allowed size
//////////		// of the body in bytes to prevent abuse.
//////////		parser_->body_limit(10000);
//////////
//////////		// Set the timeout.
//////////		beast::get_lowest_layer(
//////////			derived().stream()).expires_after(std::chrono::seconds(30));
//////////
//////////		// Read a request using the parser-oriented interface
//////////		http::async_read(
//////////			derived().stream(),
//////////			buffer_,
//////////			*parser_,
//////////			beast::bind_front_handler(
//////////				&http_session::on_read,
//////////				derived().shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(beast::error_code ec, std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		// This means they closed the connection
//////////		if (ec == http::error::end_of_stream)
//////////			return derived().do_eof();
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		// See if it is a WebSocket Upgrade
//////////		if (websocket::is_upgrade(parser_->get()))
//////////		{
//////////			// Disable the timeout.
//////////			// The websocket::stream uses its own timeout settings.
//////////			beast::get_lowest_layer(derived().stream()).expires_never();
//////////
//////////			// Create a websocket session, transferring ownership
//////////			// of both the socket and the HTTP request.
//////////			return make_websocket_session(
//////////				derived().release_stream(),
//////////				parser_->release());
//////////		}
//////////
//////////		// Send the response
//////////		handle_request(*doc_root_, parser_->release(), queue_);
//////////
//////////		// If we aren't at the queue limit, try to pipeline another request
//////////		if (!queue_.is_full())
//////////			do_read();
//////////	}
//////////
//////////	void
//////////		on_write(bool close, beast::error_code ec, std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		if (close)
//////////		{
//////////			// This means we should close the connection, usually because
//////////			// the response indicated the "Connection: close" semantic.
//////////			return derived().do_eof();
//////////		}
//////////
//////////		// Inform the queue that a write completed
//////////		if (queue_.on_write())
//////////		{
//////////			// Read another request
//////////			do_read();
//////////		}
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Handles a plain HTTP connection
//////////class plain_http_session
//////////	: public http_session<plain_http_session>
//////////	, public std::enable_shared_from_this<plain_http_session>
//////////{
//////////	beast::tcp_stream stream_;
//////////
//////////public:
//////////	// Create the session
//////////	plain_http_session(
//////////		beast::tcp_stream&& stream,
//////////		beast::flat_buffer&& buffer,
//////////		std::shared_ptr<std::string const> const& doc_root)
//////////		: http_session<plain_http_session>(
//////////			std::move(buffer),
//////////			doc_root)
//////////		, stream_(std::move(stream))
//////////	{
//////////	}
//////////
//////////	// Start the session
//////////	void
//////////		run()
//////////	{
//////////		this->do_read();
//////////	}
//////////
//////////	~plain_http_session() {
//////////		int i = 100;
//////////	}
//////////
//////////	// Called by the base class
//////////	beast::tcp_stream&
//////////		stream()
//////////	{
//////////		return stream_;
//////////	}
//////////
//////////	// Called by the base class
//////////	beast::tcp_stream
//////////		release_stream()
//////////	{
//////////		return std::move(stream_);
//////////	}
//////////
//////////	// Called by the base class
//////////	void
//////////		do_eof()
//////////	{
//////////		// Send a TCP shutdown
//////////		beast::error_code ec;
//////////		stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
//////////
//////////		// At this point the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Handles an SSL HTTP connection
//////////class ssl_http_session
//////////	: public http_session<ssl_http_session>
//////////	, public std::enable_shared_from_this<ssl_http_session>
//////////{
//////////	beast::ssl_stream<beast::tcp_stream> stream_;
//////////
//////////public:
//////////	// Create the http_session
//////////	ssl_http_session(
//////////		beast::tcp_stream&& stream,
//////////		ssl::context& ctx,
//////////		beast::flat_buffer&& buffer,
//////////		std::shared_ptr<std::string const> const& doc_root)
//////////		: http_session<ssl_http_session>(
//////////			std::move(buffer),
//////////			doc_root)
//////////		, stream_(std::move(stream), ctx)
//////////	{
//////////	}
//////////
//////////	// Start the session
//////////	void
//////////		run()
//////////	{
//////////		// Set the timeout.
//////////		beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));
//////////
//////////		// Perform the SSL handshake
//////////		// Note, this is the buffered version of the handshake.
//////////		stream_.async_handshake(
//////////			ssl::stream_base::server,
//////////			buffer_.data(),
//////////			beast::bind_front_handler(
//////////				&ssl_http_session::on_handshake,
//////////				shared_from_this()));
//////////	}
//////////
//////////	// Called by the base class
//////////	beast::ssl_stream<beast::tcp_stream>&
//////////		stream()
//////////	{
//////////		return stream_;
//////////	}
//////////
//////////	// Called by the base class
//////////	beast::ssl_stream<beast::tcp_stream>
//////////		release_stream()
//////////	{
//////////		return std::move(stream_);
//////////	}
//////////
//////////	// Called by the base class
//////////	void
//////////		do_eof()
//////////	{
//////////		// Set the timeout.
//////////		beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));
//////////
//////////		// Perform the SSL shutdown
//////////		stream_.async_shutdown(
//////////			beast::bind_front_handler(
//////////				&ssl_http_session::on_shutdown,
//////////				shared_from_this()));
//////////	}
//////////
//////////private:
//////////	void
//////////		on_handshake(
//////////			beast::error_code ec,
//////////			std::size_t bytes_used)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "handshake");
//////////
//////////		// Consume the portion of the buffer used by the handshake
//////////		buffer_.consume(bytes_used);
//////////
//////////		do_read();
//////////	}
//////////
//////////	void
//////////		on_shutdown(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "shutdown");
//////////
//////////		// At this point the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Detects SSL handshakes
//////////class detect_session : public std::enable_shared_from_this<detect_session>
//////////{
//////////	beast::tcp_stream stream_;
//////////	ssl::context& ctx_;
//////////	std::shared_ptr<std::string const> doc_root_;
//////////	beast::flat_buffer buffer_;
//////////
//////////public:
//////////	explicit
//////////		detect_session(
//////////			tcp::socket&& socket,
//////////			ssl::context& ctx,
//////////			std::shared_ptr<std::string const> const& doc_root)
//////////		: stream_(std::move(socket))
//////////		, ctx_(ctx)
//////////		, doc_root_(doc_root)
//////////	{
//////////	}
//////////
//////////	~detect_session() {
//////////		int i = 10;
//////////	}
//////////
//////////	// Launch the detector
//////////	void
//////////		run()
//////////	{
//////////		// We need to be executing within a strand to perform async operations
//////////		// on the I/O objects in this session. Although not strictly necessary
//////////		// for single-threaded contexts, this example code is written to be
//////////		// thread-safe by default.
//////////		net::dispatch(
//////////			stream_.get_executor(),
//////////			beast::bind_front_handler(
//////////				&detect_session::on_run,
//////////				this->shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_run()
//////////	{
//////////		// Set the timeout.
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		beast::async_detect_ssl(
//////////			stream_,
//////////			buffer_,
//////////			beast::bind_front_handler(
//////////				&detect_session::on_detect,
//////////				this->shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_detect(beast::error_code ec, bool result)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "detect");
//////////
//////////		if (result)
//////////		{
//////////			// Launch SSL session
//////////			std::make_shared<ssl_http_session>(
//////////				std::move(stream_),
//////////				ctx_,
//////////				std::move(buffer_),
//////////				doc_root_)->run();
//////////			return;
//////////		}
//////////
//////////		// Launch plain session
//////////		std::make_shared<plain_http_session>(
//////////			std::move(stream_),
//////////			std::move(buffer_),
//////////			doc_root_)->run();
//////////	}
//////////};
//////////
//////////// Accepts incoming connections and launches the sessions
//////////class listener : public std::enable_shared_from_this<listener>
//////////{
//////////	net::io_context& ioc_;
//////////	ssl::context& ctx_;
//////////	tcp::acceptor acceptor_;
//////////	std::shared_ptr<std::string const> doc_root_;
//////////
//////////public:
//////////	listener(
//////////		net::io_context& ioc,
//////////		ssl::context& ctx,
//////////		tcp::endpoint endpoint,
//////////		std::shared_ptr<std::string const> const& doc_root)
//////////		: ioc_(ioc)
//////////		, ctx_(ctx)
//////////		, acceptor_(net::make_strand(ioc))
//////////		, doc_root_(doc_root)
//////////	{
//////////		beast::error_code ec;
//////////
//////////		// Open the acceptor
//////////		acceptor_.open(endpoint.protocol(), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "open");
//////////			return;
//////////		}
//////////
//////////		// Allow address reuse
//////////		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "set_option");
//////////			return;
//////////		}
//////////
//////////		// Bind to the server address
//////////		acceptor_.bind(endpoint, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "bind");
//////////			return;
//////////		}
//////////
//////////		// Start listening for connections
//////////		acceptor_.listen(
//////////			net::socket_base::max_listen_connections, ec);
//////////		if (ec)
//////////		{
//////////			fail(ec, "listen");
//////////			return;
//////////		}
//////////	}
//////////
//////////	// Start accepting incoming connections
//////////	void
//////////		run()
//////////	{
//////////		do_accept();
//////////	}
//////////
//////////private:
//////////	void
//////////		do_accept()
//////////	{
//////////		// The new connection gets its own strand
//////////		acceptor_.async_accept(
//////////			net::make_strand(ioc_),
//////////			beast::bind_front_handler(
//////////				&listener::on_accept,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_accept(beast::error_code ec, tcp::socket socket)
//////////	{
//////////		if (ec)
//////////		{
//////////			fail(ec, "accept");
//////////		}
//////////		else
//////////		{
//////////			// Create the detector http_session and run it
//////////			std::make_shared<detect_session>(
//////////				std::move(socket),
//////////				ctx_,
//////////				doc_root_)->run();
//////////		}
//////////
//////////		// Accept another connection
//////////		do_accept();
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////class A
//////////{
//////////public:
//////////	A() {
//////////		std::cout << "A" << std::endl;
//////////	}
//////////
//////////	~A() {
//////////		std::cout << "~A()" << std::endl;
//////////	}
//////////};
//////////
//////////int main(int argc, char* argv[])
//////////{
//////////	auto ptr = std::make_shared<A>();
//////////
//////////	ptr = std::make_shared<A>();
//////////	auto const address = net::ip::make_address("0.0.0.0");
//////////	auto const port = static_cast<unsigned short>(std::atoi("9090"));
//////////	auto const doc_root = std::make_shared<std::string>("/");
//////////	auto const threads = std::max<int>(1,5);
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc{ threads };
//////////
//////////	// The SSL context is required, and holds certificates
//////////	ssl::context ctx{ ssl::context::tlsv12 };
//////////
//////////	// This holds the self-signed certificate used by the server
//////////	//load_server_certificate(ctx);
//////////
//////////	// Create and launch a listening port
//////////	std::make_shared<listener>(
//////////		ioc,
//////////		ctx,
//////////		tcp::endpoint{ address, port },
//////////		doc_root)->run();
//////////
//////////	// Capture SIGINT and SIGTERM to perform a clean shutdown
//////////	net::signal_set signals(ioc, SIGINT, SIGTERM);
//////////	signals.async_wait(
//////////		[&](beast::error_code const&, int)
//////////		{
//////////			// Stop the `io_context`. This will cause `run()`
//////////			// to return immediately, eventually destroying the
//////////			// `io_context` and all of the sockets in it.
//////////			ioc.stop();
//////////		});
//////////
//////////	// Run the I/O service on the requested number of threads
//////////	std::vector<std::thread> v;
//////////	v.reserve(threads - 1);
//////////	for (auto i = threads - 1; i > 0; --i)
//////////		v.emplace_back(
//////////			[&ioc]
//////////			{
//////////				ioc.run();
//////////			});
//////////	ioc.run();
//////////
//////////	// (If we get here, it means we got a SIGINT or SIGTERM)
//////////
//////////	// Block until all the threads exit
//////////	for (auto& t : v)
//////////		t.join();
//////////
//////////	return EXIT_SUCCESS;
//////////}
////////
//////////#include <iostream>
//////////#include <string>
//////////#include <nlohmann_json.hpp>
//////////
//////////int main()
//////////{
//////////	nlohmann::json js;
//////////	js["code"] = 500;
//////////	js["a"] = 80;
//////////	std::string stss = js.dump();
//////////	//std::string str = R"(" + stss+)\"";
//////////	std::string str = R"("\+ stss+")";
//////////}
////////
//////////#include <iostream>
//////////#include <chrono>
//////////
//////////std::int64_t get_time_stamp()
//////////{
//////////	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp =
//////////		std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
//////////
//////////	auto tmp = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
//////////	return tmp.count();
//////////}
//////////
//////////int main()
//////////{
//////////	std::int64_t time_stamp = get_time_stamp();
//////////	int i = 100;
//////////}
////////
////////
//////////#include<iostream>
//////////#include <atomic>
//////////#include <mutex>
//////////
//////////class my_mutex {
//////////public:
//////////	my_mutex() = default;
//////////	~my_mutex() {
//////////		if (is_lock_) {
//////////			unlock();
//////////		}
//////////	}
//////////	void lock() {
//////////		is_lock_ = true;
//////////		while (mutx_flag_.test_and_set(std::memory_order_acquire));
//////////	}
//////////	void unlock() {
//////////		is_lock_ = false;
//////////		mutx_flag_.clear(std::memory_order_release);
//////////	}
//////////
//////////	bool try_lock() {
//////////		if (is_lock_) {
//////////			return true;
//////////		}
//////////
//////////		lock();
//////////		return false;
//////////	}
//////////private:
//////////	bool is_lock_ = false;
//////////	std::atomic_flag mutx_flag_ = ATOMIC_FLAG_INIT;
//////////};
//////////
//////////my_mutex mt;
//////////void display()
//////////{
//////////	//std::unique_lock<my_mutex>lock(mt,std::defer_lock);	//zhijiebu shang suo
//////////	std::unique_lock<my_mutex>lock(mt,std::try_to_lock); //diaoyong try_lock函数
//////////	//std::unique_lock<my_mutex>lock1(mt);
//////////	//std::unique_lock<my_mutex>lock2(mt, std::adopt_lock);
//////////	std::cout << "11" << std::endl;
//////////}
//////////
//////////int main()
//////////{
//////////	std::lock_guard<my_mutex>lock(mt);
//////////	display();
//////////}
////////
//////////#include <iostream>
//////////#include <chrono>
//////////
//////////std::int64_t get_time_stamp()
//////////{
//////////	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp =
//////////		std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
//////////
//////////	auto tmp = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
//////////	return tmp.count();
//////////}
//////////
//////////bool is_in_same_day(std::int64_t t1, std::int64_t t2)
//////////{
//////////	return ((t1 - 57600) / 86400 == (t2 - 57600) / 86400);
//////////}
//////////
//////////int is_in_same_day1(std::int64_t t1)
//////////{
//////////	return (t1 - 57600) / 86400;
//////////}
//////////
//////////int main()
//////////{
//////////	std::int64_t t1 = get_time_stamp();
//////////	std::int64_t t2 = 1598075201;
//////////
//////////	auto tmp = localtime(&t2);
//////////
//////////	int ss = is_in_same_day1(t1);
//////////
//////////	//bool is_same = is_in_same_day(t1, t2);
//////////}
//////////
//////////#include <iostream>
//////////
//////////class A
//////////{
//////////public:
//////////	A() {
//////////		std::cout << "A()" << std::endl;
//////////	}
//////////
//////////	~A() {
//////////		std::cout << "~A()" << std::endl;
//////////	}
//////////
//////////	void display() {
//////////		std::cout << "asdw" << std::endl;
//////////	}
//////////};
//////////
//////////int main()
//////////{
//////////
//////////}
//////////
////////////
//////////// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
////////////
//////////// Distributed under the Boost Software License, Version 1.0. (See accompanying
//////////// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////
//////////// Official repository: https://github.com/boostorg/beast
////////////
//////////
////////////------------------------------------------------------------------------------
////////////
//////////// Example: HTTP client, asynchronous
////////////
////////////------------------------------------------------------------------------------
//////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include "nlohmann_json.hpp"
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Performs an HTTP GET and prints the response
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	beast::tcp_stream stream_;
//////////	beast::flat_buffer buffer_; // (Must persist between reads)
//////////	http::request<http::string_body> req_;
//////////	http::response<http::string_body> res_;
//////////
//////////public:
//////////	// Objects are constructed with a strand to
//////////	// ensure that handlers do not execute concurrently.
//////////	explicit
//////////		session(net::io_context& ioc)
//////////		:stream_(net::make_strand(ioc))
//////////	{
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		run(
//////////			char const* host,
//////////			char const* port,
//////////			char const* target,
//////////			int version)
//////////	{
//////////		// Set up an HTTP GET request message
//////////		req_.version(version);
//////////		req_.method(http::verb::post);
//////////		req_.target(target);
//////////		req_.set(http::field::host, host);
//////////		req_.set(http::field::content_type, "application/json");
//////////		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//////////		req_.body() = "[]";
//////////		req_.prepare_payload();
//////////
//////////		//req_.set(http::field::content_type, "application/json");
//////////		//nlohmann::json js;
//////////		//std::string json_str;
//////////
//////////		//js["code"] = 1;
//////////		//js["sss"] = 2;
//////////		//try
//////////		//{
//////////		//	json_str = js.dump();
//////////		//}
//////////		//catch (...)
//////////		//{
//////////		//	json_str = "";
//////////		//}
//////////
//////////		//req_.body() = std::move(json_str);
//////////
//////////
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		boost::beast::error_code ec;
//////////		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host, ec), 8118);
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		stream_.async_connect(
//////////			endpoint,
//////////			beast::bind_front_handler(
//////////				&session::on_connect,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_connect(beast::error_code ec)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "connect");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Send the HTTP request to the remote host
//////////		http::async_write(stream_, req_,
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		// Receive the HTTP response
//////////		http::async_read(stream_, buffer_, res_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		// Write the message to standard out
//////////		std::cout << res_ << std::endl;
//////////
//////////		// Gracefully close the socket
//////////		stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
//////////
//////////		// not_connected happens sometimes so don't bother reporting it.
//////////		if (ec && ec != beast::errc::not_connected)
//////////			return fail(ec, "shutdown");
//////////
//////////		// If we get here then the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char** argv)
//////////{
//////////
//////////	auto const host = "192.168.5.189";
//////////	auto const port = "8118";
//////////	auto const target = "/statistic/detail/1598422733?serverId=298&token=5ab5bd384e6641e29a3746d342f82422";
//////////	int version =11;
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc;
//////////
//////////	// Launch the asynchronous operation
//////////	std::make_shared<session>(ioc)->run(host, port, target, version);
//////////
//////////	// Run the I/O service. The call will return when
//////////	// the get operation is complete.
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
//////////
////////
//////////
////////// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//////////
////////// Distributed under the Boost Software License, Version 1.0. (See accompanying
////////// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////
////////// Official repository: https://github.com/boostorg/beast
//////////
////////
//////////------------------------------------------------------------------------------
//////////
////////// Example: HTTP client, asynchronous
//////////
//////////------------------------------------------------------------------------------
////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <atomic>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <list>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
//////////class my_mutex {
//////////public:
//////////	my_mutex() = default;
//////////	~my_mutex() {
//////////		if (is_lock_) {
//////////			unlock();
//////////		}
//////////	}
//////////
//////////	void lock() {
//////////		is_lock_ = true;
//////////		while (mutx_flag_.test_and_set(std::memory_order_acquire));
//////////	}
//////////
//////////	void unlock() {
//////////		is_lock_ = false;
//////////		mutx_flag_.clear(std::memory_order_release);
//////////	}
//////////
//////////	bool try_lock() {
//////////		if (is_lock_) {
//////////			return true;
//////////		}
//////////
//////////		lock();
//////////		return false;
//////////	}
//////////private:
//////////	bool is_lock_ = false;
//////////	std::atomic_flag mutx_flag_ = ATOMIC_FLAG_INIT;
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Performs an HTTP GET and prints the response
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	tcp::resolver resolver_;
//////////	beast::tcp_stream stream_;
//////////	beast::flat_buffer buffer_{8192}; // (Must persist between reads)
//////////	//std::string buffer_;
//////////	std::string res_buffer_;
//////////	http::request<http::empty_body> req_;
//////////	http::response<http::string_body> res_;
//////////	boost::optional<http::response_parser<http::dynamic_body>> res_parse_;
//////////public:
//////////	// Objects are constructed with a strand to
//////////	// ensure that handlers do not execute concurrently.
//////////	explicit
//////////		session(net::io_context& ioc)
//////////		: resolver_(net::make_strand(ioc))
//////////		, stream_(net::make_strand(ioc))
//////////	{
//////////
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		run(
//////////			char const* host,
//////////			char const* port,
//////////			char const* target,
//////////			int version)
//////////	{
//////////		// Set up an HTTP GET request message
//////////		req_.version(version);
//////////		req_.method(http::verb::get);
//////////		req_.target(target);
//////////		req_.set(http::field::host, host);
//////////		//req_.set(http::field::content_type, "application/octet-stream");
//////////		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//////////
//////////		// Look up the domain name
//////////		resolver_.async_resolve(
//////////			host,
//////////			port,
//////////			beast::bind_front_handler(
//////////				&session::on_resolve,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_resolve(
//////////			beast::error_code ec,
//////////			tcp::resolver::results_type results)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "resolve");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		stream_.async_connect(
//////////			results,
//////////			beast::bind_front_handler(
//////////				&session::on_connect,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "connect");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Send the HTTP request to the remote host
//////////		http::async_write(stream_, req_,
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////		test1();
//////////		////// Receive the HTTP response
//////////		//http::async_read(stream_, buffer_, res_,
//////////		//	beast::bind_front_handler(
//////////		//		&session::on_read,
//////////		//		shared_from_this()));
//////////
//////////		//stream_.async_read_some(boost::asio::buffer(&buffer_[0],buffer_.size()),
//////////		//	beast::bind_front_handler(
//////////		//		&session::on_read,
//////////		//		shared_from_this()));
//////////		//res_parse_.emplace();
//////////		//res_parse_->body_limit((std::numeric_limits<std::uint64_t>::max)());
//////////		//http::async_read_some(stream_, buffer_, *res_parse_,
//////////		//	beast::bind_front_handler(
//////////		//		&session::on_read,
//////////		//		shared_from_this()));
//////////	}
//////////
//////////	void on_read_body() {
//////////		//res_parse_.emplace();
//////////		res_parse_->body_limit(500 * 1024 * 1024);
//////////		res_parse_->body_limit((std::numeric_limits<std::uint64_t>::max)());
//////////		http::async_read_some(stream_, buffer_, *res_parse_,
//////////			beast::bind_front_handler(
//////////				&session::read_body,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void test1() {
//////////		res_parse_.emplace();
//////////		res_parse_->body_limit(500 * 1024 * 1024);
//////////		http::async_read_header(stream_, buffer_, *res_parse_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void read_body(beast::error_code ec,
//////////		std::size_t bytes_transferred)
//////////	{
//////////		if (ec){
//////////			return fail(ec, "read");
//////////		}
//////////		
//////////		res_parse_->body_limit((std::numeric_limits<std::uint64_t>::max)());
//////////		http::async_read_some(stream_, buffer_, *res_parse_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		bool falg = res_parse_->got_some();
//////////		// Write the message to standard out
//////////		std::cout << res_ << std::endl;
//////////
//////////		on_read_body();
//////////		return;
//////////		//std::cout << res_parse_.release() << std::endl;
//////////
//////////		// Gracefully close the socket
//////////		stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
//////////
//////////		// not_connected happens sometimes so don't bother reporting it.
//////////		if (ec && ec != beast::errc::not_connected)
//////////			return fail(ec, "shutdown");
//////////
//////////		// If we get here then the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char** argv)
//////////{
//////////	auto const host ="www.319game.cn";
//////////	auto const port ="80";
//////////	auto const target = "/dowload/319Csetup.exe";
//////////	int version = 11;
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc;
//////////
//////////	// Launch the asynchronous operation
//////////	std::make_shared<session>(ioc)->run(host, port, target, version);
//////////
//////////	// Run the I/O service. The call will return when
//////////	// the get operation is complete.
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
////////
//////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Performs an HTTP GET and prints the response
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	tcp::resolver resolver_;
//////////	beast::tcp_stream stream_;
//////////	beast::flat_buffer buffer_; // (Must persist between reads)
//////////	http::request<http::empty_body> req_;
//////////	http::response<http::string_body> res_;
//////////	boost::optional<http::response_parser<http::string_body>>res1_;
//////////
//////////	boost::optional<http::response_parser<http::empty_body>>res2_;
//////////	std::wstring buffer_str_;
//////////
//////////public:
//////////	// Objects are constructed with a strand to
//////////	// ensure that handlers do not execute concurrently.
//////////	explicit
//////////		session(net::io_context& ioc)
//////////		: resolver_(net::make_strand(ioc))
//////////		, stream_(net::make_strand(ioc))
//////////	{
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		run(
//////////			char const* host,
//////////			char const* port,
//////////			char const* target,
//////////			int version)
//////////	{
//////////		// Set up an HTTP GET request message
//////////		req_.version(version);
//////////		req_.method(http::verb::get);
//////////		req_.target(target);
//////////		req_.set(http::field::host, host);
//////////		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//////////
//////////		// Look up the domain name
//////////		resolver_.async_resolve(
//////////			host,
//////////			port,
//////////			beast::bind_front_handler(
//////////				&session::on_resolve,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_resolve(
//////////			beast::error_code ec,
//////////			tcp::resolver::results_type results)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "resolve");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		stream_.async_connect(
//////////			results,
//////////			beast::bind_front_handler(
//////////				&session::on_connect,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "connect");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Send the HTTP request to the remote host
//////////		http::async_write(stream_, req_,
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void test0() {
//////////		// Receive the HTTP response
//////////		http::async_read(stream_, buffer_, res_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void test1(){
//////////		stream_.async_read_some(buffer_.prepare(1*1024*1024), beast::bind_front_handler(
//////////			&session::on_read,
//////////			shared_from_this()));
//////////	}
//////////
//////////	void test2() {
//////////		// Receive the HTTP response
//////////		//res1_->skip(true);
//////////		res1_->body_limit(500 * 1024 * 1024);
//////////		http::async_read_header(stream_, buffer_,*res1_,
//////////			beast::bind_front_handler(
//////////				&session::on_read_header,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void test3()
//////////	{
//////////		res1_->body_limit(500 * 1024 * 1024);
//////////		http::async_read_some(stream_, buffer_,*res1_,
//////////			beast::bind_front_handler(
//////////				&session::on_read1,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void test4() {
//////////		res1_.emplace();
//////////		res1_->body_limit(500 * 1024 * 1024);
//////////		// Receive the HTTP response
//////////		http::async_read_header(stream_, buffer_,*res1_,
//////////			beast::bind_front_handler(
//////////				&session::on_read2,
//////////				shared_from_this()));
//////////	}
//////////
//////////
//////////	void test5() {
//////////		res2_.emplace();
//////////		res2_->body_limit(500 * 1024 * 1024);
//////////		// Receive the HTTP response
//////////		http::async_read_header(stream_, buffer_, *res2_,
//////////			beast::bind_front_handler(
//////////				&session::on_read3,
//////////				shared_from_this()));
//////////	}
//////////	void read_body1() {
//////////		auto size = res1_->content_length().value();
//////////		buffer_str_.resize(size);
//////////		stream_.async_read_some(boost::asio::buffer(&buffer_str_[0], size), [self = shared_from_this()](beast::error_code ec,
//////////			std::size_t bytes_transferred){
//////////			if (ec){
//////////				std::cout << ec.message() << std::endl;
//////////				return;
//////////			}
//////////
//////////			boost::asio::buffer_copy(self->buffer_.prepare(self->buffer_str_.size()), boost::asio::buffer(self->buffer_str_));
//////////
//////////			self->res1_->put(self->buffer_.data(), ec);
//////////
//////////			if (self->res1_->is_done()){
//////////				std::cout << self->res1_->release() << std::endl;
//////////			}
//////////		});
//////////
//////////	}
//////////
//////////	void
//////////		on_read2(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "on_read2");
//////////
//////////		read_body1();
//////////	}
//////////
//////////
//////////	void
//////////		on_read3(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "on_read3");
//////////
//////////		res1_.emplace(std::move(res2_.get()));
//////////
//////////		//std::cout << res1_->release() << std::endl;
//////////		on_read4();
//////////	}
//////////
//////////
//////////	void on_read4() {
//////////
//////////		http::async_read(stream_, buffer_,*res1_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void on_read1(beast::error_code ec,
//////////		std::size_t bytes_transferred)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "on_read1");
//////////
//////////		std::cout << res1_->release() << std::endl;
//////////
//////////		if (!res1_->is_done()) {
//////////			buffer_.consume(bytes_transferred);
//////////			buffer_.commit(bytes_transferred);
//////////			test3();
//////////			return;
//////////		}
//////////
//////////	}
//////////
//////////
//////////	void on_read_header(beast::error_code ec,
//////////		std::size_t bytes_transferred) {
//////////		if (ec){
//////////			return fail(ec, "on_read_header");
//////////		}
//////////
//////////		read_body();
//////////	}
//////////
//////////	//void read_body(){
//////////	//	http::async_read_some(stream_, buffer_, *res1_, 
//////////	//		beast::bind_front_handler([self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred){
//////////	//		if (ec){
//////////	//			return fail(ec, "read_body");
//////////	//		}
//////////
//////////	//		std::cout << self->res1_->release() << std::endl;
//////////	//		if (!self->res1_->is_done()){
//////////	//			self->read_body();
//////////	//			return;
//////////	//		}
//////////
//////////	//		std::cout << self->res1_->release() << std::endl;
//////////	//	}));
//////////	//}
//////////
//////////	void read_body() {
//////////		stream_.async_read_some(buffer_.prepare(1 * 1024 * 1024), beast::bind_front_handler([self = shared_from_this()](beast::error_code ec,
//////////			std::size_t bytes_transferred){
//////////			if (ec) {
//////////				std::cout << self->res1_->release() << std::endl;
//////////				return fail(ec, "read_body");
//////////			}
//////////
//////////			self->buffer_.commit(bytes_transferred);
//////////			size_t size = self->res1_->put(self->buffer_.data(), ec);
//////////			//self->buffer_.consume(size);
//////////			if (!self->res1_->is_done()) {
//////////				self->read_body();
//////////				return;
//////////			}
//////////
//////////			std::cout << self->res1_->release() << std::endl;
//////////		}));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		//test0();
//////////		//res1_.emplace();
//////////		//test3();
//////////		//test2();
//////////		test5();
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		buffer_.commit(bytes_transferred);
//////////		res1_->put(buffer_.data(), ec);
//////////		if (!res1_->is_done()) {
//////////			test1();
//////////			return;
//////////		}
//////////
//////////		// Write the message to standard out
//////////		//std::cout << res_ << std::endl;
//////////		std::cout << res1_->release() << std::endl;
//////////		// Gracefully close the socket
//////////		stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
//////////
//////////		// not_connected happens sometimes so don't bother reporting it.
//////////		if (ec && ec != beast::errc::not_connected)
//////////			return fail(ec, "shutdown");
//////////
//////////		// If we get here then the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char** argv)
//////////{
//////////	//auto const host = "www.baidu.com";
//////////	//auto const port = "80";
//////////	//auto const target = "/";
//////////	//int version =  11;
//////////
//////////	auto const host = "www.319game.cn";
//////////	auto const port = "80";
//////////	auto const target = "/dowload/319Csetup.exe";
//////////	int version = 11;
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc;
//////////
//////////	// Launch the asynchronous operation
//////////	std::make_shared<session>(ioc)->run(host, port, target, version);
//////////
//////////	// Run the I/O service. The call will return when
//////////	// the get operation is complete.
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////// Report a failure
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Performs an HTTP GET and prints the response
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	tcp::resolver resolver_;
//////////	beast::tcp_stream stream_;
//////////	beast::flat_static_buffer<50*1024*1024> buffer_; // (Must persist between reads)
//////////	http::request<http::string_body> req_;
//////////	http::response<http::string_body> res_;
//////////
//////////	boost::optional<http::response_parser<http::dynamic_body>>res1_;
//////////public:
//////////	// Objects are constructed with a strand to
//////////	// ensure that handlers do not execute concurrently.
//////////	explicit
//////////		session(net::io_context& ioc)
//////////		: resolver_(net::make_strand(ioc))
//////////		, stream_(net::make_strand(ioc))
//////////	{
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		run(
//////////			char const* host,
//////////			char const* port,
//////////			char const* target,
//////////			int version)
//////////	{
//////////		// Set up an HTTP GET request message
//////////		req_.version(version);
//////////		req_.method(http::verb::get);
//////////		req_.target(target);
//////////		req_.set(http::field::host, host);
//////////		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//////////
//////////		// Look up the domain name
//////////		resolver_.async_resolve(
//////////			host,
//////////			port,
//////////			beast::bind_front_handler(
//////////				&session::on_resolve,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_resolve(
//////////			beast::error_code ec,
//////////			tcp::resolver::results_type results)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "resolve");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		stream_.async_connect(
//////////			results,
//////////			beast::bind_front_handler(
//////////				&session::on_connect,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "connect");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Send the HTTP request to the remote host
//////////		http::async_write(stream_, req_,
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		//read_header();
//////////		res1_.emplace();
//////////		res1_->body_limit(500 * 1024 * 1024);
//////////
//////////		http::async_read(stream_, buffer_, *res1_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////		//res1_->header_limit(1024);
//////////		//// Receive the HTTP response
//////////		//http::async_read(stream_, buffer_,*res1_,
//////////		//	beast::bind_front_handler(
//////////		//		&session::on_read,
//////////		//		shared_from_this()));
//////////
//////////	}
//////////
//////////	void read_header() {
//////////		res1_.emplace();
//////////		res1_->body_limit(500 * 1024 * 1024);
//////////
//////////		//buffer_.prepare(500 * 1024 * 1024);
//////////		// Receive the HTTP response
//////////		http::async_read_header(stream_, buffer_, *res1_,
//////////			beast::bind_front_handler(
//////////				&session::on_read_header,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void on_read_header(beast::error_code ec,
//////////		std::size_t bytes_transferred) {
//////////		if (ec){
//////////			return fail(ec, "on_read_header");
//////////		}
//////////
//////////		//buffer_.prepare(500 * 1024 * 1024);
//////////		http::async_read(stream_, buffer_, *res1_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		// Write the message to standard out
//////////		//std::cout << res1_->release() << std::endl;
//////////
//////////		// Gracefully close the socket
//////////		stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
//////////
//////////		// not_connected happens sometimes so don't bother reporting it.
//////////		if (ec && ec != beast::errc::not_connected)
//////////			return fail(ec, "shutdown");
//////////
//////////		// If we get here then the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char** argv)
//////////{
//////////	auto const host = "www.319game.cn";
//////////	auto const port = "80";
//////////	auto const target = "/dowload/319Csetup.exe";
//////////	int version = 11;
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc;
//////////
//////////	// Launch the asynchronous operation
//////////	std::make_shared<session>(ioc)->run(host, port, target, version);
//////////
//////////	// Run the I/O service. The call will return when
//////////	// the get operation is complete.
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
////////
////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <sstream>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////template<
//////////	bool isRequest,
//////////	class SyncReadStream,
//////////	class DynamicBuffer>
//////////	void
//////////	read_and_print_body(
//////////		SyncReadStream& stream,
//////////		DynamicBuffer& buffer,
//////////		beast::error_code& ec)
//////////{
//////////	http::parser<isRequest,http::buffer_body> p;
//////////	p.body_limit(500* 1024 * 1024);
//////////	read_header(stream, buffer, p, ec);
//////////	if (ec) {
//////////		std::cout << ec.message() << std::endl;
//////////		return;
//////////	}
//////////
//////////	std::string str_buffer;
//////////	std::ostringstream istr;
//////////	while (!p.is_done())
//////////	{
//////////		char buf[512] = {0};
//////////
//////////		p.get().body().data = &buf[0];
//////////		p.get().body().size = sizeof(buf);
//////////		read(stream, buffer, p, ec);
//////////		if (ec == beast::http::error::need_buffer)
//////////			ec = {};
//////////		if (ec) {
//////////			//beast::http::error::partial_message
//////////			std::cout << ec.message() << std::endl;
//////////			break;
//////////		}
//////////
//////////		istr.write(buf, sizeof(buf) - p.get().body().size);
//////////	}
//////////
//////////	str_buffer = istr.str();
//////////	std::cout << str_buffer << std::endl;
//////////}
//////////
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Performs an HTTP GET and prints the response
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	tcp::resolver resolver_;
//////////	beast::tcp_stream stream_;
//////////	beast::flat_buffer buffer_; // (Must persist between reads)
//////////	http::request<http::empty_body> req_;
//////////	http::response<http::string_body> res_;
//////////	boost::optional<http::response_parser<http::buffer_body>>p_;
//////////
//////////public:
//////////	// Objects are constructed with a strand to
//////////	// ensure that handlers do not execute concurrently.
//////////	explicit
//////////		session(net::io_context& ioc)
//////////		: resolver_(net::make_strand(ioc))
//////////		, stream_(net::make_strand(ioc))
//////////	{
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		run(
//////////			char const* host,
//////////			char const* port,
//////////			char const* target,
//////////			int version)
//////////	{
//////////		// Set up an HTTP GET request message
//////////		req_.version(version);
//////////		req_.method(http::verb::get);
//////////		req_.target(target);
//////////		req_.set(http::field::host, host);
//////////		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//////////
//////////		// Look up the domain name
//////////		resolver_.async_resolve(
//////////			host,
//////////			port,
//////////			beast::bind_front_handler(
//////////				&session::on_resolve,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_resolve(
//////////			beast::error_code ec,
//////////			tcp::resolver::results_type results)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "resolve");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		stream_.async_connect(
//////////			results,
//////////			beast::bind_front_handler(
//////////				&session::on_connect,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "connect");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Send the HTTP request to the remote host
//////////		http::async_write(stream_, req_,
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		//test1();
//////////		test2();
//////////	}
//////////
//////////	void test2(){
//////////		beast::error_code ec;
//////////		read_and_print_body<false, beast::tcp_stream, beast::flat_buffer>(stream_, buffer_, ec);
//////////		//p_.emplace();
//////////		//p_->body_limit(500 * 1024 * 1024);
//////////
//////////		//beast::error_code ec;
//////////		//buffer_.prepare(1024 * 1024);
//////////		//auto size = http::read_header(stream_, buffer_,*p_, ec);
//////////		//if (ec) {
//////////		//	fail(ec, "test2");
//////////		//	return;
//////////		//}
//////////
//////////		//buffer_.consume(size);
//////////		//std::string str_buffer;
//////////		//while (!p_->is_done())
//////////		//{
//////////		//	char buf[512] = { 0 };
//////////		//	p_->get().body().data = buf;
//////////		//	p_->get().body().size = sizeof(buf);
//////////		//	buffer_.prepare(1024 * 1024);
//////////		//	http::read(stream_, buffer_, *p_, ec);
//////////		//	if (ec == beast::http::error::need_buffer)
//////////		//		ec.assign(0, ec.category());
//////////		//	if (ec) {
//////////		//		return fail(ec, "test2");
//////////		//	}
//////////		//		
//////////		//	size = p_->get().body().size;
//////////		//	str_buffer.append(buf, p_->get().body().size);
//////////		//}
//////////	}
//////////
//////////	void test1() {
//////////		// Receive the HTTP response
//////////		http::async_read(stream_, buffer_, res_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		// Write the message to standard out
//////////		std::cout << res_ << std::endl;
//////////
//////////		// Gracefully close the socket
//////////		stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
//////////
//////////		// not_connected happens sometimes so don't bother reporting it.
//////////		if (ec && ec != beast::errc::not_connected)
//////////			return fail(ec, "shutdown");
//////////
//////////		// If we get here then the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char** argv)
//////////{
//////////	auto const host = "www.319game.cn";
//////////	auto const port = "80";
//////////	auto const target = "/dowload/319Csetup.exe";
//////////	int version = 11;
//////////
//////////	//auto const host = "www.baidu.com";
//////////	//auto const port = "80";
//////////	//auto const target = "/";
//////////	//int version = 11;
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc;
//////////
//////////	// Launch the asynchronous operation
//////////	std::make_shared<session>(ioc)->run(host, port, target, version);
//////////
//////////	// Run the I/O service. The call will return when
//////////	// the get operation is complete.
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
//////////
//////////#include <boost/beast/core.hpp>
//////////#include <boost/beast/http.hpp>
//////////#include <boost/beast/version.hpp>
//////////#include <boost/asio/strand.hpp>
//////////#include <cstdlib>
//////////#include <functional>
//////////#include <iostream>
//////////#include <memory>
//////////#include <string>
//////////#include <sstream>
//////////
//////////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////////namespace net = boost::asio;            // from <boost/asio.hpp>
//////////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////////
////////////------------------------------------------------------------------------------
//////////
//////////template<
//////////	bool isRequest,
//////////	class SyncReadStream,
//////////	class DynamicBuffer>
//////////	void
//////////	read_and_print_body(
//////////		SyncReadStream& stream,
//////////		DynamicBuffer& buffer,
//////////		beast::error_code& ec)
//////////{
//////////	http::parser<isRequest, http::buffer_body> p;
//////////	p.body_limit(500 * 1024 * 1024);
//////////	read_header(stream, buffer, p, ec);
//////////
//////////	if (ec) {
//////////		std::cout << ec.message() << std::endl;
//////////		return;
//////////	}
//////////
//////////	std::string str_buffer;
//////////	std::ostringstream istr;
//////////	while (!p.is_done()) {
//////////		char buf[8192] = { 0 };//
//////////
//////////		p.get().body().data = &buf[0];
//////////		p.get().body().size = sizeof(buf);
//////////		read(stream, buffer, p, ec);
//////////		if (ec == beast::http::error::need_buffer)
//////////			ec = {};
//////////		if (ec) {
//////////			//beast::http::error::partial_message
//////////			std::cout << ec.message() << std::endl;
//////////			break;
//////////		}
//////////
//////////		auto size = sizeof(buf) - p.get().body().size;
//////////		istr.write(buf, size);
//////////	}
//////////
//////////	str_buffer = istr.str();
//////////	std::cout << str_buffer << std::endl;
//////////
//////////}
//////////
//////////void
//////////fail(beast::error_code ec, char const* what)
//////////{
//////////	std::cerr << what << ": " << ec.message() << "\n";
//////////}
//////////
//////////// Performs an HTTP GET and prints the response
//////////class session : public std::enable_shared_from_this<session>
//////////{
//////////	tcp::resolver resolver_;
//////////	beast::tcp_stream stream_;
//////////	beast::flat_buffer buffer_; // (Must persist between reads)
//////////	http::request<http::empty_body> req_;
//////////	http::response<http::string_body> res_;
//////////	http::response<http::file_body> res2_;
//////////	boost::optional<http::response_parser<http::buffer_body>>p_;
//////////	boost::optional<http::response_parser<http::string_body>>res1_;
//////////	std::ostringstream istr_;
//////////	std::array<char, 1 * 1024 * 1024>res_buffer_;
//////////	std::string rec_str_buf_;
//////////	const size_t read_size_ = 8192;
//////////public:
//////////	// Objects are constructed with a strand to
//////////	// ensure that handlers do not execute concurrently.
//////////	explicit
//////////		session(net::io_context& ioc)
//////////		: resolver_(net::make_strand(ioc))
//////////		, stream_(net::make_strand(ioc))
//////////	{
//////////		rec_str_buf_.resize(read_size_);
//////////	}
//////////
//////////	// Start the asynchronous operation
//////////	void
//////////		run(
//////////			char const* host,
//////////			char const* port,
//////////			char const* target,
//////////			int version)
//////////	{
//////////		// Set up an HTTP GET request message
//////////		req_.version(version);
//////////		req_.method(http::verb::get);
//////////		req_.target(target);
//////////		req_.set(http::field::host, host);
//////////		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//////////
//////////		// Look up the domain name
//////////		resolver_.async_resolve(
//////////			host,
//////////			port,
//////////			beast::bind_front_handler(
//////////				&session::on_resolve,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_resolve(
//////////			beast::error_code ec,
//////////			tcp::resolver::results_type results)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "resolve");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////
//////////		// Make the connection on the IP address we get from a lookup
//////////		stream_.async_connect(
//////////			results,
//////////			beast::bind_front_handler(
//////////				&session::on_connect,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
//////////	{
//////////		if (ec)
//////////			return fail(ec, "connect");
//////////
//////////		// Set a timeout on the operation
//////////		stream_.expires_after(std::chrono::seconds(30));
//////////		req_.set(http::field::content_type, "application/octet-stream");
//////////		// Send the HTTP request to the remote host
//////////		http::async_write(stream_, req_,
//////////			beast::bind_front_handler(
//////////				&session::on_write,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_write(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "write");
//////////
//////////		//test1();
//////////		test2();
//////////		//res1_.emplace();
//////////		//test3();
//////////
//////////		//test4();
//////////	}
//////////
//////////	void test4() {
//////////		//res1_.emplace();
//////////		//res1_->body_limit(500 * 1024 * 1024);
//////////		//http::async_read_header(stream_, buffer_, *res1_, 
//////////		//	[self = shared_from_this()](beast::error_code ec,
//////////		//	std::size_t bytes_transferred){
//////////		//if (ec){
//////////		//	return fail(ec, "test4");
//////////		//}
//////////
//////////		//self->do_read_body();
//////////		//});
//////////	}
//////////
//////////	void do_read_body() {
//////////		stream_.async_read_some(buffer_.prepare(8192),[self = shared_from_this()](beast::error_code ec,
//////////			std::size_t bytes_transferred){
//////////			if (ec) {
//////////				return fail(ec, "do_read_body");
//////////			}
//////////
//////////			std::string str = self->istr_.str();
//////////			self->res1_->get().body() = str;
//////////			std::cout << self->istr_.str() << std::endl;
//////////			if (!self->res1_->is_done()) {
//////////				self->do_read_body();
//////////				return;
//////////			}
//////////
//////////			std::cout << self->res1_->release() << std::endl;
//////////
//////////		});
//////////	}
//////////
//////////	void test3()
//////////	{
//////////		stream_.async_read_some(boost::asio::buffer(&res_buffer_[0], res_buffer_.size()), [self = shared_from_this()](beast::error_code ec,
//////////			std::size_t bytes_transferred){
//////////			if (ec) {
//////////				return fail(ec, "test3");
//////////			}
//////////
//////////			self->res1_->put(boost::asio::buffer(&self->res_buffer_[0], self->res_buffer_.size()), ec);
//////////			std::cout << self->res1_->release() << std::endl;
//////////			if (!self->res1_->is_done()) {
//////////				self->test3();
//////////				return;
//////////			}
//////////
//////////			std::cout << self->res1_->release() << std::endl;
//////////		});
//////////	}
//////////	void test2() {
//////////		//beast::error_code ec;
//////////		//read_and_print_body<false, beast::tcp_stream, beast::flat_buffer>(stream_, buffer_, ec);
//////////		p_.emplace();
//////////		p_->body_limit(500 * 1024 * 1024);
//////////
//////////		beast::error_code ec;
//////////		auto size = http::read_header(stream_, buffer_, *p_, ec);
//////////		if (ec) {
//////////			fail(ec, "test2");
//////////			return;
//////////		}
//////////
//////////		std::string str_buffer;
//////////		std::ostringstream istr;
//////////
//////////		while (!p_->is_done())
//////////		{
//////////			char buf[512] = { 0 };
//////////			p_->get().body().data = buf;
//////////			p_->get().body().size = sizeof(buf);
//////////			http::read(stream_, buffer_, *p_, ec); //64K
//////////			if (ec == beast::http::error::need_buffer)
//////////				ec.assign(0, ec.category());
//////////			if (ec) {
//////////				fail(ec, "test2");
//////////				break;
//////////			}
//////////
//////////			auto size =sizeof(buf) - p_->get().body().size;
//////////			istr.write(buf, size);
//////////		}
//////////
//////////		str_buffer = istr.str();
//////////		res_.base() = p_->get().base();
//////////		res_.body() = str_buffer;
//////////		std::cout << res_ << std::endl;
//////////	}
//////////
//////////	void test1() {
//////////		// Receive the HTTP response
//////////		beast::error_code ec;
//////////		res2_.body().open("./1.exe",beast::file_mode::write, ec);
//////////		http::async_read(stream_, buffer_, res2_,
//////////			beast::bind_front_handler(
//////////				&session::on_read,
//////////				shared_from_this()));
//////////	}
//////////
//////////	void
//////////		on_read(
//////////			beast::error_code ec,
//////////			std::size_t bytes_transferred)
//////////	{
//////////		boost::ignore_unused(bytes_transferred);
//////////
//////////		if (ec)
//////////			return fail(ec, "read");
//////////
//////////		// Write the message to standard out
//////////		//std::cout << res_ << std::endl;
//////////
//////////		// Gracefully close the socket
//////////		stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
//////////
//////////		// not_connected happens sometimes so don't bother reporting it.
//////////		if (ec && ec != beast::errc::not_connected)
//////////			return fail(ec, "shutdown");
//////////
//////////		// If we get here then the connection is closed gracefully
//////////	}
//////////};
//////////
////////////------------------------------------------------------------------------------
//////////
//////////int main(int argc, char** argv)
//////////{
//////////	auto const host = "www.319game.cn";
//////////	auto const port = "80";
//////////	auto const target = "/dowload/319Csetup.exe";
//////////	int version = 11;
//////////
//////////	//auto const host = "www.baidu.com";
//////////	//auto const port = "80";
//////////	//auto const target = "/";
//////////	//int version = 11;
//////////
//////////	// The io_context is required for all I/O
//////////	net::io_context ioc;
//////////
//////////	// Launch the asynchronous operation
//////////	std::make_shared<session>(ioc)->run(host, port, target, version);
//////////
//////////	// Run the I/O service. The call will return when
//////////	// the get operation is complete.
//////////	ioc.run();
//////////
//////////	return EXIT_SUCCESS;
//////////}
////////
////////
//////#include <boost/beast/core.hpp>
//////#include <boost/beast/http.hpp>
//////#include <boost/beast/version.hpp>
//////#include <boost/asio.hpp>
//////#include <chrono>
//////#include <cstdlib>
//////#include <ctime>
//////#include <iostream>
//////#include <memory>
//////#include <string>
//////
//////namespace beast = boost::beast;         // from <boost/beast.hpp>
//////namespace http = beast::http;           // from <boost/beast/http.hpp>
//////namespace net = boost::asio;            // from <boost/asio.hpp>
//////using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//////
//////namespace my_program_state
//////{
//////	std::size_t
//////		request_count()
//////	{
//////		static std::size_t count = 0;
//////		return ++count;
//////	}
//////
//////	std::time_t
//////		now()
//////	{
//////		return std::time(0);
//////	}
//////}
//////
//////class http_connection : public std::enable_shared_from_this<http_connection>
//////{
//////public:
//////	http_connection(tcp::socket socket)
//////		: socket_(std::move(socket))
//////	{
//////	}
//////
//////	// Initiate the asynchronous operations associated with the connection.
//////	void
//////		start()
//////	{
//////		read_request();
//////		check_deadline();
//////	}
//////
//////private:
//////	// The socket for the currently connected client.
//////	beast::tcp_stream socket_;
//////
//////	// The buffer for performing reads.
//////	beast::flat_buffer buffer_;
//////
//////	// The request message.
//////	http::request<http::dynamic_body> request_;
//////	boost::optional<http::request_parser<http::buffer_body>> server_parser_;
//////	// The response message.
//////	http::response<http::dynamic_body> response_;
//////	http::response<http::string_body> response1_;
//////	http::response<http::buffer_body> response2_;
//////
//////	boost::optional<http::response_serializer<http::string_body>> serializer_;
//////	boost::optional<http::response_serializer<http::string_body>> serializer1_;
//////	boost::optional<http::response_serializer<http::buffer_body>> serializer2_;
//////	std::string str_buffer_;
//////	size_t index_ = 0;
//////
//////	// The timer for putting a deadline on connection processing.
//////	net::steady_timer deadline_{
//////		socket_.get_executor(), std::chrono::seconds(60) };
//////
//////	// Asynchronously receive a complete request message.
//////	void
//////		read_request()
//////	{
//////		auto self = shared_from_this();
//////		server_parser_.emplace();
//////		server_parser_->header_limit(10 * 1024 * 1024);
//////		server_parser_->body_limit(500 * 1024 * 1024);
//////		buffer_ = {};
//////
//////		http::async_read_header(
//////			socket_,
//////			buffer_,
//////			*server_parser_,
//////			[self](beast::error_code ec,
//////				std::size_t bytes_transferred)
//////			{
//////				boost::ignore_unused(bytes_transferred);
//////				if (!ec)
//////					self->process_request();
//////			});
//////	}
//////
//////	// Determine what needs to be done with the request message.
//////	void
//////		process_request()
//////	{
//////		response_.version(request_.version());
//////		response_.keep_alive(false);
//////
//////		switch (request_.method())
//////		{
//////		case http::verb::get:
//////			std::cout << request_ << std::endl;
//////			response_.result(http::status::ok);
//////			response_.set(http::field::server, "Beast");
//////			create_response();
//////			break;
//////
//////		default:
//////			// We return responses indicating an error if
//////			// we do not recognize the request method.
//////			response_.result(http::status::bad_request);
//////			response_.set(http::field::content_type, "text/plain");
//////			beast::ostream(response_.body())
//////				<< "Invalid request-method '"
//////				<< std::string(request_.method_string())
//////				<< "'";
//////			break;
//////		}
//////
//////		write_response_header2();
//////		//write_response();
//////	}
//////
//////	// Construct a response message based on the program state.
//////	void
//////		create_response()
//////	{
//////		if (request_.target() == "/count")
//////		{
//////			response_.set(http::field::content_type, "text/html");
//////			beast::ostream(response_.body())
//////				<< "<html>\n"
//////				<< "<head><title>Request count</title></head>\n"
//////				<< "<body>\n"
//////				<< "<h1>Request count</h1>\n"
//////				<< "<p>There have been "
//////				<< my_program_state::request_count()
//////				<< " requests so far.</p>\n"
//////				<< "</body>\n"
//////				<< "</html>\n";
//////		}
//////		else if (request_.target() == "/time")
//////		{
//////			response_.set(http::field::content_type, "text/html");
//////			beast::ostream(response_.body())
//////				<< "<html>\n"
//////				<< "<head><title>Current time</title></head>\n"
//////				<< "<body>\n"
//////				<< "<h1>Current time</h1>\n"
//////				<< "<p>The current time is "
//////				<< my_program_state::now()
//////				<< " seconds since the epoch.</p>\n"
//////				<< "</body>\n"
//////				<< "</html>\n";
//////		}
//////		else
//////		{
//////			response_.result(http::status::not_found);
//////			response_.set(http::field::content_type, "text/plain");
//////			beast::ostream(response_.body()) << "hello world\r\n";
//////		}
//////	}
//////
//////	//void write_response_header() {
//////	//	str_buffer_ = "hello world body";
//////	//	response1_.result(http::status::ok);
//////	//	response1_.set(http::field::server, "Beast");
//////	//	response1_.set(http::field::content_type, "text/plain");
//////	//	response1_.body() = str_buffer_;
//////	//	response1_.prepare_payload();
//////
//////	//	serializer_.emplace(response1_);
//////
//////	//	http::async_write_header(socket_, *serializer_, [self = shared_from_this()](beast::error_code ec, std::size_t size){
//////	//		if (ec){
//////	//			std::cout << ec.message() << std::endl;
//////	//			return;
//////	//		}
//////
//////	//		if (self->serializer_->is_header_done()){
//////	//			std::cout << "finsh header" << std::endl;
//////	//			self->write_response_body();
//////	//		}
//////	//	});
//////	//}
//////
//////	void write_response_header2() {
//////		str_buffer_ = "hello world body";
//////		response2_.result(http::status::ok);
//////		response2_.version(11);
//////		response2_.set(http::field::server, "Beast");
//////		response2_.set(http::field::content_type, "text/plain");
//////		response2_.set(http::field::content_length, str_buffer_.size());
//////		response2_.body().data = nullptr;
//////		response2_.body().more = true;
//////
//////		serializer2_.emplace(response2_);
//////
//////		http::async_write_header(socket_, *serializer2_, [self = shared_from_this()](beast::error_code ec, std::size_t size){
//////			if (ec) {
//////				std::cout << ec.message() << std::endl;
//////				return;
//////			}
//////
//////			if (self->serializer2_->is_header_done()) {
//////				std::cout << "finsh header" << std::endl;
//////				self->write_response_body2();
//////			}
//////		});
//////	}
//////
//////	void write_response_header1() {
//////		str_buffer_ = "hello world body";
//////		response1_.result(http::status::ok);
//////		response1_.set(http::field::server, "Beast");
//////		response1_.set(http::field::content_length, 16);
//////		response1_.set(http::field::content_type, "text/plain");
//////
//////		response1_.prepare_payload();
//////
//////		serializer1_.emplace(response1_);
//////
//////		http::async_write_header(socket_, *serializer1_, [self = shared_from_this()](beast::error_code ec, std::size_t size){
//////			if (ec) {
//////				std::cout << ec.message() << std::endl;
//////				return;
//////			}
//////
//////			if (self->serializer1_->is_header_done()) {
//////				std::cout << "finsh header" << std::endl;
//////				self->write_response_body1();
//////			}
//////		});
//////	}
//////
//////	void write_response_body1() {
//////		http::async_write(socket_, *serializer1_, [self = shared_from_this()](beast::error_code ec, std::size_t size){
//////			if (ec) {
//////				std::cout << ec.message() << std::endl;
//////				return;
//////			}
//////
//////		});
//////	}
//////
//////	void write_response_body2() {
//////		std::string buffer = "hello world";
//////
//////		response2_.body().data = &buffer[0];
//////		auto size = buffer.size();
//////		response2_.body().size = size;
//////		response2_.body().more = true;
//////
//////		http::async_write(socket_, *serializer2_, [self = shared_from_this()](beast::error_code ec, std::size_t size){
//////			if (ec == beast::http::error::need_buffer) {
//////				ec = {};
//////			}
//////
//////			if (ec) {
//////				std::cout << ec.message() << std::endl;
//////				return;
//////			}
//////
//////			self->write_response_body3();
//////		});
//////	}
//////
//////	void write_response_body3() {
//////		std::string buffer = " body";
//////
//////		response2_.body().data = &buffer[0];
//////		auto size = buffer.size();
//////		response2_.body().size = size;
//////		response2_.body().more = false;
//////
//////		http::async_write(socket_, *serializer2_, [self = shared_from_this()](beast::error_code ec, std::size_t size){
//////			if (ec == beast::http::error::need_buffer) {
//////				ec = {};
//////			}
//////
//////			if (ec) {
//////				std::cout << ec.message() << std::endl;
//////				return;
//////			}
//////
//////			std::cout << self->serializer2_->is_done() << std::endl;
//////
//////			std::cout << "body finsed" << std::endl;
//////			self->read_request();
//////		});
//////	}
//////
//////
//////	//void write_response_body4() {
//////	//	response2_.body().data = nullptr;
//////	//	response2_.body().more = false;
//////
//////	//	http::async_write(socket_, *serializer2_, [self = shared_from_this()](beast::error_code ec, std::size_t size){
//////	//		if (ec == beast::http::error::need_buffer) {
//////	//			ec = {};
//////	//		}
//////
//////	//		if (ec) {
//////	//			std::cout << ec.message() << std::endl;
//////	//			return;
//////	//		}
//////
//////	//		std::cout << self->serializer2_->is_done() << std::endl;
//////
//////	//		std::cout << "body finsed" << std::endl;
//////
//////	//	});
//////	//}
//////
//////
//////	//void write_response_body() {
//////	//	http::async_write(socket_, *serializer_,[self = shared_from_this()](beast::error_code ec, std::size_t size){
//////	//		if (ec){
//////	//			std::cout << ec.message() << std::endl;
//////	//			return;
//////	//		}
//////
//////	//		int i = 100;
//////	//	});
//////	//}
//////	// Asynchronously transmit the response message.
//////	void
//////		write_response()
//////	{
//////		auto self = shared_from_this();
//////
//////		response_.content_length(response_.body().size());
//////
//////		http::async_write(
//////			socket_,
//////			response_,
//////			[self](beast::error_code ec, std::size_t)
//////			{
//////				self->socket_.socket().shutdown(tcp::socket::shutdown_send, ec);
//////				self->deadline_.cancel();
//////			});
//////	}
//////
//////	// Check whether we have spent enough time on this connection.
//////	void
//////		check_deadline()
//////	{
//////		auto self = shared_from_this();
//////
//////		deadline_.async_wait(
//////			[self](beast::error_code ec)
//////			{
//////				if (!ec)
//////				{
//////					// Close socket to cancel any outstanding operation.
//////					self->socket_.close();
//////				}
//////			});
//////	}
//////};
//////
//////// "Loop" forever accepting new connections.
//////void
//////http_server(tcp::acceptor& acceptor, tcp::socket& socket)
//////{
//////	acceptor.async_accept(socket,
//////		[&](beast::error_code ec)
//////		{
//////			if (!ec)
//////				std::make_shared<http_connection>(std::move(socket))->start();
//////			http_server(acceptor, socket);
//////		});
//////}
//////
//////int
//////main(int argc, char* argv[])
//////{
//////	try
//////	{
//////		auto const address = net::ip::make_address("0.0.0.0");
//////		//unsigned short port = static_cast<unsigned short>(std::atoi("9090"));
//////		unsigned short port = static_cast<unsigned short>(std::atoi("12087"));
//////
//////		net::io_context ioc{ 1 };
//////
//////		tcp::acceptor acceptor{ ioc, {address, port} };
//////		tcp::socket socket{ ioc };
//////		http_server(acceptor, socket);
//////
//////		ioc.run();
//////	}
//////	catch (std::exception const& e)
//////	{
//////		std::cerr << "Error: " << e.what() << std::endl;
//////		return EXIT_FAILURE;
//////	}
//////}
////////
//////////#include "b64_encrypt.h"
//////////#include <chrono>
//////////#include <time.h>
//////////#include <iomanip>
//////////
//////////
//////////std::string create_cookie(std::string user_ip)
//////////{
//////////	std::string new_token;
//////////	std::chrono::system_clock::time_point last_time =
//////////		std::chrono::system_clock::now();
//////////	std::time_t tm = std::chrono::system_clock::to_time_t(last_time);
//////////	std::string str_posix_time;
//////////	str_posix_time.resize(50);
//////////	std::strftime(&str_posix_time[0], str_posix_time.size(), "%Y%m%dT%H%M%S", std::localtime(&tm));
//////////	str_posix_time.resize(strlen(str_posix_time.c_str()));
//////////	str_posix_time = str_posix_time.erase(str_posix_time.length() - 2, 2);
//////////	auto old_token = user_ip + str_posix_time;
//////////	//std::string old_token = "127.0.0.120200828T1149";
//////////
//////////	for (size_t i = 0; i < old_token.size(); i++) {
//////////		((char*)old_token.c_str())[i] = old_token.c_str()[i] ^ 0x38;
//////////	}
//////////	
//////////	b64_encrypt encrypt;
//////////	new_token = encrypt.b64encode(&old_token[0], old_token.size());
//////////
//////////	for (auto site = new_token.find("="); site != std::string::npos; site = new_token.find("=")) {
//////////		new_token = new_token.replace(site, 1, "");
//////////	}
//////////
//////////	return std::move(new_token);
//////////}
//////////
//////////int main()
//////////{
//////////	std::string str = create_cookie("127.0.0.1");
//////////	int i = 10;
//////////}
//////
////////#include "nlohmann_json.hpp"
////////#include <memory>
////////
////////int main()
////////{
////////	auto ptr = std::make_unique<char>()
////////}
////
//////#include <boost/log/trivial.hpp>
//////#include <boost/log/sources/severity_logger.hpp>
//////#include <boost/log/utility/setup/file.hpp>
//////#include <boost/log/utility/setup/console.hpp>
//////#include <boost/log/expressions.hpp>
//////#include <boost/log/utility/setup/common_attributes.hpp>
//////#include <boost/log/attributes/mutable_constant.hpp>
//////#include <boost/date_time/posix_time/posix_time_types.hpp>
//////#include <boost/log/support/date_time.hpp>
//////#include <boost/log/attributes/mutable_constant.hpp>
//////
//////namespace logging = boost::log;
//////namespace attrs = boost::log::attributes;
//////namespace expr = boost::log::expressions;
//////namespace src = boost::log::sources;
//////namespace keywords = boost::log::keywords;
//////
//////// New macro that includes severity, filename and line number
//////#define CUSTOM_LOG(logger, sev) \
//////   BOOST_LOG_STREAM_WITH_PARAMS( \
//////      (logger), \
//////         (set_get_attrib("File", path_to_filename(__FILE__))) \
//////         (set_get_attrib("Line", __LINE__)) \
//////         (::boost::log::keywords::severity = (boost::log::trivial::sev)) \
//////   )
//////
//////// Set attribute and return the new value
//////template<typename ValueType>
//////ValueType set_get_attrib(const char* name, ValueType value) {
//////	auto attr = logging::attribute_cast<attrs::mutable_constant<ValueType>>(logging::core::get()->get_thread_attributes()[name]);
//////	attr.set(value);
//////	return attr.get();
//////}
//////
//////// Convert file path to only the filename
//////std::string path_to_filename(std::string path) {
//////	return path.substr(path.find_last_of("/\\") + 1);
//////}
//////
//////void init() {
//////	// New attributes that hold filename and line number
//////	logging::core::get()->add_thread_attribute("File", attrs::mutable_constant<std::string>(""));
//////	logging::core::get()->add_thread_attribute("Line", attrs::mutable_constant<int>(0));
//////
//////	logging::add_file_log(
//////		keywords::file_name = "sample.log",
//////		keywords::format = (
//////			expr::stream
//////			<< expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
//////			<< ": <" << boost::log::trivial::severity << "> "
//////			<< '[' << expr::attr<std::string>("File")
//////			<< ':' << expr::attr<int>("Line") << "] "
//////			<< expr::smessage
//////			)
//////	);
//////	logging::add_common_attributes();
//////}
//////
//////int main(int argc, char* argv[]) {
//////	init();
//////	src::severity_logger<logging::trivial::severity_level> lg;
//////
//////	CUSTOM_LOG(lg, debug) << "A regular message";
//////	return 0;
//////}
////#include <cstdlib>
////#include <iostream>
////#include <boost/bind.hpp>
////#include <boost/asio.hpp>
////#include <boost/asio/ssl.hpp>
////
////#ifdef _DEBUG
////#pragma comment(lib, "crypt32")
////#pragma comment(lib, "libssl64MTd.lib")
////#pragma comment(lib, "libcrypto64MTd.lib")
////#else
////#pragma comment(lib, "crypt32")
////#pragma comment(lib, "libssl64MT.lib")
////#pragma comment(lib, "libcrypto64MT.lib")
////
////#endif
////
////static const std::string g_cert_key =
////"-----BEGIN RSA PRIVATE KEY-----\n"
////"MIIEowIBAAKCAQEAxo9757yQfmCmuuLu4I35MtQAIPtzt3X5SvdE1yCr6PciB9eR\n"
////"wAZJ5XGsRCPp6wqQh7ZAGVSnbaWEqtSu21u3oucWnqf27iDOkQuoW0QE5KEh0JuK\n"
////"4lgto7wO391vVt727+V4mRqecN0HNe7DKFIae8lJIRXBuvfE3raAPCa4aL6474G+\n"
////"jyrwP2+ThaAfrEwKTsYH74FQt8KVaAbAO/+bu6Jx3B8ITTLxAPErg/TJlGCWTxS5\n"
////"uLMdFQ3QTlDZwYp1yRZQKxGilChJUGW/q/ObdSTluoYvj8INjsJgT0ckga+B85mC\n"
////"1Ij32KCu+y55Z2z7HYbn/lfuh6OQQzmH1KhNkQIDAQABAoIBAQCe+tlRVVg357j3\n"
////"X6W1o9cIDFhCEDK5jLrafBrhSGZ8dAsKTl6DakWWcSplsH+lUmMgVhsCbRZVIzaS\n"
////"9RE/zzK8OtyQkZmTVi8uUTAuSsrEKAOEHFXaHpIETBl4wrpXytPahlfF9lsvsLkK\n"
////"RK57RSmxPRvRYrMnuSQm0ebgwnvaWJiQ2tslCEkZ9Ppl/a4/BJJ2VQF+Sx1O8bLi\n"
////"FDbS9LJjbXwk5QSF9DBPgzudMTNmaGNlZ/yo/ZEsmL6k2d3ShvK/MLYpkMnMfoPC\n"
////"WDboJqf8eMkeSOvdmB6PA9Ub1uy/pFD0p9ln4/XBxQsS3Ftu8mdT+A5SOINGZzdY\n"
////"LgNoAcVxAoGBAPI3pSX8D0ZVabFH3N22fnPvrZsLV7jtaNPhGTxapZbeDhEbNvAG\n"
////"BQP4zwjLt0ZQ142FnJZZAgTzsAz7bvNYB/At5/hl5v8z3UKfLWrw3RvXUO1+ulP+\n"
////"mibFP/ZQVbFqG8NcIgTkNI9sKxlmp8Zbp4oz/XZlCheWot24QmA5mTPVAoGBANHb\n"
////"5MrOaBQBUsF04IOZg3uDwu1P0pkQQKkotvoFpEkoL29TCZrGDSeGIgNWe1et7lYr\n"
////"Roz+LNs2zvyWR3dmefacXoNu5ZGTyAWmHwJhp1ivEgiZ7P46R+tvlpT9Ew+aVzEp\n"
////"xmjUdfcfP/9XUh7GpBCFn8bqi4OC3RqKRps51JzNAoGAbmkzHNeDVvpETY2GfoJb\n"
////"rhmJN226NQ/zgvHPARYI+XaLTvzq1ArKan8WUNob1y+uslI/iMFWDE/Q5noOn1p3\n"
////"c+JZJX++BoLrzxykJWVaRQCnYTstUHB0cEvl1i/UgCTwNuNeloA3/VC/bLrAq8jH\n"
////"3FXKqhdwvEPsRcliaF6ZGWkCgYAfq1FxkYh/TFvSufKPqYEACLhH273qP1uiq3RB\n"
////"csyCBcByylMuuiiOCF3lpw1iA+ttsYYqDMl0I2dFEuCiEiLhpmNU7k1SlLygrZsM\n"
////"XvREG9da2O+8xcrIqsRMo4xW1HHIB4fblgRfUKX1wJWx80QbAi9Ec2yuYfc/5BIX\n"
////"priBGQKBgCQpr65vbO/i58h+3rBpYaBmLwJ6Lr51eCTSo2RyFPAH9wRy+c0S4Pka\n"
////"ocfJafR7BRZn9mK8SrTW+i6HjioO7DdZ/KgZwMMxFxq57/h8ZYT+Ni7YeepXd5tg\n"
////"yrTWdxDsTvzx/ij3HRWwwkHo8XjMO4rohat3JMjp7o+c2vZ9ONv6\n"
////"-----END RSA PRIVATE KEY-----";
////
////static const std::string g_cert_pem =
////"-----BEGIN CERTIFICATE-----\n"
////"MIIFqDCCBJCgAwIBAgIQBB6xHeishzP2PUJFiEmKcTANBgkqhkiG9w0BAQsFADBe\n"
////"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
////"d3cuZGlnaWNlcnQuY29tMR0wGwYDVQQDExRSYXBpZFNTTCBSU0EgQ0EgMjAxODAe\n"
////"Fw0yMDAzMTEwMDAwMDBaFw0yMTAzMTExMjAwMDBaMBQxEjAQBgNVBAMMCSoudXBr\n"
////"Lm5ldDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMaPe+e8kH5gprri\n"
////"7uCN+TLUACD7c7d1+Ur3RNcgq+j3IgfXkcAGSeVxrEQj6esKkIe2QBlUp22lhKrU\n"
////"rttbt6LnFp6n9u4gzpELqFtEBOShIdCbiuJYLaO8Dt/db1be9u/leJkannDdBzXu\n"
////"wyhSGnvJSSEVwbr3xN62gDwmuGi+uO+Bvo8q8D9vk4WgH6xMCk7GB++BULfClWgG\n"
////"wDv/m7uicdwfCE0y8QDxK4P0yZRglk8UubizHRUN0E5Q2cGKdckWUCsRopQoSVBl\n"
////"v6vzm3Uk5bqGL4/CDY7CYE9HJIGvgfOZgtSI99igrvsueWds+x2G5/5X7oejkEM5\n"
////"h9SoTZECAwEAAaOCAqowggKmMB8GA1UdIwQYMBaAFFPKF1n8a8ADIS8aruSqqByC\n"
////"Vtp1MB0GA1UdDgQWBBRtzVU13eusG6Epih57skwhsW0leTAdBgNVHREEFjAUggkq\n"
////"LnVway5uZXSCB3Vway5uZXQwDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQWMBQGCCsG\n"
////"AQUFBwMBBggrBgEFBQcDAjA+BgNVHR8ENzA1MDOgMaAvhi1odHRwOi8vY2RwLnJh\n"
////"cGlkc3NsLmNvbS9SYXBpZFNTTFJTQUNBMjAxOC5jcmwwTAYDVR0gBEUwQzA3Bglg\n"
////"hkgBhv1sAQIwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29t\n"
////"L0NQUzAIBgZngQwBAgEwdQYIKwYBBQUHAQEEaTBnMCYGCCsGAQUFBzABhhpodHRw\n"
////"Oi8vc3RhdHVzLnJhcGlkc3NsLmNvbTA9BggrBgEFBQcwAoYxaHR0cDovL2NhY2Vy\n"
////"dHMucmFwaWRzc2wuY29tL1JhcGlkU1NMUlNBQ0EyMDE4LmNydDAJBgNVHRMEAjAA\n"
////"MIIBBAYKKwYBBAHWeQIEAgSB9QSB8gDwAHYApLkJkLQYWBSHuxOizGdwCjw1mAT5\n"
////"G9+443fNDsgN3BAAAAFwyRUzeQAABAMARzBFAiAf5V/Q1s+uCeNx2ZLdd2kQ7c5I\n"
////"Ig8fL3NFANy6pmUZPwIhAJxiTWVpHAaw6vfqUMIc2yWeAIsD1RCybpWvp+Kzo4AZ\n"
////"AHYAXNxDkv7mq0VEsV6a1FbmEDf71fpH3KFzlLJe5vbHDsoAAAFwyRUzvgAABAMA\n"
////"RzBFAiAvQpmLtTUoM+8ApI0wxf/jKKOPtY2A6E9BR2CqTtUwFgIhAJv/58T64Lj6\n"
////"2m06o6zbUaZsNicBQulwg5FTfPUlDPT1MA0GCSqGSIb3DQEBCwUAA4IBAQASjfTh\n"
////"2c8bMmZlPx9OBDyTuOK5BfEHhguFBV4Qhv4MXsRMPZQk7eN+3XV/9KSYXwz67EvB\n"
////"wQR4JeG0SHrlHfLIDqwMj0yekPwovPbMGgz3crwiwggPkA3r3xhMiaGWYwYd5ppP\n"
////"t1xZP9DhGqRWaX60MiIQ50RT1lGpEUjyqN3GpDSF/bc16JBgAHz9sjH6JdW/cLou\n"
////"R+gRRbzuln/Exlq/iHQpjd/BMfgeTZJRKjNN4nQ3Pj6VfuBpTO81qa/bA4gFehFW\n"
////"3yIkg/KlkHGoEblBzybwyghemB476HtcjQqetMWznDu7DkC9HgRRoK7G1l3CcgyY\n"
////"A1wt7MQ/TMWODKz3\n"
////"-----END CERTIFICATE-----";
////using boost::asio::ip::tcp;
////
////class session : public std::enable_shared_from_this<session>
////{
////public:
////	session(tcp::socket socket, boost::asio::ssl::context& context)
////		: socket_(std::move(socket), context)
////	{
////	}
////
////	void start()
////	{
////		do_handshake();
////	}
////
////private:
////	void do_handshake()
////	{
////		auto self(shared_from_this());
////		socket_.async_handshake(boost::asio::ssl::stream_base::server,
////			[this, self](const boost::system::error_code& error)
////			{
////				if (!error)
////				{
////					do_read();
////				}
////			});
////	}
////
////	void do_read()
////	{
////		auto self(shared_from_this());
////		//socket_.async_read_some(boost::asio::buffer(data_),
////		//	[this, self](const boost::system::error_code& ec, std::size_t length)
////		//	{
////		//		if (!ec)
////		//		{
////		//			do_write(length);
////		//		}
////		//	});
////
////		boost::asio::async_read(socket_, boost::asio::buffer(data_,1024), 
////			[this, self](const boost::system::error_code& ec, std::size_t length) {
////				if (!ec)
////				{
////					do_write(length);
////				}
////			});
////	}
////
////	void do_write(std::size_t length)
////	{
////		auto self(shared_from_this());
////		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
////			[this, self](const boost::system::error_code& ec,
////				std::size_t /*length*/)
////			{
////				if (!ec)
////				{
////					do_read();
////				}
////			});
////	}
////
////	boost::asio::ssl::stream<tcp::socket> socket_;
////	char data_[1024] = {};
////};
////
////class server
////{
////public:
////	server(boost::asio::io_context& io_context, unsigned short port)
////		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
////		context_(boost::asio::ssl::context::sslv23)
////	{
////		context_.set_options(
////			boost::asio::ssl::context::default_workarounds
////			| boost::asio::ssl::context::no_sslv2
////			| boost::asio::ssl::context::single_dh_use);
////		//context_.set_password_callback(std::bind(&server::get_password, this));
////		//context_.use_certificate_chain_file("server.pem");
////		//context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
////
////		context_.use_certificate_chain(boost::asio::const_buffer(g_cert_pem.c_str(), g_cert_pem.size()));
////		context_.use_private_key(boost::asio::const_buffer(g_cert_key.c_str(), g_cert_key.size()), boost::asio::ssl::context::pem);
////
////		//context_.use_tmp_dh_file("dh2048.pem");
////
////		do_accept();
////	}
////
////private:
////	std::string get_password() const
////	{
////		return "test";
////	}
////
////	void do_accept()
////	{
////		acceptor_.async_accept(
////			[this](const boost::system::error_code& error, tcp::socket socket)
////			{
////				if (!error)
////				{
////					std::make_shared<session>(std::move(socket), context_)->start();
////				}
////
////				do_accept();
////			});
////	}
////
////	tcp::acceptor acceptor_;
////	boost::asio::ssl::context context_;
////};
////
////int main()
////{
////	try
////	{
////		boost::asio::io_context io_context;
////
////		using namespace std; // For atoi.
////		server s(io_context, atoi("443"));
////
////		io_context.run();
////	}
////	catch (std::exception& e)
////	{
////		std::cerr << "Exception: " << e.what() << "\n";
////	}
////
////	return 0;
////}
//
////#include "super_box.h"
////#include <string>
////#include "CSystemStatus.h"
////#pragma pack(push,1)
////struct msg_header
////{
////	uint16_t cmd;
////};
////
////struct A
////{
////	msg_header header;
////	int a;
////	uint16_t b;
////	std::string str;
////};
////#pragma pack(pop) 
////int main()
////{
////	const bool b64Bit = 8 == sizeof(int*);// 是否64位
////
////	CSystemStatus status;
////	status.GetSystemCpuCurrentUsage();
////	//std::vector<NETCARDINFO> vectorNetCard;
////	//get_net_card_info(vectorNetCard);
////	//auto id = block_ip("", "192.168.5.212", 0, 1);
////	//std::string src_ip ="192.168.5.83";
////	//auto id = block_tcp(src_ip.c_str(), "", "192.168.5.212", "10808", 0, 0);
////	//delte_rule(id);
////	//int i = 10;
////
////	//auto res = connect_server();
////	//if (res ==0){
////	//	return 0;
////	//}
////
////	//char buffer[1024] = {};
////	//A* ptr = (A*)buffer;
////	//ptr->header.cmd = 21;
////	//ptr->a = 100;
////	//ptr->b = 200;
////	//memcpy(&ptr->str[0], "123", 3);
////
////	//std::string str = "123457890";
////	//send_msg(buffer,sizeof(A));
////
////	//size_t len = recv_msg();
////
////	//std::string str2;
////	//str2.resize(len);
////	//get_data(&str2[0], len);
////	//close_socket();
////}
//
////#include "WinReg.hpp"
////#include "super_box.h"
////#include <iostream>
////#include <string>
////#include <WS2tcpip.h>
////#include <vector>
////
////class A
////{
////public:
////	A() = delete;
////	explicit A(int a) :a_(a) {
////
////	}
////	~A() = default;
////
////	inline A& operator++() noexcept
////	{
////		a_++;
////		return*this;
////	}
////
////	int get_vaule()const {
////		return a_;
////	}
////private:
////	int a_;
////};
////
//////此重载运算符函数不能放入类中,否则会报错
////inline bool operator==(const A& a, const A& b) noexcept
////{
////	return a.get_vaule() == b.get_vaule();
////}
////
////inline bool operator<=(const A& a, const A& b) noexcept
////{
////	return a.get_vaule() <= b.get_vaule();
////}
////
////inline bool operator<(const A& a, const A& b) noexcept
////{
////	return a.get_vaule() < b.get_vaule();
////}
////
//////ipv4转int 类型
////static int inet4_pton(const char* cp, std::size_t& ap) {
////	std::size_t acc = 0;
////	std::size_t  dots = 0;
////	std::size_t  addr = 0;
////
////	do {
////		char cc = *cp;
////		if (cc >= '0' && cc <= '9') {
////			acc = acc * 10 + (cc - '0');
////		}
////		else if (cc == '.' || cc == '\0') {
////			if (++dots > 3 && cc == '.') {
////				return 0;
////			}
////			/* Fall through */
////
////			if (acc > 255) {
////				return 0;
////			}
////
////			//addr += (acc << (index * 8));
////			//从左往右，低位放
////			addr = addr << 8 | acc; // 这句是精华,每次将当前值左移八位加上后面的值
////			acc = 0;
////		}
////	} while (*cp++);
////
////	// Normalize the address 
////	if (dots < 3) {
////		addr <<= 8 * (3 - dots);
////	}
////
////	ap = addr;
////	return 1;
////}
////
////typedef struct _tagWfp_NetInfo
////{
////	unsigned short int       local_port;	//本地端口
////	unsigned short int       remote_port;	//远程端口
////} ST_WFP_NETINFO, * PST_WFP_NETINFO;
////#pragma pack(pop)
////
////int main()
////{
////	int ret = connect_server();
////	user_regedit("15158194593", "xw123456");
////	//std::vector<ST_WFP_NETINFO>vec{ {1,2},{3,4} };
////	//ST_WFP_NETINFO* ptr = &vec[0];
////
////	//for (int i =0;i<vec.size();++i){
////	//	std::cout << ptr[i].local_port << std::endl;
////	//	std::cout << ptr[i].remote_port << std::endl;
////	//}
////
////	//auto sss = vec.begin() + vec.size() - 1;
////	//std::string dest_ip="192.168.5.212";
////	//struct in_addr s;
////	//uint32_t ip_value = 0;
////	//inet4_pton(dest_ip.c_str(), ip_value);
////	//// 转换
////	////inet_pton(AF_INET, dest_ip.c_str(), (void*)&s);
////	//printf("inet_pton: 0x%x\n", s.s_addr); // 注意得到的字节序
////
////	//A a(1);
////	////++a;
////	//A b(2);
////	//bool ret = a == b;
////	//ret = a <= b;
////	//ret = a < b;
////	//
////	//const DWORD testDw = 0x1234ABCD;
////	//const std::vector<std::wstring> testMultiSz = { L"Hi", L"Hello", L"Ciao" };
////	//const std::wstring testSubKey = L"SOFTWARE\\GioTest";
////	//winreg::RegKey key{ HKEY_CURRENT_USER, testSubKey };
////	//key.SetDwordValue(L"TestValueDword", testDw);
////	//key.SetMultiStringValue(L"TestValueMultiString",testMultiSz);
////
////
////	//auto testDw1 = key.TryGetDwordValue(L"TestDword");
////	//std::string str;
////	//str.resize(20);
////	//sprintf(&str[0], "%x", testDw);
////	////key.Detach();	//句柄分离
////	////winreg::RegKey key;
////	////key.Open(HKEY_CURRENT_USER, testSubKey);
////
////
////	//DWORD typeId = key.QueryValueType(L"TestBinary");
////	//if (typeId != REG_BINARY)
////	//{
////	//	std::cout << L"RegKey::QueryValueType failed for REG_BINARY.\n";
////	//}
////
////	//std::vector<std::wstring> subKeyNames = key.EnumSubKeys();
////
////	//std::vector<std::pair<std::wstring, DWORD>> values = key.EnumValues();
////	//key.Close();
////}
//
//#include <iostream>
//#include <map>
//#include <unordered_map>
//#include "format.h"
//
//static const char* cpuId(void)
//{
//	unsigned long s1 = 0;
//	unsigned long s2 = 0;
//	unsigned long s3 = 0;
//	unsigned long s4 = 0;
//	__asm
//	{
//		mov eax, 00h
//		xor edx, edx
//		cpuid
//		mov s1, edx
//		mov s2, eax
//	}
//	__asm
//	{
//		mov eax, 01h
//		xor ecx, ecx
//		xor edx, edx
//		cpuid
//		mov s3, edx
//		mov s4, ecx
//	}
//
//	static char buf[100];
//	sprintf(buf, "%08X%08X%08X%08X", s1, s2, s3, s4);
//	return buf;
//}
//
//int main()
//{
//	std::string str = cpuId();
//	double d_value = 1.284;
//	float f_value = 1.2f;
//	uint32_t uint_value = 96040;
//	std::string test = util::format("This is a nice string with numbers {0} and strings {1} nicely formatted {2} hehe da {3}",
//		123, f_value, d_value, uint_value);
//
//	std::unordered_map<int, int>ms;
//	ms.emplace(1, 2);
//	ms.emplace(2, 3);
//	ms.emplace(3, 4);
//	ms.emplace(4, 5);
//	ms.emplace(5, 6);
//	ms.emplace(6, 7);
//	ms.emplace(7, 8);
//
//	for (const auto info : ms) {
//		std::cout << "first key:" << info.first << std::endl;
//		std::cout << "second key:" << info.second << std::endl;
//	}
//
//	int value = 100;
//	auto iter_begin = ms.begin();
//	for (;iter_begin != ms.end();++iter_begin){
//		std::cout << "first key:" << iter_begin->first << std::endl;
//		std::cout << "second key:" << iter_begin->second << std::endl;
//	}
//}

//#include <iostream>
//#include "shine_serial.hpp"
//
//struct B
//{
//	//int a;
//	//double b;
//	std::string c;
//	std::string d;
//	//将类型名称及需要序列化的字段用SHINE_SERIAL包裹
//	SHINE_SERIAL(B,c,d);
//};
//
////struct A {
////	int a;
////	double b;
////	std::string c;
////
////	//此处嵌套上方的结构体B
////	std::map<int, B> d;
////
////	std::list<int> e;
////	std::vector<float> f;
////	std::deque<double> g;
////	std::forward_list<long> h;
////	std::set<std::string> i;
////
////	SHINE_SERIAL(A, a, b, c, d, e, f, g, h, i);
////};
//
//int main() {
//
//	/*A a;
//	a.a = 123;
//	a.b = 345.567;
//	a.c = "hello world!";*/
//
//	B b;
//
//	//b.a = 666;
//	//b.b = 777.7777;
//	b.c = "999999!";
//	b.d = "1234";
//
//	auto data = b.shine_serial_encode();
//
//	B a2;
//	a2.shine_serial_decode(data);
// 
//	//a.d.emplace(999, b);
//
//	//a.e.emplace_back(123);
//	//a.e.emplace_back(345);
//
//	//a.f.emplace_back((float)12.34);
//	//a.f.emplace_back((float)45.67);
//
//	//a.g.emplace_back((double)456.789);
//	//a.g.emplace_back((double)78.9);
//
//	//a.h.emplace_front(666);
//	//a.h.emplace_front(555);
//
//	//a.i.emplace("A");
//	//a.i.emplace("B");
//	//a.i.emplace("C");
//
//	////将对象a序列化成字节流
//	//auto data = a.shine_serial_encode();
//
//	////将字节流反序列化成对象，反序列化后a2与a中数据相同
//	//A a2;
//	//a2.shine_serial_decode(data);
//
//	////确定结果
//	//std::cout << ((a == a2) ? "success" : "failed") << std::endl;
//
//	return 0;
//}

//#define BINPROTO_DISABLE_ASSERT
//#include "binproto.hpp"
//#include <iostream>
//
//BINPROTO_DEFINE_PACKET_P03(Student
//	, binproto::variable_len_string<1>, name
//	, binproto::uint8_obj, age
//	, binproto::fixed_len_string<10>, sex);
//
//#define BUFF_SIZE 1024
//
//int main()
//{
//	try
//	{
//		//char* buffer = new char[BUFF_SIZE];
//		{
//			Student sd;
//			sd.name = "Ren Bin";//Yeah, That's my name, HAHA!!
//			sd.age = 27;
//			sd.sex = "male";//I want a chinese girl,:)
//			int binlen = sd.get_binary_len();
//			std::string buffer;
//			buffer.resize(binlen);
//			sd.serialize_to_buffer(&buffer[0], BUFF_SIZE);
//		}
//		{
//			//Student sd;
//			//sd.parse_from_buffer(buffer, BUFF_SIZE);
//			//int binlen = sd.get_binary_len();
//			//printf("this %s named %s is %d years old.\n", sd.sex.c_str(), sd.name.c_str(), sd.age.to_int());
//		}
//	}
//	catch (const binproto::exception& ex)
//	{
//		printf("%s.\n", ex.what().c_str());
//	}
//	return 0;
//}

//#include <iostream>
//#include "multipart_parser.h"
//
//int main()
//{
////#define BOUNDARY "simple boundary"
////#define PART0_BODY                                      \
////    "This is implicitly typed plain ASCII text.\r\n"    \
////    "It does NOT end with a linebreak."
////#define PART1_BODY                                      \
////    "This is explicitly typed plain ASCII text.\r\n"    \
////    "It DOES end with a linebreak.\r\n"
////#define BODY                                            \
////    "--" BOUNDARY "\r\n"                                \
////    "\r\n"                                              \
////    PART0_BODY                                          \
////    "\r\n--" BOUNDARY "\r\n"                            \
////    "Content-type: text/plain; charset=us-ascii\r\n"    \
////    "\r\n"                                              \
////    PART1_BODY                                          \
////    "\r\n--" BOUNDARY "--\r\n"                          \
////
////    multipart_parser parser("simple boundary");
////    if (strlen(BODY) == parser.parser_data(BODY, strlen(BODY))){
////        std::cout << "nice" << std::endl;
////    }
//
//#define BOUNDARY "---839227141510734175353540"
////#define BODY                                                                \
////    "--" BOUNDARY "\r\n"                                                    \
////    "Content-Disposition: form-data; name=\"foo\"; filename=\"bar\"\r\n"    \
////    "Content-Type: application/octet-stream\r\n"                            \
////    "\r\n"                                                                  \
////    "That's the file content!\r\n"                                          \
////    "\r\n--" BOUNDARY "--\r\n"                                              \
//
//#define BODY                                                                \
//    "--" BOUNDARY "\r\n"                                                    \
//    "Content-Disposition: form-data; name=\"username\"\r\n\r\n"             \
//     "15871155532\r\n"                                                      \
//     "--" BOUNDARY "\r\n"													\
//     "Content-Disposition: form-data; name=\"password\"\r\n\r\n"             \
//     "xw123456\r\n"                                                          \
//    "--" BOUNDARY "--\r\n"                                                    \
//
//	multipart_parser parser(BOUNDARY);
//    size_t data_size = strlen(BODY);
//    size_t parser_size = parser.parser_data(BODY, strlen(BODY));
//	if (data_size == parser_size) {
//		std::cout << "nice" << std::endl;
//    }else {
//        std::cout << "no nice" << std::endl;
//    }
//
//    auto datas =parser.get_multipart_data();
//
//	return 0;
//}

//#include <iostream>
//#include <tuple>
//#include "CSystemStatus.h"
//
//namespace detail
//{
//	template<int... Is>
//	struct seq { };
//
//	template<int N, int... Is>
//	struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };
//
//	template<int... Is>
//	struct gen_seq<0, Is...> : seq<Is...> { };
//}
//
////c++11写法
//namespace detail
//{
//	template<typename T, typename F, int... Is>
//	void for_each_c11(T&& t, F f, seq<Is...>)
//	{
//		auto l = { (f(std::get<Is>(t)),0)... };
//	}
//}
//
//template<typename... Ts, typename F>
//void for_each_in_tuple_c11(std::tuple<Ts...> const& t, F f)
//{
//	detail::for_each_c11(t, f, detail::gen_seq<sizeof...(Ts)>());
//}
//
////c++14
//template<typename T, typename F, int... Is>
//void for_each_c14(T&& t, F f,std::integer_sequence<int,Is...>)
//{
//	auto l = { (f(std::get<Is>(t)),0)... };
//}
//
//template<typename... Ts, typename F>
//void for_each_in_tuple_c14(std::tuple<Ts...> const& t, F f)
//{
//	for_each_c14(t, f,std::make_integer_sequence<int,sizeof...(Ts)>());
//}
//
//int main()
//{
//	auto datas = std::make_tuple<int>(1, 2, 3, 4);
//	std::apply([](auto&& ... args) {
//		((std::cout << args << std::endl),...);
//		}, datas);
//
//	std::cout << "....." << std::endl;
//	for_each_in_tuple_c11(datas,[](auto value) {
//		std::cout << value << std::endl;
//		});
//
//	std::cout << "....." << std::endl;
//	for_each_in_tuple_c14(datas, [](auto value) {
//		std::cout << value << std::endl;
//		});
//
//	CSystemStatus status;
//	status.SystemInit(SYSSTATE_CPU_USAGE | SYSSTATE_NET_DOWNLOAD | SYSSTATE_NET_UPLOAD);
//
//	auto ss = status.GetSystemNetDownloadRate();
//}

//#include "format.h"
//#include <string>
//using namespace std;
//
//class Object {
//public:
//	Object() {
//		cout << "Object初始化" << endl;
//	}
//
//	Object(const Object& obj) {
//		cout << "Object拷贝构造" << endl;
//	}
//
//	Object& operator=(const Object& obj) {
//		cout << "Object拷贝赋值" << endl;
//	}
//
//	Object(Object&& obj) {
//		cout << "Object移动构造" << endl;
//	}
//
//	Object& operator=(Object&& obj) {
//		cout << "Object移动赋值" << endl;
//	}
//
//	~Object() {
//		cout << "Object销毁" << endl;
//	}
//
//	string ToString() {
//		return "呵呵哒";
//	}
//
//	int x;
//};
//
//Object ObjTest() {
//	Object obj;
//	cout << obj.ToString() << endl;
//	obj.x = 100;
//	return obj;
//}
//
//int main()
//{
//	std::string test = util::format("select *from {0} where name ={1}", "tb_city","\"柳州\"");
//	std::cout << test << std::endl;
//}

//#include <iostream>
//#include "format.h"
//#include "CSystemStatus.h"
//
//int main()
//{
//	//int i_value = 0x11;
//	//int f_value = 0x22;
//
//	//int valueif = f_value << 8 | i_value;
//	//std::string buffer;
//	//buffer.resize(256);
//	//memcpy(buffer.data(), (char*)&valueif, sizeof(valueif));
//
//	int value2321 = 1;
//
//	char* ptr = (char*)&value2321;
//	if (*ptr ==1){
//		int value = 0;
//	}
//
//	//int valuess = valueif >> 8;
//	//int vsssss = (valueif & 0x00ff);
//
//
//	std::string test = util::format("select *from {0} where name ={1} and age ={2};", "tb_city", "\"柳州\"",26);
//	std::cout << test << std::endl;
//
//	CSystemStatus staus;
//	auto ss = staus.GetSystemCurrentDiskUsage();
//	std::string os_info;
//	staus.GetOsInfo(os_info);
//	staus.GetCPUid(os_info);
//
//	int value = -10;
//	int value1 = value;
//
//	auto falg = value >> 31;
//
//	int copy_value = ((~value) + 1);
//}
// 
//#include <boost/beast/core.hpp>
//#include <boost/beast/websocket.hpp>
//#include <boost/asio/dispatch.hpp>
//#include <boost/asio/strand.hpp>
//#include <algorithm>
//#include <cstdlib>
//#include <functional>
//#include <iostream>
//#include <memory>
//#include <string>
//#include <thread>
//#include <vector>
//
//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//
////------------------------------------------------------------------------------
//
//// Report a failure
//void
//fail(beast::error_code ec, char const* what)
//{
//	std::cerr << what << ": " << ec.message() << "\n";
//}
//
//// Echoes back all received WebSocket messages
//class session : public std::enable_shared_from_this<session>
//{
//	websocket::stream<beast::tcp_stream> ws_;
//	beast::flat_buffer buffer_;
//
//public:
//	// Take ownership of the socket
//	explicit
//		session(tcp::socket&& socket)
//		: ws_(std::move(socket))
//	{
//	}
//
//	// Get on the correct executor
//	void
//		run()
//	{
//		// We need to be executing within a strand to perform async operations
//		// on the I/O objects in this session. Although not strictly necessary
//		// for single-threaded contexts, this example code is written to be
//		// thread-safe by default.
//		net::dispatch(ws_.get_executor(),
//			beast::bind_front_handler(
//				&session::on_run,
//				shared_from_this()));
//	}
//
//	// Start the asynchronous operation
//	void
//		on_run()
//	{
//		// Set suggested timeout settings for the websocket
//		ws_.set_option(
//			websocket::stream_base::timeout::suggested(
//				beast::role_type::server));
//
//		// Set a decorator to change the Server of the handshake
//		ws_.set_option(websocket::stream_base::decorator(
//			[](websocket::response_type& res)
//			{
//				res.set(http::field::server,
//					std::string(BOOST_BEAST_VERSION_STRING) +
//					" websocket-server-async");
//			}));
//		// Accept the websocket handshake
//		ws_.async_accept(
//			beast::bind_front_handler(
//				&session::on_accept,
//				shared_from_this()));
//	}
//
//	void
//		on_accept(beast::error_code ec)
//	{
//		if (ec)
//			return fail(ec, "accept");
//
//		// Read a message
//		do_read();
//	}
//
//	void
//		do_read()
//	{
//		// Read a message into our buffer
//		ws_.async_read(
//			buffer_,
//			beast::bind_front_handler(
//				&session::on_read,
//				shared_from_this()));
//	}
//
//	void
//		on_read(
//			beast::error_code ec,
//			std::size_t bytes_transferred)
//	{
//		boost::ignore_unused(bytes_transferred);
//
//		// This indicates that the session was closed
//		if (ec == websocket::error::closed)
//			return;
//
//		if (ec)
//			fail(ec, "read");
//
//		// Echo the message
//		ws_.text(ws_.got_text());
//		ws_.async_write(
//			buffer_.data(),
//			beast::bind_front_handler(
//				&session::on_write,
//				shared_from_this()));
//	}
//
//	void
//		on_write(
//			beast::error_code ec,
//			std::size_t bytes_transferred)
//	{
//		boost::ignore_unused(bytes_transferred);
//
//		if (ec)
//			return fail(ec, "write");
//
//		// Clear the buffer
//		buffer_.consume(buffer_.size());
//
//		// Do another read
//		do_read();
//	}
//};
//
////------------------------------------------------------------------------------
//
//// Accepts incoming connections and launches the sessions
//class listener : public std::enable_shared_from_this<listener>
//{
//	net::io_context& ioc_;
//	tcp::acceptor acceptor_;
//
//public:
//	listener(
//		net::io_context& ioc,
//		tcp::endpoint endpoint)
//		: ioc_(ioc)
//		, acceptor_(ioc)
//	{
//		beast::error_code ec;
//
//		// Open the acceptor
//		acceptor_.open(endpoint.protocol(), ec);
//		if (ec)
//		{
//			fail(ec, "open");
//			return;
//		}
//
//		// Allow address reuse
//		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
//		if (ec)
//		{
//			fail(ec, "set_option");
//			return;
//		}
//
//		// Bind to the server address
//		acceptor_.bind(endpoint, ec);
//		if (ec)
//		{
//			fail(ec, "bind");
//			return;
//		}
//
//		// Start listening for connections
//		acceptor_.listen(
//			net::socket_base::max_listen_connections, ec);
//		if (ec)
//		{
//			fail(ec, "listen");
//			return;
//		}
//	}
//
//	// Start accepting incoming connections
//	void
//		run()
//	{
//		do_accept();
//	}
//
//private:
//	void
//		do_accept()
//	{
//		// The new connection gets its own strand
//		acceptor_.async_accept(
//			net::make_strand(ioc_),
//			beast::bind_front_handler(
//				&listener::on_accept,
//				shared_from_this()));
//	}
//
//	void
//		on_accept(beast::error_code ec, tcp::socket socket)
//	{
//		if (ec)
//		{
//			fail(ec, "accept");
//		}
//		else
//		{
//			// Create the session and run it
//			std::make_shared<session>(std::move(socket))->run();
//		}
//
//		// Accept another connection
//		do_accept();
//	}
//};
//
////------------------------------------------------------------------------------
//
//int main(int argc, char* argv[])
//{
//	auto const address = net::ip::make_address("0.0.0.0");
//	auto const port = static_cast<unsigned short>(std::atoi("8080"));
//	int  threads  = std::thread::hardware_concurrency();
//
//	// The io_context is required for all I/O
//	net::io_context ioc{ threads };
//
//	// Create and launch a listening port
//	std::make_shared<listener>(ioc, tcp::endpoint{ boost::asio::ip::tcp::v4(), port })->run();
//
//	// Run the I/O service on the requested number of threads
//	std::vector<std::thread> v;
//	v.reserve(threads - 1);
//	for (auto i = threads - 1; i > 0; --i)
//		v.emplace_back(
//			[&ioc]
//			{
//				ioc.run();
//			});
//	ioc.run();
//
//	return EXIT_SUCCESS;
//}
//#include <iostream>
//#include <vector>
//
////c++17语法(折叠表达式)
//template<typename T,typename...Args>
//static void transfer(std::vector<T>& argArray, Args&&... args) {
//	(argArray.push_back(args), ...);
//}
//int main()
//{
//	std::vector<int>ags;
//	transfer(ags,1, 2,3,4,5,6);
//	int i = 10;
//
//	if (int i =4;i==4){
//		int value = 10;
//	}
//}
//
//#include <iostream>
//#include <sstream>
//#include <iomanip>
//#include <fstream>
//#include "CSystemStatus.h"
//
//double round(double number, unsigned int bits) {
//	std::stringstream ss;
//	ss << std::fixed <<std::setprecision(bits) << number;
//	ss >> number;
//	return number;
//}
//
//void str_cpy(char* dest_str, const char* src_str)
//{
//	if (dest_str == nullptr || 
//		src_str == nullptr|| dest_str ==src_str){
//		return;
//	}
//
//	while ((*dest_str++=*src_str++)!=0);
//}
//
//int str_len(const char* src_str)
//{
//	const char* temp_str = src_str;
//	while ((*++temp_str)!=0);
//
//	return temp_str - src_str;
//}
//
//int main()
//{
//	std::string str = "123456";
//	std::string dest_str;
//	dest_str.resize(str_len(str.c_str()));
//	str_cpy(dest_str.data(), str.c_str());
//	//i开头的stream,都是从外面来的数据
//	CSystemStatus status;
//
//	//总内存
//	status.GetTotalPhysicalMemory();
//
//	//获取空闲内存
//	status.GetTotalPhysicalMemoryFree();
//
//	status.GetPhysicalMemoryUsage();
//	auto sss = 3.1415926535897932;
//
//	std::istringstream write("123");
//	int value = 0;
//	write >> value;
//	std::ostringstream read;
//	read << value;
//	std::string test_value = read.str();
//
//	auto ss12 = round(sss,1);
//	ss12 = round(sss, 2);
//	ss12 = round(sss, 3);
//	ss12 = round(sss, 4);
//	ss12 = round(sss, 5);
//}

//#include <iostream>
//#include <string>
//#include <map>
//
//#include <windows.h>
//#include <TlHelp32.h>
//
//bool traverse_processes(std::map<std::wstring, int>&process_names)
//{
//	PROCESSENTRY32 pe32;
//	pe32.dwSize = sizeof(pe32);
//
//	HANDLE process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (process_snap == INVALID_HANDLE_VALUE) {
//		return false;
//	}
//
//	for (bool ret = Process32First(process_snap, &pe32);ret;
//		ret = Process32Next(process_snap, &pe32)){
//		process_names.emplace(pe32.szExeFile, pe32.th32ProcessID);
//	}
//
//	CloseHandle(process_snap);
//	return true;
//}
//
//int main()
//{
//	std::map<std::wstring, int> _nameID;
//
//	if (!traverse_processes(_nameID)) {
//		std::cout << "Start Process Error!" << std::endl;
//	}
//
//	return 0;
//}

//#include <iostream>
//#include "shine_serial.hpp"
//
//struct B
//{
//	//int a;
//	//double b;
//	std::string c;
//	std::string d;
//	//将类型名称及需要序列化的字段用SHINE_SERIAL包裹
//	SHINE_SERIAL(B,c,d);
//};
//
////struct A {
////	int a;
////	double b;
////	std::string c;
////
////	//此处嵌套上方的结构体B
////	std::map<int, B> d;
////
////	std::list<int> e;
////	std::vector<float> f;
////	std::deque<double> g;
////	std::forward_list<long> h;
////	std::set<std::string> i;
////
////	SHINE_SERIAL(A, a, b, c, d, e, f, g, h, i);
////};
//
//int main() {
//
//	/*A a;
//	a.a = 123;
//	a.b = 345.567;
//	a.c = "hello world!";*/
//
//	B b;
//
//	//b.a = 666;
//	//b.b = 777.7777;
//	b.c = "999999!";
//	b.d = "1234";
//
//	auto data = b.shine_serial_encode();
//
//	B a2;
//	a2.shine_serial_decode(data);
// 
//	//a.d.emplace(999, b);
//
//	//a.e.emplace_back(123);
//	//a.e.emplace_back(345);
//
//	//a.f.emplace_back((float)12.34);
//	//a.f.emplace_back((float)45.67);
//
//	//a.g.emplace_back((double)456.789);
//	//a.g.emplace_back((double)78.9);
//
//	//a.h.emplace_front(666);
//	//a.h.emplace_front(555);
//
//	//a.i.emplace("A");
//	//a.i.emplace("B");
//	//a.i.emplace("C");
//
//	////将对象a序列化成字节流
//	//auto data = a.shine_serial_encode();
//
//	////将字节流反序列化成对象，反序列化后a2与a中数据相同
//	//A a2;
//	//a2.shine_serial_decode(data);
//
//	////确定结果
//	//std::cout << ((a == a2) ? "success" : "failed") << std::endl;
//
//	return 0;
//}
//
//#include <iostream>
//#include <filesystem>
//#include <string>
//#include<thread>
//#include <windows.h>
//#include <sstream>
//
//int main()
//{
//	//监视C盘根目录下文件名改变的例子，其中while循环一直监控，直到程序关闭
//	HANDLE hChangeHandle = FindFirstChangeNotification(L"C:/", false, FILE_NOTIFY_CHANGE_FILE_NAME);
//	//判断是否设置成功
//	if (hChangeHandle == INVALID_HANDLE_VALUE) {
//		printf("Find First Change Notification failed\n");
//		system("pause");
//		return 0;
//	}
//
//
//
//	//等待通知
//	printf("These change a file name\n");
//	//循环等待
//	while (true) {
//		if (FindNextChangeNotification(hChangeHandle)) {
//			printf("These change a file name\n");
//			//结束监视程序调用FindCloseChangeNotification关闭句柄
//			FindCloseChangeNotification(hChangeHandle);
//		}
//
//		std::this_thread::sleep_for(std::chrono::microseconds(200));
//	}
//
//	//std::string path_str ="F:/test/test2/新建文本文档.txt";
//
//	//std::filesystem::path path(path_str);
//	//auto root_path = path.root_name().string();
//	//auto root_name = path.root_path().string();
//	//auto root_directory = path.root_directory().string();
//	//auto parent_path = path.parent_path().string();
//
//	//auto relative_path = path.relative_path().string();
//	////auto file_name = path.stem().string();
//	////auto houhui = path.extension().string();
//	////auto file_houzhui = path.stem().string();
//
//	//std::filesystem::remove(path_str);
//
//	//int value = 0x1234;
//	//std::string ret;
//	//ret.append((const char*)&value, sizeof(value));
//
//	//int value1 = 0;
//	//memcpy(&value1, ret.c_str(), sizeof(int));
//
//	//std::error_code ec;
//	//auto data_size = std::filesystem::file_size(path_str, ec);
//	//auto is_exist = std::filesystem::exists(path_str, ec);
//}
//
//#include <iostream>
//#include <filesystem>
//#include <memory>
//namespace fs = std::filesystem;
//
//void for_each_direct(const std::string& path)
//{
//	auto iter_begin = std::filesystem::directory_iterator(path);
//
//	for (;iter_begin != std::filesystem::directory_iterator();++iter_begin){
//		std::cout << iter_begin->path().string() << std::endl;
//	}
//}
//
//void find_direct(const std::string& path)
//{
//	auto iter_begin = std::filesystem::directory_iterator(path);
//
//	for (const auto& info : iter_begin){
//		std::cout << iter_begin->path().string() << std::endl;
//		std::error_code ec;
//		auto time = iter_begin->last_write_time(ec);
//		int time_stamp = 
//			std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
//		int test_value = 12;
//	}
//}
//
//int main()
//{
//
//	fs::create_directories("F:/sandbox/subdir");
//
//	find_direct("F:/sandbox/");
//	//fs::create_directory_symlink("F:/sandbox/subdir", "F:/sandbox/sym2");
//		//if (is_symlink(it->symlink_status()))
//			//std::cout << it->path().string()<< "->" << read_symlink(*it) << '\n';
//
//	fs::remove_all("sandbox");
//}

//#include <memory>
//#include <iostream>
//
//class A
//{
//public:
//	A(int value) :value_(value)
//	{
//
//	}
//private:
//	int value_;
//};
//int main()
//{
//	//c++17之前语法
//	std::shared_ptr<int> shared_good(new int[10],std::default_delete<int[]>());
//	std::shared_ptr<int[]> sp3(new int[10]()); //c++17语法
//
//	//auto ptr_arry = std::make_shared<A[]>(10,1);//c++20语法
//	int value = 10;
//}

//#include <filesystem>
//#include <iostream>
//#include <string>
//#include <tuple>
//#include <vector>
//
//template <typename F, typename ...Args>
//void for_each_args(F&& func, Args...args) {
//	//((std::forward<F>(func)(args)), ...);
//	int arry[] = { (std::forward<F>(func)(args),0)... };
//}
//
//bool find_file(const std::string& path_str, const std::string& name)
//{
//	auto iter_begin = std::filesystem::directory_iterator(path_str);
//
//	for (const auto& info : iter_begin) {
//		auto file_name =
//			iter_begin->path().filename().has_extension();
//	}
//
//	return false;
//}
//
//struct Foo {
//	Foo(int first, float second, int third) {
//		std::cout << first << ", " << second << ", " << third << "\n";
//	}
//};
//
//
//class Foo_value
//{
//private:
//	explicit Foo_value(int value) :
//		value_(value)
//	{
//
//	}
//	int value_;
//};
//
//int main()
//{
//	std::vector<Foo_value>values;
//	values.emplace_back(1);	 //这个要配合构造函数
//	values.emplace_back(2);	 //这个要配合构造函数
//	values.emplace_back(3);	 //这个要配合构造函数
//	values.emplace_back(4);	 //这个要配合构造函数
//
//
//
//	//auto tuple_arry = std::make_tuple<int>(1, 2, 3, 4, 5);
//	auto tuple = std::make_tuple(42, 3.14f, 0);
//	//std::tuple<int,int,int,int,int>	tuple_arry = std::make_tuple<int>(1, 2, 3, 4, 5);
//	for_each_args([](auto value) {std::cout << value << std::endl;}, 1, 2, 3, 4, 5);
//
//	bool falg = find_file("F:/sandbox", "1");
//	std::filesystem::path path_str("E:/test/test.cpp");
//	std::string parent_path = path_str.parent_path().string();
//	std::string relatinve_path = path_str.relative_path().string().data();
//	int value = 10;
//}

//#include <vector> 
//#include <iostream> 
//using namespace std;
//class testDemo
//{
//public:
//	testDemo(int num) :num(num) {
//		std::cout << "调用构造函数" << endl;
//	}
//	testDemo(const testDemo& other) :num(other.num) {
//		std::cout << "调用拷贝构造函数" << endl;
//	}
//	testDemo(testDemo&& other) :num(other.num) {
//		std::cout << "调用移动构造函数" << endl;
//	}
//private:
//	int num;
//};
//int main()
//{
//	cout << "emplace_back:" << endl;
//	std::vector<testDemo> demo1;
//	demo1.emplace_back(2);
//	cout << "push_back:" << endl;
//	std::vector<testDemo> demo2;
//	testDemo demo(2);
//	demo2.push_back(std::move(demo));
//}

//#include <iostream>
//#include <cstdio>
//#include <string>
//#include <vector>
//#include <map>
//#include <unordered_map>
//
//#include "rdc_fs_watcher.h"
//
//class A
//{
//public:
//	A(int value1, int value2)
//		:value_(value1)
//		, value2_(value2)
//	{
//
//	}
//
//	A(const A& other) :value_(other.value_)
//	,value2_(other.value2_){
//		std::cout << "调用拷贝构造函数" << std::endl;
//	}
//	A(A&& other) :value_(other.value_)
//	,value2_(other.value2_){
//		std::cout << "调用移动构造函数" << std::endl;
//	}
//
//private:
//	int value_;
//	int value2_;
//};
//
//class B
//{
//public:
//	B(int value1)
//		:value_(value1)
//	{
//
//	}
//
//	B(const B& other) :value_(other.value_) {
//		std::cout << "调用拷贝构造函数" << std::endl;
//	}
//	B(B&& other) :value_(other.value_){
//		std::cout << "调用移动构造函数" << std::endl;
//	}
//
//private:
//	int value_;
//};
//
//int wmain(int argc, wchar_t** argv)
//{
//	//std::vector<A>info11;
//	//info11.emplace_back(1, 2);
//	std::map<int, A>info11;
//	//分段构造
//	info11.emplace(std::piecewise_construct, std::forward_as_tuple(1), std::forward_as_tuple(1, 2));
//
//	std::map<int,B>info22;
//	info22.emplace(1,2);
//	//const auto info = info11.emplace(std::piecewise_construct, std::forward_as_tuple(1),
//	//	std::forward_as_tuple(2,3));
//	//if (!info.second) {
//	//	return false;
//	//}
//
//	//info11.emplace(1,2,3);
//
//	rdc_fs_watcher watch;
//	watch.set_change_event([](int64_t id,
//		const std::set<std::pair<std::wstring, uint32_t>>& notifications) {
//			for (const auto& notif : notifications) {
//				std::wcout << L"Change on watcher with ID=" << id
//					<< L", relative path: \"" << notif.first.c_str() << L"\""
//					L"event: " << std::to_wstring(notif.second).c_str() << std::endl;
//			}
//		});
//
//	watch.set_error_event([](int64_t id) {
//		std::wcout << L"An error has occurred, no further events will be sent for ID= " << id << std::endl;
//		});
//
//	std::wcout << L"Listening for changes on the following directories:\n";
//
//	bool result = watch.add_directory(1, L"F:/");
//	if (result) {
//		std::wcout << L"Listening for changes (ID=" << 1 << L") on: ";
//	}
//	else {
//		std::wcout << L"Failed to listen for changes on: ";
//	}
//
//	//for (int i = 1; i < argc; ++i) {
//	//	//bool result = watch.addDirectory(i, argv[i]);
//	//	//if (result) {
//	//	//	std::wcout << L"Listening for changes (ID=" << i << L") on: ";
//	//	//}
//	//	//else {
//	//	//	std::wcout << L"Failed to listen for changes on: ";
//	//	//}
//	//	std::wcout << argv[i] << std::endl;
//	//}
//	std::getchar();
//}
//
//#include <iostream>
//#include <unordered_map>
//#include "rdc_fs_watcher.h"
//#include <string>
//#include <iomanip>
//#include <sstream>
//#include <windows.h>  
//#include <psapi.h>  
//#include <direct.h>
//#include <process.h>
//
//double GetMemoryUsage(int pid)
//{
//	uint64_t mem = 0, vmem = 0;
//	PROCESS_MEMORY_COUNTERS pmc;
//
//	// get process handle by pid
//	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
//	if (process == nullptr){
//		return 0;
//	}
//
//	if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc)))
//	{
//		mem = pmc.WorkingSetSize;
//		vmem = pmc.PagefileUsage;
//	}
//
//	CloseHandle(process);
//
//	// use GetCurrentProcess() can get current process and no need to close handle
//
//	// convert mem from B to KB
//	return mem / 1024;
//}
//
//class String
//{
//public:
//	String()
//	{
//		ptr_ = new char[1];
//		*ptr_ = 0;
//	}
//
//	explicit String(const char* str)
//	{
//		int data_size = strlen(str);
//		if (data_size == 1) {
//			ptr_ = new char[1];
//			*ptr_ = 0;
//			return;
//		}
//
//		ptr_ = new char[data_size + 1];
//		memset(ptr_, 0, data_size + 1);
//		memcpy(ptr_, str, data_size);
//	}
//
//	~String()
//	{
//		if (ptr_ != nullptr) {
//			delete[]ptr_;
//			ptr_ = nullptr;
//		}
//	}
//
//	String(const String& str)
//	{
//		memcpy(ptr_, str.ptr_, strlen(str.ptr_));
//	}
//
//	String(String&& str)
//	{
//		ptr_ = str.ptr_;
//		str.ptr_ = nullptr;
//	}
//
//	String& operator=(const String& str)
//	{
//		if (&str == this) {
//			return *this;
//		}
//
//		delete[]ptr_;
//		ptr_ = nullptr;
//		int data_size = strlen(str.ptr_);
//
//		ptr_ = new char[data_size + 1];
//		memset(ptr_, 0, data_size + 1);
//		memcpy(ptr_, str.ptr_, data_size);
//		return *this;
//	}
//
//	String& operator=(String&& str)
//	{
//		if (&str == this) {
//			return *this;
//		}
//
//		delete[]ptr_;
//		ptr_ = nullptr;
//		ptr_ = str.ptr_;
//		str.ptr_ = nullptr;
//		return *this;
//	}
//private:
//	char* ptr_;
//	int len_;
//};
//
//char* strcat_s(char* dest, const char* src)
//{
//	if (dest == nullptr || src == nullptr) {
//		return nullptr;
//	}
//
//	char* temp = dest;
//	while ((*++temp) !=0);
//	while ((*temp++ = *src++) != '\0');
//
//	return dest;
//}
//
//char* strcpy(char* des, const char* src)
//{
//	char* address = des;
//	while ((*des++ = *src++) != '\0');
//
//	return address;
//}
//
//int main()
//{
//	std::string str11 = "12345";
//
//	char*ptr123 = strcat_s(str11.data(), "678");
//	//str_switch(str11.data());
//
//	{
//		String str("1234");
//		String str1;
//		str1 = std::move(str);
//	}
//
//	auto use_age = GetMemoryUsage(4140);
//
//	rdc_fs_watcher watch;
//	watch.set_change_event([](int64_t id,
//		const std::set<std::pair<std::wstring, uint32_t>>& notifications) {
//			for (const auto& notif : notifications) {
//				//std::wcout << L"Change on watcher with ID=" << id
//				//	<< L", relative path: \"" << notif.first.c_str() << L"\""
//				//	L"event: " << std::to_wstring(notif.second).c_str() << std::endl;
//
//				std::cout <<"Change on watcher with ID=" << id
//					<< ", relative path: \"" << notif.first.c_str() << "\""
//					"event: " << g_file_action_mess[notif.second-1] << std::endl;
//			}
//		});
//
//	watch.set_error_event([](int64_t id) {
//		std::wcout << L"An error has occurred, no further events will be sent for ID= " << id << std::endl;
//		});
//
//	std::wcout << L"Listening for changes on the following directories:\n";
//
//	bool result = watch.add_directory(1, L"F:/");
//	if (result) {
//		std::wcout << L"Listening for changes (ID=" << 1 << L") on: ";
//	}
//	else {
//		std::wcout << L"Failed to listen for changes on: ";
//	}
//
//	std::getchar();
//}

//#include <iostream>
//#include <windows.h>
//#include <TlHelp32.h>
//#include <mutex>
//#include<random>
//#include <unordered_map>
//
//std::wstring s2ws(const std::string& str)
//{
//	setlocale(LC_ALL, "");
//	size_t size = mbstowcs(nullptr, str.c_str(), 0);
//	std::wstring w_str;
//	w_str.resize(size);
//	//算出代转wstring字节
//	mbstowcs(w_str.data(), str.c_str(), str.size());
//	return std::move(w_str);
//}
//
//bool get_all_process_infos(std::unordered_map<std::wstring, int>& process_names)
//{
//	PROCESSENTRY32 pe32;
//	pe32.dwSize = sizeof(pe32);
//
//	HANDLE process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (process_snap == INVALID_HANDLE_VALUE) {
//		return false;
//	}
//
//	for (bool ret = Process32First(process_snap, &pe32);ret;
//		ret = Process32Next(process_snap, &pe32)) {
//		process_names.emplace(pe32.szExeFile, pe32.th32ProcessID);
//	}
//
//	CloseHandle(process_snap);
//	return true;
//}
//
//bool kill_process_from_name(const std::string& name)
//{
//	PROCESSENTRY32 pe32;
//	pe32.dwSize = sizeof(pe32);
//
//	HANDLE process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (process_snap == INVALID_HANDLE_VALUE) {
//		return false;
//	}
//
//	std::wstring process_name = s2ws(name);
//	for (bool ret = Process32First(process_snap, &pe32);ret;
//		ret = Process32Next(process_snap, &pe32)) {
//		if (s2ws(name).compare(pe32.szExeFile) ==0) {
//			//从快照进程中获取该进程的PID(即任务管理器中的PID) 
//			HANDLE process = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
//			if (process == nullptr) {
//				continue;
//			}
//
//			::TerminateProcess(process, 0);
//			CloseHandle(process);
//			return true;
//		}
//	}
//
//	return false;
//}
//
//std::string gen_random_string(int length)
//{
//	static const char char_space[] =
//		"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
//	static std::once_flag once;
//	static const size_t len = strlen(char_space);
//	static std::mt19937 rng;
//	std::uniform_int_distribution<std::mt19937::result_type> distace(0, len -1);
//
//	std::call_once(once, [&]() {
//		rng.seed(std::random_device()());
//		});
//
//	std::string str;
//	str.resize(length);
//
//	for (int i = 0; i < length; ++i){
//		int x = distace(rng);
//		x = (x % len);
//		str[i] = char_space[x];
//	}
//
//	return str;
//}
//
//template<typename T>
//void printf_value(T&& value)
//{
//	std::cout << value << std::endl;
//}
//
//template<typename...Args>
//void printf_value(Args...value)
//{
//	(printf_value(value), ...);
//}
//
//int main()
//{
//	printf_value(1, 2, 3, 4, 5);
//	for (int i =0;i<50;++i){
//		std::string value = gen_random_string(3);
//		std::cout << value << std::endl;
//	}
//
//	std::unordered_map<std::wstring, int> info;
//	get_all_process_infos(info);
//	kill_process_from_name("QQ.exe");
//}

//#include <iostream>
//#include "lock_free_queue.hpp"
//
//int main()
//{
//    lock_free_queue<int>que;
//    for (int i =1;i<10;++i){
//        que.enqueue(i);
//    }
//
//    int value = 0;
//
//    que.dequeue(value);
//}

//#include <iostream>
//#include <future>
//#include <thread>
//#include<climits>
//#include<Windows.h>
//#include <string>
//
//void print_int(std::future<int>& fut) {
//	int x = fut.get(); // 获取共享状态的值.
//	std::cout << "value: " << x << '\n'; // 打印 value: 10.
//}
//
//int main()
//{
//	std::promise<int> prom; // 生成一个 std::promise<int> 对象.
//	std::future<int> fut = prom.get_future(); // 和 future 关联.
//	std::thread t(print_int, std::ref(fut)); // 将 future 交给另外一个线程t.
//	prom.set_value(10); // 设置共享状态的值, 此处和线程t保持同步.
//	t.join();
//	return 0;
//}

//#include <iostream>
//
//void dispay(const int value)
//{
//	int test_value = 1;
//	int test = (test_value &value);
//	test_value = 20;
//}
//
//int main()
//{	 
//	//0100 0000
//	//1111 0111
//	//1101 1110 222
//	int value = 223;
//	value = 224;  // 1110 000
//	value = 32;  // 0010 0000
//	value = 16; //0001 0000
//
//	dispay(value);
//
//	value = 10;
//	if ((value++) ==10){
//		std::cout << value << std::endl;
//	}
//
//	int value2 = 0xf7;
//
//	value = (value2 & 32);
//}

//#include <iostream>
//#include <sstream>
//#include <string>
//
//int main()
//{
//	std::string str1 = "12345";
//	int port = 5231;
//	char buf[64]{};
//	snprintf(buf, sizeof(buf), ":%u", port);
//
//	std::string str(buf);
//	std::stringstream stream;
//
//	stream << 123;
//	str = stream.rdbuf()->str();
//	int value = 10;
//}

//#include <iostream>
//#include <string>
//#include "lock_free_queue.hpp"
//
//int main()
//{
//    lock_free_queue<std::string>que;
//    for (int i =1;i<10;++i){
//        std::string value = std::to_string(i);
//        int value2 = std::stoi(value);
//        que.enqueue(std::move(value));
//    }
//
//    lock_free_queue<int>que_int;
//	for (int i = 1;i < 10;++i) {
//        que_int.enqueue(std::move(i));
//	}
//
//    std::string value;
//
//    que.dequeue(value);
//}

//#include <iostream>
//#include "pe_manger.h"
//
//#include <memory>
//
//class A {
//public:
//	static ::std::shared_ptr<A> create() {
//		//return ::std::make_shared<A>();
//		struct temp_class : public A {};
//
//		return std::make_shared<temp_class>();
//	}
//
//	void display()
//	{
//		std::cout << "ss" << std::endl;
//	}
//private:
//	A() { std::cout << "A()" << std::endl; }
//	~A() {
//		std::cout << "~A()" << std::endl;
//	}
//	A(const A&) = delete;
//	const A& operator =(const A&) = delete;
//};
//
//
//class B :public A
//{
//public:
//	B() = default;
//	~B() = default;
//};
//
//int main()
//{
//	//manger.is_admin(GetCurrentProcess());
//}
//#include <iostream>
//#include <vector>
//
//void bubble_sort2(int* a, int len)
//{
//	int i, j;
//	for (i = 0;i < len - 1;i++) {
//		for (int j = 0;j < len - 1 - i;j++) {
//			//从大到小
//			//if (a[j] < a[j+1]) {
//			if (a[j] > a[j + 1]) {
//				int t = a[j];
//				a[j] = a[j + 1];
//				a[j + 1] = t;
//			}
//		}
//	}
//}
//
////选择排序
//void selection_sort(int a[], int len)
//{
//	int i, j;
//	for (i = 0;i < len - 1;++i) {
//		int min = i;
//		for (int j = i + 1;j < len;j++) {
//			if (a[j] < a[min]) {
//				min = j;
//			}
//		}
//
//		if (min != i) {
//			int temp = a[i];
//			a[i] = a[min];
//			a[min] = temp;
//		}
//	}
//}
//
////快速排序
//void quicksort(int a[], int left, int right) {
//	int i, j;
//	if (left > right) {
//		return;
//	}
//
//	//默认小区间范围内的值(基准数归位)
//	int base_value = a[left];
//	i = left;
//	j = right;
//	while (i < j) {
//		//从大的开始找
//		while (a[j] >= base_value && i < j && j--);
//
//		//找到大值再分治一次
//		a[i] = a[j];
//		//从小开始找
//		while (a[i] <= base_value && i < j && i++);
//		//找到小值再分治一次
//		a[j] = a[i];
//	}
//
//	//没有数，可以比了，基准数归位
//	a[i] = base_value;
//	//递归左边去找
//	quicksort(a, left, i - 1);
//	//递归右边去找
//	quicksort(a, i + 1, right);
//}
//
////插入排序
//void insert_sort(int array[], int n)
//{
//	int i, j, temp;
//	for (i = 1;i < n;i++) {
//		if (array[i] < array[i - 1]) {
//			temp = array[i];
//			//前一个元素
//			for (j = i;array[j - 1] > temp;j--) {
//				array[j] = array[j - 1];
//			}
//
//			array[j] = temp;
//		}
//	}
//}
//
////希尔排序
//void sheel_sort1(int array[], int n)
//{
//	int i, j, temp;
//	//希尔排序的步长
//	int gap = n;
//	do {
//		//假设按照区间划分 
//		gap = gap / 3 + 1;
//		for (i = gap;i < n;i++)
//		{
//			//取值步长控制
//			if (array[i] < array[i - gap])
//			{
//				//未排序区的第一个数
//				temp = array[i];
//				//以及排序区的值
//				for (j = i - gap;array[j] > temp;j -= gap)
//				{
//					array[j + gap] = array[j];
//				}
//				array[j + gap] = temp;
//			}
//		}
//	} while (gap > 1);
//
//}
//
////希尔排序
//void sheel_sort2(int array[], int len)
//{
//	//可以理解为列数
//	int grap = 1;
//	//一直找到奇数列
//	while (grap < len / 3) {
//		grap = 3 * grap + 1;
//	}
//
//	while (grap > 1) {
//		for (int i = grap; i >= grap && array[i] < array[i - grap]; i -= grap) {
//			int temp = array[i];
//			array[i] = array[i - grap];
//			array[i - grap] = temp;
//		}
//
//		grap = grap / 3;
//	}
//}
//
////c++实现求最长子序列的值
//int longestCommonSubsequence(const char* str1, const char* str2)
//{
//	int len1 = strlen(str1);
//	int len2 = strlen(str2);
//	int max_len = 0;
//	int max_row;
//	//这里还有个思想就是,把统计次数放入1条对接角线
//	std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));
//
//	for (int i = 1;i < len1 + 1;i++) {
//		for (int j = 1;j < len2 + 1;j++) {
//			if (str1[i - 1] == str1[j - 1]) {
//				//增加前一行和后一列的值
//				dp[i][j] = dp[i - 1][j - 1] + 1;
//			}
//			else {
//				//注意,前一行和后一列要进行比较
//				if (dp[i - 1][j] > dp[i][j - 1]) {
//					dp[i][j] = dp[i - 1][j];
//				}
//			}
//		}
//	}
//	return dp[len1][len2];
//}
//
//void swap(int* a, int* b) {
//	int temp = *b;
//	*b = *a;
//	*a = temp;
//}
//
////创建最大堆
//void max_heapify(int arr[], int start, int end) {
//	// 建立父节点索和子节点的索引
//	int dad = start;
//	int son = dad * 2 + 1;
//	while (son <= end) {
//		// 先比較兩個子節點大小，選擇最大的
//		if (son + 1 <= end && arr[son] < arr[son + 1]) {
//			son++;
//		}
//
//		//如果父節點大於子節點代表調整完畢，直接跳出函數
//		if (arr[dad] > arr[son]) {
//			break;
//		}
//
//		// 否則交換父子內容再繼續子節點和孫節點比较
//		int temp = arr[son];
//		arr[son] = arr[dad];
//		arr[dad] = temp;
//		dad = son;
//		son = dad * 2 + 1;
//	}
//}
//
//void min_heapify(int arr[], int start, int end) {
//	// 建立父节点索和子节点的索引
//	int dad = start;
//	int son = dad * 2 + 1;
//	int temp = arr[dad];
//	while (son <= end) {
//		// 先比較兩個子節點大小，選擇最小的
//		if (son + 1 <= end && arr[son] > arr[son + 1]) {
//			son++;
//			continue;
//		}
//
//		if (temp < arr[son]) {
//			break;
//		}
//
//		int temp = arr[son];
//		arr[son] = arr[dad];
//		arr[dad] = temp;
//		dad = son;
//		son = 2 * dad + 1;
//	}
//}
//
//void max_heap_sort(int arr[], int len) {
//	int i;
//	// 初始化，i從最後一個父節點開始調整
//	for (i = len / 2 - 1; i >= 0; i--) {
//		max_heapify(arr, i, len - 1);
//	}
//
//	// 先將第一個元素和已排好元素前一位做交換，再重新調整，直到排序完畢
//	for (i = len - 1; i > 0; i--) {
//		int temp = arr[i];
//		arr[i] = arr[0];
//		arr[0] = temp;
//		max_heapify(arr, 0, i - 1);
//	}
//}
//
//void min_heap_sort(int arr[], int len) {
//	int i;
//	//初始化，并先调整父节点
//	for (i = len / 2 - 1; i >= 0; i--) {
//		min_heapify(arr, i, len - 1);
//	}
//
//	// 先將第一個元素和已排好元素前一位做交換，再重新調整，直到排序完畢
//	for (i = len - 1; i > 0; i--) {
//		int temp = arr[i];
//		arr[i] = arr[0];
//		arr[0] = temp;
//		min_heapify(arr, 0, i - 1);
//	}
//}
//
////OC5A
//void test_value(int *arry,int data_size)
//{
//
//}
//
//int main()
//{
//	int arr[] = { 9,11,1,3,2,8,10,100,23 };
//	int len = (int)sizeof(arr) / sizeof(*arr);
//	//min_heap_sort(arr, len);
//	max_heap_sort(arr, len);
//
//	int ads = longestCommonSubsequence("abcdedwwwdd", "acewd");
//	int arrry[] = { 5,3,3,4,3,8,9 ,10,11 };
//	int data_size = sizeof(arrry) / sizeof(arrry[0]);
//	bubble_sort2(arrry, data_size);
//	//sheel_sort2(arrry, data_size);
//
//	//int arrry[] = {2,3,6,2,4,5};
//	//int data_size = sizeof(arrry) / sizeof(arrry[0]);
//	//sheel_sort2(arrry, data_size);
//	//int value = 2 / 3;
//	//int arrry[10] = { 1,2,4,9,5,6,8,10,11,12 };	
//	//int arrry[] = { 5,3,3,4,3,8,9 ,10,11 };
//	//int arrry[] = { 5,3,4,4,3,8,9,5,10,11 };
//	//int data_size = sizeof(arrry) / sizeof(arrry[0]);
//	//sheel_sort1(arrry, data_size);
//	//insert_sort(arrry, data_size);
//	//int arrry[10] = { 1,2,4,9,5,6,8,10,11,12 };
//	//std::cout << "快速排序:" << std::endl;
//	////int arrry[] ={ 5,3,3,4,3,8,9 ,10,11 };
//	//int data_size = sizeof(arrry) / sizeof(arrry[0]);
//
//	//std::cout << "排序前:";
//	//for (int i = 0;i < data_size;i++) {
//	//	std::cout << arrry[i]<<" ";
//	//}
//	//
//	//std::cout << std::endl;
//	//quicksort(arrry, 0, data_size-1);
//	//std::cout << "排序后:";
//	//for (int i =0;i<data_size;i++){
//	//	std::cout << arrry[i]<<" ";
//	//}
//	////selection_sort(arrr,10);
//	////bubble_sort2(arrr,10);
//}
//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h> 
//int main()
//{
//	//上三角
//	int a[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
//	int sub[5][5] = {};
//	int index = 0;
//	for (int row=0;row<5;row++){
//		for (int col =0;col<5;col++){
//			if (row == col || col>row) {
//				sub[row][col] = a[index];
//				++index;
//			}
//		}
//	}
//
//	for (int i = 0; i < 5; i++) {
//		for (int j = 0; j < 5; j++) {
//			printf("%4d", sub[i][j]);
//		}
//
//		printf("\n");
//	}
//
//	////下三角
//	//int a[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
//	//int sub[5][5] = {};
//	//int index = 0;
//	//for (int row = 0;row < 5;row++) {
//	//	for (int col = 0;col < 5;col++) {
//	//		if (row == col || row>col) {
//	//			sub[row][col] = a[index];
//	//			++index;
//	//		}
//	//	}
//	//}
//
//	//for (int i = 0; i < 5; i++) {
//	//	for (int j = 0; j < 5; j++) {
//	//		printf("%4d", sub[i][j]);
//	//	}
//
//	//	printf("\n");
//	//}
//}

//#include <iostream>
//#include <vector>
//
////实现图,邻接表,统计有向图
//struct map_info
//{
//	struct map_info* ptr;
//	int value;
//};
//
////选择排序
//void min_selection_sort(int a[], int len)
//{
//	int i, j;
//	for (i = 0;i < len - 1;++i) {
//		int min = i;
//		for (int j = i + 1;j < len;j++) {
//			if (a[j] < a[min]) {
//				min = j;
//			}
//		}
//
//		if (min != i) {
//			int temp = a[i];
//			a[i] = a[min];
//			a[min] = temp;
//		}
//	}
//}
//
//void max_selection_sort(int a[], int len)
//{
//	int i, j;
//	for (i = 0;i < len - 1;++i) {
//		int min = i;
//		for (int j = i + 1;j < len;j++) {
//			if (a[j] > a[min]) {
//				min = j;
//			}
//		}
//
//		if (min != i) {
//			int temp = a[i];
//			a[i] = a[min];
//			a[min] = temp;
//		}
//	}
//}
//
//void create_max_heapify(int arr[], int start, int end)
//{
//	int dad = start;
//	int son = 2 * start + 1;
//	while (son<=end)
//	{
//		if (son+1<=end&&arr[son]<arr[son+1]){
//			son++;
//		}
//
//		if (arr[dad]>arr[son]){
//			break;
//		}
//
//		int temp = arr[dad];
//		arr[dad] = arr[son];
//		arr[son] = temp;
//		dad = son;
//		son = 2 * dad + 1;
//	}
//}
//
//void heap_sort(int arr[], int data_size)
//{
//	for (int i =data_size/2;i>=0;i--){
//		create_max_heapify(arr, i, data_size-1);
//	}
//
//	for (int i = data_size - 1; i > 0; i--){
//		int temp = arr[i];
//		arr[i] = arr[0];
//		arr[0] = temp;
//		create_max_heapify(arr, 0, i - 1);
//	}
//}
//int main()
//{
//	//int arry[] = { 1,2,4,5,10,9,6 };
//	std::vector<int>arry = { 21,17,48,63,21};
//	int data_size = sizeof(arry) / sizeof(arry[0]);
//	min_selection_sort(&arry[0], arry.size());
//	//max_selection_sort(arry, data_size);
//	//heap_sort(arry, data_size);
//}

//#include <iostream>
//#include <algorithm>
//#include <cstring>
//using namespace std;
//
//int dp[6][11] = {};
//
//int kanp_value(int v[], int w[], int current_size, int max_value)
//{
//	int i;
//	int j;
//	for (i=1;i<=current_size;i++){
//		for (j=1;j<=max_value;j++){
//			//当前的重量
//			if (j<w[i]){
//				dp[i][j] = dp[i - 1][j];
//			}else {
//				int temp = dp[i - 1][j - w[i]] + v[i];
//				dp[i][j] = max(dp[i - 1][j], temp);
//			}
//		}
//	}
//
//	return dp[current_size][max_value];
//}
//
//int main()
//{
//	int v[] = { 0,6,3,5,4,6 };//留出一个空间存值
//	int w[] = { 0,6,6,6,5,4 };
//
//	int data_size = sizeof(v) / sizeof(v[0]);
//	int value = kanp_value(v, w, data_size - 1,10);
//}

//#include <iostream>
//#include<vector>
//using namespace std;
//
//
//int dp_select_max(int max_weight, int w[], int size, int v[]) {
//	// 防止无效输入
//	if (max_weight <= 0) {
//		return 0;
//	}
//
//	int n = size;
//	// 背包空的情况下，价值为 0
//	std::vector<int>dp;
//	dp.resize(max_weight + 1);
//	for (int i = 0; i < n; ++i) {
//		for (int j = max_weight; j >= w[i]; --j) {
//			dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
//		}
//	}
//
//	return dp[max_weight];
//
//}
//int main() {
//	int max_weight = 10;
//	int w[5] = { 2,2,6,5,4 };//重量
//	int v[5] = { 6,3,5,4,6 };//权值
//	int x = dp_select_max(max_weight, w, 5,v);
//	cout << "背包能放的最大价值为:" << x << endl;
//	system("pause");
//
//}
//#include <iostream>
//#include <vector>
//using namespace std;
//const int INF = 65530;
////求从起点s到达其它顶点的最短距离，并将最短距离存储在矩阵d中
//void Dijkstra(int n, int s, vector<vector<int>> G, vector<bool>&vis, vector<int>&d, vector<int>&pre)
//{
//	fill(d.begin(), d.end(), INF);                         //初始化最短距离矩阵，全部为INF
//
//	for (int i = 0; i < n; ++i)                            //新添加
//		pre[i] = i;
//
//	d[s] = 0;                                              //起点s到达自身的距离为0
//	for (int i = 0; i < n; ++i)
//	{
//		int u = -1;                                     //找到d[u]最小的u
//		int MIN = INF;                                  //记录最小的d[u]
//		for (int j = 0; j < n; ++j)                     //开始寻找最小的d[u]
//		{
//			if (vis[j] == false && d[j] < MIN)
//			{
//				u = j;
//				MIN = d[j];
//			}
//		}
//		//找不到小于INF的d[u]，说明剩下的顶点和起点s不连通
//		if (u == -1)
//			return;
//		vis[u] = true;                                  //标记u已被访问
//		for (int v = 0; v < n; ++v)
//		{
//			//遍历所有顶点，如果v未被访问&&u能够到达v&&以u为中介点可以使d[v]更优
//			if (vis[v] == false && d[u] + G[u][v] < d[v]) {
//				d[v] = d[u] + G[u][v];             //更新d[v]
//				pre[v] = u;                        //记录v的前驱顶点为u（新添加）
//			}
//		}
//	}
//}
//
////输出从起点s到顶点v的最短路径
//void DFSPrint(int s, int v, vector<int> pre)
//{
//	if (v == s) {
//		cout << s << " ";
//		return;
//	}
//	DFSPrint(s, pre[v], pre);
//	cout << v << " ";
//}
//
//void main()
//{
//	int n = 6;
//	vector<vector<int>> G = { {0,1,INF,4,4,INF},
//							  {INF,0,INF,2,INF,INF},
//							  {INF,INF,0,INF,INF,1},
//							  {INF,INF,2,0,3,INF},
//							  {INF,INF,INF,INF,0,3},
//							  {INF,INF,INF,INF,INF,0} };
//	vector<bool> vis(n);
//	vector<int> d(n);
//	vector<int> pre(n);
//
//	Dijkstra(n, 0, G, vis, d, pre);
//	for (auto x : d)
//		cout << x << " ";
//	cout << endl;
//
//	//输出从起点s到顶点v的最短路径
//	DFSPrint(0, 5, pre);
//}

//#include <iostream>
//
//int binarySearch(int a[], int n, int key) {
//	int low = 0;
//	int high = n - 1;
//	while (low <= high) {
//		int mid = (low + high) / 2;
//		int midVal = a[mid];
//		if (midVal < key)
//			low = mid + 1;
//		else if (midVal > key)
//			high = mid - 1;
//		else
//			return mid;
//	}
//	return -1;
//}
//
//int main()
//{
//	int arr[] = { 500,450,200,180 };
//	int value = binarySearch(arr, 4, 300);
//}

//#include <iostream>
//#include <vector>
//
//void insert_sort(int arr[], int data_size)
//{
//	int i, j, n;
//	n = data_size;
//	int temp;
//	for (i = 1;i < n;++i) {
//		if (arr[i] < arr[i - 1]) {
//			temp = arr[i];
//			for (j = i;j >= 0 && arr[j - 1] > temp;j--) {
//				arr[j] = arr[j - 1];
//			}
//
//			arr[j] = temp;
//		}
//	}
//}
//
//void asc_bubble_sort(int arr[], int data_size)
//{
//	int i, j;
//	for (i = 0;i < data_size;i++) {
//		for (j = 0;j < data_size - i - 1;++j) {
//			if (arr[j] > arr[j + 1]) {
//				int temp = arr[j];
//				arr[j] = arr[j + 1];
//				arr[j + 1] = temp;
//			}
//		}
//	}
//}
//
//class base
//{
//public:
//	base() = default;
//	virtual ~base() = default;
//	virtual void fun() { std::cout << "base" << std::endl; }
//};
//class A :public base
//{
//public:
//	A() = default;
//	virtual ~A() = default;
//	void fun() override {
//		__super::fun();
//		std::cout << "A" << std::endl;
//	}
//};
//
//void asc_select_sort(int arr[], int data_size)
//{
//	int i, j;
//	for (i = 0;i < data_size;i++) {
//		int min_index = i;
//		for (j = i + 1;j < data_size;j++) {
//			if (arr[min_index] > arr[j]) {
//				min_index = j;
//			}
//		}
//
//		if (min_index != i) {
//			int temp = arr[i];
//			arr[i] = arr[min_index];
//			arr[min_index] = temp;
//		}
//	}
//}
//
////class B :public A
////{
////public:
////	B() = default;
////	virtual ~B() = default;
////	void fun() override
////	{
////		__super::fun();
////		std::cout << "B" << std::endl;
////	}
////};
//
//void f32(int r[], int n)
//{
//	int i, j;
//	for (i = 2;i <= n;i++) {
//		r[0] = r[i];
//		j = i - 1;
//		while (j > 0 && r[j] > r[0]) {
//			r[j + 1] = r[j];
//			j--;
//		}
//
//		r[j + 1] = r[0];
//	}
//}
//
//
////哨兵模式
//void InsertSort(int a[], int n)
//{
//	int i, j;
//	for (i =2;i<n;++i){
//		a[0] = a[i];
//		for (j = i - 1;j > 0 && a[j] > a[0];j--) {
//			a[j+1] = a[j];
//		}
//
//		a[j + 1] = a[0];
//	}
//}
//
//int main()
//{
//	//A b;
//	//b.fun();       
//
//	//int&& a = 10;
//	//std::vector<int>vec{ 18,12,10,11,23,2,7 };
//	//asc_bubble_sort(&vec[0], vec.size());
//	int arr[7] = { 0,3,1,2,5,7,9 };
//	int data_size = sizeof(arr) / sizeof(arr[0]);
//	InsertSort(arr, data_size);
//	insert_sort(arr, data_size);
//}
//
//#include <iostream>
//
//
//int partition(int arr[], int low, int high)
//{
//	int base_value = arr[low];
//	while (low<high){
//		//右边查找
//		while (low<high &&arr[high]>=base_value){
//			high--;
//		}
//
//		arr[low] = arr[high];
//		//左边查找
//		while (low<high &&arr[low]<=base_value){
//			++low;
//		}
//
//		arr[high] = arr[low];
//	}
//
//	arr[low] = base_value;
//	return low;
//}
//
//void qsort2(int arr[], int low, int high)
//{
//	if (low>=high){
//		return;
//	}
//
//	int pivo = partition(arr, low, high);
//	qsort2(arr, low, pivo - 1);
//	qsort2(arr, pivo + 1, high);
//}
//
//void qsort1(int arr[], int low, int high)
//{
//	if (low >= high) {
//		return;
//	}
//
//	int base_value = arr[low];
//	int i = low;
//	int j = high;
//	while (i < j) {
//		//右边查找
//		while (i < j && arr[j] >= base_value) {
//			j--;
//		}
//
//		arr[i] = arr[j];
//		//左边查找
//		while (i < j && arr[i] <= base_value) {
//			++i;
//		}
//
//		arr[j] = arr[i];
//	}
//
//	arr[i] = base_value;
//	qsort1(arr, low, i - 1);
//	qsort1(arr, i + 1, high);
//}
//
//int main()
//{
//	int x = (-53>>6);
//
//	int y = x;
//
//	char temp2[260];
//	memset(temp2, 2, 260);
//
//	int arr[] = { 3,1,2,5,7,9 };
//	int data_size = sizeof(arr) / sizeof(arr[0]);
//	int count = 0;
//
//	for (int i =-128;i<128;++i){
//		count++;
//	}
//
//	qsort1(arr, 0, data_size - 1);
//}

//#include <iostream>
//
////递增排序
//void asc_insert_sort(int arr[], int data_size)
//{
//	int i, j;
//	for (i = 1;i < data_size;++i) {
//		//当前的元素,如果小于前一个元素
//		if (arr[i] < arr[i - 1]) {
//			int temp = arr[i];
//			for (j = i;j > 0 && arr[j - 1] > temp;--j) {
//				arr[j] = arr[j - 1];
//			}
//
//			arr[j] = temp;
//		}
//	}
//}
//
//int main()
//{
//	int arr[] = { 3,1,7,5,9,2 };
//	int data_size = sizeof(arr) / sizeof(arr[0]);
//
//	asc_insert_sort(arr, data_size);
//}

//#include <iostream>    
//using namespace std;
//
//const int N = 7;//凸多边形边数+1  
//int w[][N] = { {0,2,2,3,1,4},{2,0,1,5,2,3},{2,1,0,2,1,4},{3,5,2,0,6,2},{1,2,1,6,0,1},{4,3,4,2,1,0} };//凸多边形的权  
//int s[N + 1][N + 1] = {};
//int t[N + 1][N + 1] = {};
//
//int MinWeightTriangulation(int n);
//void Traceback(int i, int j);//构造最优解  
//int Weight(int a, int b, int c);//权函数  
//
//int main()
//{
//	cout << "此多边形的最优三角剖分值为：" << MinWeightTriangulation(N - 1) << endl;
//	cout << "最优三角剖分结构为：" << endl;
//	Traceback(1, 5); //s[i][j]记录了Vi-1和Vj构成三角形的第3个顶点的位置  
//
//	return 0;
//}
//
//int MinWeightTriangulation(int n)
//{
//	for (int i = 1; i <= n; i++){
//		t[i][i] = 0;
//	}
//
//	for (int r = 2; r <= n; r++) //r为当前计算的链长（子问题规模）    
//	{
//		for (int i = 1; i <= n - r + 1; i++)//n-r+1为最后一个r链的前边界    
//		{
//			int j = i + r - 1;//计算前边界为r，链长为r的链的后边界  
//
//			t[i][j] = t[i + 1][j] + Weight(i - 1, i, j);//将链ij划分为A(i) * ( A[i+1:j] )这里实际上就是k=i  
//
//			s[i][j] = i;
//
//			for (int k = i + 1; k < j; k++)
//			{
//				//将链ij划分为( A[i:k] )* (A[k+1:j])     
//				int u = t[i][k] + t[k + 1][j] + Weight(i - 1, k, j);
//				if (u < t[i][j])
//				{
//					t[i][j] = u;
//					s[i][j] = k;
//				}
//			}
//		}
//	}
//	return t[1][N - 2];
//}
//
//void Traceback(int i, int j)
//{
//	if (i == j) return;
//	Traceback(i, s[i][j]);
//	Traceback(s[i][j] + 1, j);
//	cout << "三角剖分顶点：V" << i - 1 << ",V" << j << ",V" << s[i][j] << endl;
//}
//
//int Weight(int a, int b, int c)
//{
//	return w[a][b] + w[b][c] + w[a][c];
//}

//#include <iostream>
//#include <string>
//#include <windows.h>
//
//#include <assert.h>
//using namespace std;
//
////create big heap
//void create_heap(int arr[], int partent_index,int end_index)
//{
//	int son_index = (partent_index << 1) + 1;
//	while (son_index <=end_index){
//		if (son_index+1<=end_index && 
//			arr[son_index]<arr[son_index +1]){
//			son_index++;
//		}
//
//		if (arr[partent_index]>arr[son_index]){
//			break;
//		}
//
//		arr[son_index]^=arr[partent_index];
//		arr[partent_index] ^= arr[son_index];
//		arr[son_index] ^= arr[partent_index];
//
//		partent_index = son_index;
//		son_index = (partent_index << 1) + 1;
//	}
//}
//
////create small heap
//void create_heap1(int arr[], int partent_index, int end_index)
//{
//	int son_index = (partent_index << 1) + 1;
//	while (son_index <= end_index) {
//		if (son_index + 1 <= end_index &&
//			arr[son_index] > arr[son_index + 1]) {
//			son_index++;
//		}
//
//		if (arr[partent_index] < arr[son_index]) {
//			break;
//		}
//
//		arr[son_index] ^= arr[partent_index];
//		arr[partent_index] ^= arr[son_index];
//		arr[son_index] ^= arr[partent_index];
//
//		partent_index = son_index;
//		son_index = (partent_index << 1) + 1;
//	}
//}
//
////create small heap
//void create_heap2(int arr[], int partent_index, int end_index)
//{
//	int son_index = (partent_index << 1) + 1;
//	while (son_index <= end_index) {
//		if (son_index + 1 <= end_index &&
//			arr[son_index] > arr[son_index + 1]) {
//			son_index++;
//		}
//
//		if (arr[partent_index] < arr[son_index]) {
//			break;
//		}
//
//		arr[son_index] ^= arr[partent_index];
//		arr[partent_index] ^= arr[son_index];
//		arr[son_index] ^= arr[partent_index];
//
//		partent_index = son_index;
//		son_index = (partent_index << 1) + 1;
//	}
//}
//
//void heap_sort(int arr[], int data_size)
//{
//	for (int index=(data_size>>1);index>=0;--index ){
//		create_heap(arr, index, data_size - 1);
//		//create_heap1(arr, index, data_size - 1);
//	}
//
//	//for (int index = data_size-1;index >0;--index){
//	//	arr[0] ^= arr[index];
//	//	arr[index] ^= arr[0];
//	//	arr[0] ^= arr[index];
//	//	create_heap(arr,0,index-1);
//	//	//create_heap1(arr, 0, index - 1);
//	//}
//}
//
//int func(int n) {
//	int i = 0, sum = 0;
//	while (sum < n) {
//		sum += ++i;
//	}
//	return sum;
//}
//
//
//int main()
//{
//	//控制台输出
//	OutputDebugString(L"SW12");
//	int arr[] = { 6,9,1,5,8,4,7};
//	int data_size = sizeof(arr) / sizeof(arr[0]);
//	heap_sort(arr, data_size);
//	int* ptr = nullptr;
//	assert(ptr != nullptr);
//	std::cout << *ptr << std::endl;
//	  
//	//int x = func(10);
//	//int a = 0;
//	//for (;a == 0;a++);
//	//int b = 0;
//	//for (;b = 1;b++) {
//	//	std::cout << "11" << std::endl;
//	//}
//
//	//int data_size = sizeof(arr) / sizeof(arr[0]);
//	//heap_sort(arr, data_size);
//}


#include <iostream>
#include <mutex>
#include <string>
std::mutex  cout_mutex;

class A //final
{
public:
	A() = default;
	~A() = default;
};

class B :public A
{

};

void thread_func(const std::string& thread_name) 
{
	//for (int i = 0; i < 3; ++i) {
	//	thread_local int x = 1;
	//	x++;
	//	//std::lock_guard<std::mutex> lock(cout_mutex);
	//	std::cout << "thread[" << thread_name << "]: x = " << x << std::endl;
	//}
	//x++;    //编译会出错：error: ‘x’ was not declared in this scope
	//return;
}

//常量表达式函数的定义
constexpr int display(int x) {
	return 1 + 2 + x;
}

int main()
{
	int a[display(1)] = { 1,2,3,4 };
	//constexpr int num = 20;	//编译可以解决
	//int arr[num] = {};
	const int num = 20;	//编译可以解决,加一个const也可以解决
	int arr[num] = {};

	std::string str = "123";
	int a1 = 10;
}
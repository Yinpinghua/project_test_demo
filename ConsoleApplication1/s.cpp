//#include <iostream>
//#include <wheel/http_server.hpp>
//#include <iguana/json.hpp>
//#include <sstream>
//
//namespace client
//{
//	struct person
//	{
//		std::string	name;
//		int64_t		 age;
//	};
//
//	REFLECTION(person, name, age);
//}
//
//struct service_proxy_configure
//{
//	int log_per_size;
//	int log_keep_num;
//	int log_level;
//};
//
//REFLECTION(service_proxy_configure,log_per_size, log_keep_num, log_level);
//
//using namespace wheel::http_servers;
//wheel::http_servers::http_server server;
//int main()
//{
//	client::person p1 = { "tom", 20 };
//	client::person p2 = { "jack", 19 };
//	client::person p3 = { "mike", 21 };
//
//	std::vector<client::person> v{ p1, p2, p3 };
//	iguana::string_stream ss;
//	iguana::json::to_json(ss, v);
//	auto json_str = ss.str();
//	std::cout << json_str << std::endl;
//
//	std::vector<client::person> v1;
//	iguana::json::from_json(v1, json_str.data(), json_str.length());
//
//	//service_proxy_configure t;
//	//t.log_per_size = 1;
//	//t.log_keep_num = 2;
//	//t.log_level = 3;
//
//	//std::string str = "{\"log_per_size\":200,\"log_keep_num\":1,\"log_level\":20}";
//	//bool r = iguana::json::from_json0(t, str.data(), str.length());
//
//	//std::stringstream ss;
//	//iguana::json::to_json(ss, t);
//	//auto json_str = ss.str();
//
//	//wheel::reflector::for_each_tuple_front(t, [](const auto& tp, const auto index) {
//	//	//auto value = wheel::reflector::get<index>(tp);
//	//	});
//
//	//auto tp = wheel::reflector::get_array<service_proxy_configure>();
//	server.listen(8084);
//
//	server.set_http_handler<GET, POST, OPTIONS, PUT>("/test", [](request& req, response& res) {
//		res.render_string("12345678");
//		});
//
//	server.run();
//}

//#include <boost/beast/core.hpp>
//#include <boost/beast/http.hpp>
//#include <boost/beast/version.hpp>
//#include <boost/asio.hpp>




//#include <chrono>
//#include <cstdlib>
//#include <ctime>
//#include <iostream>
//#include <memory>
//#include <string>
//#include <boost/beast/http/parser.hpp>
//
//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//
//namespace my_program_state
//{
//	std::size_t
//		request_count()
//	{
//		static std::size_t count = 0;
//		return ++count;
//	}
//
//	std::time_t
//		now()
//	{
//		return std::time(0);
//	}
//}
//
//class http_connection : public std::enable_shared_from_this<http_connection>
//{
//public:
//	http_connection(tcp::socket socket)
//		: socket_(std::move(socket))
//	{
//	}
//
//	// Initiate the asynchronous operations associated with the connection.
//	void
//		start()
//	{
//		read_request();
//		check_deadline();
//	}
//
//private:
//	// The socket for the currently connected client.
//	tcp::socket socket_;
//
//	// The buffer for performing reads.
//	beast::flat_buffer buffer_{ 8192 };
//
//	// The request message.
//	http::request<http::dynamic_body> request_;
//
//	// The response message.
//	http::response<http::dynamic_body> response_;
//
//	// The timer for putting a deadline on connection processing.
//	net::steady_timer deadline_{
//		socket_.get_executor(), std::chrono::seconds(60) };
//
//	// Asynchronously receive a complete request message.
//	void
//		read_request()
//	{
//		auto self = shared_from_this();
//	
//		http::parser ps;
//		parser.on_chunk_header(callback);
//		http::async_read(
//			socket_,
//			buffer_,
//			request_,
//			[self](beast::error_code ec,
//				std::size_t bytes_transferred)
//			{
//				boost::ignore_unused(bytes_transferred);
//				if (!ec)
//					self->process_request();
//			});
//
//
//	}
//
//	// Determine what needs to be done with the request message.
//	void
//		process_request()
//	{
//		response_.version(request_.version());
//		response_.keep_alive(false);
//
//		switch (request_.method())
//		{
//		case http::verb::get:
//			response_.result(http::status::ok);
//			response_.set(http::field::server, "Beast");
//			create_response();
//			break;
//
//		default:
//			// We return responses indicating an error if
//			// we do not recognize the request method.
//			response_.result(http::status::bad_request);
//			response_.set(http::field::content_type, "text/plain");
//			beast::ostream(response_.body())
//				<< "Invalid request-method '"
//				<< std::string(request_.method_string())
//				<< "'";
//			break;
//		}
//
//		write_response();
//	}
//
//	// Construct a response message based on the program state.
//	void
//		create_response()
//	{
//		if (request_.target() == "/count")
//		{
//			response_.set(http::field::content_type, "text/html");
//			beast::ostream(response_.body())
//				<< "<html>\n"
//				<< "<head><title>Request count</title></head>\n"
//				<< "<body>\n"
//				<< "<h1>Request count</h1>\n"
//				<< "<p>There have been "
//				<< my_program_state::request_count()
//				<< " requests so far.</p>\n"
//				<< "</body>\n"
//				<< "</html>\n";
//		}
//		else if (request_.target() == "/time")
//		{
//			response_.set(http::field::content_type, "text/html");
//			beast::ostream(response_.body())
//				<< "<html>\n"
//				<< "<head><title>Current time</title></head>\n"
//				<< "<body>\n"
//				<< "<h1>Current time</h1>\n"
//				<< "<p>The current time is "
//				<< my_program_state::now()
//				<< " seconds since the epoch.</p>\n"
//				<< "</body>\n"
//				<< "</html>\n";
//		}
//		else
//		{
//			response_.result(http::status::not_found);
//			response_.set(http::field::content_type, "text/plain");
//			beast::ostream(response_.body()) << "File not found\r\n";
//		}
//	}
//
//	// Asynchronously transmit the response message.
//	void
//		write_response()
//	{
//		auto self = shared_from_this();
//
//		response_.set(http::field::content_length, response_.body().size());
//
//		http::async_write(
//			socket_,
//			response_,
//			[self](beast::error_code ec, std::size_t)
//			{
//				self->socket_.shutdown(tcp::socket::shutdown_send, ec);
//				self->deadline_.cancel();
//			});
//	}
//
//	// Check whether we have spent enough time on this connection.
//	void
//		check_deadline()
//	{
//		auto self = shared_from_this();
//
//		deadline_.async_wait(
//			[self](beast::error_code ec)
//			{
//				if (!ec)
//				{
//					// Close socket to cancel any outstanding operation.
//					self->socket_.close(ec);
//				}
//			});
//	}
//};
//
//// "Loop" forever accepting new connections.
//void
//http_server(tcp::acceptor& acceptor, tcp::socket& socket)
//{
//	acceptor.async_accept(socket,
//		[&](beast::error_code ec)
//		{
//			if (!ec)
//				std::make_shared<http_connection>(std::move(socket))->start();
//			http_server(acceptor, socket);
//		});
//}
//
//int
//main()
//{
//	try
//	{
//
//
//		auto const address = net::ip::make_address("127.0.0.1");
//		unsigned short port = static_cast<unsigned short>(std::atoi("8083"));
//
//		net::io_context ioc{ 1 };
//
//		tcp::acceptor acceptor{ ioc, {address, port} };
//		tcp::socket socket{ ioc };
//		http_server(acceptor, socket);
//
//		ioc.run();
//	}
//	catch (std::exception const& e)
//	{
//		std::cerr << "Error: " << e.what() << std::endl;
//		return EXIT_FAILURE;
//	}
//}

//#include <boost/beast/core.hpp>
//#include <boost/beast/http.hpp>
//#include <boost/beast/version.hpp>
//#include <boost/asio/connect.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <cstdlib>
//#include <iostream>
//#include <string>
//
//namespace beast = boost::beast;     // from <boost/beast.hpp>
//namespace http = beast::http;       // from <boost/beast/http.hpp>
//namespace net = boost::asio;        // from <boost/asio.hpp>
//using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
//
//				  // Performs an HTTP GET and prints the response
//int main()
//{
//	try
//	{
//		//const char* host = "47.111.173.190";
//		//const short port = 8111;
//		const std::string host = "127.0.0.1";
//		const short port = 8084;
//		//std::string target = "api/web/node/server/config";
//		std::string target = "/test";
//		int version = 11;
//
//		// The io_context is required for all I/O
//		net::io_context ioc;
//
//		// These objects perform our I/O
//		tcp::resolver resolver(ioc);
//		beast::tcp_stream stream(ioc);
//
//		// Look up the domain name
//
//		// Make the connection on the IP address we get from a lookup
//		stream.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
//
//		// Set up an HTTP GET request message
//		http::request<http::string_body> req{ http::verb::get, target, version };
//		req.set("Token", "07728ee9faea4f619a563fbb7c15e390");
//		req.set("serverId", "257");
//
//		// Send the HTTP request to the remote host
//		http::write(stream, req);
//
//		// This buffer is used for reading and must be persisted
//		beast::flat_buffer buffer;
//
//		// Declare a container to hold the response
//		http::response<http::dynamic_body> res;
//
//		// Receive the HTTP response
//		http::read(stream, buffer, res);
//
//		// Write the message to standard out
//		std::cout << res << std::endl;
//
//		// Gracefully close the socket
//		beast::error_code ec;
//		stream.socket().shutdown(tcp::socket::shutdown_both, ec);
//
//		// not_connected happens sometimes
//		// so don't bother reporting it.
//		//
//		if (ec && ec != beast::errc::not_connected)
//			throw beast::system_error{ ec };
//
//		// If we get here then the connection is closed gracefully
//	}
//	catch (std::exception const& e)
//	{
//		std::cerr << "Error: " << e.what() << std::endl;
//		return EXIT_FAILURE;
//	}
//	return EXIT_SUCCESS;
//}

////#define WHEEL_ENABLE_SSL
//#include <iostream>
//#include <wheel/http_server.hpp>
//#include <iguana/json.hpp>
//#include <sstream>
//#include <string>
//
//using namespace wheel::http_servers;
//wheel::http_servers::http_server server;
//int main()
//{
//	server.listen(8083);
//	//server.set_ssl_conf({"./user.api.chaojidun.com.crt","./user.api.chaojidun.com.key"});
//
//	//server.listen(9090);
//	server.set_http_handler<GET, POST, OPTIONS, PUT>("/test", [](request& req, response& res) {
//		res.render_string("123456783");
//		});
//
//	server.run();
//}

//
//#include <iostream>
//#include "proxyEngine.h"
//#include <wheel/websocket_server.hpp>
//#include <unordered_map>
//#include <wheel/unit.hpp>
//
//
//
//int main()
//{
//	std::random_device rd;  // 将用于为随机数引擎获得种子
//	std::mt19937 gen(rd()); // 以播种标准 mersenne_twister_engine
//	std::uniform_int_distribution<> dis(0, 3);
//	uint32_t ret = dis(gen);
//
//	std::unordered_map<std::string, bool>distributed_ips_;
//	std::string ip = "127.0.0.1";
//	distributed_ips_.emplace(std::move(ip), true);
//	for (const auto info : distributed_ips_) {
//		std::cout << info.first << std::endl;
//	}
//
//	ProxyEngine eng;
//
//	std::shared_ptr<wheel::websocket::websocket_server> ptr = std::make_shared<wheel::websocket::websocket_server>(std::bind(&ProxyEngine::OnMessage1, &eng, std::placeholders::_1, std::placeholders::_2));
//	ptr->init(8084, 1);
//	//ptr->init(9090, 1);
//	ptr->run();
//
//}
#define WHEEL_ENABLE_SSL
#include <iostream>
#include <wheel/http_server.hpp>
#include <sstream>
#include <string>

using namespace wheel::http_servers;
wheel::http_servers::http_server server;
int main()
{
	server.set_ssl_conf({"./crt.crt","./key.key"});

	server.listen(443,5000);
	server.set_http_handler<GET, POST, OPTIONS, PUT>("", [](request& req, response& res) {
          res.render_string("hello world");
		});

	server.run(30);
}
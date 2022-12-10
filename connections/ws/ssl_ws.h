#ifndef _SSL_WS_H_
#define _SSL_WS_H_

#include "../common/common.h"

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

namespace ws {
	typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
	typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;

	class connection_metadata {
	public:
		typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

		connection_metadata(client*, int, websocketpp::connection_hdl, std::string);

		void on_open(client *, websocketpp::connection_hdl);
		void on_fail(client *, websocketpp::connection_hdl);
		void on_close(client *, websocketpp::connection_hdl);
		void on_message(websocketpp::connection_hdl, client::message_ptr);

		websocketpp::connection_hdl get_hdl() const;
		int get_id() const;
		std::string get_status() const;

		friend std::ostream & operator<< (std::ostream &, connection_metadata const &);

		bool msg_queue_empty(void);
		std::string pop_message(void);

	private:
		int m_id;
		websocketpp::connection_hdl m_hdl;
		std::string m_status;
		std::string m_uri;
		std::string m_server;
		std::string m_error_reason;
		client *m_endpoint;
		std::queue<std::string> m_msg_q;
		mutable std::mutex m_msg_q_mutex;
		std::condition_variable m_msg_q_cdt;

		void push_message(std::string);
	};


	class websocket_endpoint {
	public:
		websocket_endpoint();
		~websocket_endpoint();

		int connect(std::string const &);
		void close(int, websocketpp::close::status::value, std::string);
		void send(int, std::string);
		connection_metadata::ptr get_metadata(int) const;
		std::string read(int) const;
	private:
		typedef std::map<int, connection_metadata::ptr> con_list;
		client m_endpoint;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
		con_list m_connection_list;
		int m_next_id;
	};
}

#endif


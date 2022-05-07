#include "ssl_ws.h"

// TODO: rewrite this in the header

namespace ws {

	connection_metadata::connection_metadata(client *endpoint, int id, websocketpp::connection_hdl hdl, std::string  uri)
		: m_id(id)
		, m_hdl(hdl)
		, m_status("Connecting")
		, m_uri(uri)
		, m_server("N/A")
		, m_endpoint(endpoint) // I really hate this, maybe the whole send function altogether should be in this class?
	{}

	void connection_metadata::on_open(client *c, websocketpp::connection_hdl hdl) {
		websocketpp::lib::error_code ec;
		m_status = "Open";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		m_server = con->get_response_header("Server");

		sleep(1); /* How to wait for status? */
		m_endpoint->send(m_hdl, "2probe", websocketpp::frame::opcode::text, ec);
		if (ec) {
			std::cout << "> Error sending 2probe: " << ec.message() << std::endl;
		    	return;
		}

		sleep(1); /* How to wait for status? */
		m_endpoint->send(m_hdl, "5", websocketpp::frame::opcode::text, ec);
		if (ec) {
			std::cout << "> Error sending 5: " << ec.message() << std::endl;
		    	return;
		}
	}

	void connection_metadata::on_fail(client *c, websocketpp::connection_hdl hdl) {
		m_status = "Failed";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		m_server = con->get_response_header("Server");
		m_error_reason = con->get_ec().message();
	}

	void connection_metadata::on_close(client *c, websocketpp::connection_hdl hdl) {
		m_status = "Closed";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " ("
		  << websocketpp::close::status::get_string(con->get_remote_close_code())
		  << "), close reason: " << con->get_remote_close_reason();
		m_error_reason = s.str();
	}

	void connection_metadata::on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
		if (msg->get_opcode() == websocketpp::frame::opcode::text) {
			std::string payload = msg->get_payload();

			if (payload == "2") {
				websocketpp::lib::error_code ec;
				m_endpoint->send(m_hdl, "3", websocketpp::frame::opcode::text, ec);
				if (ec) {
					std::cout << "> Error sending pong 3: " << ec.message() << std::endl;
				    	return;
				}

			} else if (payload.substr(0, 2) == "42") {
				std::string payload_format = payload.substr(3, payload.length() - 4);
	    			push_message(payload_format);
			}
	    	}
	}

	websocketpp::connection_hdl connection_metadata::get_hdl() const {
		return m_hdl;
	}

	int connection_metadata::get_id() const {
		return m_id;
	}

	std::string connection_metadata::get_status() const {
		return m_status;
	}

	/**
	  Should it use the cdt variable?
	  If it doesn't lock this thread up maybe, otherwise it should be defined externally.
	  Isn't the connection in another thread anyways?
	*/
	bool connection_metadata::msg_queue_empty(void) {
		return m_msg_q.empty();
	}

	std::string connection_metadata::pop_message(void) {
		std::unique_lock<std::mutex> lock(m_msg_q_mutex);
		while(m_msg_q.empty())
		{
			// release lock as long as the wait and reaquire it afterwards.
			m_msg_q_cdt.wait(lock);
		}
		std::string ret = m_msg_q.front();
		m_msg_q.pop();
		return ret;
	}

	void connection_metadata::push_message(std::string msg) {
		std::lock_guard<std::mutex> lock(m_msg_q_mutex);
		m_msg_q.push(msg);
		m_msg_q_cdt.notify_one();
	}

	std::ostream & operator<< (std::ostream & out, connection_metadata const & data) {
	    out
		<< "> URI: " << data.m_uri << "\n"
	        << "> Status: " << data.m_status << "\n"
	        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
	        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);
	    return out;
	}





	/*******************************************************************************************
	  					Connection endpoint
	*******************************************************************************************/





	/**
	  TLS initialization.
	*/
	static context_ptr on_tls_init() {
	    // establishes a SSL connection
	    context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

	    try {
	        ctx->set_options(boost::asio::ssl::context::default_workarounds |
	                         boost::asio::ssl::context::no_sslv2 |
	                         boost::asio::ssl::context::no_sslv3 |
	                         boost::asio::ssl::context::single_dh_use);
	    } catch (std::exception &e) {
	        std::cout << "Error in context pointer: " << e.what() << std::endl;
	    }
	    return ctx;
	}


	 websocket_endpoint::websocket_endpoint() {
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.init_asio();
		m_endpoint.set_tls_init_handler(bind(&on_tls_init));
		m_endpoint.start_perpetual();
		m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
	}

	 websocket_endpoint::~websocket_endpoint() {
		m_endpoint.stop_perpetual();

		for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it) {
			if (it->second->get_status() != "Open") {
				// Close only open connections
				continue;
			}

			std::cout << "> Closing connection " << it->second->get_id() << std::endl;

			websocketpp::lib::error_code ec;
			m_endpoint.close(it->second->get_hdl(), websocketpp::close::status::going_away, "", ec);
			if (ec) {
				std::cout << "> Error closing connection " << it->second->get_id() << ": "
					<< ec.message() << std::endl;
			}
		}
	}

	int  websocket_endpoint::connect(std::string const &uri) {
		websocketpp::lib::error_code ec;

		client::connection_ptr con = m_endpoint.get_connection(uri, ec);
		con->append_header("Cookie", "holosesh=s%3AxmS8xBlQk4kH_rXQOaNjHk_3OuaBDsfA.M0yi%2BZmkiq%2BAmJBRj%2FNg9S%2BaSQpsfHRJcEeYVHLiKXg");

		if(ec) {
			std::cout << ">Connect initialization error: " << ec.message() << std::endl;
			return -1;
		}

		int new_id = m_next_id++;
		connection_metadata::ptr metadata_ptr(new connection_metadata(&m_endpoint, new_id, con->get_handle(), uri));
		m_connection_list[new_id] = metadata_ptr;

		con->set_open_handler(websocketpp::lib::bind(
			&connection_metadata::on_open,
			metadata_ptr,
			&m_endpoint,
			websocketpp::lib::placeholders::_1
		));
		con->set_fail_handler(websocketpp::lib::bind(
			&connection_metadata::on_fail,
			metadata_ptr,
			&m_endpoint,
			websocketpp::lib::placeholders::_1
		));
		con->set_message_handler(websocketpp::lib::bind(
			&connection_metadata::on_message,
			metadata_ptr,
			websocketpp::lib::placeholders::_1,
			websocketpp::lib::placeholders::_2
		));

		m_endpoint.connect(con);

		return new_id;
	}

	void  websocket_endpoint::close(int id, websocketpp::close::status::value code, std::string reason) {
		websocketpp::lib::error_code ec;

		con_list::iterator metadata_it = m_connection_list.find(id);
		if (metadata_it == m_connection_list.end()) {
			std::cout << "> No connection found with id " << id << std::endl;
			return;
		}

		m_endpoint.close(metadata_it->second->get_hdl(), code, "", ec);
		if (ec) {
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
		}
	}

	void  websocket_endpoint::send(int id, std::string message) {
		websocketpp::lib::error_code ec;

		con_list::iterator metadata_it = m_connection_list.find(id);
		if (metadata_it == m_connection_list.end()) {
			std::cout << "> No connection found with id " << id << std::endl;
		    	return;
		}

		m_endpoint.send(metadata_it->second->get_hdl(), message, websocketpp::frame::opcode::text, ec);
		if (ec) {
			std::cout << "> Error sending message: " << ec.message() << std::endl;
		    	return;
		}

	//	metadata_it->second->record_sent_message(message);
	}

	connection_metadata::ptr  websocket_endpoint::get_metadata(int id) const {
		con_list::const_iterator metadata_it = m_connection_list.find(id);
		if(metadata_it == m_connection_list.end()) {
			return connection_metadata::ptr();
		} else {
			return metadata_it->second;
		}
	}
	std::string  websocket_endpoint::get_queue_front(int id) const {
		con_list::const_iterator metadata_it = m_connection_list.find(id);
		if(metadata_it == m_connection_list.end()) {
			return "";
		} else {
			return metadata_it->second->pop_message();
		}
	}
}

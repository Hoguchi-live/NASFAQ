#include "my_ssl.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;

class connection_metadata {
public:
	typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

	connection_metadata(client *endpoint, int id, websocketpp::connection_hdl hdl, std::string  uri)
		: m_id(id)
		, m_hdl(hdl)
		, m_status("Connecting")
		, m_uri(uri)
		, m_server("N/A")
		, m_endpoint(endpoint) // I really hate this, maybe the whole send function altogether should be in this class?
	{}

	void on_open(client *c, websocketpp::connection_hdl hdl) {
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

	void on_fail(client *c, websocketpp::connection_hdl hdl) {
		m_status = "Failed";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		m_server = con->get_response_header("Server");
		m_error_reason = con->get_ec().message();
	}

	void on_close(client *c, websocketpp::connection_hdl hdl) {
		m_status = "Closed";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " ("
		  << websocketpp::close::status::get_string(con->get_remote_close_code())
     		  << "), close reason: " << con->get_remote_close_reason();
    		m_error_reason = s.str();
	}

	void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
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
				/* Add message to a raw struct and pass to parser */
				raw_message_t rmsg; /* should be cached somewhere? */
				rmsg.type = msg_type_detect(payload.substr(2, 20)); /* Get message type, this should be out of this loop and encompassing the pongs and probes */
				rmsg.data = payload.substr(3, payload.length() - 1); /* Should use regex */

				std::cout << "Got payload of type: " << rmsg.type << std::endl;

    	  			//nlohmann::json jres = nlohmann::json::parse(payload.substr(21, payload.length()-21-1));
	    			m_messages.push_back(payload);
			}
	    	}
	}

	websocketpp::connection_hdl get_hdl() const {
		return m_hdl;
	}

	int get_id() const {
		return m_id;
	}

	std::string get_status() const {
		return m_status;
	}

	friend std::ostream & operator<< (std::ostream & out, connection_metadata const & data);

	void record_sent_message(std::string message) {
		m_messages.push_back(">> " + message);
	}

private:
	int m_id;
	websocketpp::connection_hdl m_hdl;
	std::string m_status;
	std::string m_uri;
	std::string m_server;
	std::string m_error_reason;
	std::vector<std::string> m_messages;
	client *m_endpoint;
};

std::ostream & operator<< (std::ostream & out, connection_metadata const & data) {
    out

	<< "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);

	std::vector<std::string>::const_iterator it;
    	for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
        	out << *it << "\n";
    	}

    return out;
}

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

class websocket_endpoint {
public:
	websocket_endpoint() {
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        	m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.init_asio();
        	m_endpoint.set_tls_init_handler(bind(&on_tls_init));
		m_endpoint.start_perpetual();
		m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
	}

	~websocket_endpoint() {
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

	int connect(std::string const &uri) {
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

	void close(int id, websocketpp::close::status::value code, std::string reason) {
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

	void send(int id, std::string message) {
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

    		metadata_it->second->record_sent_message(message);
	}

	connection_metadata::ptr get_metadata(int id) const {
		con_list::const_iterator metadata_it = m_connection_list.find(id);
		if(metadata_it == m_connection_list.end()) {
			return connection_metadata::ptr();
		} else {
			return metadata_it->second;
		}
	}
private:
	typedef std::map<int, connection_metadata::ptr> con_list;

	client m_endpoint;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

	con_list m_connection_list;
	int m_next_id;
};

int connect_ws(std::string sid) {

	bool done = false;
	std::string input;
	websocket_endpoint endpoint;

	while(!done) {

		std::cout << "Enter command: ";
		std::getline(std::cin, input);

		if(input == "quit" || input == "q" ) done = true;
		else if(input == "help") {
			std::cout
                	<< "\nCommand List:\n"
                	<< "help: Display this help text\n"
                	<< "quit: Exit the program\n"
                	<< std::endl;
		} else if (input.substr(0, 3) == "nsf") {
			std::string uri = "wss://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=websocket&sid=" + sid;
			int id = endpoint.connect(uri);
			if(id != -1) {
				std::cout << ">Created nsf connection with id " << id << std::endl;
			}
		} else if (input.substr(0, 7) == "connect") {
			int id = endpoint.connect(input.substr(8));
			if(id != -1) {
				std::cout << ">Created connection with id " << id << std::endl;
			}
		} else if (input.substr(0,4) == "show") {
			int id = atoi(input.substr(5).c_str());

			connection_metadata::ptr metadata = endpoint.get_metadata(id);
			if (metadata) {
				std::cout << *metadata << std::endl;
			} else {
				std::cout << ">Unknown connection id " << id << std::endl;
			}
		} else if (input.substr(0, 5) == "close") {
			std::stringstream ss(input);

			std::string cmd;
			int id;
			int close_code = websocketpp::close::status::normal;
			std::string reason;

			ss >> cmd >> id >> close_code;
			std::getline(ss, reason);

			endpoint.close(id, close_code, reason);
		} else if (input.substr(0,4) == "send") {
			std::stringstream ss(input);

		        std::string cmd;
		        int id;
		        std::string message = "";

		        ss >> cmd >> id;
		        std::getline(ss,message);

		        endpoint.send(id, message);
		} else {
			std::cout  << "Unrecognized command" << std::endl;
		}
	}
	return 0;
}

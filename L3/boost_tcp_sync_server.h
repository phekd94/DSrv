#pragma once

#include <boost/asio.hpp>

namespace DSrv {

using namespace boost::asio;

template <typename Storage, template <typename T> class Base>
class BoostTcpSyncServer : public Base<Storage> {
	public:
		struct TcpServerConnectParameters : Base<Storage>::ConnectParameters {
			TcpServerConnectParameters(const ip::port_type& port_) : port(port_) {}
			ip::port_type port;
		};

		BoostTcpSyncServer(/*std::optional = nullopt*/) // TODO
			: Base<Storage>(kMaxPacketSize, Base<Storage>::InterfaceType::PACKET), m_socket(m_io_context) {}

		void connect(const typename Base<Storage>::ConnectParameters& params) override final;
		void disconnect() override final {} // TODO
		void send_data(const typename Storage::external_data_type& data) override final {}
		void receive_data() override final {}
	private:
		static const size_t kMaxPacketSize = 65535;
		io_context m_io_context; // TODO static?
		ip::tcp::socket m_socket;
		void data_parser(const typename Storage::external_data_type& data) override {} // TODO remove
};

template <typename Storage, template <typename T> class Base>
void BoostTcpSyncServer<Storage, Base>::connect(const typename Base<Storage>::ConnectParameters& params) {
	auto p = dynamic_cast<const TcpServerConnectParameters&>(params);
	ip::tcp::acceptor a { m_io_context, ip::tcp::endpoint(ip::tcp::v4(), p.port) };
	a.accept(m_socket);
}

} // namespace DSrv

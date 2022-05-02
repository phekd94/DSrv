#pragma once

#include <cstddef>
#include <span>

namespace DSrv {

template <typename Storage>
class Base : protected Storage {
	protected:
		struct ConnectParameters {
			virtual ~ConnectParameters() = default;
		};
		const enum class InterfaceType {
			NONE, PACKET, SERIAL
		} m_interface_type;

		explicit Base(const size_t max_data_size, const InterfaceType interface_type = InterfaceType::NONE)
			: Storage(max_data_size), m_interface_type(interface_type) {}
		virtual ~Base() = default;
		Base(const Base&) = delete;
		Base& operator=(const Base&) = delete;

		// Interface class should realize this function
		virtual void connect(const ConnectParameters& params) = 0;
		virtual void disconnect() = 0;
		virtual void send_data(const typename Storage::external_data_type& data) = 0;
		virtual void receive_data() = 0;

		// Parser class should realize this function
		virtual void data_parser(const typename Storage::external_data_type& data) = 0;
};

} // namespace DSrv

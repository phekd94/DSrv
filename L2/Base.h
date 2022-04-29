#pragma once

namespace DSrv {

template <typename Storage>
class Base : protected Storage {
	protected:
		const enum class InterfaceType {
			NONE, PACKET, SERIAL
		} m_interface_type;

		explicit Base(const InterfaceType interface_type = InterfaceType::NONE) : m_interface_type(interface_type) { }
		virtual ~Base() = default;
		Base(const Base&) = delete;
		Base& operator=(const Base&) = delete;

		// Interface class should realize this function
		template <typename...Args>
		virtual void connect(Args...args) = 0;
		virtual void disconnect() = 0;
		virtual void send_data(const std::span<std::byte>& data) = 0;
		virtual void receive_data() = 0;

		// Parser class should realize this function
		virtual void data_parser(const std::span<std::byte>& data) = 0;
};

} // namespace DSrv

#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <span>
#include <queue>
#include <utility>

namespace DSrv {

class Storage {
	public:
		typedef std::pair<std::unique_ptr<std::byte[]>, size_t> data_type;
		typedef std::queue<data_type> container_type;
		typedef container_type::size_type container_size_type;

		Storage(const size_t data_size);
		void set_data(const std::span<std::byte>& data);
		void complete_data();
		data_type get_data();
		container_size_type queue_size() const noexcept { return m_data.size(); }
	private:
		container_type m_data;
		data_type m_filling_data;
		const size_t m_data_size;
		std::mutex m_mutex;
		void new_filling_data() { m_filling_data = data_type(new std::byte[m_data_size], 0); }
};

} // namespace DSrv

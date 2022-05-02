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
		using internal_data_type = std::pair<std::unique_ptr<std::byte[]>, size_t>;
		using external_data_type = std::span<std::byte>;
		using container_type = std::queue<internal_data_type>;
		using container_size_type = container_type::size_type;

		Storage(const size_t max_data_size);
		void set_data(const external_data_type& data);
		void complete_data();
		internal_data_type get_data();
		container_size_type queue_size() const noexcept { return m_data.size(); }
	private:
		container_type m_data;
		internal_data_type m_filling_data;
		const size_t m_data_size;
		std::mutex m_mutex;
		void new_filling_data() { m_filling_data = internal_data_type(new std::byte[m_data_size], 0); }
};

} // namespace DSrv

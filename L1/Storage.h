#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

namespace DSrv {

class Storage {
	public:
		typedef std::pair<std::unique_ptr<uint8_t[]>, size_t> data_type;
		typedef std::queue<data_type> container_type;
		typedef container_type::size_type queue_size_type;

		Storage(size_t data_size);
		void set_data(const uint8_t* data, const size_t size);
		void complete_data();
		data_type data();
		queue_size_type queue_size() const noexcept { return m_data.size(); }
	private:
		container_type m_data;
		data_type m_filling_data;
		const size_t m_data_size;
		std::mutex m_mutex;
};

} // namespace DSrv

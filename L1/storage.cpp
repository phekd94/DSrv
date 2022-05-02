#include "storage.h"

#include <algorithm>

using namespace DSrv;

Storage::Storage(const size_t max_data_size) : m_data_size(max_data_size) {
	new_filling_data();
}

void Storage::set_data(const external_data_type& data) {
	std::lock_guard<decltype(m_mutex)> lock { m_mutex };
	if (m_filling_data.second + data.size() > m_data_size)
		throw std::runtime_error("Data size and data is too much");
	std::copy(data.begin(), data.end(), m_filling_data.first.get() + m_filling_data.second);
	m_filling_data.second += data.size();
}

void Storage::complete_data() {
	std::lock_guard<decltype(m_mutex)> lock { m_mutex };
	m_data.emplace(std::move(m_filling_data));
	new_filling_data();
}

Storage::internal_data_type Storage::get_data() {
	std::lock_guard<decltype(m_mutex)> lock { m_mutex };
	if (!queue_size())
		throw std::runtime_error("Container with data is empty");
	internal_data_type data = std::move(m_data.front());
	m_data.pop();
	return data;
}

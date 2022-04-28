#include "Storage.h"

#include <algorithm>
#include <cassert>

#include <iostream>
#include <iterator>

using namespace DSrv;

Storage::Storage(size_t data_size) : m_data_size(data_size) {
	m_filling_data = data_type(new std::byte[m_data_size], 0); // TODO private method
}

void Storage::set_data(const std::byte* data, const size_t size) { // TODO span of std::byte
	assert(data);
	std::lock_guard<decltype(m_mutex)> lock { m_mutex }; // TODO without decltype(m_mutex) ???
	if (m_filling_data.second + size > m_data_size)
		throw std::runtime_error("Data size and data is too much");
	std::copy(data, data + size, m_filling_data.first.get() + m_filling_data.second);
	m_filling_data.second += size;
}

void Storage::complete_data() {
	std::lock_guard<decltype(m_mutex)> lock { m_mutex }; // TODO without decltype(m_mutex) ???
	m_data.emplace(std::move(m_filling_data));
	m_filling_data = data_type(new std::byte[m_data_size], 0); // TODO private method
}

Storage::data_type Storage::data() {
	std::lock_guard<decltype(m_mutex)> lock { m_mutex }; // TODO without decltype(m_mutex) ???
	if (m_data.empty())
		throw std::runtime_error("Container with data is empty");
	data_type data = std::move(m_data.front());
	m_data.pop();
	return data;
}

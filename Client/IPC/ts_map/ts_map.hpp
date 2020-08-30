#pragma once

#include <map>
#include <mutex>
#include <functional>
#include <optional>
#include <stdexcept>
#include <cinttypes>

template < class K, class V, class Compare = std::less<K>, class Allocator = std::allocator<std::pair<const K, V> > >
class ts_map {
	std::map<K, V, Compare, Allocator> _map;
	std::mutex _m;
public:
	void remove(K key) {
		printf(" > " __FUNCTION__ "\n");
		std::lock_guard<std::mutex> lk(this->_m);
		this->_map.erase(key);
	}
	void add(K key) {
		printf(" > " __FUNCTION__ "\n");
		std::lock_guard<std::mutex> lk(this->_m);
		this->_map[key] = V();
	}
	std::optional<std::reference_wrapper<V>> get(K key) {
		std::lock_guard<std::mutex> lk(this->_m);
		try
		{
			return this->_map.at(key);
		}
		catch (const std::exception& ex)
		{
			return std::nullopt;
		}
	}
	bool is_empty() {
		std::lock_guard<std::mutex> lk(this->_m);
		return this->_map.empty();
	}
};

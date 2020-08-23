#pragma once

#include <queue>
#include <mutex>

template < class _Ty, class _Container = std::deque< _Ty >>
class ts_queue {
	std::queue<_Ty, _Container> _queue;
	std::mutex _m;
public:
	void push(_Ty value) {
		printf("   " "   " "   " " > " __FUNCTION__ "\n");
		std::lock_guard<std::mutex> lk(this->_m);
		this->_queue.push(value);
	}
	_Ty pop() {
		printf("   " "   " "   " " > " __FUNCTION__ "\n");
		std::lock_guard<std::mutex> lk(this->_m);
		auto copy = this->_queue.front();
		this->_queue.pop();
		return copy;
	}
	bool is_empty() {
		std::lock_guard<std::mutex> lk(this->_m);
		return this->_queue.empty();
	}
};
#pragma once

#include <exception>
#include <iostream>
#include <utility>

namespace bmstu
{
template <typename T>
class stack
{
   public:
	stack() : data_(nullptr), size_(0u) {}

	~stack() {
		if (data_ != nullptr){
		for (size_t i=0; i< size_; ++i){
			data_[i].~T();
		}
		operator delete(data_); 
	}}

	bool empty() const noexcept { return size_ == 0u; }

	size_t size() const noexcept { return size_; }

	template <typename... Args>
	void emplace(Args&&... args)
	{
		pushv(std::forward<Args>(args) ...);
	}
 
	void push(T&& value) {
		pushv(std::move(value));
	}

	void clear() noexcept {
		for (size_t i=0; i< size_; ++i){
			data_[i].~T();
		}
		operator delete(data_); 
		data_ = nullptr;
		size_ = 0u;
	}

	void push(const T& value) {
		pushv(value);
	}

	void pop() {
		if (empty()){
			throw std::underflow_error("stack is empty!!!");
		}
		data_[--size_].~T();
		if (size_==0){
			operator delete(data_);
			data_ = nullptr;
		}
	}

	T& top() { 
		if (!empty()){
			return data_[size_ - 1];
		}
		else{
			throw std::underflow_error("stack is empty!!!");
		}
	}

	const T& top() const { 
		if (!empty()){
			return data_[size_ - 1]; 
		}
		else{
			throw std::underflow_error("stack is empty!!!");
		}
	}

   private:
	template <typename... Args>
	void pushv(Args&&... args){
		T* new_data = (T*)(operator new(sizeof(T)*(size_ + 1)));
		for (size_t i=0; i<size_; ++i){
			new (new_data + i) T(std::move(data_[i]));
		}
		new(new_data + size_) T(std::forward<Args>(args) ...);
		for (size_t i=0; i< size_; ++i){
			data_[i].~T();
		}
		operator delete(data_); 
		data_ = new_data;
		size_++;
	}

	T* data_;
	size_t size_;
};
}  // namespace bmstu

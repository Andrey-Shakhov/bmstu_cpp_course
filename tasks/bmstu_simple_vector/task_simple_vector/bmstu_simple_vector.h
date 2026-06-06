#include <ostream>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

namespace bmstu
{
template <typename T>
class simple_vector
{
   public:
	class iterator
	{
	   public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using difference_type = std::ptrdiff_t;

		iterator() = default;

		iterator(const iterator& other) : ptr_(other.ptr_) {}

		iterator(std::nullptr_t) noexcept : ptr_(nullptr) {}

		iterator(iterator&& other) noexcept : ptr_(other.ptr_)
		{
			other.ptr_ = nullptr;
		}
		explicit iterator(pointer ptr) : ptr_(ptr) {}

		reference operator*() const { return *ptr_; }

		pointer operator->() const { return ptr_; }

		friend pointer to_address(const iterator& it) noexcept
		{
			return it.ptr_;
		}

		iterator& operator=(const iterator& other) = default;

		iterator& operator=(iterator&& other) noexcept
		{
			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
			return *this;
		}

#pragma region Operators
		iterator& operator++()
		{
			ptr_++;
			return *this;
		}

		iterator& operator--()
		{
			ptr_--;
			return *this;
		}

		iterator operator++(int)
		{
			iterator temp = iterator(ptr_);
			ptr_++;
			return temp;
		}

		iterator operator--(int)
		{
			iterator temp = iterator(ptr_);
			ptr_--;
			return temp;
		}

		explicit operator bool() const { return ptr_ != nullptr; }

		friend bool operator==(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ == rhs.ptr_;
		}

		friend bool operator==(const iterator& lhs, std::nullptr_t)
		{
			return lhs.ptr_ == nullptr;
		}

		iterator& operator=(std::nullptr_t) noexcept
		{
			ptr_ = nullptr;
			return *this;
		}

		friend bool operator==(std::nullptr_t, const iterator& rhs)
		{
			return rhs.ptr_ == nullptr;
		}

		friend bool operator!=(const iterator& lhs, const iterator& rhs)
		{
			return lhs.ptr_ != rhs.ptr_;
		}

		iterator operator+(const difference_type& n) const noexcept
		{
			return iterator(ptr_ + n);
		}

		iterator operator+=(const difference_type& n) noexcept
		{
			ptr_ += n;
			return *this;
		}

		iterator operator-(const difference_type& n) const noexcept
		{
			return iterator(ptr_ - n);
		}

		iterator operator-=(const difference_type& n) noexcept
		{
			ptr_ -= n;
			return *this;
		}

		friend difference_type operator-(const iterator& end,
										 const iterator& begin) noexcept
		{
			return end.ptr_ - begin.ptr_;
		}

#pragma endregion
	   private:
		pointer ptr_ = nullptr;
	};

	simple_vector() noexcept = default;

	~simple_vector() = default;

	simple_vector(std::initializer_list<T> init) noexcept
		: size_(init.size()), capacity_(init.size()), data_(init.size())
	{
		size_t i = 0;
		for (const T& value : init)
		{
			data_[i++] = value;
		}
	}

	simple_vector(const simple_vector& other)
		: size_(other.size_), capacity_(other.size_), data_(other.size_)
	{
		for (size_t i = 0; i < size_; i++)
		{
			data_[i] = other.data_[i];
		}
	}

	simple_vector(simple_vector&& other) noexcept
		: data_(std::move(other.data_)),
		  size_(other.size_),
		  capacity_(other.capacity_)
	{
		other.size_ = 0;
		other.capacity_ = 0;
	}

	simple_vector& operator=(const simple_vector& other)
	{
		if (this != &other)
		{
			simple_vector temp(other);
			swap(temp);
		}
		return *this;
	}
	simple_vector& operator=(simple_vector&& other)
	{
		if (this != &other)
		{
			swap(other);
		}
		return *this;
	}

	simple_vector(size_t size, const T& value = T{})
		: size_(size), capacity_(size), data_(size)
	{
		for (size_t i = 0; i < size_; i++)
		{
			data_[i] = value;
		}
	}

	iterator begin() noexcept { return iterator(data_.get()); }

	iterator end() noexcept { return iterator(data_.get() + size_); }

	using const_iterator = iterator;

	const_iterator begin() const noexcept
	{
		return const_iterator(data_.get());
	}

	const_iterator end() const noexcept
	{
		return const_iterator(data_.get() + size_);
	}

	typename iterator::reference operator[](size_t index) noexcept
	{
		return data_[index];
	}

	typename const_iterator::reference operator[](size_t index) const noexcept
	{
		return data_[index];
	}

	typename iterator::reference at(size_t index)
	{
		if (index >= size_)
		{
			throw std::out_of_range("Index out of range");
		}
		return data_.get()[index];
	}

	typename const_iterator::reference at(size_t index) const
	{
		if (index >= size_)
		{
			throw std::out_of_range("Index out of range");
		}
		return data_.get()[index];
	}

	size_t size() const noexcept { return size_; }

	size_t capacity() const noexcept { return capacity_; }

	void swap(simple_vector& other) noexcept
	{
		data_.swap(other.data_);
		size_t temp_s = size_;
		size_ = other.size_;
		other.size_ = temp_s;

		size_t temp_c = capacity_;
		capacity_ = other.capacity_;
		other.capacity_ = temp_c;
	}

	friend void swap(simple_vector& lhs, simple_vector& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	void reserve(size_t new_cap)
	{
		if (new_cap > capacity_)
		{
			array_ptr<T> new_data_(new_cap);
			for (size_t i = 0; i < size_; i++)
			{
				new_data_[i] = std::move(data_[i]);
			}
			new_data_.swap(data_);
			capacity_ = new_cap;
		}
	}

	void resize(size_t new_size)
	{
		if (new_size > capacity_)
		{
			reserve(new_size);
		}
		if (new_size > size_)
		{
			for (size_t i = size_; i < new_size; i++)
			{
				data_[i] = T();
			}
		}
		else
		{
			for (size_t i = new_size; i < size_; i++)
			{
				data_[i] = T();
			}
		}
		size_ = new_size;
	}

	iterator insert(const_iterator where, T&& value)
	{
		size_t index = where - begin();
		if (capacity_ == 0)
		{
			reserve(1);
		}
		if (size_ == capacity_)
		{
			reserve(capacity_ * 2);
		}
		for (size_t i = size_; i > index; i--)
		{
			data_[i] = std::move(data_[i - 1]);
		}
		data_[index] = std::move(value);
		size_++;
		return iterator(data_.get() + index);
	}

	iterator insert(const_iterator where, const T& value)
	{
		size_t index = where - begin();
		if (capacity_ == 0)
		{
			reserve(1);
		}
		if (size_ == capacity_)
		{
			reserve(capacity_ * 2);
		}
		for (size_t i = size_; i > index; i--)
		{
			data_[i] = std::move(data_[i - 1]);
		}
		data_[index] = value;
		size_++;
		return iterator(data_.get() + index);
	}

	void push_back(T&& value)
	{
		if (capacity_ == 0)
		{
			reserve(1);
		}
		if (size_ == capacity_)
		{
			reserve(capacity_ * 2);
		}
		data_[size_] = std::move(value);
		size_++;
	}

	void clear() noexcept
	{
		for (size_t i = 0; i < size_; i++)
		{
			data_[i].~T();
		}
		size_ = 0;
	}

	void push_back(const T& value)
	{
		if (capacity_ == 0)
		{
			reserve(1);
		}
		if (size_ == capacity_)
		{
			reserve(capacity_ * 2);
		}
		data_[size_] = value;
		size_++;
	}

	bool empty() const noexcept { return size_ == 0; }

	void pop_back()
	{
		if (size_ != 0)
		{
			data_[size_ - 1].~T();
			size_--;
		}
	}
	iterator find(T name)
	{
		size_t index = -1;
		for (size_t i=0; i<size_; i++){
			if (data_[i]==name){
				index = i;
			}
		}
		if (index==-1){
			return end();
		}
		return iterator(begin() + index);
	}

	friend bool operator==(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (lhs.size() != rhs.size())
		{
			return false;
		}
		for (size_t i = 0; i < lhs.size(); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return false;
			}
		}
		return true;
	}

	friend bool operator!=(const simple_vector& lhs, const simple_vector& rhs)
	{
		return !(lhs == rhs);
	}

	friend auto operator<=>(const simple_vector& lhs, const simple_vector& rhs)
	{
		size_t min = (lhs.size() < rhs.size()) ? lhs.size() : rhs.size();
		for (size_t i = 0; i < min; i++)
		{
			auto cmp = lhs.data_[i] <=> rhs.data_[i];
			if (cmp != 0)
			{
				return cmp;
			}
		}
		return lhs.size() <=> rhs.size();
	}

	friend std::ostream& operator<<(std::ostream& os, const simple_vector& vec)
	{
		for (size_t i = 0; i < vec.size(); i++)
		{
			os << vec.data_[i];
		}
		return os;
	}

	iterator erase(iterator where)
	{
		size_t index = where - begin();
		for (size_t i = index; i < size_ - 1; i++)
		{
			data_[i] = std::move(data_[i + 1]);
		}
		size_--;
		return iterator(data_.get() + index);
	}

   private:
	static bool alphabet_compare(const simple_vector<T>& lhs,
								 const simple_vector<T>& rhs)
	{
		size_t min = (lhs.size() < rhs.size()) ? lhs.size() : rhs.size();
		for (size_t i = 0; i < min; i++)
		{
			if (lhs.data_[i] < rhs.data_[i])
			{
				return true;
			}
			if (lhs.data_[i] > rhs.data_[i])
			{
				return false;
			}
		}
		return lhs.size() < rhs.size();
	}
	array_ptr<T> data_;
	size_t size_ = 0;
	size_t capacity_ = 0;
};
}  // namespace bmstu

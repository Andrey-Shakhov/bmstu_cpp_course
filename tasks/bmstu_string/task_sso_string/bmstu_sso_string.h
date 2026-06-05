#pragma once

#include <exception>
#include <iostream>
#include <utility>

namespace bmstu
{
template <typename T>
class basic_string;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template <typename T>
class basic_string
{
   private:
	static constexpr size_t SSO_CAPACITY =
		(sizeof(T*) + sizeof(size_t) + sizeof(size_t)) / sizeof(T) - 1;

	struct LongString
	{
		T* ptr;
		size_t size;
		size_t capacity;
	};

	struct ShortString
	{
		T buffer[SSO_CAPACITY + 1];
		unsigned char size;
	};

	union Data
	{
		LongString long_str;
		ShortString short_str;
	};

	Data data_;
	bool is_long_;

	bool is_long() const { return is_long_; }

	T* get_ptr()
	{
		return (is_long_) ? data_.long_str.ptr : data_.short_str.buffer;
	}

	const T* get_ptr() const
	{
		return (is_long_) ? data_.long_str.ptr : data_.short_str.buffer;
	}

	size_t get_size() const
	{
		return (is_long_) ? data_.long_str.size : data_.short_str.size;
	}

	size_t get_capacity() const
	{
		return (is_long_) ? data_.long_str.capacity : SSO_CAPACITY;
	}

   public:
	basic_string()
	{
		is_long_ = false;
		data_.short_str.buffer[0] = T();
		data_.short_str.size = 0;
	}

	basic_string(size_t size)
	{
		if (size > SSO_CAPACITY)
		{
			is_long_ = true;
			data_.long_str.capacity = size * 2;
			data_.long_str.size = size;
			data_.long_str.ptr = new T[data_.long_str.capacity + 1];
		}
		else
		{
			is_long_ = false;
			data_.short_str.size = size;
		}
		for (size_t i = 0; i < size; i++)
		{
			get_ptr()[i] = T(' ');
		}
		get_ptr()[size] = T();
	}

	basic_string(std::initializer_list<T> il)
	{
		size_t size = il.size();
		if (size > SSO_CAPACITY)
		{
			is_long_ = true;
			data_.long_str.capacity = size * 2;
			data_.long_str.size = size;
			data_.long_str.ptr = new T[data_.long_str.capacity + 1];
		}
		else
		{
			is_long_ = false;
			data_.short_str.size = size;
		}
		size_t i = 0;
		for (T val : il)
		{
			get_ptr()[i] = val;
			i++;
		}
		get_ptr()[size] = T();
	}

	basic_string(const T* c_str)
	{
		size_t len = strlen_(c_str);
		if (len > SSO_CAPACITY)
		{
			is_long_ = true;
			data_.long_str.capacity = len * 2;
			data_.long_str.size = len;
			data_.long_str.ptr = new T[data_.long_str.capacity + 1];
		}
		else
		{
			is_long_ = false;
			data_.short_str.size = len;
		}
		for (size_t i = 0; i <= len; i++)
		{
			get_ptr()[i] = c_str[i];
		}
	}

	basic_string(const basic_string& other)
	{
		is_long_ = other.is_long_;
		if (is_long_)
		{
			data_.long_str.size = other.data_.long_str.size;
			data_.long_str.capacity = other.data_.long_str.capacity;
			data_.long_str.ptr = new T[other.data_.long_str.capacity + 1];
		}
		else
		{
			data_.short_str.size = other.data_.short_str.size;
		}
		for (size_t i = 0; i <= size(); i++)
		{
			get_ptr()[i] = other.get_ptr()[i];
		}
	}

	basic_string(basic_string&& dying) noexcept
	{
		is_long_ = dying.is_long_;
		if (is_long_)
		{
			data_.long_str = dying.data_.long_str;
			dying.is_long_ = false;
			dying.data_.short_str.size = 0;
			dying.data_.short_str.buffer[0] = T();
		}
		else
		{
			data_.short_str = dying.data_.short_str;
		}
	}

	~basic_string() { clean_(); }

	const T* c_str() const { return get_ptr(); }

	size_t size() const { return get_size(); }

	size_t capacity() const { return get_capacity(); }

	basic_string& operator=(basic_string&& other) noexcept
	{
		if (this != &other)
		{
			clean_();
			is_long_ = other.is_long_;
			if (is_long_)
			{
				data_.long_str = other.data_.long_str;
				other.is_long_ = false;
			}
			else
			{
				data_.short_str = other.data_.short_str;
			}
			other.data_.short_str.size = 0;
			other.data_.short_str.buffer[0] = T();
		}

		return *this;
	}

	basic_string& operator=(const T* c_str)
	{
		size_t len = strlen_(c_str);
		clean_();
		if (len > SSO_CAPACITY)
		{
			is_long_ = true;
			data_.long_str.capacity = len * 2;
			data_.long_str.size = len;
			data_.long_str.ptr = new T[data_.long_str.capacity + 1];
		}
		else
		{
			is_long_ = false;
			data_.short_str.size = len;
		}
		for (size_t i = 0; i <= len; i++)
		{
			get_ptr()[i] = c_str[i];
		}
		return *this;
	}

	basic_string& operator=(const basic_string& other)
	{
		if (this != &other)
		{
			clean_();
			is_long_ = other.is_long_;
			if (is_long_)
			{
				data_.long_str.size = other.data_.long_str.size;
				data_.long_str.capacity = other.data_.long_str.capacity;
				data_.long_str.ptr = new T[data_.long_str.capacity + 1];
			}
			else
			{
				data_.short_str.size = other.data_.short_str.size;
			}
			for (size_t i = 0; i <= size(); i++)
			{
				get_ptr()[i] = other.get_ptr()[i];
			}
		}
		return *this;
	}

	friend basic_string<T> operator+(const basic_string<T>& left,
									 const basic_string<T>& right)
	{
		basic_string new_str = left;
		new_str += right;
		return new_str;
	}

	template <typename S>
	friend S& operator<<(S& os, const basic_string& obj)
	{
		os << obj.c_str();
		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, basic_string& obj)
	{
		obj.clean_();
		T sym;
		while (is.get(sym))
		{
			obj += sym;
		}
		return is;
	}

	basic_string& operator+=(const basic_string& other)
	{
		size_t new_size = get_size() + other.get_size();
		if (new_size > get_capacity())
		{
			T* new_ptr = new T[new_size * 2 + 1];
			for (size_t i = 0; i < get_size(); i++)
			{
				new_ptr[i] = get_ptr()[i];
			}
			if (is_long_)
			{
				delete[] data_.long_str.ptr;
			}
			data_.long_str.ptr = new_ptr;
			is_long_ = true;
		}
		if (!is_long_)
		{
			data_.short_str.size = new_size;
		}
		else
		{
			data_.long_str.size = new_size;
			data_.long_str.capacity = new_size * 2;
		}
		for (size_t i = 0; i <= other.get_size(); i++)
		{
			get_ptr()[new_size - other.get_size() + i] = other.get_ptr()[i];
		}
		return *this;
	}

	basic_string& operator+=(T symbol)
	{
		size_t new_size = get_size() + 1;
		if (new_size > get_capacity())
		{
			T* new_ptr = new T[new_size * 2 + 1];
			for (size_t i = 0; i < get_size(); i++)
			{
				new_ptr[i] = get_ptr()[i];
			}
			clean_();
			data_.long_str.ptr = new_ptr;
			is_long_ = true;
		}
		if (!is_long_)
		{
			data_.short_str.size = new_size;
		}
		else
		{
			data_.long_str.size = new_size;
			data_.long_str.capacity = new_size * 2;
		}
		get_ptr()[new_size - 1] = symbol;
		get_ptr()[new_size] = T();
		return *this;
	}

	T& operator[](size_t index) noexcept { return get_ptr()[index]; }
	bool is_using_sso() const { return !is_long_; }

	T& at(size_t index)
	{
		if (index >= size())
		{
			throw std::out_of_range("Wrong index");
		}
		return get_ptr()[index];
	}

	T* data() { return get_ptr(); }

   private:
	static size_t strlen_(const T* str)
	{
		size_t len = 0;
		while (str[len])
			len++;
		return len;
	}

	void clean_()
	{
		if (is_long_)
		{
			delete[] data_.long_str.ptr;
			data_.long_str.size = 0;
			data_.long_str.ptr = nullptr;
			data_.long_str.capacity = 0;
		}
		else
		{
			data_.short_str.size = 0;
			data_.short_str.buffer[0] = T();
		}
	}
};
}  // namespace bmstu

#include <cstddef>
#include <iterator>
#include <ostream>
#include "abstract_iterator.h"

namespace bmstu
{
template <typename T>
class list
{
	struct node
	{
		node() = default;

		node(node* prev, const T& value, node* next)
			: next_node_(next), prev_node_(prev), value_(value)
		{
		}

		T value_;
		node* next_node_ = nullptr;
		node* prev_node_ = nullptr;
	};

   public:
	struct iterator
		: public abstract_iterator<iterator, T, std::bidirectional_iterator_tag>
	{
		node* current;
		iterator() : current(nullptr) {}
		iterator(node* node) : current(node) {}
		iterator& operator++() override
		{
			current = current->next_node_;
			return *this;
		}
		iterator& operator--() override
		{
			current = current->prev_node_;
			return *this;
		}
		iterator operator++(int) override
		{
			iterator temp = *this;
			current = current->next_node_;
			return temp;
		}
		iterator operator--(int) override
		{
			iterator temp = *this;
			current = current->prev_node_;
			return temp;
		}
		iterator& operator+=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (typename abstract_iterator<
					 iterator, T,
					 std::bidirectional_iterator_tag>::difference_type i = 0;
				 i < n; ++i)
			{
				current = current->next_node_;
			}
			return *this;
		}
		iterator& operator-=(
			const typename abstract_iterator<
				iterator,
				T,
				std::bidirectional_iterator_tag>::difference_type& n) override
		{
			for (typename abstract_iterator<
					 iterator, T,
					 std::bidirectional_iterator_tag>::difference_type i = 0;
				 i < n; ++i)
			{
				current = current->prev_node_;
			}
			return *this;
		}
		iterator operator+(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator temp = *this;
			for (typename abstract_iterator<
					 iterator, T,
					 std::bidirectional_iterator_tag>::difference_type i = 0;
				 i < n; ++i)
			{
				temp.current = temp.current->next_node_;
			}
			return temp;
		}
		iterator operator-(const typename abstract_iterator<
						   iterator,
						   T,
						   std::bidirectional_iterator_tag>::difference_type& n)
			const override
		{
			iterator temp = *this;
			for (typename abstract_iterator<
					 iterator, T,
					 std::bidirectional_iterator_tag>::difference_type i = 0;
				 i < n; ++i)
			{
				temp.current = temp.current->prev_node_;
			}
			return temp;
		}
		typename abstract_iterator<iterator,
								   T,
								   std::bidirectional_iterator_tag>::reference
		operator*() const override
		{
			return current->value_;
		}
		typename abstract_iterator<iterator,
								   T,
								   std::bidirectional_iterator_tag>::pointer
		operator->() const override
		{
			return &(current->value_);
		}
		bool operator==(const iterator& other) const override
		{
			return current == other.current;
		}
		bool operator!=(const iterator& other) const override
		{
			return current != other.current;
		}
		explicit operator bool() const override { return current != nullptr; }
		typename abstract_iterator<
			iterator,
			T,
			std::bidirectional_iterator_tag>::difference_type
		operator-(const iterator& other) const override
		{
			typename abstract_iterator<
				iterator, T, std::bidirectional_iterator_tag>::difference_type
				dist = 0;
			iterator temp = *this;
			while (temp != other)
			{
				temp.current = temp.current->prev_node_;
				dist++;
			}
			return dist;
		}
	};
	using const_iterator = iterator;

	list()
	{
		head_ = new node();
		tail_ = new node();
		head_->next_node_ = tail_;
		tail_->prev_node_ = head_;
		size_ = 0;
	}

	template <typename it>
	list(it begin, it end) : list()
	{
		while (begin != end)
		{
			push_back(*begin);
			++begin;
		}
	}

	list(std::initializer_list<T> values) : list()
	{
		for (const T& value : values)
		{
			push_back(value);
		}
	}

	list(const list& other) : list()
	{
		iterator it = other.begin();
		for (it; it != other.end(); it++)
		{
			push_back(*it);
		}
	}

	list(list&& other)
	{
		head_ = other.head_;
		tail_ = other.tail_;
		size_ = other.size_;
		other.head_ = new node();
		other.tail_ = new node();
		other.head_->next_node_=other.tail_;
		other.tail_->prev_node_=other.head_;
		other.size_ = 0;
	}

#pragma endregion
#pragma region pushs

	template <typename Type>
	void push_back(const Type& value)
	{
		node* last = tail_->prev_node_;
		node* new_last = new node(tail_->prev_node_, value, tail_);
		tail_->prev_node_ = new_last;
		last->next_node_ = new_last;
		++size_;
	}
	template <typename Type>
	void push_front(const Type& value)
	{
		// адрес реального последнего элемента
		node* first = head_->next_node_;
		node* new_first = new node(head_, value, first);
		head_->next_node_ = new_first;
		first->prev_node_ = new_first;
		++size_;
	}

#pragma endregion

	bool empty() const noexcept { return (size_ == 0u); }

	~list()
	{
		clear();
		delete head_;
		delete tail_;
	}

	void clear()
	{
		node* temp = head_->next_node_;
		while (temp != tail_)
		{
			node* new_temp = temp->next_node_;
			delete temp;
			temp = new_temp;
		}
		size_ = 0;
		head_->next_node_ = tail_;
		tail_->prev_node_ = head_;
	}

	size_t size() const { return size_; }

	void swap(list& other) noexcept
	{
		node* temp = head_;
		head_ = other.head_;
		other.head_ = temp;

		node* temp1 = tail_;
		tail_ = other.tail_;
		other.tail_ = temp1;

		size_t temp2 = size_;
		size_ = other.size_;
		other.size_ = temp2;
	}

	friend void swap(list& l, list& r) { l.swap(r); }

#pragma region iterators

	iterator begin() noexcept { return iterator{head_->next_node_}; }

	iterator end() noexcept { return iterator{tail_}; }

	const_iterator begin() const noexcept
	{
		return const_iterator{head_->next_node_};
	}

	const_iterator end() const noexcept { return const_iterator{tail_}; }

	const_iterator cbegin() const noexcept
	{
		return const_iterator{head_->next_node_};
	}

	const_iterator cend() const noexcept { return const_iterator{tail_}; }

#pragma endregion

	T operator[](size_t pos) const
	{
		iterator it = begin();
		it += pos;
		return *it;
	}

	T& operator[](size_t pos)
	{
		iterator it = begin();
		it += pos;
		return *it;
	}

	friend bool operator==(const list& l, const list& r)
	{
		if (l.size_ != r.size_)
		{
			return false;
		}
		iterator it1 = l.begin();
		iterator it2 = r.begin();
		while (it1 != l.end())
		{
			if (*it1 != *it2)
			{
				return false;
			}
			it1++;
			it2++;
		}
		return true;
	}

	friend bool operator!=(const list& l, const list& r) { return !(l == r); }

	friend auto operator<=>(const list& lhs, const list& rhs)
	{
		return lexicographical_compare_(lhs, rhs);
	}

	friend std::ostream& operator<<(std::ostream& os, const list& other)
	{
		os << "{";
		bool first = true;
		for (iterator it = other.begin(); it != other.end(); it++)
		{
			if (!first)
			{
				os << ", ";
			}
			os << *it;
			first = false;
		}
		os << "}";
		return os;
	}

	iterator insert(const_iterator pos, const T& value)
	{
		node* curr = pos.current;
		node* prev = pos.current->prev_node_;
		node* new_node = new node(prev, value, curr);
		prev->next_node_ = new_node;
		curr->prev_node_ = new_node;
		size_++;
		return iterator(new_node);
	}

   private:
	static bool lexicographical_compare_(const list<T>& l, const list<T>& r)
	{
		iterator it1 = l.begin();
		iterator it2 = r.begin();
		while (it1 != l.end() && it2 != r.end())
		{
			if (*it1 < *it2)
			{
				return true;
			}
			else if (*it1 > *it2)
			{
				return false;
			}
			it1++;
			it2++;
		}
		return (l.size() < r.size()) ? true : false;
	}
	size_t size_ = 0;
	node* tail_ = nullptr;
	node* head_ = nullptr;
};
}  // namespace bmstu

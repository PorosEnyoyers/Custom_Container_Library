#pragma once
#include <vector>
#include <array>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <exception>
#include <utility>
#include <algorithm>
#include <variant>
#include <concepts>
#include <memory>
#include <type_traits>
namespace custom
{
	template<typename T>
	concept Heapable = std::destructible<T> && requires(const T& a, const T& b) {
		{ a < b } -> std::convertible_to<bool>;
		{ a == b } -> std::convertible_to<bool>;
	};

	template<Heapable T>
	class heap_element_wrapper
	{
	public:
		static bool is_direct = sizeof(T) <= 64 || (sizeof(T) <= 128 && std::is_trivially_copyable_v<T>);
		using wrapped_type = std::conditional_t<is_direct, T, std::unique_ptr<T>>;
		//constructors
		heap_element_wrapper() = delete;
		heap_element_wrapper(const T& value)
			: m_data{copy_data(value)}
		{

		}
		heap_element_wrapper(T&& value)
			: m_data{move_data(std::move(value))}
		{
	
		}
		~heap_element_wrapper() = default;
		heap_element_wrapper(heap_element_wrapper&& other) noexcept = default;
		heap_element_wrapper& operator=(heap_element_wrapper&& other) noexcept = default;
		//custom copy constructor and copy assigment for unique pointer
		heap_element_wrapper(const heap_element_wrapper& other)
			: m_data{copy_wrapped(other)}
		{
		}
		heap_element_wrapper& operator=(const heap_element_wrapper& other)
		{
			if (this == &other)
				return *this;
			if constexpr (is_direct)
			{
				m_data = other.m_data;
			}
			else
			{
				this->m_data = std::make_unique<T>(*(other.m_data));
			}
			return *this;
		}
		//access the wrapped data
		T& value()
		{
			if constexpr (is_direct)
				return m_data;
			else
				return *m_data;
		}
		const T& value() const
		{
			if constexpr (is_direct)
				return m_data;
			else
				return *m_data;
		}
		//overloaded comparison
		friend bool operator<(const heap_element_wrapper& a, const heap_element_wrapper& b)
		{
			return a.value() < b.value();
		}
		friend bool operator>(const heap_element_wrapper& a, const heap_element_wrapper& b)
		{
			return b < a;
		}
		friend bool operator<=(const heap_element_wrapper& a, const heap_element_wrapper& b)
		{
			return !(b < a);
		}
		friend bool operator>=(const heap_element_wrapper& a, const heap_element_wrapper& b)
		{
			return !(a < b);
		}
		friend bool operator==(const heap_element_wrapper& a, const heap_element_wrapper& b)
		{
			return a.value() == b.value();
		}
		friend bool operator!=(const heap_element_wrapper& a, const heap_element_wrapper& b)
		{
			return !(a == b);
		}
	private:
		
		static wrapped_type copy_data(const T& value)
		{
			if constexpr (is_direct)
				return value;
			else
				return std::make_unique<T>(value);
		}
		static wrapped_type move_data(T&& value)
		{
			if constexpr (is_direct)
				return std::move(value);
			else
				return std::make_unique<T>(std::move(value));
		}
		static wrapped_type copy_wrapped(const heap_element_wrapper& other)
		{
			if constexpr (is_direct)
				return other.m_data;
			else
				return std::make_unique<T>(*other.m_data);
		}

		wrapped_type m_data;
	};

	enum class heap_type
	{
		MAX,
		MIN,
	};

	template<heap_type TYPE = heap_type::MAX, Heapable T, std::size_t stack_cap = 1000>
	class binary_heap
	{
	public:
		//Declare alias
		using value_type = T;
		using size_type = std::size_t;
		using reference = value_type&;
		using const_reference = value_type const&;
		using pointer = value_type*;
		using const_pointer = value_type const*;
		using Element = heap_element_wrapper<value_type>;
		using Array_Storage = std::array<std::optional<Element>, stack_cap>;
		using Vector_Storage = std::vector<Element>;
		//Constructor
		binary_heap() : m_storage{ Array_Storage{} }, m_size{ 0 } {}
		binary_heap(T value) : m_storage{ Array_Storage{} }, m_size{ 0 }
		{
			this->push(std::move(value));
		}
		//Push function
		void push(T value)
		{
			bool is_array = std::holds_alternative<Array_Storage>(m_storage);
			if (is_array && m_size == stack_cap)
			{
				promote_to_vector();
				is_array = false;
			}
			if (is_array)
			{
				auto& arr = std::get<Array_Storage>(m_storage);
				arr[m_size] = Element(std::move(value));
			}
			else
			{
				auto& arr = std::get<Vector_Storage>(m_storage);
				arr.push_back(std::move(value));
			}
			heapify_up(m_size, is_array);
			++m_size;
		}
	private:
		void promote_to_vector()
		{
			auto& arr = std::get<Array_Storage>(m_storage);
			Vector_Storage v;
			v.reserve(stack_cap * 2);
			for (auto& i : arr)
			{
				v.push_back(std::move(*i));
			}
			m_storage = std::move(v);
		}
		bool compare_element(const Element& a, const Element& b)
		{
			if constexpr (TYPE == heap_type::MAX)
			{
				return a > b;
			}
			else
			{
				return a < b;
			}
		}
		void heapify_up(size_type index, bool is_array)
		{
			if (is_array)
			{
				auto& arr = std::get<Array_Storage>(m_storage);
				auto value = arr[index];
				while (index > 0 && compare_element(value, arr[(index - 1) / 2]))
				{
					arr[index] = arr[(index - 1) / 2];
					index = (index - 1) / 2;
				}
				arr[index] = value;
			}
			else
			{
				auto& vec = std::get<Vector_Storage>(m_storage);
				auto value = vec[index];
				while (index > 0 && compare_element(value, vec[(index - 1) / 2]))
				{
					vec[index] = vec[(index - 1) / 2];
					index = (index - 1) / 2;
				}
				vec[index] = value;
			}
		}

		std::variant<Array_Storage, Vector_Storage> m_storage;
		size_type m_size;
	};
}
#pragma once

#include <array>
#include <algorithm>
#include <cstddef>

/*
 * The EnumArrayWrapper is a custom C++ template class designed to provide a type-safe and syntactically clean way 
 * to use a strong enum class as an index for a fixed-size array. 
 * It is an encapsulation built around the standard library container std::array.
*/
template <typename T, typename EnumType>
class EnumArray {
    // Calculate the size by converting the MAX value
    static constexpr std::size_t SIZE = static_cast<std::size_t>(EnumType::COUNT);
    std::array<T, SIZE> data_;

public:
    EnumArray() = default;

    // Overloading the access operator [] which takes the class enum as index
    T& operator[](EnumType index) {
        // Conversion with static_cast integrated with the call
        return data_[static_cast<std::size_t>(index)];
    }

    const T& operator[](EnumType index) const {
        return data_[static_cast<std::size_t>(index)];
    }

    
    void fill(const T& value) {
        data_.fill(value); // Use fill method from std::array
    }
    
    // STL compatibility
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.cbegin(); }
    auto end() const { return data_.cend(); }
    constexpr std::size_t size() const { return SIZE; }
};
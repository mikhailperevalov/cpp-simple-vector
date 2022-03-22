#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <stdexcept>

#include "array_ptr.h"

class ReserveProxyObj{
public:
    ReserveProxyObj(size_t capacity) :capacity_(capacity)
    {}
    size_t capacity_ = 0;
};

ReserveProxyObj Reserve(size_t capacity){
    return ReserveProxyObj(capacity);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        ArrayPtr<Type> temp(size);

        std::fill(temp.Get(), temp.Get() + size, Type());
        vector_.swap(temp);

        size_ = size;
        capacity_ = size;
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        ArrayPtr<Type> temp(size);

        std::fill(temp.Get(), temp.Get() + size, value);
        vector_.swap(temp);

        size_ = size;
        capacity_ = size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        ArrayPtr<Type> temp(init.size());
        int counter = 0;

        for(auto& element : init){
            temp[counter] = element;
            ++counter;
        }
        vector_.swap(temp);

        size_ = init.size();
        capacity_ = init.size();
    }
    
    SimpleVector(ReserveProxyObj reserve) {
        Reserve(reserve.capacity_);
    };
        
    SimpleVector(const SimpleVector& other) {
        SimpleVector temp(other.GetSize());

        int counter = 0;
        for(auto& element : other){
            temp[counter] = element;
            ++counter;
        }
        swap(temp);
    }
    
    SimpleVector(SimpleVector&& other) {
        vector_ = std::move(other.vector_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            vector_ = std::move(rhs.vector_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }
    
    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size_){
            throw std::out_of_range("out_of_range");
        }
        return vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if(index >= size_){
            throw std::out_of_range("out_of_range");
        }
        return vector_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость--------------------------------------------------------------
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if(new_size <= size_){
            size_ = new_size;
            return;
        }

        if(new_size <= capacity_){
            for (auto it = (begin() + size_); it != (begin() + new_size); ++it) {
                 *it = Type();
            }
            size_ = new_size;
            return;
        }

        if(new_size > capacity_){
            size_t new_capacity = capacity_ * 2 > new_size ? capacity_ * 2 : new_size;
            ArrayPtr<Type> temp(new_capacity);

            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.Get());
            vector_.swap(temp);

            size_ = new_size;
            capacity_ = new_capacity;
        }
    }
    
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора  
    void PushBack(const Type& item) {
        size_t temp = size_;
        if(size_ == capacity_){
            Resize(size_ + 1);
        } else {
            ++size_;
        }
        vector_[temp] = item;
    }
    
    void PushBack(Type&& item) {
        size_t temp = size_;
        if(size_ == capacity_){
            Resize(size_ + 1);
        } else {
            ++size_;
        }
        vector_[temp] = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        int index = std::distance(cbegin(), pos);
        if(size_ == 0){
            PushBack(value);
        } else if(size_ == capacity_){
            Resize(size_ + 1);
        } else if(size_ < capacity_){
            ++size_;
        }

        std::copy_backward(begin() + index, end() - 1, end());
        vector_[index] = value;
        return begin() + index;
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
        int index = std::distance(cbegin(), pos);
        if(size_ == 0){
            PushBack(std::move(value));
        } else if(size_ == capacity_){
            Resize(size_ + 1);
        } else if(size_ < capacity_){
            ++size_;
        }

        std::copy_backward(std::make_move_iterator(begin() + index), std::make_move_iterator(end() - 1), end());
        vector_[index] = std::move(value);
        return begin() + index;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        size_ -= size_ > 0 ? 1 : 0;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if (pos == (end() - 1)) {
            PopBack();
            return end();
        }
        int index = std::distance(cbegin(), pos);
        std::copy(std::make_move_iterator(begin() + index + 1), std::make_move_iterator(end()), begin() + index);
        --size_;
        return begin() + index;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        vector_.swap(other.vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    
    void Reserve(size_t new_capacity) {
        if(new_capacity > capacity_){
            ArrayPtr<Type> temp(new_capacity);

            std::copy(begin(), end(), temp.Get());
            vector_.swap(temp);
            capacity_ = new_capacity;
        }
    }
    
    // Возвращает итератор на начало массива----------------------------------------------------------------------------
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return Iterator{vector_.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return Iterator{vector_.Get() + size_};
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return Iterator{vector_.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return Iterator{vector_.Get() + size_};
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return end();
    }
private: 
    ArrayPtr<Type> vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if(lhs.GetSize() != rhs.GetSize()){
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs <= lhs;
}

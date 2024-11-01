/*
 * std::vector реализация от Малкова Максима (НИУ ВШЭ ФКН, программа ПМИ, группа
 * БПМИ231)
 * Версия стандарта C++ 20, список методов и их описание взято с
 * cppreference.com
 */

#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace my_vector {

template <class T, class Allocator = std::allocator<T>>
class vector {
   public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<Allocator>::pointer;
    using const_pointer = std::allocator_traits<Allocator>::const_pointer;

    class Iterator {
       public:
        using iterator_category = std::contiguous_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        constexpr Iterator() = default;
        constexpr explicit Iterator(pointer ptr) : ptr_(ptr) {}

        constexpr reference operator*() const { return *ptr_; }
        constexpr pointer operator->() const { return ptr_; }

        constexpr Iterator& operator++() {
            ++ptr_;
            return *this;
        }

        constexpr Iterator operator++(int) {
            Iterator old(ptr_);
            ++ptr_;
            return old;
        }

        constexpr Iterator& operator--() {
            --ptr_;
            return *this;
        }

        constexpr Iterator operator--() const { return Iterator(ptr_ - 1); }

        constexpr Iterator operator--(int) {
            Iterator old(ptr_);
            --ptr_;
            return old;
        }

        constexpr Iterator& operator+=(difference_type n) {
            ptr_ += n;
            return *this;
        }

        constexpr Iterator& operator-=(difference_type n) {
            ptr_ -= n;
            return *this;
        }

        constexpr Iterator operator+(difference_type n) const {
            return iterator(ptr_ + n);
        }

        constexpr Iterator operator-(difference_type n) const {
            return iterator(ptr_ - n);
        }

        constexpr bool operator==(const Iterator& other) const {
            return ptr_ == other.ptr_;
        }

        constexpr bool operator!=(const Iterator& other) const {
            return ptr_ != other.ptr_;
        }

        constexpr auto operator<=>(const Iterator& other) const {
            return ptr_ <=> other.ptr_;
        }

        constexpr reference operator[](difference_type n) {
            return *(ptr_ + n);
        }

        constexpr reference operator[](difference_type n) const {
            return *(ptr_ + n);
        }

        constexpr difference_type operator-(const Iterator& other) const {
            return ptr_ - other.ptr_;
        }

        friend constexpr Iterator operator+(difference_type n,
                                            const Iterator& it) {
            return Iterator(it.ptr_ + n);
        }

        constexpr pointer base() const { return ptr_; }

       private:
        pointer ptr_;
    };

    using iterator = Iterator;
    using const_iterator = const Iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // cppreference #1
    constexpr vector() noexcept(noexcept(Allocator()))
        : data_{nullptr, Allocator()} {}

    // cppreference #2
    constexpr explicit vector(const Allocator& allocator) noexcept
        : data_(nullptr, allocator) {}

    // cppreference #3
    constexpr vector(size_t count, const T& value,
                     const Allocator& allocator = Allocator())
        : capacity_(count), size_(count), data_(nullptr, allocator) {
        std::get<0>(data_) = std::allocator_traits<allocator_type>::allocate(
            std::get<1>(data_), capacity_);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + i, value);
        }
    }

    // cppreference #4
    vector(size_t count, const Allocator& allocator = Allocator())
        : capacity_(count), size_(count), data_(nullptr, allocator) {
        std::get<0>(data_) = std::allocator_traits<allocator_type>::allocate(
            std::get<1>(data_), capacity_);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
    }

    // cppreference #5
    template <std::input_iterator InputIt>
    constexpr vector(InputIt first, InputIt last,
                     const Allocator& allocator = Allocator())
        : data_(nullptr, allocator) {
        if constexpr (std::forward_iterator<InputIt> or
                      std::bidirectional_iterator<InputIt> or
                      std::random_access_iterator<InputIt>) {
            capacity_ = std::distance(first, last);
            size_ = capacity_;

            std::get<0>(data_) =
                std::allocator_traits<allocator_type>::allocate(
                    std::get<1>(data_), capacity_);
            InputIt it = first;
            for (size_t i = 0; i < size_; ++i, ++it) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i, *it);
            }
        } else {
            capacity_ = 0;
            size_ = 0;
            for (InputIt it = first; it != last; ++it) {
                if constexpr (std::is_integral_v<InputIt>) {
                    push_back(it);
                } else {
                    push_back(*it);
                }
            }
        }
    }

    // cppreference #6
    constexpr vector(const vector& other)
        : capacity_(other.capacity_),
          size_(other.size_),
          data_(nullptr, std::allocator_traits<Allocator>::
                             select_on_container_copy_construction(
                                 other.get_allocator())) {
        std::get<0>(data_) = std::allocator_traits<allocator_type>::allocate(
            std::get<1>(data_), capacity_);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + i, other[i]);
        }
    }

    // cppreference #7
    constexpr vector(const vector& other, const Allocator& allocator)
        : capacity_(other.capacity_),
          size_(other.size_),
          data_(nullptr, allocator) {
        std::get<0>(data_) = std::allocator_traits<allocator_type>::allocate(
            std::get<1>(data_), capacity_);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + i, other[i]);
        }
    }

    // cppreference #8
    constexpr vector(vector&& other)
        : capacity_(std::move(other.capacity_)),
          size_(std::move(other.size_)),
          data_(std::move(other.data_)) {
        other.capacity_ = 0;
        other.size_ = 0;
        std::get<0>(other.data_) = nullptr;
    }

    // cppreference #9
    constexpr vector(vector&& other, const Allocator& allocator)
        : capacity_(std::move(other.capacity_)),
          size_(std::move(other.size_)),
          data_(nullptr, allocator) {
        if (allocator == other.get_allocator()) {
            std::get<0>(data_) = std::move(std::get<0>(other.data_));
        } else {
            std::get<0>(data_) =
                std::allocator_traits<allocator_type>::allocate(
                    std::get<1>(data_), capacity_);
            for (size_t i = 0; i < size_; ++i) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i,
                    std::move(other[i]));
            }
            other.deepClear();
        }
        other.capacity_ = 0;
        other.size_ = 0;
        std::get<0>(other.data_) = 0;
    }

    // cppreference #10
    constexpr vector(std::initializer_list<T> init,
                     const Allocator& allocator = Allocator())
        : capacity_(init.size()),
          size_(init.size()),
          data_(nullptr, allocator) {
        std::get<0>(data_) = std::allocator_traits<allocator_type>::allocate(
            std::get<1>(data_), capacity_);
        auto it = init.begin();
        for (size_t i = 0; i < size_; ++i, ++it) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + i, *it);
        }
    }

    constexpr ~vector() { deepClear(); }

    constexpr vector& operator=(const vector& other) {
        vector new_vector(other);
        swap(new_vector);
        return *this;
    }

    constexpr vector& operator=(vector&& other) noexcept(
        std::allocator_traits<
            Allocator>::propagate_on_container_move_assignment::value or
        std::allocator_traits<Allocator>::is_always_equal::value) {
        if constexpr (std::allocator_traits<allocator_type>::
                          propagate_on_container_move_assignment::value) {
            vector new_vector(std::move(other), other.get_allocator());
            swap(new_vector);
        } else {
            if (get_allocator() != other.get_allocator()) {
                vector new_vector(std::move(other), get_allocator());
                swap(new_vector);
            } else {
                vector new_vector(std::move(other));
                swap(new_vector);
            }
        }
        return *this;
    }

    constexpr vector& operator=(std::initializer_list<T> init) {
        vector new_vector(init, std::get<1>(data_));
        swap(new_vector);
        return *this;
    }

    constexpr void assign(size_type count, const T& value) {
        vector new_vector(count, value, std::get<1>(data_));
        swap(new_vector);
    }

    template <std::input_iterator InputIt>
    constexpr void assign(InputIt first, InputIt last) {
        vector new_vector(first, last, std::get<1>(data_));
        swap(new_vector);
    }

    constexpr void assign(std::initializer_list<T> init) {
        vector new_vector(init, std::get<1>(data_));
        swap(new_vector);
    }

    constexpr Allocator get_allocator() const { return std::get<1>(data_); }

    // =============================
    // Element access (cppreference)
    // =============================
    constexpr T& at(size_t position) {
        if (position >= size_) {
            throw std::out_of_range("Index is out of vector size");
        }
        return std::get<0>(data_)[position];
    }

    constexpr const T& at(size_t position) const {
        if (position >= size_) {
            throw std::out_of_range("Index is out of vector size");
        }
        return std::get<0>(data_)[position];
    }

    constexpr T& operator[](size_t position) {
        return std::get<0>(data_)[position];
    }

    constexpr const T& operator[](size_t position) const {
        return std::get<0>(data_)[position];
    }

    constexpr T& front() { return std::get<0>(data_)[0]; }

    constexpr const T& front() const { return std::get<0>(data_)[0]; }

    constexpr T& back() { return std::get<0>(data_)[size_ - 1]; }

    constexpr const T& back() const { return std::get<0>(data_)[size_ - 1]; }

    T* data() { return std::get<0>(data_); }

    const T* data() const { return std::get<0>(data_); }

    // ========================
    // Iterators (cppreference)
    // ========================

    constexpr iterator begin() noexcept { return iterator(std::get<0>(data_)); }

    constexpr const_iterator begin() const noexcept {
        return const_iterator(std::get<0>(data_));
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(std::get<0>(data_));
    }

    constexpr iterator end() noexcept {
        return iterator(std::get<0>(data_) + size_);
    }

    constexpr const_iterator end() const noexcept {
        return const_iterator(std::get<0>(data_) + size_);
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(std::get<0>(data_) + size_);
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // =======================
    // Capacity (cppreference)
    // =======================

    constexpr bool empty() const { return size_ == 0; }

    constexpr size_t size() const { return size_; }

    constexpr size_t max_size() const {
        return std::allocator_traits<allocator_type>::max_size(
                   std::get<1>(data_)) /
               sizeof(T);
    }

    constexpr void reserve(size_t new_capacity) {
        if (new_capacity > max_size()) {
            throw std::length_error("");
        }
        if (new_capacity > capacity_) {
            reallocate(new_capacity);
        }
    }

    constexpr size_t capacity() const { return capacity_; }

    constexpr void shrink_to_fit() {
        if (capacity_ > size_) {
            reallocate(size_);
        }
    }

    // ========================
    // Modifiers (cppreference)
    // ========================

    constexpr void clear() {
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
        size_ = 0;
    }

    constexpr iterator insert(const_iterator position, const T& value) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                reallocate(1);
            } else {
                reallocate(capacity_ * 2);
            }
        }
        size_type insert_index =
            std::distance(std::get<0>(data_), position.base());
        for (size_type i = size_ - 1; i >= insert_index; --i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + 1,
                    std::move(std::get<0>(data_)[i]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + 1,
                    std::get<0>(data_)[i]);
            }
            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
        std::allocator_traits<allocator_type>::construct(
            std::get<1>(data_), std::get<0>(data_) + insert_index, value);
        ++size_;
        return iterator(std::get<0>(data_) + insert_index);
    }

    constexpr iterator insert(const_iterator position, T&& value) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                reallocate(1);
            } else {
                reallocate(capacity_ * 2);
            }
        }
        size_type insert_index =
            std::distance(std::get<0>(data_), position.base());
        for (size_type i = size_ - 1; i >= insert_index; --i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + 1,
                    std::move(std::get<0>(data_)[i]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + 1,
                    std::get<0>(data_)[i]);
            }

            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }

        std::allocator_traits<allocator_type>::construct(
            std::get<1>(data_), std::get<0>(data_) + insert_index,
            std::move(value));
        ++size_;
        return iterator(std::get<0>(data_) + insert_index);
    }

    constexpr iterator insert(const_iterator position, size_type count,
                              const T& value) {
        if (count == 0) {
            return iterator(position.base());
        }
        if (size_ + count > capacity_) {
            if (capacity_ == 0) {
                reallocate(count);
            } else {
                reallocate(std::max(capacity_ * 2, size_ + count));
            }
        }
        size_type insert_index =
            std::distance(std::get<0>(data_), position.base());
        for (size_type i = size_ - 1; i >= insert_index; --i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + count,
                    std::move(std::get<0>(data_)[i]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + count,
                    std::get<0>(data_)[i]);
            }

            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
        for (size_type i = 0; i < count; ++i) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + insert_index + i,
                value);
        }
        size_ += count;
        return iterator(std::get<0>(data_) + insert_index);
    }

    template <class InputIt>
    constexpr iterator insert(const_iterator position, InputIt first,
                              InputIt last) {
        if (first == last) {
            return iterator(position.base());
        }
        size_type count = std::distance(first, last);
        if (size_ + count > capacity_) {
            if (capacity_ == 0) {
                reallocate(count);
            } else {
                reallocate(std::max(capacity_ * 2, size_ + count));
            }
        }
        size_type insert_index =
            std::distance(std::get<0>(data_), position.base());
        for (size_type i = size_ - 1; i >= insert_index; --i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + count,
                    std::move(std::get<0>(data_)[i]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + count,
                    std::get<0>(data_)[i]);
            }

            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
        InputIt it = first;
        for (size_type i = 0; i < count; ++i, ++it) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + insert_index + i, *it);
        }
        size_ += count;
        return iterator(std::get<0>(data_) + insert_index);
    }

    constexpr iterator insert(const_iterator position,
                              std::initializer_list<T> init) {
        if (init.empty()) {
            return iterator(position.base());
        }
        size_type count = init.size();
        if (size_ + count > capacity_) {
            if (capacity_ == 0) {
                reallocate(count);
            } else {
                reallocate(std::max(capacity_ * 2, size_ + count));
            }
        }
        size_type insert_index =
            std::distance(std::get<0>(data_), position.base());
        for (size_type i = size_ - 1; i >= insert_index; --i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + count,
                    std::move(std::get<0>(data_)[i]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + count,
                    std::get<0>(data_)[i]);
            }

            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
        auto it = init.begin();
        for (size_type i = 0; i < count; ++i, ++it) {
            std::allocator_traits<allocator_type>::construct(
                std::get<1>(data_), std::get<0>(data_) + insert_index + i, *it);
        }
        size_ += count;
        return iterator(std::get<0>(data_) + insert_index);
    }

    template <class... Args>
    constexpr iterator emplace(const_iterator position, Args&&... args) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                reallocate(1);
            } else {
                reallocate(capacity_ * 2);
            }
        }
        size_type insert_index =
            std::distance(std::get<0>(data_), position.base());
        for (size_type i = size_ - 1; i >= insert_index; --i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + 1,
                    std::move(std::get<0>(data_)[i]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i + 1,
                    std::get<0>(data_)[i]);
            }

            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
        std::allocator_traits<allocator_type>::construct(
            std::get<1>(data_), std::get<0>(data_) + insert_index,
            std::forward<Args>(args)...);
        ++size_;
        return iterator(std::get<0>(data_) + insert_index);
    }

    constexpr iterator erase(const_iterator position) {
        size_type erase_index =
            std::distance(std::get<0>(data_), position.base());
        std::allocator_traits<allocator_type>::destroy(
            std::get<1>(data_), std::get<0>(data_) + erase_index);
        --size_;
        for (size_type i = erase_index; i < size_; ++i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i,
                    std::move(std::get<0>(data_)[i + 1]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i,
                    std::get<0>(data_)[i + 1]);
            }

            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i + 1);
        }
        return iterator(std::get<0>(data_) + erase_index);
    }

    constexpr iterator erase(const_iterator first, const_iterator last) {
        if (first == last) {
            return iterator(last.base());
        }
        size_type erase_index = std::distance(std::get<0>(data_), first.base());
        size_type count = std::distance(first, last);
        for (size_type i = 0; i < count; ++i) {
            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + erase_index + i);
        }
        size_ -= count;
        for (size_type i = erase_index; i < size_; ++i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i,
                    std::move(std::get<0>(data_)[i + count]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), std::get<0>(data_) + i,
                    std::get<0>(data_)[i + count]);
            }

            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i + count);
        }
        return iterator(std::get<0>(data_) + erase_index);
    }

    constexpr void push_back(const T& value) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                reallocate(1);
            } else {
                reallocate(capacity_ * 2);
            }
        }

        std::allocator_traits<allocator_type>::construct(
            std::get<1>(data_), std::get<0>(data_) + size_, value);
        ++size_;
    }

    constexpr void push_back(T&& value) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                reallocate(1);
            } else {
                reallocate(capacity_ * 2);
            }
        }

        std::allocator_traits<allocator_type>::construct(
            std::get<1>(data_), std::get<0>(data_) + size_, std::move(value));
        ++size_;
    }

    template <class... Args>
    constexpr reference emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                reallocate(1);
            } else {
                reallocate(capacity_ * 2);
            }
        }

        std::allocator_traits<allocator_type>::construct(
            std::get<1>(data_), std::get<0>(data_) + size_,
            std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    constexpr void pop_back() {
        --size_;
        std::allocator_traits<allocator_type>::destroy(
            std::get<1>(data_), std::get<0>(data_) + size_);
    }

    constexpr void resize(size_type count) {
        if (size_ == count) {
            return;
        }
        if (size_ > count) {
            reallocate(count);
            size_ = count;
            return;
        }
        if (count > max_size()) {
            throw std::length_error("");
        }
        reallocate(count);
        while (size_ < count) {
            emplace_back();
        }
    }

    constexpr void resize(size_type count, const T& value) {
        if (size_ == count) {
            return;
        }
        if (size_ > count) {
            reallocate(count);
            size_ = count;
            return;
        }
        reallocate(count);
        while (size_ < count) {
            push_back(value);
        }
    }

    constexpr void swap(vector& other) noexcept(
        std::allocator_traits<Allocator>::propagate_on_container_swap::value or
        std::allocator_traits<Allocator>::is_always_equal::value) {
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(std::get<0>(data_), std::get<0>(other.data_));
        if constexpr (std::allocator_traits<
                          allocator_type>::propagate_on_container_swap::value) {
            using std::swap;
            swap(std::get<1>(data_), std::get<1>(other.data_));
        }
    }

   private:
    size_type capacity_{0};
    size_type size_{0};
    std::tuple<pointer, allocator_type>
        data_;  // возможно применение EOB для пустого Allocator

    void reallocate(size_t new_capacity) {
        pointer new_data_ptr = std::allocator_traits<allocator_type>::allocate(
            std::get<1>(data_), new_capacity);
        size_type new_size = std::min(size_, new_capacity);
        for (size_type i = 0; i < new_size; ++i) {
            if constexpr (std::is_move_constructible_v<value_type>) {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), new_data_ptr + i,
                    std::move(std::get<0>(data_)[i]));
            } else {
                std::allocator_traits<allocator_type>::construct(
                    std::get<1>(data_), new_data_ptr + i,
                    std::get<0>(data_)[i]);
            }
        }
        for (size_type i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::destroy(
                std::get<1>(data_), std::get<0>(data_) + i);
        }
        if (std::get<0>(data_) != nullptr) {
            std::allocator_traits<allocator_type>::deallocate(
                std::get<1>(data_), std::get<0>(data_), capacity_);
        }
        std::get<0>(data_) = new_data_ptr;
        capacity_ = new_capacity;
        size_ = new_size;
    }

    constexpr void deepClear() {
        clear();
        if (std::get<0>(data_) != nullptr) {
            std::allocator_traits<allocator_type>::deallocate(
                std::get<1>(data_), std::get<0>(data_), capacity_);
            std::get<0>(data_) = nullptr;
        }
        capacity_ = 0;
    }
};

template <class T, class Allocator>
bool operator==(const vector<T, Allocator>& lhs,
                const vector<T, Allocator>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    auto lit = lhs.begin();
    auto rit = rhs.begin();
    for (; lit != lhs.end(); ++lit, ++rit) {
        if (*lit != *rit) {
            return false;
        }
    }
    return true;
}

template <class T, class Allocator>
auto operator<=>(const vector<T, Allocator>& lhs,
                 const vector<T, Allocator>& rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
                                                  rhs.begin(), rhs.end());
}

template <class T, class Allocator, class U>
constexpr vector<T, Allocator>::size_type erase(vector<T, Allocator>& vec,
                                                const U& value) {
    auto it = std::remove(vec.begin(), vec.end(), value);
    auto r = vec.end() - it;
    vec.erase(it, vec.end());
    return r;
}

template <class T, class Allocator, class Pred>
constexpr vector<T, Allocator>::size_type erase_if(vector<T, Allocator>& vec,
                                                   Pred predicate) {
    auto it = std::remove_if(vec.begin(), vec.end(), predicate);
    auto r = vec.end() - it;
    vec.erase(it, vec.end());
    return r;
}

template <class InputIt,
          class Allocator = std::allocator<
              typename std::iterator_traits<InputIt>::value_type>>
vector(InputIt, InputIt, Allocator = Allocator())
    -> vector<typename std::iterator_traits<InputIt>::value_type, Allocator>;
}  // namespace my_vector

namespace std {
template <class T, class Allocator>
void swap(
    my_vector::vector<T, Allocator>& lhs,
    my_vector::vector<T, Allocator>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
}  // namespace std

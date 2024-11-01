#include "my_vector.h"
#define CATCH_CONFIG_MAIN

#include "catch/catch.hpp"

template <typename T>
class TestAllocator {
   public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    TestAllocator() = default;

    template <typename U>
    TestAllocator(const TestAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t) { ::operator delete(p); }

    template <typename U>
    struct rebind {
        using other = TestAllocator<U>;
    };
};

template <typename T, typename U>
bool operator==(const TestAllocator<T>&, const TestAllocator<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const TestAllocator<T>&, const TestAllocator<U>&) {
    return false;
}

TEST_CASE("Empty base optimization") {
    static_assert(sizeof(my_vector::vector<int>) == 3 * sizeof(int*));
}

TEST_CASE("Vector Constructors", "[vector][constructor]") {
    SECTION("Default Constructor") {
        constexpr my_vector::vector<int> v1;
        REQUIRE(v1.empty());
        REQUIRE(v1.size() == 0);
        REQUIRE(v1.capacity() == 0);
    }

    SECTION("Iterator Constructor") {
        int arr[] = {1, 2, 3, 4, 5};
        my_vector::vector<int> v1(std::begin(arr), std::end(arr));
        REQUIRE(v1.size() == 5);
        REQUIRE(std::equal(v1.begin(), v1.end(), arr));
    }

    SECTION("Copy Constructor") {
        my_vector::vector<int> v1{1, 2, 3};
        my_vector::vector<int> v2(v1);
        REQUIRE(v1 == v2);
    }
}

TEST_CASE("Vector Modification", "[vector][push][emplace]") {
    SECTION("Push Back") {
        my_vector::vector<int> v1;
        v1.push_back(10);
        REQUIRE(v1.size() == 1);
        REQUIRE(v1[0] == 10);
    }

    SECTION("Reserve and Capacity") {
        constexpr my_vector::vector<int> v1;
        constexpr auto initial_capacity = v1.capacity();

        REQUIRE(initial_capacity == 0);
        my_vector::vector<int> v2;
        v2.reserve(100);
        REQUIRE(v2.capacity() >= 100);
    }
}

TEST_CASE("Vector with Custom Allocator", "[vector][allocator]") {
    SECTION("Construct with Custom Allocator") {
        my_vector::vector<int, TestAllocator<int>> v1;
        v1.push_back(42);
        REQUIRE(v1.size() == 1);
        REQUIRE(v1[0] == 42);
    }

    SECTION("Copy with Custom Allocator") {
        my_vector::vector<int, TestAllocator<int>> v1{1, 2, 3};
        my_vector::vector<int, TestAllocator<int>> v2(v1);
        REQUIRE(v1 == v2);
    }
}

TEST_CASE("Vector Iterators", "[vector][iterator]") {
    SECTION("Basic Iterator Operations") {
        my_vector::vector<int> v1{1, 2, 3, 4, 5};
        auto it = v1.begin();
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
    }
}

TEST_CASE("Vector Assign and Resize", "[vector][assign][resize]") {
    SECTION("Assign with Count and Value") {
        my_vector::vector<int> v1;
        v1.assign(5, 10);
        REQUIRE(v1.size() == 5);
        for (auto& el : v1) {
            REQUIRE(el == 10);
        }
    }

    SECTION("Assign with Range") {
        int arr[] = {1, 2, 3, 4, 5};
        my_vector::vector<int> v1;
        v1.assign(std::begin(arr), std::end(arr));
        REQUIRE(v1.size() == 5);
        REQUIRE(std::equal(v1.begin(), v1.end(), arr));
    }

    SECTION("Resize Larger") {
        my_vector::vector<int> v1(3, 10);
        v1.resize(5, 42);
        REQUIRE(v1.size() == 5);
        REQUIRE(v1[3] == 42);
        REQUIRE(v1[4] == 42);
    }

    SECTION("Resize Smaller") {
        my_vector::vector<int> v1(5, 10);
        v1.resize(3);
        REQUIRE(v1.size() == 3);
    }
}

TEST_CASE("Vector Clear and Shrink to Fit", "[vector][clear][shrink_to_fit]") {
    SECTION("Clear") {
        my_vector::vector<int> v1{1, 2, 3};
        v1.clear();
        REQUIRE(v1.size() == 0);
        REQUIRE(v1.empty());
    }

    SECTION("Shrink to Fit") {
        my_vector::vector<int> v1;
        v1.reserve(100);
        v1.push_back(42);
        auto old_capacity = v1.capacity();
        v1.shrink_to_fit();
        REQUIRE(v1.capacity() < old_capacity);
    }
}

TEST_CASE("Vector Pop and Erase", "[vector][pop][erase]") {
    SECTION("Pop Back") {
        my_vector::vector<int> v1{1, 2, 3};
        v1.pop_back();
        REQUIRE(v1.size() == 2);
        REQUIRE(v1.back() == 2);
    }

    SECTION("Erase Single Element") {
        my_vector::vector<int> v1{1, 2, 3};
        v1.erase(v1.begin() + 1);
        REQUIRE(v1.size() == 2);
        REQUIRE(v1[0] == 1);
        REQUIRE(v1[1] == 3);
    }

    SECTION("Erase Range") {
        my_vector::vector<int> v1{1, 2, 3, 4, 5};
        v1.erase(v1.begin() + 1, v1.begin() + 4);
        REQUIRE(v1.size() == 2);
        REQUIRE(v1[0] == 1);
        REQUIRE(v1[1] == 5);
    }
}

TEST_CASE("Vector Move Constructor and Assignment", "[vector][move]") {
    SECTION("Move Constructor") {
        my_vector::vector<int> v1{1, 2, 3};
        my_vector::vector<int> v2(std::move(v1));
        REQUIRE(v2.size() == 3);
        REQUIRE(v2[0] == 1);
        REQUIRE(v2[1] == 2);
        REQUIRE(v2[2] == 3);
        REQUIRE(v1.empty());
    }

    SECTION("Move Assignment") {
        my_vector::vector<int> v1{1, 2, 3};
        my_vector::vector<int> v2;
        v2 = std::move(v1);
        REQUIRE(v2.size() == 3);
        REQUIRE(v2[0] == 1);
        REQUIRE(v2[1] == 2);
        REQUIRE(v2[2] == 3);
        REQUIRE(v1.empty());
    }
}

TEST_CASE("Vector Comparison Operators", "[vector][comparison]") {
    SECTION("Equality") {
        my_vector::vector<int> v1{1, 2, 3};
        my_vector::vector<int> v2{1, 2, 3};
        REQUIRE(v1 == v2);
    }

    SECTION("Inequality") {
        my_vector::vector<int> v1{1, 2, 3};
        my_vector::vector<int> v2{1, 2, 4};
        REQUIRE(v1 != v2);
    }
}

class TestObject {
   public:
    TestObject() : value_(0), id_(counter_++) {}
    TestObject(int v) : value_(v), id_(counter_++) {}
    TestObject(const TestObject& other)
        : value_(other.value_), id_(counter_++) {
        copy_count_++;
    }
    TestObject(TestObject&& other) noexcept
        : value_(other.value_), id_(other.id_) {
        move_count_++;
        other.value_ = 0;
    }
    ~TestObject() { destructor_count_++; }

    TestObject& operator=(const TestObject& other) {
        if (this != &other) {
            value_ = other.value_;
            copy_count_++;
        }
        return *this;
    }

    TestObject& operator=(TestObject&& other) noexcept {
        if (this != &other) {
            value_ = other.value_;
            id_ = other.id_;
            other.value_ = 0;
            move_count_++;
        }
        return *this;
    }

    bool operator==(const TestObject& other) const {
        return value_ == other.value_;
    }

    static void reset_counters() {
        counter_ = 0;
        copy_count_ = 0;
        move_count_ = 0;
        destructor_count_ = 0;
    }

    static size_t get_copy_count() { return copy_count_; }
    static size_t get_move_count() { return move_count_; }
    static size_t get_destructor_count() { return destructor_count_; }

    int value_;
    size_t id_;

   private:
    static size_t counter_;
    static size_t copy_count_;
    static size_t move_count_;
    static size_t destructor_count_;
};

size_t TestObject::counter_ = 0;
size_t TestObject::copy_count_ = 0;
size_t TestObject::move_count_ = 0;
size_t TestObject::destructor_count_ = 0;

TEST_CASE("Vector Exception Safety", "[vector][exception]") {
    SECTION("Strong Exception Guarantee during push_back") {
        struct ThrowOnCopy {
            ThrowOnCopy() = default;
            ThrowOnCopy(const ThrowOnCopy&) {
                throw std::runtime_error("Copy constructor throw");
            }
            ThrowOnCopy& operator=(const ThrowOnCopy&) = delete;
        };

        my_vector::vector<ThrowOnCopy> v;
        v.reserve(5);
        ThrowOnCopy obj;

        REQUIRE_THROWS_AS(v.push_back(obj), std::runtime_error);
        REQUIRE(v.size() == 0);
        REQUIRE(v.capacity() == 5);
    }
}

TEST_CASE("Vector Memory Management", "[vector][memory]") {
    SECTION("Reallocation Behavior") {
        TestObject::reset_counters();
        my_vector::vector<TestObject> v;

        size_t last_capacity = v.capacity();
        for (int i = 0; i < 100; ++i) {
            v.push_back(TestObject(i));
            if (v.capacity() != last_capacity) {
                REQUIRE(v.capacity() >= last_capacity * 1.5);
                last_capacity = v.capacity();
            }
        }
    }

    SECTION("Move Operations During Reallocation") {
        TestObject::reset_counters();
        my_vector::vector<TestObject> v;
        v.reserve(5);

        for (int i = 0; i < 5; ++i) {
            v.emplace_back(i);
        }

        size_t moves_before = TestObject::get_move_count();
        v.push_back(TestObject(5));

        REQUIRE(TestObject::get_move_count() > moves_before);
    }
}

TEST_CASE("Vector Edge Cases", "[vector][edge]") {
    SECTION("Zero-sized Operations") {
        my_vector::vector<int> v;
        v.resize(0);
        REQUIRE(v.empty());

        v.reserve(0);
        REQUIRE(v.capacity() == 0);

        v.assign(0, 42);
        REQUIRE(v.empty());
    }

    SECTION("Large Size Operations") {
        my_vector::vector<char> v;
        size_t large_size = 1000000;

        REQUIRE_NOTHROW(v.reserve(large_size));
        REQUIRE(v.capacity() >= large_size);

        REQUIRE_NOTHROW(v.resize(large_size, 'a'));
        REQUIRE(v.size() == large_size);
        REQUIRE(v[large_size - 1] == 'a');
    }

    SECTION("Maximum Size Handling") {
        my_vector::vector<int> v;
        REQUIRE_THROWS_AS(v.reserve(v.max_size() + 1), std::length_error);
        REQUIRE_THROWS_AS(v.resize(v.max_size() + 1), std::length_error);
    }
}

TEST_CASE("Vector Iterator Invalidation", "[vector][iterator]") {
    SECTION("Iterator Invalidation After Reallocation") {
        my_vector::vector<int> v{1, 2, 3};
        auto original_capacity = v.capacity();
        auto it = v.begin();

        while (v.size() < original_capacity) {
            v.push_back(42);
        }
        v.push_back(42);

        bool iterator_was_invalidated = (it != v.begin());
        REQUIRE(iterator_was_invalidated);
    }
}

TEST_CASE("Vector Const Correctness", "[vector][const]") {
    SECTION("Const Vector Operations") {
        const my_vector::vector<int> v{1, 2, 3};

        REQUIRE(v.size() == 3);
        REQUIRE(v[0] == 1);
        REQUIRE(v.front() == 1);
        REQUIRE(v.back() == 3);

        auto it = v.cbegin();
        REQUIRE(*it == 1);

        auto rit = v.crbegin();
        REQUIRE(*rit == 3);
    }
}

TEST_CASE("Vector Self Operations", "[vector][self]") {
    SECTION("Self Assignment") {
        my_vector::vector<int> v{1, 2, 3};
        v = v;
        REQUIRE(v.size() == 3);
        REQUIRE(v[0] == 1);
        REQUIRE(v[1] == 2);
        REQUIRE(v[2] == 3);
    }

    SECTION("Self Move") {
        my_vector::vector<int> v{1, 2, 3};
        v = std::move(v);
        REQUIRE(v.size() == 3);
    }
}

TEST_CASE("Vector Type Requirements", "[vector][type]") {
    SECTION("Non-Default-Constructible Type") {
        struct NonDefaultConstructible {
            NonDefaultConstructible() = delete;
            explicit NonDefaultConstructible(int x) : value(x) {}
            int value;
        };

        my_vector::vector<NonDefaultConstructible> v;
        v.push_back(NonDefaultConstructible(42));
        REQUIRE(v.size() == 1);
        REQUIRE(v[0].value == 42);
    }

    SECTION("Non-Copy-Constructible Type") {
        struct NonCopyConstructible {
            NonCopyConstructible() = default;
            NonCopyConstructible(const NonCopyConstructible&) = delete;
            NonCopyConstructible(NonCopyConstructible&&) = default;
            NonCopyConstructible& operator=(const NonCopyConstructible&) =
                delete;
            NonCopyConstructible& operator=(NonCopyConstructible&&) = default;
        };

        my_vector::vector<NonCopyConstructible> v;
        v.emplace_back();
        v.push_back(NonCopyConstructible());
        REQUIRE(v.size() == 2);
    }
}

#include <iostream>
#include <cstring>
#include <vector>
#include <tuple>
#include <boost/type_index.hpp>


class C {
  public:
    C() = delete;

    static int counter;

    C(const char* str, int c) : size(strlen(str)), data(new char[size]), category(c) {
        id = counter;
        counter++;
        std::cout << id << " constructor\n";
        memcpy(data, str, size);
    }
    C(const char* str) : C(str, 0) {}
    C(std::string&& str, int c) : C(str.c_str(), c) {
        std::cout << id << " move\n";
    }
    C(std::string const& str, int c) : C(str.c_str(), c) {
        std::cout << id << " copy\n";
    }

    ~C() noexcept {
        std::cout << id << " destructor\n";
        if (data != nullptr) {
            delete[] data;
        }
    }

    C(C const& other)
        : size(other.size),
          data(other.data != nullptr ? new char[size] : nullptr), category(other.category) {
        id = counter;
        counter++;
        std::cout << id << " copy constructor\n";

        if (other.data != nullptr) {
            memcpy(data, other.data, size);
        }
    }
    C(C&& other) noexcept : size(other.size), data(other.data), category(other.category) {
        id = counter;
        counter++;
        std::cout << id << " move constructor\n";
        other.data = nullptr;
    }

    auto operator=(C const& other) -> C& {
        std::cout << "copy assignment\n";
        size = other.size;
        category = other.category;
        if (data != nullptr) {
            delete[] data;
        }
        data = new char[size];
        if (other.data != nullptr) {
            memcpy(data, other.data, size);
        }
        return *this;
    }
    auto operator=(C&& other) noexcept -> C& {
        std::cout << "move assignment\n";
        size = other.size;
        category = other.category;
        if (data != nullptr) {
            delete[] data;
        }
        data = other.data;
        other.data = nullptr;
        return *this;
    }

    std::size_t size{};
    char* data{};
    int id;
    int category;
};

int C::counter = 0;

template<typename T, typename... Args>
auto createVector(Args&&... args) {
    std::vector<T> v;
    v.reserve(sizeof...(Args));
    auto emplace = [&v](auto&&... args) -> typename decltype(v)::reference {
        return v.emplace_back(std::forward<decltype(args)>(args)...);
    };
    (std::apply(emplace, std::forward<Args>(args)), ...);

    return v;
}

int main(){
    auto s = std::string{"quux"};
    auto t = [](auto&&... args) {
        return std::forward_as_tuple(std::forward<decltype(args)>(args)...);
    };
    auto y = createVector<C>(
        t(std::string{"hello"}, 1),
        t("world"),
        t("foobar", 2),
        t(s, 3)
    );

    return 0;
}

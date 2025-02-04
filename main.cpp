#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iterator>

template<typename T>
class FlattenIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    FlattenIterator(std::vector<std::vector<T>>* container, size_t global_index)
        : container(container), global_index(global_index) {}

    reference operator*() {
        auto pos = locate(global_index);
        return (*container)[pos.first][pos.second];
    }

    pointer operator->() {
        return &(**this);
    }

    FlattenIterator& operator++() {
        ++global_index;
        return *this;
    }

    FlattenIterator operator++(int) {
        FlattenIterator temp = *this;
        ++(*this);
        return temp;
    }

    FlattenIterator& operator--() {
        --global_index;
        return *this;
    }

    FlattenIterator operator--(int) {
        FlattenIterator temp = *this;
        --(*this);
        return temp;
    }

    FlattenIterator operator+(difference_type n) const {
        return FlattenIterator(container, global_index + n);
    }

    FlattenIterator operator-(difference_type n) const {
        return FlattenIterator(container, global_index - n);
    }

    difference_type operator-(const FlattenIterator& other) const {
        return static_cast<difference_type>(global_index) - static_cast<difference_type>(other.global_index);
    }

    FlattenIterator& operator+=(difference_type n) {
        global_index += n;
        return *this;
    }

    FlattenIterator& operator-=(difference_type n) {
        global_index -= n;
        return *this;
    }

    reference operator[](difference_type n) {
        return *(*this + n);
    }

    bool operator==(const FlattenIterator& other) const {
        return container == other.container && global_index == other.global_index;
    }

    bool operator!=(const FlattenIterator& other) const {
        return !(*this == other);
    }

    bool operator<(const FlattenIterator& other) const {
        return global_index < other.global_index;
    }

    bool operator>(const FlattenIterator& other) const {
        return global_index > other.global_index;
    }

    bool operator<=(const FlattenIterator& other) const {
        return global_index <= other.global_index;
    }

    bool operator>=(const FlattenIterator& other) const {
        return global_index >= other.global_index;
    }

private:
    std::vector<std::vector<T>>* container;
    size_t global_index;

    std::pair<size_t, size_t> locate(size_t index) const {
        size_t outer = 0;
        while (outer < container->size()) {
            size_t inner_size = (*container)[outer].size();
            if (index < inner_size) {
                return {outer, index};
            }
            index -= inner_size;
            ++outer;
        }
        throw std::out_of_range("Index out of range");
    }
};

template<typename T>
class FlattenedVector {
public:
    using iterator = FlattenIterator<T>;
    using const_iterator = FlattenIterator<T>;

    FlattenedVector(std::vector<std::vector<T>>& data) : data(&data) {
        total_size = 0;
        for (const auto& row : data) {
            total_size += row.size();
        }
    }

    iterator begin() {
        return iterator(data, 0);
    }

    iterator end() {
        return iterator(data, total_size);
    }

    const_iterator begin() const {
        return const_iterator(data, 0);
    }

    const_iterator end() const {
        return const_iterator(data, total_size);
    }

    friend std::ostream& operator<<(std::ostream& os, const FlattenedVector<T>& fv) {
        for (size_t i = 0; i < fv.data->size(); ++i) {
            os << "[ ";
            if ((*fv.data)[i].empty()) {
                os << "<empty>";
            } else {
                for (size_t j = 0; j < (*fv.data)[i].size(); ++j) {
                    if (j > 0) {
                        os << ", ";
                    }
                    os << (*fv.data)[i][j];
                }
            }
            os << " ]" << std::endl;
        }
        return os;
    }

    void print_flat(std::ostream& os) const {
        os << "[ ";
        bool first = true;
        for (auto it = begin(); it != end(); ++it) {
            if (!first) {
                os << ", ";
            }
            os << *it;
            first = false;
        }
        os << " ]" << std::endl;
    }

private:
    std::vector<std::vector<T>>* data;
    size_t total_size;
};

int main() {
    std::vector<std::vector<int>> data = { {3, 1, 4}, {}, {1, 5, 9}, {2, 6, 5, 3}, {} };
    FlattenedVector<int> flat(data);

    int min_val = *std::min_element(flat.begin(), flat.end());
    int max_val = *std::max_element(flat.begin(), flat.end());

    std::cout << "Содержимое до сортировки (матричный вид):" << std::endl;
    std::cout << flat << std::endl;

    std::sort(flat.begin(), flat.end());

    std::cout << "Содержимое после сортировки (матричный вид):" << std::endl;
    std::cout << flat << std::endl;

    std::cout << "Плоское представление элементов:" << std::endl;
    flat.print_flat(std::cout);
    std::cout << std::endl;

    std::cout << "Минимальный элемент: " << min_val << std::endl;
    std::cout << "Максимальный элемент: " << max_val << std::endl;

    return 0;
}

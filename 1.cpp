#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iterator>
#include <algorithm>
#include <stdexcept>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }

        Type value{};
        Node* next_node = nullptr;
    };

public:
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_(node) {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto old_value = *this;
            ++(*this);
            return old_value;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() 
        : head_()
        , size_(0) {
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        if (values.size() == 0) {
            return;
        }

        auto it = values.begin();
        head_.next_node = new Node(*it, nullptr);
        Node* current = head_.next_node;
        ++it;
        ++size_;

        try {
            while (it != values.end()) {
                current->next_node = new Node(*it, nullptr);
                current = current->next_node;
                ++it;
                ++size_;
            }
        } catch (...) {
            Clear();
            throw;
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        if (other.head_.next_node == nullptr) {
            return;
        }
        
        head_.next_node = new Node(other.head_.next_node->value, nullptr);
        Node* current_dest = head_.next_node;
        Node* current_src = other.head_.next_node->next_node;
        
        try {
            while (current_src != nullptr) {
                current_dest->next_node = new Node(current_src->value, nullptr);
                current_dest = current_dest->next_node;
                current_src = current_src->next_node;
            }
        } catch (...) {
            Clear();
            throw;
        }
        size_ = other.size_;
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList copy(rhs);
            swap(copy);
        }
        return *this;
    }

    ~SingleLinkedList() {
        Clear();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void PopFront() noexcept {
        if (head_.next_node != nullptr) {
            Node* victim = head_.next_node;
            head_.next_node = victim->next_node;
            delete victim;
            --size_;
        }
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* victim = head_.next_node;
            head_.next_node = victim->next_node;
            delete victim;
        }
        size_ = 0;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* prev_node = pos.node_; 
        Node* new_node = new Node(value, prev_node->next_node);
        prev_node->next_node = new_node;
        ++size_;
        return Iterator(new_node);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* prev_node = pos.node_;
        Node* victim = prev_node->next_node;
        
        if (victim != nullptr) {
            prev_node->next_node = victim->next_node;
            delete victim;
            --size_;
            return Iterator(prev_node->next_node);
        }
        
        return Iterator(nullptr);
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    
    auto it_lhs = lhs.begin();
    auto it_rhs = rhs.begin();
    
    while (it_lhs != lhs.end()) {
        if (*it_lhs != *it_rhs) {
            return false;
        }
        ++it_lhs;
        ++it_rhs;
    }
    
    return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    auto it_lhs = lhs.begin();
    auto it_rhs = rhs.begin();
    
    while (it_lhs != lhs.end() && it_rhs != rhs.end()) {
        if (*it_lhs < *it_rhs) {
            return true;
        }
        if (*it_rhs < *it_lhs) {
            return false;
        }
        ++it_lhs;
        ++it_rhs;
    }
    
    return it_lhs == lhs.end() && it_rhs != rhs.end();
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
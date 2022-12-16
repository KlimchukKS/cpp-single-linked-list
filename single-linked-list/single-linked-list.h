#pragma once

#include <cassert>
#include <utility>
#include <iterator>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
                : value(val)
                , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) {
            node_ = node;
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {
            assert (this->node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }


        BasicIterator operator++(int) noexcept {
            auto old_value(*this);
            ++(*this);
            return old_value;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert (this->node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert (this->node_ != nullptr);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return  Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return  ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return  ConstIterator(nullptr);
    }

    SingleLinkedList()
        : size_(0) {}

    void Clear() noexcept {
        if(!head_.next_node){
            size_ = 0;
            last = nullptr;
            return;
        }
        Node* deleted_node = head_.next_node;
        head_.next_node = deleted_node->next_node;
        delete deleted_node;
        deleted_node = nullptr;
        Clear();
    }

    ~SingleLinkedList() {
        Clear();
    }

    void SetFirstNode(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        last = head_.next_node;
        ++size_;
    }

    void PushFront(const Type& value) {
        if (IsEmpty()) {
            SetFirstNode(value);
            return;
        }
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void PushBack(const Type& value) {
        if (IsEmpty()) {
            SetFirstNode(value);
            return;
        }
        last->next_node = new Node(value, nullptr);
        last = last->next_node;
        ++size_;
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return !size_;
    }

    template<typename ContainerType>
    void SetSingleLinkedList(ContainerType& container){
        for(auto it : container) {
            this->PushBack(it);
        }
    }
    SingleLinkedList(std::initializer_list<Type> values) {
        SetSingleLinkedList(values);
    }
    
    SingleLinkedList(const SingleLinkedList& other) {
        SetSingleLinkedList(other);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        SingleLinkedList tmp(rhs);
        swap(tmp);
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(this->head_.next_node, other.head_.next_node);
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{&head_};
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator{new_node};
    }

    void PopFront() noexcept {
        assert (head_.next_node != nullptr);
        Node* tmp = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = tmp;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert (pos.node_->next_node != nullptr);
        Node* tmp = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = tmp;

        return Iterator{pos.node_->next_node};
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    // указатель на последний узел
    Node* last;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return ((lhs < rhs) || (lhs == rhs));
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs <= lhs;
}

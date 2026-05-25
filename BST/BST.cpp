#include "BST.h"
#include <iostream>
#include <algorithm>
static void* global_tree_ptr = nullptr;




BinarySearchTree::Node::Node(Key key, Value value, Node* par, Node* l, Node* r)
    : keyValuePair(key, value), parent(par), left(l), right(r) {}

BinarySearchTree::Node::Node(const Node& src)
    : keyValuePair(src.keyValuePair), parent(nullptr), left(nullptr), right(nullptr) {
    if (src.left) {
        left = new Node(*src.left);
        left->parent = this;
    }
    if (src.right) {
        right = new Node(*src.right);
        right->parent = this;
    }
}

bool BinarySearchTree::Node::operator==(const Node& rhs) const {
    return keyValuePair == rhs.keyValuePair;
}

void BinarySearchTree::Node::output_node_tree() const {
    std::cout << "(" << keyValuePair.first << ", " << keyValuePair.second << ")";
    bool L = (left != 0);
    bool R = (right != 0);
    if (L || R) {
        std::cout << " [";
        if (L) left->output_node_tree();
        else std::cout << "null";
        std::cout << ", ";
        if (R) right->output_node_tree();
        else std::cout << "null";
        std::cout << "]";
    }
}

void BinarySearchTree::Node::insert(const Key& k, const Value& v) {
    if (k < keyValuePair.first) {
        if (left) left->insert(k, v);
        else left = new Node(k, v, this);
    } else {
        if (right) right->insert(k, v);
        else right = new Node(k, v, this);
    }
}

void BinarySearchTree::Node::erase(const Key&) {}



BinarySearchTree::BinarySearchTree(const BinarySearchTree& src)
    : _size(src._size), _root(nullptr) {
    if (src._root) _root = new Node(*src._root);
}

BinarySearchTree& BinarySearchTree::operator=(const BinarySearchTree& src) {
    if (this == &src) return *this;
    
    while (_root) erase(_root->keyValuePair.first);
    
    _root = src._root ? new Node(*src._root) : nullptr;
    _size = src._size;
    return *this;
}

BinarySearchTree::BinarySearchTree(BinarySearchTree&& src) noexcept
    : _size(src._size), _root(src._root) {
    src._root = nullptr;
    src._size = 0;
}

BinarySearchTree& BinarySearchTree::operator=(BinarySearchTree&& src) noexcept {
    if (this == &src) return *this;
    
    while (_root) erase(_root->keyValuePair.first);
    
    _root = src._root;
    _size = src._size;
    src._root = nullptr;
    src._size = 0;
    return *this;
}

BinarySearchTree::~BinarySearchTree() {
    while (_root) erase(_root->keyValuePair.first);
}

void BinarySearchTree::insert(const Key& k, const Value& v) {
    if (!_root) _root = new Node(k, v);
    else _root->insert(k, v);
    ++_size;
}

void BinarySearchTree::erase(const Key& k) {
    while (true) {
        Node* cur = _root;
        Node* target = nullptr;
        
        while (cur) {
            if (k < cur->keyValuePair.first) cur = cur->left;
            else if (k > cur->keyValuePair.first) cur = cur->right;
            else { target = cur; break; }
        }
        
        if (!target) break;
        cur = target;
        
        
        if (!cur->left && !cur->right) {
            if (cur == _root) _root = nullptr;
            else if (cur->parent->left == cur) cur->parent->left = nullptr;
            else cur->parent->right = nullptr;
            delete cur;
        }
        
        else if (cur->left && !cur->right) {
            Node* kid = cur->left;
            if (cur == _root) {
                _root = kid;
                kid->parent = nullptr;
            } else {
                if (cur->parent->left == cur) cur->parent->left = kid;
                else cur->parent->right = kid;
                kid->parent = cur->parent;
            }
            cur->left = nullptr;
            delete cur;
        }
    
        else if (!cur->left && cur->right) {
            Node* kid = cur->right;
            if (cur == _root) {
                _root = kid;
                kid->parent = nullptr;
            } else {
                if (cur->parent->left == cur) cur->parent->left = kid;
                else cur->parent->right = kid;
                kid->parent = cur->parent;
            }
            cur->right = nullptr;
            delete cur;
        }
        
        else {
            Node* nxt = cur->right;
            while (nxt->left) nxt = nxt->left;
            
            cur->keyValuePair = nxt->keyValuePair;
            
            if (nxt->parent->left == nxt) nxt->parent->left = nxt->right;
            else nxt->parent->right = nxt->right;
            
            if (nxt->right) nxt->right->parent = nxt->parent;
            
            delete nxt;
        }
        --_size;
    }
}

BinarySearchTree::ConstIterator BinarySearchTree::find(const Key& k) const {
    const Node* cur = _root;
    const Node* ans = nullptr;
    while (cur) {
        if (k < cur->keyValuePair.first) cur = cur->left;
        else if (k > cur->keyValuePair.first) cur = cur->right;
        else { ans = cur; cur = cur->left; }
    }
    return ans ? ConstIterator(ans) : cend();
}

BinarySearchTree::Iterator BinarySearchTree::find(const Key& k) {
    Node* cur = _root;
    Node* ans = nullptr;
    while (cur) {
        if (k < cur->keyValuePair.first) cur = cur->left;
        else if (k > cur->keyValuePair.first) cur = cur->right;
        else { ans = cur; cur = cur->left; }
    }
    return ans ? Iterator(ans) : end();
}

std::pair<BinarySearchTree::Iterator, BinarySearchTree::Iterator>
BinarySearchTree::equalRange(const Key& k) {
    auto L = find(k);
    if (L == end()) return {end(), end()};
    auto R = L;
    while (R != end() && R->first == k) ++R;
    return {L, R};
}

std::pair<BinarySearchTree::ConstIterator, BinarySearchTree::ConstIterator>
BinarySearchTree::equalRange(const Key& k) const {
    auto L = find(k);
    if (L == cend()) return {cend(), cend()};
    auto R = L;
    while (R != cend() && R->first == k) ++R;
    return {L, R};
}

BinarySearchTree::ConstIterator BinarySearchTree::min() const {
    if (!_root) return cend();
    const Node* cur = _root;
    while (cur->left) cur = cur->left;
    return ConstIterator(cur);
}

BinarySearchTree::ConstIterator BinarySearchTree::max() const {
    if (!_root) return cend();
    const Node* cur = _root;
    while (cur->right) cur = cur->right;
    return ConstIterator(cur);
}

BinarySearchTree::ConstIterator BinarySearchTree::min(const Key& k) const {
    auto rg = equalRange(k);
    if (rg.first == cend()) return cend();
    auto best = rg.first;
    for (auto it = rg.first; it != rg.second; ++it) {
        if (it->second < best->second) best = it;
    }
    return best;
}

BinarySearchTree::ConstIterator BinarySearchTree::max(const Key& k) const {
    auto rg = equalRange(k);
    if (rg.first == cend()) return cend();
    auto best = rg.first;
    for (auto it = rg.first; it != rg.second; ++it) {
        if (it->second > best->second) best = it;
    }
    return best;
}

BinarySearchTree::Iterator BinarySearchTree::begin() {
    global_tree_ptr = static_cast<void*>(_root);
    if (!_root) return Iterator(nullptr);
    Node* cur = _root;
    while (cur->left) cur = cur->left;
    return Iterator(cur);
}

BinarySearchTree::Iterator BinarySearchTree::end() {
    global_tree_ptr = static_cast<void*>(_root);
    return Iterator(nullptr);
}

BinarySearchTree::ConstIterator BinarySearchTree::cbegin() const {
    global_tree_ptr = static_cast<void*>(_root);
    if (!_root) return ConstIterator(nullptr);
    const Node* cur = _root;
    while (cur->left) cur = cur->left;
    return ConstIterator(cur);
}

BinarySearchTree::ConstIterator BinarySearchTree::cend() const {
    global_tree_ptr = static_cast<void*>(_root);
    return ConstIterator(nullptr);
}

size_t BinarySearchTree::size() const { return _size; }

void BinarySearchTree::output_tree() {
    if (_root) _root->output_node_tree();
    else std::cout << "Empty tree";
    std::cout << std::endl;
}



BinarySearchTree::Iterator::Iterator(Node* ptr) : _node(ptr) {}

std::pair<Key, Value>& BinarySearchTree::Iterator::operator*() { return _node->keyValuePair; }
const std::pair<Key, Value>& BinarySearchTree::Iterator::operator*() const { return _node->keyValuePair; }
std::pair<Key, Value>* BinarySearchTree::Iterator::operator->() { return &_node->keyValuePair; }
const std::pair<Key, Value>* BinarySearchTree::Iterator::operator->() const { return &_node->keyValuePair; }

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++() {
    if (!_node) return *this;
    
    if (_node->right) {
        _node = _node->right;
        while (_node->left) _node = _node->left;
    } else {
        Node* p = _node->parent;
        while (p && _node == p->right) {
            _node = p;
            p = p->parent;
        }
        _node = p;
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(int) {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--() {
    if (!_node) {
        _node = static_cast<Node*>(global_tree_ptr);
        if (!_node) return *this;
        while (_node->right) _node = _node->right;
        return *this;
    }
    
    if (_node->left) {
        _node = _node->left;
        while (_node->right) _node = _node->right;
    } else {
        Node* p = _node->parent;
        while (p && _node == p->left) {
            _node = p;
            p = p->parent;
        }
        _node = p;
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(int) {
    Iterator tmp = *this;
    --(*this);
    return tmp;
}

bool BinarySearchTree::Iterator::operator==(const Iterator& rhs) const { return _node == rhs._node; }
bool BinarySearchTree::Iterator::operator!=(const Iterator& rhs) const { return _node != rhs._node; }



BinarySearchTree::ConstIterator::ConstIterator(const Node* ptr) : _node(ptr) {}

const std::pair<Key, Value>& BinarySearchTree::ConstIterator::operator*() const { return _node->keyValuePair; }
const std::pair<Key, Value>* BinarySearchTree::ConstIterator::operator->() const { return &_node->keyValuePair; }

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++() {
    if (!_node) return *this;
    
    if (_node->right) {
        _node = _node->right;
        while (_node->left) _node = _node->left;
    } else {
        const Node* p = _node->parent;
        while (p && _node == p->right) {
            _node = p;
            p = p->parent;
        }
        _node = p;
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++(int) {
    ConstIterator tmp = *this;
    ++(*this);
    return tmp;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--() {
    if (!_node) {
        _node = static_cast<const Node*>(global_tree_ptr);
        if (!_node) return *this;
        while (_node->right) _node = _node->right;
        return *this;
    }
    
    if (_node->left) {
        _node = _node->left;
        while (_node->right) _node = _node->right;
    } else {
        const Node* p = _node->parent;
        while (p && _node == p->left) {
            _node = p;
            p = p->parent;
        }
        _node = p;
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(int) {
    ConstIterator tmp = *this;
    --(*this);
    return tmp;
}

bool BinarySearchTree::ConstIterator::operator==(const ConstIterator& rhs) const { return _node == rhs._node; }
bool BinarySearchTree::ConstIterator::operator!=(const ConstIterator& rhs) const { return _node != rhs._node; }


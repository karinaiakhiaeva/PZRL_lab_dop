#include "BST.h"
#include <iostream>
#include <functional>
#include <algorithm>

static void* global_tree_ptr = nullptr;

// RED = true, BLACK = false
static const bool RED   = true;
static const bool BLACK = false;



BinarySearchTree::Node::Node(Key key, Value value, Node *parent, Node *left, Node *right)
    : keyValuePair(key, value), parent(parent), left(left), right(right), color(false) {}

BinarySearchTree::Node::Node(const Node &other)
    : keyValuePair(other.keyValuePair), parent(nullptr), left(nullptr), right(nullptr), color(other.color)
{
    if (other.left) {
        left = new Node(*other.left);
        left->parent = this;
    }
    if (other.right) {
        right = new Node(*other.right);
        right->parent = this;
    }
}

bool BinarySearchTree::Node::operator==(const Node &other) const
{
    return keyValuePair == other.keyValuePair;
}

void BinarySearchTree::Node::output_node_tree() const
{
    std::cout << "(" << keyValuePair.first << ", " << keyValuePair.second << ")";
    if (left || right) {
        std::cout << " [";
        if (left)  left->output_node_tree();
        else       std::cout << "null";
        std::cout << ", ";
        if (right) right->output_node_tree();
        else       std::cout << "null";
        std::cout << "]";
    }
}

void BinarySearchTree::Node::insert(const Key &, const Value &) {}
void BinarySearchTree::Node::erase(const Key &) {}



BinarySearchTree::BinarySearchTree(const BinarySearchTree &other)
    : _size(other._size), _root(nullptr)
{
    if (other._root)
        _root = new Node(*other._root);
}

BinarySearchTree &BinarySearchTree::operator=(const BinarySearchTree &other)
{
    if (this != &other) {
        while (_root) erase(_root->keyValuePair.first);
        if (other._root)
            _root = new Node(*other._root);
        else
            _root = nullptr;
        _size = other._size;
    }
    return *this;
}

BinarySearchTree::BinarySearchTree(BinarySearchTree &&other) noexcept
    : _size(other._size), _root(other._root)
{
    other._root = nullptr;
    other._size = 0;
}

BinarySearchTree &BinarySearchTree::operator=(BinarySearchTree &&other) noexcept
{
    if (this != &other) {
        while (_root) erase(_root->keyValuePair.first);
        _root = other._root;
        _size = other._size;
        other._root = nullptr;
        other._size = 0;
    }
    return *this;
}

BinarySearchTree::~BinarySearchTree()
{
    while (_root)
        erase(_root->keyValuePair.first);
}



void BinarySearchTree::insert(const Key &key, const Value &value)
{
    // --- лямбды для RB-вставки ---
    auto rotateLeft = [&](Node *x) {
        Node *y   = x->right;
        x->right  = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent)           _root = y;
        else if (x == x->parent->left) x->parent->left  = y;
        else                           x->parent->right = y;
        y->left   = x;
        x->parent = y;
    };

    auto rotateRight = [&](Node *y) {
        Node *x   = y->left;
        y->left   = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (!y->parent)           _root = x;
        else if (y == y->parent->left) y->parent->left  = x;
        else                           y->parent->right = x;
        x->right  = y;
        y->parent = x;
    };

    // Обычная BST-вставка
    Node *z   = new Node(key, value);
    z->color  = RED;
    Node *y   = nullptr;
    Node *x   = _root;
    while (x) {
        y = x;
        x = (key < x->keyValuePair.first) ? x->left : x->right;
    }
    z->parent = y;
    if (!y)                                 _root       = z;
    else if (key < y->keyValuePair.first)  y->left     = z;
    else                                    y->right    = z;

    // Fix-up
    Node *cur = z;
    while (cur->parent && cur->parent->color == RED) {
        Node *grandpa = cur->parent->parent;
        if (!grandpa) break;

        if (cur->parent == grandpa->left) {
            Node *uncle = grandpa->right;
            if (uncle && uncle->color == RED) {
                cur->parent->color = BLACK;
                uncle->color       = BLACK;
                grandpa->color     = RED;
                cur = grandpa;
            } else {
                if (cur == cur->parent->right) {
                    cur = cur->parent;
                    rotateLeft(cur);
                }
                cur->parent->color = BLACK;
                grandpa->color     = RED;
                rotateRight(grandpa);
            }
        } else {
            Node *uncle = grandpa->left;
            if (uncle && uncle->color == RED) {
                cur->parent->color = BLACK;
                uncle->color       = BLACK;
                grandpa->color     = RED;
                cur = grandpa;
            } else {
                if (cur == cur->parent->left) {
                    cur = cur->parent;
                    rotateRight(cur);
                }
                cur->parent->color = BLACK;
                grandpa->color     = RED;
                rotateLeft(grandpa);
            }
        }
    }
    _root->color = BLACK;
    ++_size;
}


void BinarySearchTree::erase(const Key &key)
{
    auto rotateLeft = [&](Node *x) {
        Node *y  = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent)            _root = y;
        else if (x == x->parent->left) x->parent->left  = y;
        else                           x->parent->right = y;
        y->left   = x;
        x->parent = y;
    };

    auto rotateRight = [&](Node *y) {
        Node *x  = y->left;
        y->left  = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (!y->parent)            _root = x;
        else if (y == y->parent->left) y->parent->left  = x;
        else                           y->parent->right = x;
        x->right  = y;
        y->parent = x;
    };

    auto transplant = [&](Node *u, Node *v) {
        if (!u->parent)            _root = v;
        else if (u == u->parent->left) u->parent->left  = v;
        else                           u->parent->right = v;
        if (v) v->parent = u->parent;
    };

    // Fix-up после удаления чёрного узла
    // x — замена удалённого, xParent — его родитель (нужен когда x == nullptr)
    auto fixup = [&](Node *x, Node *xParent) {
        while (x != _root && !(x && x->color == RED)) {
            if (x == (xParent ? xParent->left : nullptr)) {
                Node *w = xParent ? xParent->right : nullptr;
                if (w && w->color == RED) {
                    w->color       = BLACK;
                    xParent->color = RED;
                    rotateLeft(xParent);
                    w = xParent->right;
                }
                bool wLeftBlack  = !(w && w->left  && w->left->color  == RED);
                bool wRightBlack = !(w && w->right && w->right->color == RED);
                if (wLeftBlack && wRightBlack) {
                    if (w) w->color = RED;
                    x       = xParent;
                    xParent = x ? x->parent : nullptr;
                } else {
                    if (wRightBlack) {
                        if (w && w->left)  w->left->color = BLACK;
                        if (w) w->color = RED;
                        if (w) rotateRight(w);
                        w = xParent ? xParent->right : nullptr;
                    }
                    if (w) w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w && w->right) w->right->color = BLACK;
                    rotateLeft(xParent);
                    x = _root;
                    xParent = nullptr;
                }
            } else {
                Node *w = xParent ? xParent->left : nullptr;
                if (w && w->color == RED) {
                    w->color       = BLACK;
                    xParent->color = RED;
                    rotateRight(xParent);
                    w = xParent->left;
                }
                bool wRightBlack = !(w && w->right && w->right->color == RED);
                bool wLeftBlack  = !(w && w->left  && w->left->color  == RED);
                if (wLeftBlack && wRightBlack) {
                    if (w) w->color = RED;
                    x       = xParent;
                    xParent = x ? x->parent : nullptr;
                } else {
                    if (wLeftBlack) {
                        if (w && w->right) w->right->color = BLACK;
                        if (w) w->color = RED;
                        if (w) rotateLeft(w);
                        w = xParent ? xParent->left : nullptr;
                    }
                    if (w) w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w && w->left) w->left->color = BLACK;
                    rotateRight(xParent);
                    x = _root;
                    xParent = nullptr;
                }
            }
        }
        if (x) x->color = BLACK;
    };

    while (true) {
        // Найти самый левый (первый in-order) узел с ключом key
        Node *current = _root;
        Node *target  = nullptr;
        while (current) {
            if (key < current->keyValuePair.first)
                current = current->left;
            else if (key > current->keyValuePair.first)
                current = current->right;
            else {
                target  = current;
                current = current->left;
            }
        }
        if (!target) break;

        Node *z          = target;
        Node *y          = z;
        bool  yOrigColor = y->color;
        Node *x          = nullptr;
        Node *xParent    = nullptr;

        if (!z->left) {
            x       = z->right;
            xParent = z->parent;
            transplant(z, z->right);
        } else if (!z->right) {
            x       = z->left;
            xParent = z->parent;
            transplant(z, z->left);
        } else {
            // In-order successor — минимум правого поддерева
            y = z->right;
            while (y->left) y = y->left;
            yOrigColor = y->color;
            x          = y->right;

            if (y->parent == z) {
                xParent = y;
            } else {
                xParent = y->parent;
                transplant(y, y->right);
                y->right         = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left         = z->left;
            y->left->parent = y;
            y->color        = z->color;
        }

        delete z;
        --_size;

        if (yOrigColor == BLACK)
            fixup(x, xParent);
    }
}


BinarySearchTree::ConstIterator BinarySearchTree::find(const Key &key) const
{
    const Node *current = _root;
    const Node *result  = nullptr;
    while (current) {
        if (key < current->keyValuePair.first)
            current = current->left;
        else if (key > current->keyValuePair.first)
            current = current->right;
        else {
            result  = current;
            current = current->left;
        }
    }
    return result ? ConstIterator(result) : cend();
}

BinarySearchTree::Iterator BinarySearchTree::find(const Key &key)
{
    Node *current = _root;
    Node *result  = nullptr;
    while (current) {
        if (key < current->keyValuePair.first)
            current = current->left;
        else if (key > current->keyValuePair.first)
            current = current->right;
        else {
            result  = current;
            current = current->left;
        }
    }
    return result ? Iterator(result) : end();
}


std::pair<BinarySearchTree::Iterator, BinarySearchTree::Iterator>
BinarySearchTree::equalRange(const Key &key)
{
    auto first = find(key);
    if (first == end()) return {end(), end()};
    auto last = first;
    while (last != end() && last->first == key) ++last;
    return {first, last};
}

std::pair<BinarySearchTree::ConstIterator, BinarySearchTree::ConstIterator>
BinarySearchTree::equalRange(const Key &key) const
{
    auto first = find(key);
    if (first == cend()) return {cend(), cend()};
    auto last = first;
    while (last != cend() && last->first == key) ++last;
    return {first, last};
}


BinarySearchTree::ConstIterator BinarySearchTree::min() const
{
    if (!_root) return cend();
    const Node *cur = _root;
    while (cur->left) cur = cur->left;
    return ConstIterator(cur);
}

BinarySearchTree::ConstIterator BinarySearchTree::max() const
{
    if (!_root) return cend();
    const Node *cur = _root;
    while (cur->right) cur = cur->right;
    return ConstIterator(cur);
}

BinarySearchTree::ConstIterator BinarySearchTree::min(const Key &key) const
{
    auto range = equalRange(key);
    if (range.first == cend()) return cend();
    auto best = range.first;
    for (auto it = range.first; it != range.second; ++it)
        if (it->second < best->second) best = it;
    return best;
}

BinarySearchTree::ConstIterator BinarySearchTree::max(const Key &key) const
{
    auto range = equalRange(key);
    if (range.first == cend()) return cend();
    auto best = range.first;
    for (auto it = range.first; it != range.second; ++it)
        if (it->second > best->second) best = it;
    return best;
}


BinarySearchTree::Iterator BinarySearchTree::begin()
{
    global_tree_ptr = static_cast<void*>(_root);
    if (!_root) return Iterator(nullptr);
    Node *cur = _root;
    while (cur->left) cur = cur->left;
    return Iterator(cur);
}

BinarySearchTree::Iterator BinarySearchTree::end()
{
    global_tree_ptr = static_cast<void*>(_root);
    return Iterator(nullptr);
}

BinarySearchTree::ConstIterator BinarySearchTree::cbegin() const
{
    global_tree_ptr = static_cast<void*>(_root);
    if (!_root) return ConstIterator(nullptr);
    const Node *cur = _root;
    while (cur->left) cur = cur->left;
    return ConstIterator(cur);
}

BinarySearchTree::ConstIterator BinarySearchTree::cend() const
{
    global_tree_ptr = static_cast<void*>(_root);
    return ConstIterator(nullptr);
}


size_t BinarySearchTree::size() const { return _size; }

void BinarySearchTree::output_tree()
{
    if (_root) _root->output_node_tree();
    else       std::cout << "Empty tree";
    std::cout << std::endl;
}

size_t BinarySearchTree::max_height() const
{
    std::function<size_t(const Node*)> h = [&](const Node *n) -> size_t {
        if (!n) return 0;
        return 1 + std::max(h(n->left), h(n->right));
    };
    return h(_root);
}


BinarySearchTree::Iterator::Iterator(Node *node) : _node(node) {}

std::pair<Key, Value> &BinarySearchTree::Iterator::operator*()             { return _node->keyValuePair; }
const std::pair<Key, Value> &BinarySearchTree::Iterator::operator*() const { return _node->keyValuePair; }
std::pair<Key, Value> *BinarySearchTree::Iterator::operator->()            { return &_node->keyValuePair; }
const std::pair<Key, Value> *BinarySearchTree::Iterator::operator->() const{ return &_node->keyValuePair; }

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++()
{
    if (!_node) return *this;
    if (_node->right) {
        _node = _node->right;
        while (_node->left) _node = _node->left;
    } else {
        Node *p = _node->parent;
        while (p && _node == p->right) { _node = p; p = p->parent; }
        _node = p;
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(int)
{
    Iterator tmp = *this; ++(*this); return tmp;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--()
{
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
        Node *p = _node->parent;
        while (p && _node == p->left) { _node = p; p = p->parent; }
        _node = p;
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(int)
{
    Iterator tmp = *this; --(*this); return tmp;
}

bool BinarySearchTree::Iterator::operator==(const Iterator &other) const { return _node == other._node; }
bool BinarySearchTree::Iterator::operator!=(const Iterator &other) const { return _node != other._node; }


BinarySearchTree::ConstIterator::ConstIterator(const Node *node) : _node(node) {}

const std::pair<Key, Value> &BinarySearchTree::ConstIterator::operator*() const  { return _node->keyValuePair; }
const std::pair<Key, Value> *BinarySearchTree::ConstIterator::operator->() const { return &_node->keyValuePair; }

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++()
{
    if (!_node) return *this;
    if (_node->right) {
        _node = _node->right;
        while (_node->left) _node = _node->left;
    } else {
        const Node *p = _node->parent;
        while (p && _node == p->right) { _node = p; p = p->parent; }
        _node = p;
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++(int)
{
    ConstIterator tmp = *this; ++(*this); return tmp;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--()
{
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
        const Node *p = _node->parent;
        while (p && _node == p->left) { _node = p; p = p->parent; }
        _node = p;
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(int)
{
    ConstIterator tmp = *this; --(*this); return tmp;
}

bool BinarySearchTree::ConstIterator::operator==(const ConstIterator &other) const { return _node == other._node; }
bool BinarySearchTree::ConstIterator::operator!=(const ConstIterator &other) const { return _node != other._node; }


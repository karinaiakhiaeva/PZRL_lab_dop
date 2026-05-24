#pragma once
#include <cstddef>

template<typename Key, typename Value>
class SplayTree {
private:
    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;
        Node* parent;
        
        Node(const Key& k, const Value& v) 
            : key(k), value(v), left(nullptr), right(nullptr), parent(nullptr) {}
    };
    
    Node* _root;
    size_t _size;
    
    void rotateLeft(Node* x) {
        Node* y = x->right;
        if (!y) return;
        
        x->right = y->left;
        if (y->left) y->left->parent = x;
        
        y->parent = x->parent;
        if (!x->parent) {
            _root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
    }
    
    void rotateRight(Node* x) {
        Node* y = x->left;
        if (!y) return;
        
        x->left = y->right;
        if (y->right) y->right->parent = x;
        
        y->parent = x->parent;
        if (!x->parent) {
            _root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->right = x;
        x->parent = y;
    }
    
    void splay(Node* x) {
        if (!x) return;
        
        while (x->parent) {
            Node* p = x->parent;
            Node* g = p->parent;
            
            if (!g) {
                if (x == p->left) rotateRight(p);
                else rotateLeft(p);
            } else if (x == p->left && p == g->left) {
                rotateRight(g);
                rotateRight(p);
            } else if (x == p->right && p == g->right) {
                rotateLeft(g);
                rotateLeft(p);
            } else if (x == p->right && p == g->left) {
                rotateLeft(p);
                rotateRight(g);
            } else {
                rotateRight(p);
                rotateLeft(g);
            }
        }
    }
    
    Node* findNode(const Key& key) {
        Node* current = _root;
        Node* last = nullptr;
        
        while (current) {
            last = current;
            if (key < current->key) {
                current = current->left;
            } else if (key > current->key) {
                current = current->right;
            } else {
                return current;
            }
        }
        return last;
    }
    
    const Node* findNode(const Key& key) const {
        Node* current = _root;
        Node* last = nullptr;
        
        while (current) {
            last = current;
            if (key < current->key) {
                current = current->left;
            } else if (key > current->key) {
                current = current->right;
            } else {
                return current;
            }
        }
        return last;
    }
    
    void clearSubtree(Node* node) {
        if (!node) return;
        clearSubtree(node->left);
        clearSubtree(node->right);
        delete node;
    }
    
    bool isValidBST(Node* node, const Key* minKey, const Key* maxKey) const {
        if (!node) return true;
        
        if (minKey && node->key <= *minKey) return false;
        if (maxKey && node->key >= *maxKey) return false;
        
        return isValidBST(node->left, minKey, &node->key) && 
               isValidBST(node->right, &node->key, maxKey);
    }
    
public:
    SplayTree() : _root(nullptr), _size(0) {}
    
    ~SplayTree() {
        clearSubtree(_root);
    }
    
    void insert(const Key& key, const Value& value) {
        if (!_root) {
            _root = new Node(key, value);
            _size++;
            return;
        }
        
        Node* current = _root;
        Node* parent = nullptr;
        
        while (current) {
            parent = current;
            if (key < current->key) {
                current = current->left;
            } else if (key > current->key) {
                current = current->right;
            } else {
                current->value = value;
                splay(current);
                return;
            }
        }
        
        Node* newNode = new Node(key, value);
        newNode->parent = parent;
        
        if (key < parent->key) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }
        
        splay(newNode);
        _size++;
    }
    
    bool remove(const Key& key) {
        Node* node = findNode(key);
        if (!node || node->key != key) {
            if (node) splay(node);
            return false;
        }
        
        splay(node);
        
        if (!node->left) {
            if (!node->parent) {
                _root = node->right;
            } else if (node == node->parent->left) {
                node->parent->left = node->right;
            } else {
                node->parent->right = node->right;
            }
            if (node->right) node->right->parent = node->parent;
        } else if (!node->right) {
            if (!node->parent) {
                _root = node->left;
            } else if (node == node->parent->left) {
                node->parent->left = node->left;
            } else {
                node->parent->right = node->left;
            }
            if (node->left) node->left->parent = node->parent;
        } else {
            Node* leftSubtree = node->left;
            leftSubtree->parent = nullptr;
            Node* rightSubtree = node->right;
            rightSubtree->parent = nullptr;
            
            Node* maxLeft = leftSubtree;
            while (maxLeft->right) maxLeft = maxLeft->right;
            splay(maxLeft);
            
            maxLeft->right = rightSubtree;
            rightSubtree->parent = maxLeft;
            
            if (!node->parent) {
                _root = maxLeft;
            } else if (node == node->parent->left) {
                node->parent->left = maxLeft;
            } else {
                node->parent->right = maxLeft;
            }
            maxLeft->parent = node->parent;
        }
        
        delete node;
        _size--;
        return true;
    }
    
    Value* search(const Key& key) {
        Node* node = findNode(key);
        if (node) {
            splay(node);
            if (node->key == key) {
                return &node->value;
            }
        }
        return nullptr;
    }
    
    const Value* search(const Key& key) const {
        const Node* node = findNode(key);
        if (node && node->key == key) {
            return &node->value;
        }
        return nullptr;
    }
    
    bool isValidBST() const {
        if (!_root) return true;
        return isValidBST(_root, nullptr, nullptr);
    }
    
    size_t size() const {
        return _size;
    }
    
    bool empty() const {
        return _size == 0;
    }
};

#include "BST.h"
#include <iostream>
#include <algorithm>
static void* global_tree_ptr = nullptr;

BinarySearchTree::Node::Node(Key key, Value value, Node *parent, Node *left, Node *right) : keyValuePair(key, value),parent(parent),left(left),right(right) {}

BinarySearchTree::Node::Node(const Node &other): keyValuePair(other.keyValuePair),parent(nullptr),left(nullptr),right(nullptr)
{
	if (other.left) 
    	{
		left = new Node(*other.left);
        	left->parent = this;
    	}
    	if (other.right) 
	{
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
    	if (left || right) 
	{
		std::cout << " [";
		if (left)
            		left->output_node_tree();
        	else
            		std::cout << "null";
		std::cout << ", ";
		if (right)
            		right->output_node_tree();
        	else
            		std::cout << "null";

        	std::cout << "]";
    	}	
}

void BinarySearchTree::Node::insert(const Key &key, const Value &value)
{
	if (key < keyValuePair.first)
    	{
		if (left)
			left->insert(key, value);
		else
			left = new Node(key, value, this);
    	}
    	else 
    	{
		if (right)
			right->insert(key, value);
        	else
            		right = new Node(key, value, this);
    	}
}

void BinarySearchTree::Node::erase(const Key &) {}

BinarySearchTree::BinarySearchTree(const BinarySearchTree &other): _size(other._size), _root(nullptr)
{
	if (other._root)
        	_root = new Node(*other._root);
}

BinarySearchTree& BinarySearchTree::operator=(const BinarySearchTree &other)
{
    	if (this != &other) {

        	while (_root) {
            		erase(_root->keyValuePair.first);
        	}

        	if (other._root)
        	    	_root = new Node(*other._root);
        	else
            		_root = nullptr;

        	_size = other._size;
    	}

    	return *this;
}

BinarySearchTree::BinarySearchTree(BinarySearchTree &&other) noexcept : _size(other._size), _root(other._root)
{
    	other._root = nullptr;
    	other._size = 0;
}

BinarySearchTree& BinarySearchTree::operator=(BinarySearchTree &&other) noexcept
{
    	if (this != &other) {
		while (_root) {
            		erase(_root->keyValuePair.first);
        	}

        	_root = other._root;
        	_size = other._size;

        	other._root = nullptr;
        	other._size = 0;
	}
	return *this;
}

BinarySearchTree::~BinarySearchTree()
{
    	while (_root) {
        	erase(_root->keyValuePair.first);
    	}
}

void BinarySearchTree::insert(const Key &key, const Value &value)
{
    	if (!_root)
        	_root = new Node(key, value);

    	else
        	_root->insert(key, value);

    	++_size;
}

void BinarySearchTree::erase(const Key &key)
{
    	while (true) {
		Node *current = _root;
		while (current) {
			if (key < current->keyValuePair.first)
                		current = current->left;

            		else if (key > current->keyValuePair.first)
                		current = current->right;

            		else
                		break;
        	}

        	if (!current)
            		break;

        	if (!current->left && !current->right) {
            		if (current == _root)
                		_root = nullptr;
			else if (current->parent->left == current)
                		current->parent->left = nullptr;
			else
                		current->parent->right = nullptr;

            		delete current;
        	}

      
        	else if (current->left && !current->right) {
			Node *child = current->left;

            		if (current == _root) {
				_root = child;
                		child->parent = nullptr;
            		}
            		else {
				if (current->parent->left == current)
                    			current->parent->left = child;
                		else
                    			current->parent->right = child;

                		child->parent = current->parent;
            		}
			current->left = nullptr;
		delete current;
        	}

	        else if (!current->left && current->right) {
			Node *child = current->right;

            		if (current == _root) {
                		_root = child;
                		child->parent = nullptr;
            		}
            		else {
				if (current->parent->left == current)
                    			current->parent->left = child;
                		else
                    			current->parent->right = child;

                		child->parent = current->parent;
            		}

            		current->right = nullptr;
			delete current;
        	}
        	else {
			Node *succ = current->right;
	
            		while (succ->left)
                		succ = succ->left;

            		current->keyValuePair = succ->keyValuePair;

            		if (succ->parent->left == succ)
                		succ->parent->left = succ->right;
            		else
                		succ->parent->right = succ->right;

            		if (succ->right)
                		succ->right->parent = succ->parent;

            		succ->left = nullptr;
           		succ->right = nullptr;

            	delete succ;
        	}

        	--_size;
    	}
}

BinarySearchTree::ConstIterator BinarySearchTree::find(const Key &key) const
{
	const Node *current = _root;
    	const Node *result = nullptr;
	while (current) {

        	if (key < current->keyValuePair.first) {
            		current = current->left;
        	}
        	else if (key > current->keyValuePair.first) {
            		current = current->right;
        	}
        	else {
            		result = current;
            		current = current->left;
        	}
    	}
    	if (result)
        	return ConstIterator(result);
    	return cend();
}

BinarySearchTree::Iterator BinarySearchTree::find(const Key &key)
{
    	Node *current = _root;
    	Node *result = nullptr;
    	while (current) {
        	if (key < current->keyValuePair.first) {
            		current = current->left;
        	}
        	else if (key > current->keyValuePair.first) {
            		current = current->right;
        	}
        	else {
            		result = current;
            		current = current->left;
        	}
    	}
    	if (result)
        	return Iterator(result);
	return end();
}

std::pair<BinarySearchTree::Iterator,BinarySearchTree::Iterator> BinarySearchTree::equalRange(const Key &key)
{
    	auto first = find(key);
    	if (first == end())
        	return {end(), end()};
    	auto last = first;

    	while (last != end() && last->first == key)
        	++last;

    	return {first, last};
}

std::pair<BinarySearchTree::ConstIterator,BinarySearchTree::ConstIterator> BinarySearchTree::equalRange(const Key &key) const
{
	auto first = find(key);
    	if (first == cend())
        	return {cend(), cend()};

    	auto last = first;
    	while (last != cend() && last->first == key)
        	++last;

    	return {first, last};
}

BinarySearchTree::ConstIterator BinarySearchTree::min() const
{
    	if (!_root)
        	return cend();

    	const Node *current = _root;

    	while (current->left)
        	current = current->left;

    	return ConstIterator(current);
}

BinarySearchTree::ConstIterator BinarySearchTree::max() const
{
    	if (!_root)
        	return cend();

    	const Node *current = _root;

    	while (current->right)
        	current = current->right;

    	return ConstIterator(current);
}

BinarySearchTree::ConstIterator BinarySearchTree::min(const Key &key) const
{
    	auto range = equalRange(key);

    	if (range.first == cend())
        	return cend();

    	auto best = range.first;

    	for (auto it = range.first; it != range.second; ++it) {
        	if (it->second < best->second)
            		best = it;
    	}

    	return best;
}

BinarySearchTree::ConstIterator BinarySearchTree::max(const Key &key) const
{
    	auto range = equalRange(key);
    	if (range.first == cend())
        	return cend();

    	auto best = range.first;

    	for (auto it = range.first; it != range.second; ++it) {
		if (it->second > best->second)
            	best = it;
    	}

    	return best;
}

BinarySearchTree::Iterator BinarySearchTree::begin()
{
    	global_tree_ptr = static_cast<void*>(_root);
    	if (!_root)
        	return Iterator(nullptr);

    	Node *current = _root;

    	while (current->left)
        	current = current->left;

    	return Iterator(current);
}

BinarySearchTree::Iterator BinarySearchTree::end()
{
    	global_tree_ptr = static_cast<void*>(_root);
    	return Iterator(nullptr);
}

BinarySearchTree::ConstIterator BinarySearchTree::cbegin() const
{
    	global_tree_ptr = static_cast<void*>(_root);

    	if (!_root)
        	return ConstIterator(nullptr);

    	const Node *current = _root;

    	while (current->left)
        	current = current->left;

    	return ConstIterator(current);
}

BinarySearchTree::ConstIterator BinarySearchTree::cend() const
{
    	global_tree_ptr = static_cast<void*>(_root);
    	return ConstIterator(nullptr);
}

size_t BinarySearchTree::size() const
{
    	return _size;
}

void BinarySearchTree::output_tree()
{
    	if (_root)
        	_root->output_node_tree();
    	else
        	std::cout << "Empty tree";

    	std::cout << std::endl;
}

BinarySearchTree::Iterator::Iterator(Node *node): _node(node) {}

std::pair<Key, Value>& BinarySearchTree::Iterator::operator*()
{
    	return _node->keyValuePair;
}

const std::pair<Key, Value>& BinarySearchTree::Iterator::operator*() const
{
    	return _node->keyValuePair;
}

std::pair<Key, Value>* BinarySearchTree::Iterator::operator->()
{
    	return &_node->keyValuePair;
}

const std::pair<Key, Value>* BinarySearchTree::Iterator::operator->() const
{
    	return &_node->keyValuePair;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++()
{
    	if (!_node)
        	return *this;

    	if (_node->right) {

        	_node = _node->right;

        	while (_node->left)
            		_node = _node->left;
    	}
    	else {

        	Node *parent = _node->parent;

        	while (parent && _node == parent->right) {
            		_node = parent;
            		parent = parent->parent;
        	}

        	_node = parent;
    	}

    	return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(int)
{
    	Iterator temp = *this;
    	++(*this);
    	return temp;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--()
{
    	if (!_node) {

        	_node = static_cast<Node*>(global_tree_ptr);

        	if (!_node)
            		return *this;

        	while (_node->right)
            		_node = _node->right;

        	return *this;
    	}

    	if (_node->left) {
		_node = _node->left;
		while (_node->right)
            		_node = _node->right;
    	}
    	else {
		Node *parent = _node->parent;

        	while (parent && _node == parent->left) {
            		_node = parent;
            		parent = parent->parent;
        	}

        	_node = parent;
    	}

    	return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(int)
{
    	Iterator temp = *this;
    	--(*this);
    	return temp;
}

bool BinarySearchTree::Iterator::operator==(const Iterator &other) const
{
    	return _node == other._node;
}

bool BinarySearchTree::Iterator::operator!=(const Iterator &other) const
{
    	return _node != other._node;
}

BinarySearchTree::ConstIterator::ConstIterator(const Node *node): _node(node){}

const std::pair<Key, Value>& BinarySearchTree::ConstIterator::operator*() const
{
    	return _node->keyValuePair;
}

const std::pair<Key, Value>* BinarySearchTree::ConstIterator::operator->() const
{
    	return &_node->keyValuePair;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++()
{
    	if (!_node)
        	return *this;

    	if (_node->right) {
		_node = _node->right;
		while (_node->left)
            		_node = _node->left;
    	}
    	else {
		const Node *parent = _node->parent;

        	while (parent && _node == parent->right) {
            		_node = parent;
            		parent = parent->parent;
        	}
		_node = parent;
    	}
	return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++(int)
{
    	ConstIterator temp = *this;
    	++(*this);
    	return temp;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--()
{
    	if (!_node) {
		_node = static_cast<const Node*>(global_tree_ptr);
		if (!_node)
            		return *this;
		while (_node->right)
            		_node = _node->right;

        	return *this;
    	}
	if (_node->left) {
		_node = _node->left;
		while (_node->right)
            		_node = _node->right;
    	}
    	else {
        	const Node *parent = _node->parent;
        	while (parent && _node == parent->left) {
            		_node = parent;
            		parent = parent->parent;
        	}
	_node = parent;
    }

    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(int)
{
    	ConstIterator temp = *this;
    	--(*this);
    	return temp;
}

bool BinarySearchTree::ConstIterator::operator==(const ConstIterator &other) const
{
   	return _node == other._node;
}

bool BinarySearchTree::ConstIterator::operator!=(const ConstIterator &other) const
{
    	return _node != other._node;
}

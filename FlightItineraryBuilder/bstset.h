#ifndef BST_INCLUDED
#define BST_INCLUDED
#include <iostream>
using namespace std;

template<typename T>
class BSTSet {
public:
	class SetIterator {
	public:
		SetIterator(typename BSTSet<T>::Node* cur, typename BSTSet<T>::Node* r);
		const T* get_and_advance();
	private:
		typename BSTSet<T>::Node* root;
		typename BSTSet<T>::Node* curptr;
	};

	BSTSet();
	~BSTSet();
	void insert(const T& value);
	SetIterator find(const T& value) const;
	SetIterator find_first_not_smaller(const T& value) const;

private:
	struct Node {
		Node(const T& val) : value(val), left(nullptr), right(nullptr) {}
		T value;
		Node* left;
		Node* right;
	};
	Node* root;
	void deleteSet(Node* root);
	Node* insertHelper(Node* node, const T& value);
};

template<typename T>
BSTSet<T>::SetIterator::SetIterator(typename BSTSet<T>::Node* cur, typename BSTSet<T>::Node* r) : curptr(cur), root(r) {}

template<typename T>
const T* BSTSet<T>::SetIterator::get_and_advance() {
	if (curptr == nullptr) {
		return nullptr;
	}

	const T* currentVal = &(curptr->value);

	if (curptr->right != nullptr) {
		curptr = curptr->right;
		while (curptr->left != nullptr) {
			curptr = curptr->left;
		}
	}
	else {
		Node* next = nullptr;
		Node* i = root;
		while (i != nullptr) {
			if (curptr->value < i->value) {
				next = i;
				i = i->left;
			}
			else if (curptr->value > i->value) {
				i = i->right;
			}
			else {
				break;
			}
		}
		curptr = next;
	}
	return currentVal;
}

template<typename T>
BSTSet<T>::BSTSet() : root(nullptr) {}

template<typename T>
BSTSet<T>::~BSTSet() {
	deleteSet(root);
}

template<typename T>
void BSTSet<T>::deleteSet(Node* root) {
	if (root == nullptr) {
		return;
	}

	deleteSet(root->left);
	deleteSet(root->right);

	delete root;
}

template<typename T>
void BSTSet<T>::insert(const T& value) {
	root = insertHelper(root, value);
}

template<typename T>
typename BSTSet<T>::Node* BSTSet<T>::insertHelper(Node* node, const T& value) {
	if (node == nullptr) {
		return new Node(value);
	}

	if (value < node->value) {
		node->left = insertHelper(node->left, value);
	}
	else if (value > node->value) {
		node->right = insertHelper(node->right, value);
	}

	return node;
}

template<typename T>
typename BSTSet<T>::SetIterator BSTSet<T>::find(const T& value) const {
	Node* cur = root;
	while (cur != nullptr) {
		if (value == cur->value) {
			return SetIterator(cur, root);
		}
		else if (value < cur->value) {
			cur = cur->left;
		}
		else {
			cur = cur->right;
		}
	}
	return SetIterator(nullptr, root);
}

template<typename T>
typename BSTSet<T>::SetIterator BSTSet<T>::find_first_not_smaller(const T& value) const {
	Node* min = nullptr;
	Node* cur = root;

	while (cur != nullptr) {
		if (value <= cur->value) {
			min = cur;
			cur = cur->left;
		}
		else {
			cur = cur->right;
		}
	}
	return SetIterator(min, root);
}

#endif //BST_INCLUDED
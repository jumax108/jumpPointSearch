#pragma once

template<typename T>
class linkedList {
public:

	struct _node {
		T value;
		_node* parent;
		_node* child;

		inline _node* operator=(const _node* const node) {
			value = node->value;
			parent = node->parent;
			child = node->child;

			return this;
		}

		inline bool operator==(const _node& const node) {
			return value == node.value && parent == node.parent && child == node.child;
		}

	};

	class iterator {
	public:
		iterator() {
			node = nullptr;
		}

		iterator(const _node* const node) {
			this->node = (_node*)node;
		}

		inline iterator* operator=(const _node* const node) {
			this->node = (_node*)node;

			return this;
		}

		inline T& operator*() {
			return node->value;
		}

		inline iterator* operator++() {
			node = node->child;
			return this;
		}

		inline iterator operator+(int addNum) {
			iterator iter = *this;
			for (int addCnt = 0; addCnt < addNum; ++addCnt) {
				++iter;
			}
			return iter;
		}



		inline iterator* operator--() {
			node = node->parent;
			return this;
		}

		inline bool operator!=(const _node* const node) {
			return !(*(this->node) == *node);
		}

		inline bool operator==(const _node* const node) {
			return *(this->node) == *node;
		}

	private:
		_node *node;
		friend class linkedList;
	};

	linkedList();

	void push_front(
		const T const value // 저장할 값
	);
	void push_back(
		const T const value // 저장할 값
	);

	void insert(
		const T const value, // 값을 저장할 데이터
		_node* parent  // 이 노드의 자식으로 저장됩니다.
	);

	iterator erase(
		iterator iter // 지울 노드를 전달합니다.
	) {

		linkedList<T>::_node* node = iter.node;
		linkedList<T>::_node* child = node->child;

		node->child->parent = node->parent;
		node->parent->child = node->child;

		linkedList<T>::iterator* nextIter = ++iter;

		free(node);

		return *(nextIter);
	}

	void clear();

	bool empty() {
		return head.child == &tail;
	}

	inline _node* begin() {
		return head.child;
	}

	inline _node* end() {
		return &tail;
	}

	~linkedList();



private:
	_node head;
	_node tail;

};


/*
template<typename T>
linkedList<T>::iterator linkedList<T>::erase(linkedList<T>::iterator iter) {

	linkedList<T>::_node* node = iter.node;
	linkedList<T>::_node* child = node->child;

	node->child->parent = node->parent;
	node->parent->child = node->child;

	free(node);

	return *(++iter);
}*/


template<typename T>
linkedList<T>::linkedList(){
	
	head.parent = nullptr;
	head.child = &tail;

	tail.parent = &head;
	tail.child = nullptr;
}

template<typename T>
linkedList<T>::~linkedList() {
	clear();
}

template<typename T>
void linkedList<T>::insert(T value, linkedList<T>::_node* parent) {

	_node* node = (linkedList<T>::_node*)malloc(sizeof(linkedList<T>::_node));

	node->value = value;
	node->parent = parent;

	parent->child->parent = node;
	node->child = parent->child;
	parent->child = node;
	
}

template<typename T>
void linkedList<T>::push_front(const T const value) {
	_node* node = (linkedList<T>::_node*)malloc(sizeof(linkedList<T>::_node));

	node->value = value;

	node->parent = &head;
	node->child = head.child;

	head.child->parent = node;
	head.child = node;
}

template<typename T>
void linkedList<T>::push_back(const T const value) {
	_node* node = (linkedList<T>::_node*)malloc(sizeof(linkedList<T>::_node));

	node->value = value;
	node->parent = tail.parent;
	node->child = &tail;

	tail.parent->child = node;
	tail.parent = node;
}

template<typename T>
void linkedList<T>::clear() {
	for (linkedList<T>::iterator iter = begin(); iter != end(); ) {
		iter = erase(iter);
	}
}
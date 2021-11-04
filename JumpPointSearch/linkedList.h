#pragma once

template<typename T>
class CLinkedList {
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
		friend class CLinkedList;
	};

	CLinkedList();

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

	bool empty();

	iterator erase(
		iterator iter // 지울 노드를 전달합니다.
	) {

		CLinkedList<T>::_node* node = iter.node;
		CLinkedList<T>::_node* child = node->child;

		node->child->parent = node->parent;
		node->parent->child = node->child;

		iterator nextIter = *(++iter);

		free(node);

		return nextIter;
	}

	void clear();

	inline _node* begin() {
		return head.child;
	}

	inline _node* end() {
		return &tail;
	}

	~CLinkedList();



private:
	_node head;
	_node tail;

};

template<typename T>
CLinkedList<T>::CLinkedList(){
	
	head.parent = nullptr;
	head.child = &tail;

	tail.parent = &head;
	tail.child = nullptr;
}

template<typename T>
CLinkedList<T>::~CLinkedList() {
	clear();
}

template<typename T>
void CLinkedList<T>::insert(T value, CLinkedList<T>::_node* parent) {

	_node* node = (CLinkedList<T>::_node*)malloc(sizeof(CLinkedList<T>::_node));

	node->value = value;
	node->parent = parent;

	parent->child->parent = node;
	node->child = parent->child;
	parent->child = node;
	
}

template<typename T>
void CLinkedList<T>::push_front(const T const value) {
	_node* node = (CLinkedList<T>::_node*)malloc(sizeof(CLinkedList<T>::_node));

	node->value = value;

	node->parent = &head;
	node->child = head.child;

	head.child->parent = node;
	head.child = node;
}

template<typename T>
void CLinkedList<T>::push_back(const T const value) {
	_node* node = (CLinkedList<T>::_node*)malloc(sizeof(CLinkedList<T>::_node));

	node->value = value;
	node->parent = tail.parent;
	node->child = &tail;

	tail.parent->child = node;
	tail.parent = node;
}

template<typename T>
void CLinkedList<T>::clear() {
	for (CLinkedList<T>::iterator iter = begin(); iter != end(); ) {
		iter = erase(iter);
	}
}

template<typename T>
bool CLinkedList<T>::empty(){

	return head.child == &tail;

}
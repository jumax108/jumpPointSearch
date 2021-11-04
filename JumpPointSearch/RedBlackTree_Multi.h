#pragma once

template<typename T>
class CObjectFreeList;

class SimpleProfiler;
class CRedBlackTreeLess;

#if defined(SPEED_TEST)
extern SimpleProfiler* sp;
#endif

#if defined(_WINDOWS_)
extern RECT windowRect;
#endif

template<typename T, class C = CRedBlackTreeLess>
class CRedBlackTree_Multi {

	friend class CJumpPointSearch;

public:


	struct stNode {

		stNode() {}

		CLinkedList<T>* _dataList;
		stNode* _left;
		stNode* _right;
		stNode* _parent;
		bool _isRed;
		bool _isNill;

	};

	// 노드를 받고 초기화할 함수
	stNode* allocNode(bool isRed = true, bool isNill = false, stNode* parent = nullptr);

	// 노드를 해제할 함수
	void freeNode(stNode* node);

	CRedBlackTree_Multi(C compareFunctor = CRedBlackTreeLess());
	~CRedBlackTree_Multi();

	// 데이터 삽입
	void insert(T data);
	// 데이터 삭제
	void erase(T data);

	// T를 찾아서 T를 줍니다
	// 현재 tree가 key 구조라서 결국 있냐를 묻는 느낌
	T find(T data);

	// 전위순회의 순서대로 데이터를 배열에 담아서 전달해줍니다.
	T* inorder();

	// 전체 제거
	void clear();

	// 노드 개수 얻기
	int getNodeNum();

	// 트리가 비어있는가
	bool empty();

#ifdef _WINDOWS_

	// 테스트 동작, addNum 만큼의 수를 넣음
	static const int addNum = 100;
	// 테스트 동작, eraseNUm 만큼의 수를 제거함
	static const int eraseNum = 50;
	// 결과적으로 테스트가 종료되면, addNum - eraseNum 만큼의 숫자가 트리 내부에 남게되는 구조

	// 단 한 번의 루프만 돌고 리턴되는 함수
	// 100개 삽입하는 테스트라면 1개 삽입하고 리턴하고
	// 다시 함수에 진입하면 또 1개 삽입하고 리턴하는 식
	// 문제가 있을 때, 한 단계씩 보고 싶어서 제작함
	static CRedBlackTree_Multi<int>* singleCaseTest(int seed);

	// 전체 루프를 완료하고 리턴되는 함수
	// 100개를 삽입하는 테스트라면 100개를 삽입하고 리턴함
	static CRedBlackTree_Multi<int>* test(CRedBlackTree_Multi<int>*);

	// 출력 관련
	// treeViewer에 출력함
	void print(HDC hdc, int x);

#endif

private:


	// stNode에 대한 free list
	CObjectFreeList<stNode>* nodeFreeList;

	// CLinekdList 에 대한 free list
	CObjectFreeList<CLinkedList<T>>* listFreeList;

	// 비교 펑터
	C _compareFunctor;

	// 현재 트리에 들어간 노드의 개수
	int _nodeNum;

	// 루트 노드
	stNode* _root;

	// 실질적으로 노드가 지워지는 함수
	stNode* eraseNode(stNode** node, bool* isRed);

	// 삽입 후 밸런스 잡기 함수
	void insertBalance(stNode* node);
	// 삭제 후 밸런스 잡기 함수
	void eraseBalance(stNode* node);

	void leftRotation(stNode* parent, stNode* right);
	void rightRotation(stNode* parent, stNode* left);

	// 자체 진단용 함수
	// 트리가 현재 정상적인지를 판단함
	void diagnosis(stNode* node = _root);

#if defined(_VECTOR_)
	// 트리를 벡터로 변환해서 전달해줌
	// 테스트용으로 쓰임
	void treeToList(std::vector<T>* vector, std::vector<T>* blackNumList, stNode* node, int blackNum = 0);
#endif

	// 실질적으로 전위순회하는 함수
	void inorderLoop(T* arr, int* index, stNode* node);

#ifdef _WINDOWS_
	HDC _hdc;
	int printLoop(stNode* node, int deepth, int* printCnt, int x);
#endif

};

#ifndef _REDBLACKTREELESS_
#define _REDBLACKTREELESS_

class CRedBlackTreeLess {
public:
	template<typename T>
	bool operator()(T left, T right) {
		return left < right;
	}
};

#endif

template <typename T, class C>
typename CRedBlackTree_Multi<T, C>::stNode* CRedBlackTree_Multi<T,C>::allocNode(bool isRed = true, bool isNill = false, stNode* parent = nullptr){

	stNode* node = nodeFreeList->allocObject();
	stNode** left = &node->_left;
	stNode** right = &node->_right;

	node->_dataList = listFreeList->allocObject();

	if (isNill == false) {

		*left = allocNode(false, true, node);
		*right = allocNode(false, true, node);

	}
	else {
		*left = nullptr;
		*right = nullptr;
	}

	node->_parent = parent;
	node->_isRed = isRed;
	node->_isNill = isNill;

	return node;
}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::freeNode(stNode* node){
	listFreeList->freeObject(node->_dataList);
	nodeFreeList->freeObject(node);
}

template <typename T, class C>
CRedBlackTree_Multi<T, C>::CRedBlackTree_Multi(C compareFunctor) {

	listFreeList = new CObjectFreeList<CLinkedList<T>>(100);
	nodeFreeList = new CObjectFreeList<stNode>(100);

	_root = allocNode(false, true);

	_compareFunctor = compareFunctor;
	_nodeNum = 0;

}

template <typename T, class C>
CRedBlackTree_Multi<T, C>::~CRedBlackTree_Multi() {

	if (_root->_isNill == true) {
		freeNode(_root);
		return;
	}

	while (_root->_left->_isNill == false || _root->_right->_isNill == false) {
		erase(_root->_dataList->begin()->value);
	}

	freeNode(_root->_left);
	freeNode(_root->_right);
	freeNode(_root);

	delete nodeFreeList;
	nodeFreeList = nullptr;

	delete listFreeList;
	listFreeList = nullptr;
}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::insert(T data) {

#if defined(SPEED_TEST)
	//sp->profileBegin("insert");
#endif

	do {
		//stNode* newNode = nodeFreeList->allocObject();
		//new (newNode) stNode(nodeFreeList);

		stNode* newNode = allocNode();

		if (_root->_isNill == true) {
			
			freeNode(_root);
			newNode->_isRed = false;
			_root = newNode;
			newNode->_dataList->push_back(data);
			_nodeNum = 1;
			break;
		}

		stNode* node = _root;
		while (true) {

			CLinkedList<T>* list = node->_dataList;
			T value = list->begin()->value;

			if (_compareFunctor(value, data) == true) {
				if (node->_right->_isNill == true) {

					freeNode(node->_right);
					node->_right = newNode;
					newNode->_dataList->push_back(data);
					newNode->_parent = node;
					insertBalance(newNode);

					_nodeNum += 1;

#if defined(LOGIC_TEST)
					diagnosis(_root);
#endif

					// return 하는 상황. profile 처리를 return 직전에 해야해서 처리하러 goto 로 이동
					// break 하면 노드가 delete 되어버림
					goto FIN;
				}
				node = node->_right;
			}
			else if (_compareFunctor(data, value) == true) {
				if (node->_left->_isNill == true) {

					freeNode(node->_left);
					node->_left = newNode;
					newNode->_dataList->push_back(data);
					newNode->_parent = node;
					insertBalance(newNode);

					_nodeNum += 1;

#if defined(LOGIC_TEST)
					diagnosis(_root);
#endif

					// return 하는 상황. profile 처리를 return 직전에 해야해서 처리하러 goto 로 이동
					// break 하면 노드가 delete 되어버림
					goto FIN;
				}
				node = node->_left;
			}
			else {
				list->push_back(data);
				break;
			}
		}
		freeNode(newNode);
	} while (false);

FIN:;

#if defined(SPEED_TEST)
	//sp->profileEnd("insert");
#endif

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::erase(T data) {

#if defined(SPEED_TEST)
	//sp->profileBegin("erase");
#endif

	stNode** node = &_root;

	while ((*node)->_isNill == false) {

		CLinkedList<T>* list = (*node)->_dataList;
		T value = list->begin()->value;

		if (_compareFunctor(value, data) == true) {
			//right
			node = &(*node)->_right;
		}
		else if (_compareFunctor(data, value) == true) {
			//left
			node = &(*node)->_left;
		}
		else {
			// correct

			// 맞으면 리스트에서 해당 항목 제거
			typename CLinkedList<T>::iterator iter = list->begin();
			for (; iter != list->end(); ++iter) {
				if (*iter == data) {
					list->erase(iter);
					break;
				}
			}

			if (list->empty() == false) {
				break;
			}

			// 제거 했는데 리스트가 비어있다면 해당 노드 제거
			bool isRed;
			stNode* erasedNode = eraseNode(node, &isRed);
			_nodeNum -= 1;
			if (isRed == false) {
				eraseBalance(erasedNode);

#if defined(LOGIC_TEST)
				diagnosis(_root);
#endif
			}
			break;
		}
	}

#if defined(SPEED_TEST)
	//sp->profileEnd("erase");
#endif
}

template <typename T, class C>
T CRedBlackTree_Multi<T, C>::find(T data) {

#if defined(SPEED_TEST)
	sp->profileBegin("find");
#endif

	stNode* node = _root;
	T result = NULL;

	while (node != nullptr) {

		if (node->_isNill == true) {
			break;
		}

		if (_compareFunctor(data, node->_dataList->begin()->value) == true) {
			// data가 더 작음
			node = node->_left;
		}
		else if (_compareFunctor(node->_dataList->begin()->value, data) == true) {
			// data가 더 큼
			node = node->_right;
		}
		else {
			// 같음
			result = node->_dataList->begin()->value;
			break;
		}

	}

#if defined(SPEED_TEST)
	sp->profileEnd("find");
#endif

	return result;

}

#if defined(_WINDOWS_)
template <typename T, class C>
CRedBlackTree_Multi<int>* CRedBlackTree_Multi<T, C>::singleCaseTest(int seed) {

	static CRedBlackTree_Multi<int>* tree = nullptr;

	static std::vector<int>* addNumList = nullptr;
	static std::vector<int>* eraseIndexList = nullptr;

	static std::vector<int>* ableNumList = nullptr;

	static std::vector<int>::iterator addNumListIter;
	static std::vector<int>::iterator eraseIndexListIter;

	if (addNumList == nullptr) {

		printf("seed : %d\n", seed);
		srand(seed);

		delete(tree);
		tree = new CRedBlackTree();
		addNumList = new std::vector<int>();
		eraseIndexList = new std::vector<int>();
		ableNumList = new std::vector<int>();

		std::vector<int> answerList;

		for (int numCnt = 0; numCnt < addNum; numCnt++) {
			ableNumList->push_back(numCnt);
		}

		printf("add num: ");
		for (int numCnt = 0; numCnt < addNum; numCnt++) {
			int index = rand() % ableNumList->size();
			addNumList->push_back(ableNumList->at(index));
			printf("%d ", ableNumList->at(index));
			answerList.push_back(ableNumList->at(index));
			ableNumList->erase(ableNumList->begin() + index);
		}
		printf("\n");

		for (int indexCnt = 0; indexCnt < eraseNum; ++indexCnt) {
			int index = rand() % (addNum - indexCnt);
			eraseIndexList->push_back(index);
			answerList.erase(answerList.begin() + index);
		}

		std::sort(answerList.begin(), answerList.end());

		addNumListIter = addNumList->begin();
		eraseIndexListIter = eraseIndexList->begin();

		printf("answer: ");
		for (std::vector<int>::iterator answerIter = answerList.begin(); answerIter != answerList.end(); ++answerIter) {

			printf("%d ", *answerIter);

		}
		printf("\n");

	}

	if (addNumListIter != addNumList->end()) {


		tree->insert(*addNumListIter);
		++addNumListIter;
		singleCaseTest(seed);
		return tree;

	}

	if (eraseIndexListIter != eraseIndexList->end()) {

		tree->erase(addNumList->at(*eraseIndexListIter));
		addNumList->erase(addNumList->begin() + *eraseIndexListIter);
		++eraseIndexListIter;
		addNumListIter = addNumList->end();
		return tree;

	}

	std::vector<int> treeData;
	std::vector<int> blackNodeNum;
	tree->treeToList(&treeData, &blackNodeNum, tree->_root);

	std::vector<int>::iterator treeIter;
	std::vector<int>::iterator numIter;
	sort(addNumList->begin(), addNumList->end());

	for (treeIter = treeData.begin(), numIter = addNumList->begin(); treeIter != treeData.end(); ++treeIter, ++numIter) {
		if (*treeIter != *numIter) {
			break;
		}
	}
	if (treeIter != treeData.end()) {
		printf("트리의 데이터가 예측된 데이터가 아님\n");
		system("PAUSE>NUL");
	}

	std::vector<int>::iterator blackCntIter;
	for (blackCntIter = blackNodeNum.begin() + 1; blackCntIter != blackNodeNum.end(); ++blackCntIter) {
		if (*blackCntIter != blackNodeNum[0]) {
			break;
		}
	}

	if (blackCntIter != blackNodeNum.end()) {
		printf("각 경로의 검정 노드 수가 일치하지 않습니다.");
		system("PAUSE>NUL");
	}


	delete(addNumList);
	addNumList = nullptr;
	delete(eraseIndexList);
	eraseIndexList = nullptr;
	delete(ableNumList);
	ableNumList = nullptr;

	tree = test();

	return tree;
}

template <typename T, class C>
CRedBlackTree_Multi<int>* CRedBlackTree_Multi<T, C>::test(CRedBlackTree_Multi<int>* tree) {

	//CRedBlackTree<int>* tree = nullptr;

	std::vector<int>* addNumList = nullptr;
	std::vector<int>* eraseIndexList = nullptr;

	std::vector<int>* ableNumList = nullptr;

	std::vector<int>::iterator addNumListIter;
	std::vector<int>::iterator eraseIndexListIter;

	int seed = rand() % 10000;
	//int seed = 6699;
	printf("seed : %d\n", seed);
	srand(seed);

	//tree = new CRedBlackTree();
	addNumList = new std::vector<int>();
	eraseIndexList = new std::vector<int>();
	ableNumList = new std::vector<int>();

	std::vector<int> answerList;

	for (int numCnt = 0; numCnt < addNum; numCnt++) {
		ableNumList->push_back(numCnt);
	}

	printf("add num: ");
	for (int numCnt = 0; numCnt < addNum; numCnt++) {
		int index = rand() % ableNumList->size();
		addNumList->push_back(ableNumList->at(index));
		printf("%d ", ableNumList->at(index));
		answerList.push_back(ableNumList->at(index));
		ableNumList->erase(ableNumList->begin() + index);
	}
	printf("\n");

	for (int indexCnt = 0; indexCnt < eraseNum; ++indexCnt) {
		int index = rand() % (addNum - indexCnt);
		eraseIndexList->push_back(index);
		answerList.erase(answerList.begin() + index);
	}

	std::sort(answerList.begin(), answerList.end());

	printf("answer: ");
	for (std::vector<int>::iterator answerIter = answerList.begin(); answerIter != answerList.end(); ++answerIter) {

		printf("%d ", *answerIter);

	}
	printf("\n");


	addNumListIter = addNumList->begin();
	for (; addNumListIter != addNumList->end(); ++addNumListIter) {
		tree->insert(*addNumListIter);
	}


	eraseIndexListIter = eraseIndexList->begin();
	for (; eraseIndexListIter != eraseIndexList->end(); ++eraseIndexListIter) {
		tree->erase(addNumList->at(*eraseIndexListIter));
		addNumList->erase(addNumList->begin() + *eraseIndexListIter);
	}


	std::vector<int> treeData;
	std::vector<int> blackNodeNum;
	tree->treeToList(&treeData, &blackNodeNum, tree->_root);

	std::vector<int>::iterator treeIter;
	std::vector<int>::iterator numIter;
	sort(addNumList->begin(), addNumList->end());

	for (treeIter = treeData.begin(), numIter = addNumList->begin(); treeIter != treeData.end(); ++treeIter, ++numIter) {
		if (*treeIter != *numIter) {
			break;
		}
	}

	if (treeIter != treeData.end()) {
		printf("트리의 데이터가 예측된 데이터가 아님\n");
		system("PAUSE>NUL");
	}

	std::vector<int>::iterator blackCntIter;
	for (blackCntIter = blackNodeNum.begin() + 1; blackCntIter != blackNodeNum.end(); ++blackCntIter) {
		if (*blackCntIter != blackNodeNum[0]) {
			break;
		}
	}

	if (blackCntIter != blackNodeNum.end()) {
		printf("각 경로의 검정 노드 수가 일치하지 않습니다.");
		system("PAUSE>NUL");
	}



	delete(addNumList);
	addNumList = nullptr;
	delete(eraseIndexList);
	eraseIndexList = nullptr;
	delete(ableNumList);
	ableNumList = nullptr;


	return tree;
}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::print(HDC hdc, int x) {
	do {
		if (_root->_isNill == true) {
			break;
		}

		_hdc = hdc;
		int printCnt = 0;
		printLoop(_root, 0, &printCnt, x);
	} while (false);
	{
		// 우측 상단 조작키 설명

		int left = windowRect.right - 200;
		int top = 0;
		int right = windowRect.right - 10;
		int bottom = 125;

		Rectangle(hdc, left, top, right, bottom);

		TextOutW(hdc, left + 10, top + 10, L"좌측이동 : a", 8);
		TextOutW(hdc, left + 10, top + 25, L"우측이동 : d", 8);
		TextOutW(hdc, left + 10, top + 40, L"랜덤추가 : z", 8);
		TextOutW(hdc, left + 10, top + 55, L"랜덤제거 : x", 8);
		TextOutW(hdc, left + 10, top + 70, L"테스트코드 실행 : c", 12);
		TextOutW(hdc, left + 10, top + 85, L"선택값 삽입 : q", 10);
		TextOutW(hdc, left + 10, top + 100, L"선택값 제거 : e", 10);

	}

}
#endif

template <typename T, class C>
typename CRedBlackTree_Multi<T, C>::stNode* CRedBlackTree_Multi<T, C>::eraseNode(stNode** node, bool* isRed) {

	stNode* left = (*node)->_left;
	stNode* right = (*node)->_right;

	bool isLeftNill = left->_isNill;
	bool isRightNill = right->_isNill;

	if (isLeftNill == true) {

		// 모두 닐인 경우는 어짜피 여기서 부모에서 내 노드가 연결되어 있던 곳이 닐로 연결됨
		// 노드의 우측 자식 존재

		right->_parent = (*node)->_parent;
		freeNode(left);
		//delete(left); // 좌측 닐 제거
		*isRed = (*node)->_isRed;
		freeNode(*node);
		//delete(*node); // 현재 노드 제거
		*node = right; // 부모 노드가 바라보고 있는 노드를 우측 노드로 변경
		return *node;

	}
	else if (isRightNill == true) {

		// 노드의 좌측 자식 존재

		left->_parent = (*node)->_parent;
		freeNode(right);
		//delete(right); // 우측 닐 제거
		*isRed = (*node)->_isRed;
		freeNode(*node);
		//delete(*node); // 현재 노드 제거
		*node = left;  // 부모가 바라보고 있는 노드를 좌측 노드로 변경
		return *node;

	}
	else {

		// 노드의 양쪽 자식 존재
		// 노드의 왼쪽 자식의 최우측 자식과 데이터 교체

		if (left->_right->_isNill == true) {

			freeNode(left->_right);
			//delete(left->_right); // 닐 제거

			// 노드의 왼쪽 자식의 우측 자식이 없을 때

			// 노드의 왼쪽 자식과 노드의 데이터를 교체하고
			// 노드의 왼쪽 자식의 왼쪽 자식을 노드와 연결

			(*node)->_dataList = left->_dataList; // 노드의 데이터를 왼쪽 자식의 데이터로 변경
			(*node)->_left = left->_left; // 노드의 왼쪽 자식을 왼쪽 자식의 왼쪽 자식으로 변경
			left->_left->_parent = *node;
			*isRed = left->_isRed;

			freeNode(left);
			//delete(left); // 왼쪽 노드 제거
			return (*node)->_left;

		}
		else {

			// 노드의 왼쪽 자식의 우측 자식이 있을 때

			// 최우측 자식을 찾기 위해 루프
			stNode** lastRightNode = &left; // 가장 마지막 오른쪽 자식
			while ((*lastRightNode)->_right->_isNill == false) { // 우측 자식이 닐이 아니면
				lastRightNode = &((*lastRightNode)->_right); // 우측 자식으로 교체
			}

			freeNode((*lastRightNode)->_right);
			//delete((*lastRightNode)->_right); // 닐제거

			(*node)->_dataList = (*lastRightNode)->_dataList; // 노드의 값을 최우측노드의 값으로 변경

			// 최우측 노드의 좌측 노드 존재 가능
			// 해당 노드는 최우측 노드의 부모와 연결
			// 닐이면 닐 연결됨

			stNode* lastNodeLeft = (*lastRightNode)->_left; // 최우측 노드의 좌측 노드
			lastNodeLeft->_parent = (*lastRightNode)->_parent;
			*isRed = (*lastRightNode)->_isRed;

			freeNode(*lastRightNode);
			//delete(*lastRightNode); // 최우측 노드 제거
			*lastRightNode = lastNodeLeft; // 최우측 노드의 부모 노드와 최우측 노드의 좌측 노드 연결


			return *lastRightNode;
		}

	}

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::insertBalance(stNode* node) {

	// 노드가 루트면 탈출
	if (node == _root) {
		node->_isRed = false;
		return;
	}

	stNode* parent = node->_parent;

	// 노드의 부모가 검정이면 탈출
	if (parent->_isRed == false) {
		return;
	}

	stNode* grand = parent->_parent;
	stNode* uncle;
	bool isNodeLeftGrand;
	if (grand->_left == parent) {
		uncle = grand->_right;
		isNodeLeftGrand = true;
	}
	else {
		uncle = grand->_left;
		isNodeLeftGrand = false;
	}

	stNode* sibling;
	bool isSiblingLeftParent;
	if (parent->_left == node) {
		sibling = parent->_right;
		isSiblingLeftParent = false;
	}
	else {
		sibling = parent->_left;
		isSiblingLeftParent = true;
	}

	if (parent->_isRed == true) {

		// 부모와 삼촌이 빨강일 경우
		if (uncle->_isRed == true) {
			grand->_isRed = true; // 조부를 빨강으로 함
			parent->_isRed = false; // 부모를 검정으로 함
			uncle->_isRed = false; // 삼촌을 검정으로 함
			insertBalance(grand); // 조부로 재귀
			return;
		}

		// 부모만 빨강일 경우

		if (isSiblingLeftParent == true) {
			// 형제가 왼쪽에 있을 경우
			if (isNodeLeftGrand == true) {
				// 조부 왼쪽일 경우

				// 노드 - 부모 - 형제간 회전

				// 조부 왼쪽으로 노드를 이동
				grand->_left = node;
				node->_parent = grand;


				// 부모 오른쪽을 닐노드로 변경
				parent->_right = node->_left;
				node->_left->_parent = parent;

				// 노드 왼쪽으로 부모를 이동
				node->_left = parent;
				parent->_parent = node;


				// 노드 명칭 정리
				stNode* temp = node;
				node = parent;
				parent = temp;
				sibling = parent->_right; // 닐 노드

			}
		}
		else {

			// 형제가 오른쪽에 있을 경우
			if (isNodeLeftGrand == false) {
				// 조부 오른쪽일 경우

				// 노드 - 부모 - 형제간 회전

				// 조부 오른쪽으로 노드를 이동
				grand->_right = node;
				node->_parent = grand;

				// 부모 왼쪽을 닐노드로 변경
				parent->_left = node->_right;
				node->_right->_parent = parent;

				// 노드 오른쪽으로 부모를 이동
				node->_right = parent;
				parent->_parent = node;

				// 노드 명칭 정리
				stNode* temp = node;
				node = parent;
				parent = temp;
				sibling = parent->_left; // 닐 노드
			}

		}

		// 조부까지 포함한 회전

		if (grand->_parent != nullptr) {
			if (grand->_parent->_left == grand) {
				grand->_parent->_left = parent;
				parent->_parent = grand->_parent;
			}
			else {
				grand->_parent->_right = parent;
				parent->_parent = grand->_parent;
			}
		}
		else {
			parent->_parent = nullptr;
			_root = parent;
		}


		if (isNodeLeftGrand == true) {
			// 노드가 조부 왼쪽에 있을 경우
			grand->_left = sibling;
			sibling->_parent = grand;
			grand->_parent = parent;
			parent->_right = grand;

		}
		else {
			// 노드가 조부 오른쪽에 있을 경우
			grand->_right = sibling;
			sibling->_parent = grand;
			grand->_parent = parent;
			parent->_left = grand;

		}

		grand->_isRed = true;
		parent->_isRed = false;

		return;
	}
}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::leftRotation(stNode* parent, stNode* right) {

	right->_parent = parent->_parent;
	if (parent->_parent != nullptr) {
		if (parent->_parent->_left == parent) {
			parent->_parent->_left = right;
		}
		else {
			parent->_parent->_right = right;
		}
	}
	else {
		_root = right;
	}

	parent->_right = right->_left;
	right->_left->_parent = parent;

	right->_left = parent;
	parent->_parent = right;

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::rightRotation(stNode* parent, stNode* left) {

	left->_parent = parent->_parent;
	if (parent->_parent != nullptr) {
		if (parent->_parent->_left == parent) {
			parent->_parent->_left = left;
		}
		else {
			parent->_parent->_right = left;
		}
	}
	else {
		_root = left;
	}

	parent->_left = left->_right;
	left->_right->_parent = parent;

	left->_right = parent;
	parent->_parent = left;


}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::eraseBalance(stNode* node) {

	if (node == _root) {
		_root->_isRed = false;
		return;

	}

	if (node->_isRed == true) {
		node->_isRed = false;
		return;
	}

	stNode* parent = node->_parent;
	stNode* sibling;
	bool _isNodeLeftParent;

	if (parent->_left == node) {
		sibling = parent->_right;
		_isNodeLeftParent = true;
	}
	else {
		sibling = parent->_left;
		_isNodeLeftParent = false;
	}

	if (parent->_isRed == true) {
		// 부모가 빨강일 경우

		if (sibling->_isNill == true) {
			return;
		}

		if (sibling->_isRed == false) {
			// 형제가 검정색 일 때
			if (sibling->_left->_isRed == false && sibling->_right->_isRed == false) {
				// 형제의 자식도 모두 검정일 때
				if (_isNodeLeftParent == true) {
					leftRotation(parent, sibling);
				}
				else {
					rightRotation(parent, sibling);
				}
				return;
			}

			if (sibling->_left->_isRed == true && sibling->_right->_isRed == true) {
				// 형제 자식 모두 빨강일 때
				sibling->_isRed = true;
				parent->_isRed = false;
				if (_isNodeLeftParent == true) {
					sibling->_right->_isRed = false;
					leftRotation(parent, sibling);
				}
				else {
					sibling->_left->_isRed = false;
					rightRotation(parent, sibling);
				}
				return;
			}

			stNode* left = sibling->_left;
			stNode* right = sibling->_right;

			if (_isNodeLeftParent == true) {

				// 노드가 부모의 왼쪽에 있을 때
				if (left->_isRed == true) {
					// 형제의 좌측 노드가 빨강이면
					sibling->_isRed = true;
					left->_isRed = false;
					rightRotation(sibling, left);
					sibling = left;
				}

				// 형제의 우측 자식이 빨강이면
				leftRotation(parent, sibling);

			}
			else {
				// 노드가 부모의 오른쪽에 있을 때
				if (right->_isRed == true) {
					// 형제의 우측 노드가 빨강이면
					sibling->_isRed = true;
					right->_isRed = false;
					leftRotation(sibling, right);
					sibling = right;
				}

				// 형제의 좌측 노드가 빨강이면
				rightRotation(parent, sibling);
			}
			return;
		}




		return;

	}

	// 부모가 검정일 경우

	if (sibling->_isRed == false) {
		// 형제가 검정일 경우

		if (sibling->_isNill == true) {
			return;
		}

		if (_isNodeLeftParent == true) {

			// 형제가 우측에 있을 경우
			if (sibling->_left->_isRed == true) {

				// 형제의 좌측 자식이 빨강일 경우

				// 형제 - 형제의 자식 간 우회전
				stNode* left = sibling->_left;
				stNode* right = sibling->_right;

				sibling->_isRed = true;
				left->_isRed = false;
				rightRotation(sibling, left);
				sibling = parent->_right;

			}

			if (sibling->_right->_isRed == true) {

				// 형제의 우측 자식이 빨강일 경우
				sibling->_right->_isRed = false;
				leftRotation(parent, sibling);
				//eraseBalance(parent);

				return;
			}
		}
		else {
			// 형제가 좌측에 있을 경우
			if (sibling->_right->_isRed == true) {
				// 형제의 우측 자식이 빨강일 경우

				// 형제 - 형제의 자식 간 좌회전
				stNode* left = sibling->_left;
				stNode* right = sibling->_right;

				sibling->_isRed = true;
				right->_isRed = false;
				leftRotation(sibling, right);
				sibling = parent->_left;
			}
			if (sibling->_left->_isRed == true) {

				// 형제의 좌측 자식이 빨강일 경우
				sibling->_left->_isRed = false;
				rightRotation(parent, sibling);
				//eraseBalance(parent);

				return;
			}


		}
		sibling->_isRed = true;
		eraseBalance(parent);
		return;
	}

	if (sibling->_isRed == true) {
		// 형제가 빨강일 경우

		sibling->_isRed = false;
		parent->_isRed = true;


		if (_isNodeLeftParent == true) {
			// 부모 형제간 좌회전
			sibling->_isRed = false;
			leftRotation(parent, sibling);
		}
		else {

			// 부모 형제간 우회전
			rightRotation(parent, sibling);
		}

		eraseBalance(node);

	}


}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::diagnosis(stNode* node) {

	if (node->_left != nullptr) {

		if (node->_left->_parent != node) {
			// left의 부모가 내가 아닐 경우
			printf("{\n");
			printf("\t좌측 노드의 부모가 현재 노드가 아닙니다.\n");
			printf("\t현재 노드 값 : %d\n", node->_dataList->begin()->value);
			printf("\t좌측 노드 값 : %d\n", node->_left->_dataList->begin()->value);
			printf("\t좌측 노드 부모 값 : %d\n", node->_left->_parent->_dataList->begin()->value);
			printf("}\n");
			system("PAUSE>NUL");
		}

		if (node->_isRed == true) {
			if (node->_left->_isRed == true) {

				printf("{\n");
				printf("\t나와 내 왼쪽 노드가 빨강입니다.\n");
				printf("\tnode data: %d\n", node->_dataList->begin()->value);
				printf("}\n");
				system("PAUSE>NUL");
			}
		}

		diagnosis(node->_left);
	}
	else if (node->_isNill == false) {
		// left가 널인데, 내가 닐 노드가 아니면 오류
		printf("{\n");
		printf("\t좌측 노드가 Null 이지만 내가 닐 노드가 아닙니다.\n");
		printf("\tnode data: %d\n", node->_dataList->begin()->value);
		printf("}\n");
		system("PAUSE>NUL");
	}

	//printf("중위 순회, 현재 노드 값: %d\n", node->_data);

	if (node->_isNill == true) {
		if (node->_isRed == true) {
			// 닐노드 색 변경
			printf("{\n");
			printf("\tNill 노드가 빨강색입니다.\n");
			printf("\tnode data: %d\n", node->_dataList->begin()->value);
			printf("}\n");
			system("PAUSE>NUL");
		}
	}

	if (node->_right != nullptr) {

		if (node->_right->_parent != node) {
			// right의 부모가 내가 아닐 경우
			printf("{\n");
			printf("\t우측 노드의 부모가 현재 노드가 아닙니다.\n");
			printf("\t현재 노드 값 : %d\n", node->_dataList->begin()->value);
			printf("\t우측 노드 값 : %d\n", node->_right->_dataList->begin()->value);
			printf("\t우측 노드 부모 값 : %d\n", node->_right->_parent->_dataList->begin()->value);
			printf("}\n");
			system("PAUSE>NUL");
		}

		if (node->_isRed == true) {
			if (node->_right->_isRed == true) {

				printf("{\n");
				printf("\t나와 내 오른 노드가 빨강입니다.\n");
				printf("\tnode data: %d\n", node->_dataList->begin()->value);
				printf("}\n");
				system("PAUSE>NUL");
			}
		}

		diagnosis(node->_right);
	}
	else if (node->_isNill == false) {
		// right가 널인데, 내가 닐 노드가 아니면 오류
		printf("{\n");
		printf("\t우측 노드가 Null 이지만 내가 닐 노드가 아닙니다.\n");
		printf("\tnode data: %d\n", node->_dataList->begin()->value);
		printf("}\n");
		system("PAUSE>NUL");
	}

}

#if defined(_VECTOR_)
template <typename T, class C>
void CRedBlackTree_Multi<T, C>::treeToList(std::vector<T>* vector, std::vector<T>* blackNumList, stNode* node, int blackNum) {
	blackNum += (node->_isRed == false);
	if (node->_left->_isNill == false) {
		treeToList(vector, blackNumList, node->_left, blackNum);
	}
	else {
		blackNumList->push_back(blackNum);
	}

	T value = node->_dataList->begin()->value;
	vector->push_back(value);

	if (node->_right->_isNill == false) {
		treeToList(vector, blackNumList, node->_right, blackNum);
	}
	else {
		blackNumList->push_back(blackNum);
	}

}
#endif

#if defined(_WINDOWS_)
template <typename T, class C>
int CRedBlackTree_Multi<T, C>::printLoop(stNode* node, int deepth, int* printCnt, int x) {

	if (node->_left->_isNill == false) {
		int leftPrintCnt = printLoop(node->_left, deepth + 1, printCnt, x);

		// 좌측 노드의 간선 시작점
		int leftLineX = leftPrintCnt * 80 + 80 - (80 * 0.14); // right
		int leftLineY = (deepth + 1) * 80 + (80 * 0.14); // top

		// 현재 노드의 간선 시작점
		int nodeLineX = *printCnt * 80 + (80 * 0.14); // left
		int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

		MoveToEx(_hdc, leftLineX - x, leftLineY, nullptr);
		LineTo(_hdc, nodeLineX - x, nodeLineY);

	}

	RECT ellipseRect;
	ZeroMemory(&ellipseRect, sizeof(RECT));

	ellipseRect.left = *printCnt * 80;
	ellipseRect.top = deepth * 80;
	ellipseRect.right = ellipseRect.left + 80;
	ellipseRect.bottom = ellipseRect.top + 80;

	HBRUSH ellipseBrush;
	if (node->_isRed == true) {
		ellipseBrush = CreateSolidBrush(RGB(240, 70, 80));
		SetBkColor(_hdc, RGB(240, 70, 80));
		SetTextColor(_hdc, RGB(0, 0, 0));
	}
	else {
		ellipseBrush = CreateSolidBrush(RGB(43, 43, 43));
		SetBkColor(_hdc, RGB(43, 43, 43));
		SetTextColor(_hdc, RGB(255, 255, 255));
	}

	HBRUSH oldEllipseBrush = (HBRUSH)SelectObject(_hdc, ellipseBrush);
	Ellipse(_hdc, ellipseRect.left - x, ellipseRect.top, ellipseRect.right - x, ellipseRect.bottom);
	SelectObject(_hdc, oldEllipseBrush);
	DeleteObject(ellipseBrush);


	WCHAR text[10] = { 0, };
	_itow_s(node->_dataList->begin()->value, text, 10, 10);
	TextOutW(_hdc, ellipseRect.left + 40 - x, ellipseRect.top + 40, text, wcslen(text));

	SetTextColor(_hdc, RGB(0, 0, 0));
	SetBkColor(_hdc, RGB(255, 255, 255));
	int nodePrintCnt = *printCnt;
	*printCnt += 1;

	if (node->_right->_isNill == false) {
		int rightPrintCnt = printLoop(node->_right, deepth + 1, printCnt, x);

		// 우측 노드의 간선 시작점
		int rightLineX = rightPrintCnt * 80 + (80 * 0.14); // left
		int rightLineY = (deepth + 1) * 80 + (80 * 0.14); // top

		// 노드의 간선 시작점
		int nodeLineX = nodePrintCnt * 80 + 80 - (80 * 0.14); // right
		int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

		MoveToEx(_hdc, rightLineX - x, rightLineY, nullptr);
		LineTo(_hdc, nodeLineX - x, nodeLineY);

	}

	return nodePrintCnt;

}
#endif

template <typename T, class C>
T* CRedBlackTree_Multi<T, C>::inorder() {

#if defined(SPEED_TEST)
	sp->profileBegin("Inorder");
#endif

	T* arr = new T[_nodeNum];
	int index = 0;

	inorderLoop(arr, &index, _root);

#if defined(SPEED_TEST)
	sp->profileEnd("Inorder");
#endif

	return arr;

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::inorderLoop(T* arr, int* index, stNode* node) {

	stNode* left = node->_left;
	stNode* right = node->_right;

	if (left->_isNill == false) {

		inorderLoop(arr, index, left);

	}

	arr[*index] = node->_dataList->begin()->value;
	*index += 1;

	if (right->_isNill == false) {

		inorderLoop(arr, index, right);

	}

}

template <typename T, class C>
void CRedBlackTree_Multi<T, C>::clear() {

	if (_root->_isNill == true) {
		freeNode(_root);

		//delete(_root);
		return;
	}

	while (_root->_left->_isNill == false || _root->_right->_isNill == false) {
		erase(_root->_dataList->begin()->value);
	}

	freeNode(_root->left);
	freeNode(_root->right);
	freeNodE(_root);

	//delete(_root->_left);
	//delete(_root->_right);
	//delete(_root);

	_root = allocNode(false, true);
	//_root = nodeFreeList->allocObject();// new stNode(0, false, true);
	//new (_root) stNode(nodeFreeList, false, true);

	_nodeNum = 0;

}

template <typename T, class C>
int CRedBlackTree_Multi<T, C>::getNodeNum() {
	return _nodeNum;
}

template <typename T, class C>
bool CRedBlackTree_Multi<T, C>::empty() {
	return _nodeNum == 0;
}
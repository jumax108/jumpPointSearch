#pragma once

template<typename T>
class CRedBlackTree {

public:

	struct stNode {

		stNode(T data, bool isRed = true, bool isNill = false, stNode* parent = nullptr) {
			_data = data;
			if (isNill == false) {
				_left = new stNode(0, false, true, this);
				_right = new stNode(0, false, true, this);
			}
			else {
				_left = nullptr;
				_right = nullptr;
			}
			_parent = parent;
			_isRed = isRed;
			_isNill = isNill;
		}

		T _data;
		stNode* _left;
		stNode* _right;
		stNode* _parent;
		bool _isRed;
		bool _isNill;

	};

	CRedBlackTree() {
		_root = new stNode(0, false, true);
	}

	void insert(T data) {

		stNode* newNode = new stNode(data);

		if (_root->_isNill == true) {
			delete(_root);
			newNode->_isRed = false;
			_root = newNode;
			return;
		}

		stNode* node = _root;
		while (node->_data != data) {
			if (node->_data < data) {
				if (node->_right->_isNill == true) {
					delete(node->_right);
					node->_right = newNode;
					newNode->_parent = node;;
					insertBalance(newNode);
					return;
				}
				node = node->_right;
			}
			else if (node->_data > data) {
				if (node->_left->_isNill == true) {
					delete(node->_left);
					node->_left = newNode;
					newNode->_parent = node;
					insertBalance(newNode);
					return;
				}
				node = node->_left;
			}
		}

		delete(newNode);
	}

	void erase(T data) {

		stNode** node = &_root;

		while ((*node)->_isNill == false) {

			if ((*node)->_data < data) {
				//right
				node = &(*node)->_right;
			}
			else if ((*node)->_data > data) {
				//left
				node = &(*node)->_left;
			}
			else {
				// correct
				bool isRed = node->_isRed;
				eraseNode(node);
				if (!isRed) {
					eraseBalance(*node);
				}
				break;
			}
		}

	}

#ifdef _WINDOWS_
	void print(HDC hdc) {

		if (_root->_isNill == true) {
			return;
		}

		_hdc = hdc;
		int printCnt = 0;
		printLoop(_root, 0, &printCnt);

	}

	static CRedBlackTree<T>* test(HDC hdc, HWND hWnd) {

		srand(1000);

		constexpr int dataNum = 10;
		constexpr int eraseNum = 3;

		std::vector<int> insertData;
		std::vector<int> eraseIndex;
		std::vector<int> ableData;

		CRedBlackTree<int>* tree = new CRedBlackTree<int>();

		for (int dataCnt = 0; dataCnt < dataNum; ++dataCnt) {
			ableData.push_back(dataCnt);
		}

		printf("Insert Data: ");
		for (int dataCnt = 0; dataCnt < dataNum; ++dataCnt) {
			int index = rand() % ableData.size();
			int data = ableData[index];
			ableData.erase(ableData.begin() + index);
			insertData.push_back(data);
			tree->insert(data);
			printf("%d ", data);
		}
		printf("\n");

		for (int eraseCnt = 0; eraseCnt < eraseNum; ++eraseCnt) {
			eraseIndex.push_back(rand() % (dataNum - eraseCnt));
		}

		for (std::vector<int>::iterator erase = eraseIndex.begin(); erase != eraseIndex.end(); ++erase) {

			InvalidateRect(hWnd, nullptr, true);
			tree->print(hdc);

			int data = *(insertData.begin() + *erase);
			insertData.erase(insertData.begin() + *erase);
			tree->erase(data);

		}

		std::sort(insertData.begin(), insertData.end());
		printf("correct answer: ");
		for (std::vector<int>::iterator data = insertData.begin(); data != insertData.end(); ++data) {
			printf("%d ", *data);
		}
		printf("\n");

		return tree;
	}
#endif

private:

	stNode* _root;

	void eraseNode(stNode** node) {

		stNode* left = (*node)->_left;
		stNode* right = (*node)->_right;

		bool isLeftNill = left->_isNill;
		bool isRightNill = right->_isNill;

		if (isLeftNill == true) {

			// 모두 닐인 경우는 어짜피 여기서 부모에서 내 노드가 연결되어 있던 곳이 닐로 연결됨
			// 노드의 우측 자식 존재

			delete(left); // 좌측 닐 제거
			delete(*node); // 현재 노드 제거
			*node = right; // 부모 노드가 바라보고 있는 노드를 우측 노드로 변경

		}
		else if (isRightNill == true) {

			// 노드의 좌측 자식 존재

			delete(right); // 우측 닐 제거
			delete(*node); // 현재 노드 제거
			*node = left;  // 부모가 바라보고 있는 노드를 좌측 노드로 변경

		}
		else {

			// 노드의 양쪽 자식 존재
			// 노드의 왼쪽 자식의 최우측 자식과 데이터 교체

			if (left->_right->_isNill == true) {

				// 노드의 왼쪽 자식의 우측 자식이 없을 때

				// 노드의 왼쪽 자식과 노드의 데이터를 교체하고
				// 노드의 왼쪽 자식의 왼쪽 자식을 노드와 연결

				(*node)->_data = left->_data; // 노드의 데이터를 왼쪽 자식의 데이터로 변경
				(*node)->_left = left->_left; // 노드의 왼쪽 자식을 왼쪽 자식의 왼쪽 자식으로 변경
				delete(left); // 왼쪽 노드 제거

			}
			else {

				// 노드의 왼쪽 자식의 우측 자식이 있을 때

				// 최우측 자식을 찾기 위해 루프
				stNode** lastRightNode = &left; // 가장 마지막 오른쪽 자식
				while ((*lastRightNode)->_right->_isNill == false) { // 우측 자식이 닐이 아니면
					lastRightNode = &((*lastRightNode)->_right); // 우측 자식으로 교체
				}

				(*node)->_data = (*lastRightNode)->_data; // 노드의 값을 최우측노드의 값으로 변경

				// 최우측 노드의 좌측 노드 존재 가능
				// 해당 노드는 최우측 노드의 부모와 연결
				// 닐이면 닐 연결됨

				stNode* lastNodeLeft = (*lastRightNode)->_left; // 최우측 노드의 좌측 노드
				delete(*lastRightNode); // 최우측 노드 제거
				*lastRightNode = lastNodeLeft; // 최우측 노드의 부모 노드와 최우측 노드의 좌측 노드 연결

			}

		}

	}


	void insertBalance(stNode* node) {

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
					parent->right = node->_left;

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

			if (grand->_parent->_left == grand) {
				grand->_parent->_left = parent;
			}
			else {
				grand->_parent->_right = parent;
			}


			if (isNodeLeftGrand == true) {
				// 노드가 조부 왼쪽에 있을 경우
				grand->_left = sibling;
				grand->_parent = parent;
				parent->_right = grand;

			


			}
			else {
				// 노드가 조부 오른쪽에 있을 경우
				grand->_right = sibling;
				grand->_parent = parent;
				parent->left = grand;

			}

			grand->_isRed = true;
			parent->_isRed = false;

			return;
		}



	}

	void eraseBalance(stNode* node) {

		if (node == _root) {
			_root->_isRed = false;
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
			parent->_isRed = false;
			sibling->_isRed = true;
			return;
		}

		// 부모가 검정일 경우

		if (sibling->_isRed == false) {
			// 형제가 검정일 경우
			sibling->_isRed = true;
		}

		if (sibling->_isRed == true) {
			// 형제가 빨강일 경우

			if (_isNodeLeftParent == true) {
				// 부모 형제간 좌회전
				sibling->_isRed = false;

				// 조부모의 자식 노드를 형제로 변경
				if (parent->_parent->_left == parent) {
					parent->_parent->_left = sibling;
				}
				else {
					parent->_parent->_right = sibling;
				}
				sibling->_parent = parent->_parent;// 형제의 부모를 조부모로 변경

				parent->_parent = sibling;// 조부모를 형제로 변경

				parent->_right = sibling->_left; // 부모의 오른 자식을 형제의 왼자식으로 변경
				sibling->_left->_parent = parent; // 형제의 자식의 부모를 부모로 변경

				sibling->_left = parent; // 형제의 왼자식을 부모로 변경
			}
			else {

				// 부모 형제간 우회전
				sibling->_isRed = false;

				// 조부모의 자식 노드를 형제로 변경
				if (parent->_parent->_left == parent) {
					parent->_parent->_left = sibling;
				}
				else {
					parent->_parent->_right = sibling;
				}
				sibling->_parent = parent->_parent;// 형제의 부모를 조부모로 변경

				parent->_parent = sibling;// 조부모를 형제로 변경

				parent->_left = sibling->_right; // 부모의 왼자식을 형제의 오른자식으로 변경
				sibling->_right->_parent = parent; // 형제의 자식의 부모를 부모로 변경

				sibling->_right = parent; // 형제의 오른자식을 부모로 변경
			}


		}


	}


#ifdef _WINDOWS_
	HDC _hdc;
	int printLoop(stNode* node, int deepth, int* printCnt) {

		if (node->_left->_isNill == false) {
			int leftPrintCnt = printLoop(node->_left, deepth + 1, printCnt);

			// 좌측 노드의 간선 시작점
			int leftLineX = leftPrintCnt * 80 + 80 - (80 * 0.14); // right
			int leftLineY = (deepth + 1) * 80 + (80 * 0.14); // top

			// 현재 노드의 간선 시작점
			int nodeLineX = *printCnt * 80 + (80 * 0.14); // left
			int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

			MoveToEx(_hdc, leftLineX, leftLineY, nullptr);
			LineTo(_hdc, nodeLineX, nodeLineY);

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
		Ellipse(_hdc, ellipseRect.left, ellipseRect.top, ellipseRect.right, ellipseRect.bottom);
		SelectObject(_hdc, oldEllipseBrush);
		DeleteObject(ellipseBrush);

		WCHAR text[10] = { 0, };
		_itow_s(node->_data, text, 10, 10);
		TextOutW(_hdc, ellipseRect.left + 40, ellipseRect.top + 40, text, wcslen(text));

		int nodePrintCnt = *printCnt;
		*printCnt += 1;

		if (node->_right->_isNill == false) {
			int rightPrintCnt = printLoop(node->_right, deepth + 1, printCnt);

			// 우측 노드의 간선 시작점
			int rightLineX = rightPrintCnt * 80 + (80 * 0.14); // left
			int rightLineY = (deepth + 1) * 80 + (80 * 0.14); // top

			// 노드의 간선 시작점
			int nodeLineX = nodePrintCnt * 80 + 80 - (80 * 0.14); // right
			int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

			MoveToEx(_hdc, rightLineX, rightLineY, nullptr);
			LineTo(_hdc, nodeLineX, nodeLineY);

		}

		return nodePrintCnt;

	}
#endif
};
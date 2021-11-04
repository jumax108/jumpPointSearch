#include "framework.h"
#include "SimpleProfiler.h"
#include "linkedList.h"
#include "ObjectFreeList.h"
#include "RedBlackTree_Multi.h"
#include "JumpPointSearch.h"

#define abs(x) ((x) < 0 ? -(x) : (x))

CJumpPointSearch::CJumpPointSearch(int width, int height) {

	_width = width;
	_height = height;

	int mapByte = sizeof(MAP_STATE) * _width * _height;
	_map = (MAP_STATE*)malloc(mapByte);
	ZeroMemory(_map, mapByte);

	int mapColorByte = sizeof(stRGB) * _width * _height;
	_mapColor = (stRGB*)malloc(mapColorByte);
	ZeroMemory(_mapColor, mapColorByte);

	int lineColorByte = sizeof(stRGB) * _width * _height;
	_lineColor = (stRGB*)malloc(lineColorByte);
	ZeroMemory(_lineColor, lineColorByte);

	int nodeMapByte = sizeof(stNode*) * _width * _height;
	_nodeMap = (stNode**)malloc(nodeMapByte);
	ZeroMemory(_nodeMap, nodeMapByte);

	_openList = new CLinkedList<stNode*>();
	//_openList = new CRedBlackTree_Multi<stNode*, CJpsLess>(CJpsLess());


	_path = new CLinkedList<stNode*>();
	_line = new CLinkedList<stNode*>();

	_start._x = -1;
	_start._y = -1;

	_end._x = -1;
	_end._y = -1;

	nodeFreeList = new CObjectFreeList<stNode>(_width * _height);
	coordFreeList = new CObjectFreeList<stCoord>(_width * _height);

}

void CJumpPointSearch::pathFindInit() {

	int mapColorByte = sizeof(stRGB) * _width * _height;
	ZeroMemory(_mapColor, mapColorByte);
	int lineColorByte = sizeof(stRGB) * _width * _height;
	ZeroMemory(_lineColor, lineColorByte);
	listClear();
	int nodeMapByte = sizeof(stNode*) * _width * _height;
	ZeroMemory(_nodeMap, nodeMapByte);

	stCoord* coord = coordFreeList->allocObject();
	new (coord) stCoord(_start._y, _start._x);

	stNode* node = nodeFreeList->allocObject();
	new (node) stNode(nullptr, 0, abs(_end._x - _start._x) + abs(_end._y - _start._y), coord);

	_nodeMap[_start._y * _width + _start._x] = node;// new stNode(nullptr, 0, abs(_end._x - _start._x) + abs(_end._y - _start._y), new stCoord(_start._y, _start._x));
	_openList->push_back(node);
	//_openList->insert(node);
}

bool CJumpPointSearch::isNodeInList(stCoord* coord, CLinkedList<stNode*>* list) {

	for (CLinkedList<stNode*>::iterator iter = list->begin(); iter != list->end(); ++iter) {
		stCoord* coordInList = (*iter)->_coord;
		if (coordInList->_y == coord->_y  && coordInList->_x == coord->_x) {
			return true;
		}
	}

	return false;

}

void CJumpPointSearch::makeNode(stCoord* corner, stNode* parent) {

	int cornerY = corner->_y;
	int cornerX = corner->_x;

	stNode** newNode = &_nodeMap[cornerY * _width + cornerX];

	if (*newNode != nullptr) {
		return;
	}

	int moveCnt = parent->_moveCnt + 1;
	int distance = abs(_end._y -  cornerY) + abs(_end._x - cornerX);

    *newNode = nodeFreeList->allocObject();
	new (*newNode) stNode(parent, moveCnt, distance, corner);

	_openList->push_back(*newNode);
	if((*newNode)->_coord->_x == 46 && (*newNode)->_coord->_y == 23){
		int k = 1;
	}
	//_openList->insert(*newNode);

}

CJumpPointSearch::stNode* CJumpPointSearch::pathFindSingleLoop() {

	#ifdef SPEED_TEST
		sp->profileBegin("singleLoop");
	#endif

	CLinkedList<stNode*>::iterator minIter = *findMin(_openList);
	//stNode* minNode = findMin(_openList);
	stNode* selectNode = *minIter;
	//stNode* selectNode = minNode;
	//_openList->erase(minNode);
	_openList->erase(minIter);

	if(selectNode->_coord->_x == 46 && selectNode->_coord->_y == 23){
		int k =1;
	}

	stCoord* coord = selectNode->_coord;

	if (coord->_y == _end._y && coord->_x == _end._x) {

		#ifdef SPEED_TEST
			sp->profileEnd("singleLoop");
		#endif
		return selectNode;
	}

	stCoord* leftResult;
	stCoord* rightResult;
	stCoord* upResult;
	stCoord* downResult;

	stCoord* leftUpResult;
	stCoord* rightUpResult;
	stCoord* leftDownResult;
	stCoord* rightDownResult;

	int y = coord->_y;
	int x = coord->_x;

	stRGB color;
	color.red   = rand() % 200 + 55;
	color.green = rand() % 200 + 55;
	color.blue  = rand() % 200 + 55;

	switch (coord->_dir) {
		case DIRECTION::NONE:
		{
			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
			if (leftUpResult != nullptr) {
				makeNode(leftUpResult, selectNode);
			}

			leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
			if (leftDownResult != nullptr) {
				makeNode(leftDownResult, selectNode);
			}

			rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
			if (rightUpResult != nullptr) {
				makeNode(rightUpResult, selectNode);
			}

			rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
			if (rightDownResult != nullptr) {
				makeNode(rightDownResult, selectNode);
			}

			break;
		}
		case DIRECTION::LEFT:
		{
			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			if (y+1 < _height && *map(y+1, x) == MAP_STATE::WALL) {
				// 하단에 벽이 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

		}
			break;

		case DIRECTION::RIGHT:
		{
			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			if (y + 1 < _height && *map(y + 1, x) == MAP_STATE::WALL) {
				// 하단에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}

			}
		}
		break;
		case DIRECTION::UP:
		{

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				// 좌측에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

		}
		break;
		case DIRECTION::DOWN:
		{
			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				//좌측에 벽 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
			}
		}
		break;
		case DIRECTION::LEFT_DOWN:
		{
			leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
			if (leftDownResult != nullptr) {
				makeNode(leftDownResult, selectNode);
			}

			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
				
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

		}
		break;
		case DIRECTION::LEFT_UP:
		{
			leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
			if (leftUpResult != nullptr) {
				makeNode(leftUpResult, selectNode);
			}

			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}
			}

			if (y + 1 < _height && *map(y + 1, x) == MAP_STATE::WALL) {
				// 하단에 벽 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
				
			}
		}
		break;
		case DIRECTION::RIGHT_DOWN:
		{
			rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
			if (rightDownResult != nullptr) {
				makeNode(rightDownResult, selectNode);
			}

			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				// 좌측에 벽 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
				
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}
			}
		}
		break;
		case DIRECTION::RIGHT_UP:
		{
			rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
			if (rightUpResult != nullptr) {
				makeNode(rightUpResult, selectNode);
			}

			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				// 좌측에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

			if (y + 1 < _height && *map(y + 1, x) == MAP_STATE::WALL) {
				//하단에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
			}
		}
		break;

	}

	if (_openList->empty() == false) {
		// 아직 확인할 노드가 남음
		#ifdef SPEED_TEST
			sp->profileEnd("singleLoop");
		#endif
		return (stNode*)1;
	}

	#ifdef SPEED_TEST
		sp->profileEnd("singleLoop");
	#endif
	return nullptr;
}

CJumpPointSearch::stCoord* CJumpPointSearch::checkOrthogonal(DIRECTION dir, int y, int x, const stRGB* color) {

	#ifdef SPEED_TEST
		sp->profileBegin("checkOrthogonal");
	#endif
	int tyArr[4] = { 0,0,-1,1 };
	int txArr[4] = { -1,1,0,0 };

	int ty = tyArr[(int)dir];
	int tx = txArr[(int)dir];


	for (;;) {

		y += ty;
		x += tx;

		if (y < 0 || x < 0 || y >= _height || x >= _width) {
			// 맵 끝까지 감
			#ifdef SPEED_TEST
				sp->profileEnd("checkOrthogonal");
			#endif
			return nullptr;
		}

		if (*map(y, x) == MAP_STATE::WALL) {
			// 길 막힘
			#ifdef SPEED_TEST
				sp->profileEnd("checkOrthogonal");
			#endif
			return nullptr;
		}

		stRGB* pMapColor = mapColor(y, x);
		pMapColor->red = color->red;
		pMapColor->blue = color->blue;
		pMapColor->green = color->green;

		if (y == _end._y && x == _end._x) {
			// 목표지점 발견
			#ifdef SPEED_TEST
				sp->profileEnd("checkOrthogonal");
			#endif
			stCoord* coord = coordFreeList->allocObject();
 			new (coord) stCoord(y, x, dir);
			return coord;
		}

		// y - ty 등 계산 예외처리를 한 번에 하면, 
		// 맵 끝자락에서 코너를 발견했을 때 아래 오류가 발견됨.

		/*
		* 아래 상황에서 코너이나 노드 아래에 대한 계산식이 맵 밖이라 break 되었음
		* 0 벽 0
		* 0 녿 0
		* --------- < 맵 끝
		*/

		// 그래서 이동 중 코너 체크를 분리함

		if (0 <= y + tx && y + tx < _height &&
			0 <= x + ty && x + ty < _width &&
			0 <= y + tx + ty && y + tx + ty < _height &&
			0 <= x + ty + tx && x + ty + tx < _width) {

			if (*map(y + tx, x + ty) == MAP_STATE::WALL && *map(y + tx + ty, x + ty + tx) == MAP_STATE::ROAD) {
				// 코너 발견
				#ifdef SPEED_TEST
					sp->profileEnd("checkOrthogonal");
				#endif
				stCoord* coord = coordFreeList->allocObject();
				new (coord) stCoord(y, x, dir);
				return coord;
			}
		}


		if (0 <= y - tx && y - tx < _height &&
			0 <= x - ty && x - ty < _width &&
			0 <= y - tx + ty && y - tx + ty < _height &&
			0 <= x - ty + tx && x - ty + tx < _width) {

			if (*map(y - tx, x - ty) == MAP_STATE::WALL && *map(y - tx + ty, x - ty + tx) == MAP_STATE::ROAD) {
				// 코너 발견
				#ifdef SPEED_TEST
					sp->profileEnd("checkOrthogonal");
				#endif
				stCoord* coord = coordFreeList->allocObject();
				new (coord) stCoord(y, x, dir);
				return coord;
			}
		}

	}

	#ifdef SPEED_TEST
		sp->profileEnd("checkOrthogonal");
	#endif
	return nullptr;
}

CJumpPointSearch::stCoord* CJumpPointSearch::checkDiagonal(DIRECTION dir, int y, int x, const stRGB* color) {

	#ifdef SPEED_TEST
	sp->profileBegin("checkDiagonal");
	#endif
	int tyArr[4] = {-1, -1, 1, 1};
	int txArr[4] = {-1, 1, -1, 1};

	dir = (DIRECTION)((int)dir - (int)DIRECTION::LEFT_UP);

	int ty = tyArr[(int)dir];
	int tx = txArr[(int)dir];

	/* 
	LEFT_UP
	RIGHT_UP
	LEFT_DOWN
	RIGHT_DOWN
	*/

	DIRECTION lineCheckDir[4][2] = {
		{DIRECTION::LEFT, DIRECTION::UP},
		{DIRECTION::RIGHT, DIRECTION::UP},
		{DIRECTION::LEFT, DIRECTION::DOWN},
		{DIRECTION::RIGHT, DIRECTION::DOWN}
	};

	for (;;) {

		y += ty;
		x += tx;


		if (y < 0 || x < 0 || y >= _height || x >= _width) {
			// 맵 끝까지 감
			#ifdef SPEED_TEST
				sp->profileEnd("checkDiagonal");
			#endif
			return nullptr;
		}

		if (*map(y, x) == MAP_STATE::WALL) {
			// 길 막힘
			#ifdef SPEED_TEST
				sp->profileEnd("checkDiagonal");
			#endif
			return nullptr;
		}

		stRGB* pMapColor = mapColor(y, x);
		pMapColor->red = color->red;
		pMapColor->blue = color->blue;
		pMapColor->green = color->green;
		if (y == _end._y && x == _end._x) {
			// 목표지점 발견
			#ifdef SPEED_TEST
				sp->profileEnd("checkDiagonal");
			#endif
			stCoord* coord = coordFreeList->allocObject();
			new (coord) stCoord(y, x, dir);
			return coord;
		}

		for (int lineCheckCnt = 0; lineCheckCnt < 2; ++lineCheckCnt) {
			// 대각선 이동 중 저 멀리서 코너 발견함
			stCoord* lineCheckResult = checkOrthogonal(lineCheckDir[(int)dir][lineCheckCnt], y, x, color);
			if (lineCheckResult != nullptr) {
				stCoord* result = coordFreeList->allocObject();
				new (result) stCoord(y, x, (DIRECTION)((int)dir + (int)DIRECTION::LEFT_UP));
				coordFreeList->freeObject(lineCheckResult);
				#ifdef SPEED_TEST
					sp->profileEnd("checkDiagonal");
				#endif		
				return result;
			}
		}

		

		// y - ty 등 계산 예외처리를 한 번에 하면, 
		// 맵 끝자락에서 코너를 발견했을 때 아래 오류가 발견됨.

		/*
		* 아래 상황에서 코너이나 노드 아래에 대한 계산식이 맵 밖이라 break 되었음
		* 0 벽 0
		* 0 녿 0
		* --------- < 맵 끝
		*/

		// 그래서 이동 중 코너 체크를 분리함

		if (0 <= y - ty && y - ty < _height &&
			0 <= x + tx && x + tx <  _width) {
			if ((*map(y - ty, x) == MAP_STATE::WALL && *map(y - ty, x + tx) == MAP_STATE::ROAD)) {
				// 대각선 이동 중 코너 발견함
				#ifdef SPEED_TEST
					sp->profileEnd("checkDiagonal");
				#endif
				stCoord* coord = coordFreeList->allocObject();
				new (coord) stCoord(y, x, (DIRECTION)((int)dir + (int)DIRECTION::LEFT_UP));
				return coord;
			}
		}

		if (0 <= x - tx && x - tx < _width &&
			0 <= y + ty && y + ty < _height) {
			if (*map(y, x - tx) == MAP_STATE::WALL && *map(y + ty, x - tx) == MAP_STATE::ROAD) {
				// 대각선 이동 중 코너 발견함
				#ifdef SPEED_TEST
					sp->profileEnd("checkDiagonal");
				#endif		
				stCoord* coord = coordFreeList->allocObject();
				new (coord) stCoord(y, x, (DIRECTION)((int)dir + (int)DIRECTION::LEFT_UP));
				return coord;
			}
		}


	}

	#ifdef SPEED_TEST
		sp->profileEnd("checkDiagonal");
	#endif
	return nullptr;
}

CLinkedList<CJumpPointSearch::stNode*>::iterator* CJumpPointSearch::findMin(CLinkedList<stNode*>* list) {

	int minValue = 0x7FFFFFFF;
	CLinkedList<stNode*>::iterator minIter;
	for (CLinkedList<stNode*>::iterator iter = list->begin(); iter != list->end(); ++iter) {
		int distance = (*iter)->_distance;
		int moveCnt = (*iter)->_moveCnt;
		int fValue = distance + moveCnt;
		if (minValue > fValue) {
			minValue = fValue;
			minIter = iter;
		}
	}

	return &minIter;
}

CJumpPointSearch::stNode* CJumpPointSearch::findMin(CRedBlackTree_Multi<CJumpPointSearch::stNode*, CJpsLess>* list){

	CRedBlackTree_Multi<CJumpPointSearch::stNode*, CJpsLess>::stNode* node = list->_root;

	if(node == nullptr){
		return nullptr;
	}

	while(node->_isNill != true){


		if(node->_left->_isNill == true){
			return node->_dataList->begin()->value;
		}

		node = node->_left;

	}

	return nullptr;


}

#ifdef _WINDOWS_
void CJumpPointSearch::print(HDC hdc, int blockSize, iterator endNodeIter) {
	
	{
		// 선 그리기
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		for (int heightCnt = 0; heightCnt < _height + 1; ++heightCnt) {

			int y = heightCnt * blockSize;
			MoveToEx(hdc, 0, y, nullptr);
			LineTo(hdc, _width * blockSize, y);

		}

		for (int widthCnt = 0; widthCnt < _width + 1; ++widthCnt) {

			int x = widthCnt * blockSize;
			MoveToEx(hdc, x, 0, nullptr);
			LineTo(hdc, x, _height * blockSize);

		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

	}

	{
		// 맵 색 채우기

		for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				stRGB* pMapColor = mapColor(heightCnt, widthCnt);
				if (pMapColor->red != 0 && pMapColor->blue != 0 && pMapColor->green != 0) {
					HBRUSH hBrush = CreateSolidBrush(RGB(pMapColor->red, pMapColor->green, pMapColor->blue));
					HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

					int left = widthCnt * blockSize + 1;
					int top = heightCnt * blockSize + 1;
					int right = left + blockSize - 1;
					int bottom = top + blockSize - 1;

					Rectangle(hdc, left, top, right, bottom);

					SelectObject(hdc, hOldBrush);
					DeleteObject(hBrush);
				}
			}
		}

	}



	do{
		// 오픈 리스트 그리기

		if (_openList->empty() == true) {
			break;
		}

		HBRUSH hBrush = CreateSolidBrush(RGB(150, 210, 230));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		
		//stNode** arr = _openList->inorder();
		//stNode** arrEnd = arr + _openList->getNodeNum();
		//stNode** arrIter = arr;

		//for (; arrIter != arrEnd; ++arrIter){

		//  stCoord* coord = (*arrIter)->_coord;

		CLinkedList<stNode*>::iterator iter = _openList->begin();

		for(; iter != _openList->end(); ++iter){

			stCoord* coord = (*iter)->_coord;
			int y = coord->_y;
			int x = coord->_x;

			int left = x * blockSize + 1;
			int right = left + blockSize - 1;
			int top = y * blockSize + 1;
			int bottom = top + blockSize - 1;

			bool result = Ellipse(hdc, left, top, right, bottom);
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}while(false);

	{
		// 클로즈 리스트 그리기
		HBRUSH hBrush = CreateSolidBrush(RGB(220, 220, 0));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);


		for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				stNode* node = getNode(heightCnt, widthCnt);
				if (node == nullptr || node->_parent == nullptr) {
					continue;
				}
				stCoord* coord = node->_coord;
				int y = coord->_y;
				int x = coord->_x;

				int left = x * blockSize + 1;
				int right = left + blockSize - 1;
				int top = y * blockSize + 1;
				int bottom = top + blockSize - 1;

				bool result = Ellipse(hdc, left, top, right, bottom);
			}
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}


	do {
		// 시작 지점 그리기

		if (_start._x == -1 || _start._y == -1) {
			break;
		}
		HBRUSH hBrush = CreateSolidBrush(RGB(50, 200, 50));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		int left = _start._x * blockSize + 1;
		int right = left + blockSize - 1;
		int top = _start._y * blockSize + 1;
		int bottom = top + blockSize - 1;

		bool result = Ellipse(hdc, left, top, right, bottom);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	} while (false);

	do {
		// 끝 지점 그리기

		if (_end._x == -1 || _end._y == -1) {
			break;
		}

		HBRUSH hBrush = CreateSolidBrush(RGB(200, 50, 50));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		int left = _end._x * blockSize + 1;
		int right = left + blockSize - 1;
		int top = _end._y * blockSize + 1;
		int bottom = top + blockSize - 1;

		bool result = Ellipse(hdc, left, top, right, bottom);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	} while (false);

	do {

		// 선으로 판정된 노드를 칠해보자
		
		for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {

				stRGB* rgb = lineColor(heightCnt, widthCnt);

				if (rgb->red == 0 && rgb->blue == 0 && rgb->green == 0) {
					continue;
				}

				HBRUSH hBrush = CreateSolidBrush(RGB(rgb->red, rgb->green, rgb->blue));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

				int left = widthCnt * blockSize + 1;
				int top = heightCnt * blockSize + 1;
				int right = left + blockSize - 1;
				int bottom = top + blockSize - 1;

				Rectangle(hdc, left, top, right, bottom);

				SelectObject(hdc, hOldBrush);
				DeleteObject(hBrush);

			}
		}


	} while (false);

	do{
		break;
		// 각 노드의 부모를 이어보자 !

		HPEN hPen = CreatePen(PS_SOLID, blockSize / 8, RGB(190, 110, 190));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		if (_openList->empty() == false) {

			CLinkedList<stNode*>::iterator iter = _openList->begin();

			for (; iter != _openList->end(); ++iter) {

				stCoord* coord = (*iter)->_coord;
				stCoord* parentCoord = (*iter)->_parent->_coord;
				/*
				if ((*arrIter)->_parent == nullptr) {
					continue;
				}

				stCoord* coord = (*arrIter)->_coord;
				stCoord* parentCoord = (*arrIter)->_parent->_coord;
				*/

				MoveToEx(hdc, coord->_x * blockSize + blockSize / 2, coord->_y * blockSize + blockSize / 2, nullptr);
				LineTo(hdc, parentCoord->_x * blockSize + blockSize / 2, parentCoord->_y * blockSize + blockSize / 2);
			}
		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
	} while (false);

	{
		// 벽 그리기

		HBRUSH hBrush = CreateSolidBrush(RGB(200, 200, 200));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				if (*map(heightCnt, widthCnt) == MAP_STATE::WALL) {
					int left = widthCnt * blockSize + 1;
					int top = heightCnt * blockSize + 1;
					int right = left + blockSize - 1;
					int bottom = top + blockSize - 1;
					Rectangle(hdc, left, top, right, bottom);
				}
			}
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}
	{

		// 확정된 길을 이어봅니다 ~

		HPEN hPen = CreatePen(PS_SOLID, blockSize / 4, RGB(240, 70, 70));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		if (endNodeIter != pathEnd()) {
			iterator nodeIter = pathBegin();
			++nodeIter;
			for(; nodeIter != pathEnd(); ++nodeIter) {
				stNode* node = *nodeIter;

				stCoord* coord = node->_coord;
				stCoord* parentCoord = node->_parent->_coord;

				MoveToEx(hdc, coord->_x * blockSize + blockSize / 2, coord->_y * blockSize + blockSize / 2, nullptr);
				LineTo(hdc, parentCoord->_x * blockSize + blockSize / 2, parentCoord->_y * blockSize + blockSize / 2);

			}

		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

	}

	{

		int left = windowRect.right - 230;
		int right = windowRect.right - 5;
		int top = 0;
		int bottom = 195;

		Rectangle(hdc, left, top, right, bottom);

		int strHeight = top + 10;
		TextOutW(hdc, left + 10, strHeight, L"좌클릭: 벽 생성 / 제거 ", 15); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"우클릭: 길 찾기 시작", 12); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"마우스휠: 블럭 크기 조절", 14); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"s: 출발 지점 설정", 11); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"e: 도착 지점 설정", 11); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"c: 길찾기 정보 초기화", 13); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"z: 완전 초기화", 9); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"t: 테스트 코드 실행", 12); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"b: 비트맵으로 출력", 11); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"p: 시작, 끝 지점을 이어보기", 17); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"화살표: 맵 크기 변경", 12); strHeight += 15;
		TextOutW(hdc, left + 10, strHeight, L"l: 노드 스킵", 8); strHeight += 15;

	}

}
#endif

void CJumpPointSearch::listClear() {

	_openList->clear();
	//_openList->~CRedBlackTree_Multi();
	new (_openList) CRedBlackTree_Multi<stNode*, CJpsLess>(CJpsLess());

	for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
		for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
			stNode* node = getNode(heightCnt, widthCnt);
			if(node == nullptr){
				continue;
			}
			coordFreeList->freeObject(node->_coord);
			nodeFreeList->freeObject(node);
		}
	}

}

CJumpPointSearch::iterator CJumpPointSearch::pathFind() {

	#ifdef SPEED_TEST
		sp->profileBegin("JumpPointSearch");
	#endif
	pathFindInit();
	void* singleLoopResult = nullptr;
	do {
		singleLoopResult = pathFindSingleLoop();
		// 결과가 1 이면 반복해야함
	} while ((int)singleLoopResult == 1);

	if (singleLoopResult == nullptr) {
		return pathEnd();
	}

	#ifdef SPEED_TEST
		sp->profileEnd("JumpPointSearch");
	#endif
	// 못찾으면 nullptr, 찾으면 도착 지점 노드
	return makePath((stNode*)singleLoopResult);

}

void CJumpPointSearch::test(const WCHAR* fileName) {

	MAP_STATE patterns[][3][3] = {
		{
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD},
			{MAP_STATE::WALL, MAP_STATE::WALL, MAP_STATE::WALL},
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD}
		} ,
		{
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD}
		}, {
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD},
			{MAP_STATE::WALL, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD}
		}
	};

	int width = 90;
	int height = 90;

	CJumpPointSearch* jps = new CJumpPointSearch(width, height);

	srand(rand() % 10000);

	int patternCnt = 0;
	for (int heightCnt = 0; heightCnt < height; heightCnt += 3) {

		memcpy(jps->map(heightCnt, 0), patterns[patternCnt][0], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 1, 0), patterns[patternCnt][1], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 2, 0), patterns[patternCnt][2], sizeof(MAP_STATE) * 3);

		patternCnt = 1 - patternCnt;

		memcpy(jps->map(heightCnt, width - 3), patterns[patternCnt][0], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 1, width - 3), patterns[patternCnt][1], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 2, width - 3), patterns[patternCnt][2], sizeof(MAP_STATE) * 3);

		for (int widthCnt = 3; widthCnt < width - 3; widthCnt += 3) {

			int randPatternIndex = rand() % 3;

			memcpy(jps->map(heightCnt, widthCnt), patterns[randPatternIndex][0], sizeof(MAP_STATE) * 3);
			memcpy(jps->map(heightCnt + 1, widthCnt), patterns[randPatternIndex][1], sizeof(MAP_STATE) * 3);
			memcpy(jps->map(heightCnt + 2, widthCnt), patterns[randPatternIndex][2], sizeof(MAP_STATE) * 3);

		}

	}

	jps->_start._x = 0;
	jps->_start._y = 0;

	jps->_end._x = width - 1;
	jps->_end._y = height - 1;

	iterator findResultIter = jps->pathFind();
	if (findResultIter == jps->pathEnd()) {
		MessageBoxW(NULL, L"길 찾기 실패", L"JumpPointSearch", MB_OK);
	}
	jps->nodeSkip();

	jps->printToBitmap(fileName, 20, findResultIter);

	delete(jps);

}

void CJumpPointSearch::printToBitmap(const WCHAR* fileName, const int printRatio, iterator endNodeIter) {

	BITMAPINFOHEADER info;
	ZeroMemory(&info, sizeof(BITMAPINFOHEADER));

	info.biWidth = _width * printRatio;
	info.biHeight = _height * printRatio;
	info.biPlanes = 1;
	info.biBitCount = 24;
	info.biSize = sizeof(BITMAPINFOHEADER);

	BITMAPFILEHEADER header;
	ZeroMemory(&header, sizeof(BITMAPFILEHEADER));

	header.bfType = 0x4d42;
	header.bfSize = _width * printRatio * _height * printRatio * 3;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	FILE* bitmapFile;
	_wfopen_s(&bitmapFile, fileName, L"w");

	fwrite(&header, sizeof(BITMAPFILEHEADER), 1, bitmapFile);
	fwrite(&info, sizeof(BITMAPINFOHEADER), 1, bitmapFile);

	stReverseRGB* rgbData = (stReverseRGB*)malloc(sizeof(stReverseRGB) * _width * printRatio * _height * printRatio);
	
	stReverseRGB* singleRgb = rgbData;

	// 기본 맵 출력
	for (int heightCnt = _height - 1; heightCnt >= 0; --heightCnt) {
		for (int heightRatioCnt = 0; heightRatioCnt < printRatio; ++heightRatioCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				for (int widthRatioCnt = 0; widthRatioCnt < printRatio; ++widthRatioCnt) {

					switch (*map(heightCnt, widthCnt)) {
					case MAP_STATE::ROAD:
						singleRgb->red = 43;
						singleRgb->green = 43;
						singleRgb->blue = 43;
						break;
					case MAP_STATE::WALL:
						singleRgb->red = 200;
						singleRgb->green = 200;
						singleRgb->blue = 200;
						break;
					}

					singleRgb += 1;
				}
			}
		}
	}

	singleRgb = rgbData;

	// 탐색 경로 출력
	for (int heightCnt = _height - 1; heightCnt >= 0; --heightCnt) {
		for (int heightRatioCnt = 0; heightRatioCnt < printRatio; ++heightRatioCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				for (int widthRatioCnt = 0; widthRatioCnt < printRatio; ++widthRatioCnt, singleRgb += 1) {

					stRGB* mapRgb = mapColor(heightCnt, widthCnt);
					if (mapRgb->red == 0 && mapRgb->blue == 0 && mapRgb->green == 0) {
						continue;
					}

					singleRgb->red = mapRgb->red;
					singleRgb->blue = mapRgb->blue;
					singleRgb->green = mapRgb->green;

				}
			}
		}
	}

	// 클로즈 리스트 노드 출력

	for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
		for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
			stNode* node = getNode(heightCnt, widthCnt);
			if (node == nullptr || node->_parent == nullptr) {
				continue;
			}
			int nodeX = node->_coord->_x * printRatio;
			int nodeY = (_height - 1 - node->_coord->_y) * printRatio;
			
			for (int heightRatioCnt = 1; heightRatioCnt <= printRatio / 2; ++heightRatioCnt) {
				nodeX = node->_coord->_x * printRatio + printRatio / 2 - heightRatioCnt;
				for (int widthRatioCnt = 0; widthRatioCnt < heightRatioCnt * 2 - 1; ++widthRatioCnt) {

					stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
					rgb->red = 220;
					rgb->green = 220;
					rgb->blue = 0;
				}
			}
			
			nodeY += printRatio / 2;

			for (int heightRatioCnt = 1; heightRatioCnt <= printRatio / 2; ++heightRatioCnt) {
				nodeX = node->_coord->_x * printRatio + heightRatioCnt;
				for (int widthRatioCnt = 0; widthRatioCnt < (printRatio / 2 - 1 - heightRatioCnt) * 2 - 1; ++widthRatioCnt) {
					stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
					rgb->red = 220;
					rgb->green = 220;
					rgb->blue = 0;
				}
			}
		}
	}

	// 오픈 리스트 노드 출력
	do{

		if(_openList->empty() == true){
			break;
		}

		CLinkedList<stNode*>::iterator iter = _openList->begin();
		for (; iter != _openList->end(); ++iter) {

			//int nodeX = (*arrIter)->_coord->_x * printRatio;
			int nodeX = (*iter)->_coord->_x * printRatio;
			//int nodeY = (_height - 1 - (*arrIter)->_coord->_y) * printRatio;
			int nodeY = (_height - 1 - (*iter)->_coord->_y) * printRatio;

			for (int heightRatioCnt = 1; heightRatioCnt <= printRatio / 2; ++heightRatioCnt) {
				//nodeX = (*arrIter)->_coord->_x * printRatio + printRatio / 2 - heightRatioCnt;
				nodeX = (*iter)->_coord->_x * printRatio + printRatio / 2 - heightRatioCnt;
				for (int widthRatioCnt = 0; widthRatioCnt < heightRatioCnt * 2 - 1; ++widthRatioCnt) {
					stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
					rgb->red = 150;
					rgb->green = 210;
					rgb->blue = 230;
				}
			}

			nodeY += printRatio / 2;

			for (int heightRatioCnt = 1; heightRatioCnt <= printRatio / 2; ++heightRatioCnt) {
				//nodeX = (*arrIter)->_coord->_x * printRatio + heightRatioCnt;
				nodeX = (*iter)->_coord->_x * printRatio + heightRatioCnt;
				for (int widthRatioCnt = 0; widthRatioCnt < (printRatio / 2 - 1 - heightRatioCnt) * 2 - 1; ++widthRatioCnt) {
					stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
					rgb->red = 150;
					rgb->green = 210;
					rgb->blue = 230;
				}
			}

		}

	}while(false);

	// 경로 출력
	do {
		//break;
		if (endNodeIter == pathEnd()) {
			break;
		}
		
		iterator nodeIter = pathBegin();
		++nodeIter;
		for(; nodeIter != pathEnd(); ++nodeIter) {

			stNode* node = *nodeIter;

			stCoord* coord = node->_coord;
			int startX = coord->_x * printRatio + printRatio / 4;
			int startY = (_height - coord->_y - 1) * printRatio + printRatio / 4;

			stCoord* parentCoord = node->_parent->_coord;
			int endX = parentCoord->_x * printRatio + printRatio / 4;
			int endY = (_height - parentCoord->_y - 1) * printRatio + printRatio / 4;

			int distanceX = endX - startX;
			int distanceY = endY - startY;

			int xDirection = (distanceX < 0) * -1 + (distanceX > 0);
			int yDirection = (distanceY < 0) * -1 + (distanceY > 0);

			lineTo(startX, startY, endX, endY);

			for (iterator lineIter = lineBegin(); lineIter != lineEnd(); ++lineIter) {

				stCoord* lineCoord = (*lineIter)->_coord;
				int y = lineCoord->_y;
				int x = lineCoord->_x;
				for (int heightCnt = y; heightCnt < y + printRatio / 2; ++heightCnt) {
					for (int widthCnt = x; widthCnt < x + printRatio / 2; ++widthCnt) {
						stReverseRGB* rgb = &rgbData[heightCnt * (_width * printRatio) + widthCnt];

						rgb->red = 200;
						rgb->green = 0;
						rgb->blue = 0;

					}
				}

			}

		}
	} while (false);

	fwrite(rgbData, sizeof(stReverseRGB), (size_t)(_width * printRatio * _height * printRatio), bitmapFile);
	fclose(bitmapFile); 

	free(rgbData);

}

CJumpPointSearch::iterator CJumpPointSearch::makePath(stNode* endNode) {
	

	_path->clear();
	while (endNode->_parent != nullptr) {
		_path->push_front(endNode);
		endNode = endNode->_parent;
	}

	_path->push_front(endNode);

	return pathBegin();
}

bool CJumpPointSearch::lineTo(int sx, int sy, int ex, int ey, bool draw, bool wallCheck) {

	#ifdef SPEED_TEST
		sp->profileBegin("lineTo");
	#endif
	_line->clear();

	stCoord absDistance(abs(ey - sy) + 1, abs(ex - sx) + 1);
	if (absDistance._y < absDistance._x) {
		if (ex < sx) {
			int temp = sx;
			sx = ex;
			ex = temp;

			temp = sy;
			sy = ey;
			ey = temp;
		}
	}
	else {
		if (ey < sy) {
			int temp = sx;
			sx = ex;
			ex = temp;

			temp = sy;
			sy = ey;
			ey = temp;
		}
	}

	stCoord direction(ey - sy, ex - sx);

	stCoord move( (direction._y < 0) * -1 + (direction._y > 0), (direction._x < 0) * -1 + (direction._x > 0));

	int *mainAxis;
	int mainDistance;
	int mainAxisMove;

	int *subAxis;
	int subDistance;
	int subAxisMove;

	int addWeight;
	int minusWeight;

	stCoord coord(absDistance._y, absDistance._x);

	if (absDistance._x > absDistance._y) {
		addWeight = absDistance._y;
		minusWeight = absDistance._x;

		mainAxis = &coord._x;
		mainDistance = absDistance._x;
		mainAxisMove = move._x;

		subAxis = &coord._y;
		subDistance = absDistance._y;
		subAxisMove = move._y;
	}
	else {
		addWeight = absDistance._x;
		minusWeight = absDistance._y;

		mainAxis = &coord._y;
		mainDistance = absDistance._y;
		mainAxisMove = move._y;

		subAxis = &coord._x;
		subDistance = absDistance._x;
		subAxisMove = move._x;
	}

	stCoord mid((ey - sy) / 2 + sy, (ex - sx) / 2 + sx);
	stCoord left(mid._y, mid._x);
	stCoord right(mid._y, mid._x);

	int midWeight = 0;

	// 가운데 라인 그리기
	if (subDistance % 2 == 1) {

		int remain = mainDistance % subDistance;
		int midNodeCnt = mainDistance / subDistance + remain % 2;

		midWeight = midNodeCnt * addWeight;

		int cnt = 1;
		int createDirection = 1;
		coord._x = mid._x;
		coord._y = mid._y;

		while (midNodeCnt > 0) {

			if (wallCheck == true && (*map(coord._y, coord._x)) == MAP_STATE::WALL) {
				#ifdef SPEED_TEST
					sp->profileEnd("lineTo");
				#endif
				return false;
			}

			if (createDirection == -1) {
				stCoord* newCoord = coordFreeList->allocObject();
				new (newCoord) stCoord(coord._y, coord._x);

				stNode* newNode = nodeFreeList->allocObject();
				new (newNode) stNode(nullptr, 0, 0, newCoord);

				_line->push_back(newNode);

				//_line->push_back(new stNode(nullptr, 0, 0, new stCoord(coord._y, coord._x)));
				new (&right) stCoord(coord._y, coord._x);
			}
			else {

				stCoord* newCoord = coordFreeList->allocObject();
				new (newCoord) stCoord(coord._y, coord._x);

				stNode* newNode = nodeFreeList->allocObject();
				new (newNode) stNode(nullptr, 0, 0, newCoord);

				_line->push_front(newNode);

				//_line->push_front(new stNode(nullptr, 0, 0, new stCoord(coord._y, coord._x)));
				new (&left) stCoord(coord._y, coord._x);
			}
			if (draw == true) {
				stRGB* rgb = lineColor(coord._y, coord._x);
				rgb->red = 160;
				rgb->green = 140;
				rgb->blue = 200;
			}
			*mainAxis += mainAxisMove * createDirection * cnt;

			createDirection = 0 - createDirection;
			midNodeCnt -= 1;
			cnt += 1;
		}

	}

	int leftWeight;

	// 왼쪽 라인 그리기
	{
		int weight = midWeight;
		new (&coord) stCoord(left._y, left._x);
		*mainAxis += mainAxisMove * (subDistance % 2 == 0);

		if (subDistance % 2 == 1) {
			*subAxis -= subAxisMove;
			weight -= minusWeight;
			if (weight < 0) {
				weight = 0;
			}
		}

		while (coord._y != sy || coord._x != sx) {
			*mainAxis -= mainAxisMove;

			if (wallCheck == true && (*map(coord._y, coord._x)) == MAP_STATE::WALL) {
				#ifdef SPEED_TEST
					sp->profileEnd("lineTo");
				#endif
				return false;
			}

			stCoord* newCoord = coordFreeList->allocObject();
			new (newCoord) stCoord(coord._y, coord._x);

			stNode* newNode = nodeFreeList->allocObject();
			new (newNode) stNode(nullptr, 0, 0, newCoord);

			_line->push_front(newNode);

			//_line->push_front(new stNode(nullptr, 0, 0, new stCoord(coord._y, coord._x)));
			if (draw == true) {
				stRGB* rgb = lineColor(coord._y, coord._x);
				rgb->red = 10;
				rgb->green = 128;
				rgb->blue = 255;
			}

			weight += addWeight;

			if (weight >= minusWeight) {
				weight -= minusWeight;
				*subAxis -= subAxisMove;
			}

			stCoord coordDistance(abs(coord._y - sy), abs(coord._x - sx));
			if (-1 <= coordDistance._x && coordDistance._x <= 1 && -1 <= coordDistance._y && coordDistance._y <= 1) {
				break;
			}

		}

		leftWeight = weight;

	}

	// 오른쪽 라인 그리기
	{
		int weight = midWeight;
		new (&coord) stCoord(right._y, right._x);

		*subAxis += subAxisMove;
		if (subDistance % 2 == 1) {
			weight -= minusWeight;
			if (weight < 0) {
				weight = 0;
			}
		}
		else {
			weight = leftWeight + addWeight - minusWeight;
		}

		while (coord._y != ey || coord._x != ex) {

			*mainAxis += mainAxisMove;

			if (wallCheck == true && (*map(coord._y, coord._x)) == MAP_STATE::WALL) {
				#ifdef SPEED_TEST
					sp->profileEnd("lineTo");
				#endif
				return false;
			}

			stCoord* newCoord = coordFreeList->allocObject();
			new (newCoord) stCoord(coord._y, coord._x);

			stNode* newNode = nodeFreeList->allocObject();
			new (newNode) stNode(nullptr, 0, 0, newCoord);

			_line->push_back(newNode);

			//_line->push_back(new stNode(nullptr, 0, 0, new stCoord(coord._y, coord._x)));
			if (draw == true) {
				stRGB* rgb = lineColor(coord._y, coord._x);
				rgb->red = 100;
				rgb->green = 220;
				rgb->blue = 10;
			}

			weight += addWeight;

			if (weight >= minusWeight) {
				weight -= minusWeight;
				*subAxis += subAxisMove;
			}

			//stCoord coordDistance(abs(coord._y - ey), abs(coord._x - ex));

			stCoord* coordDistance = coordFreeList->allocObject();
			new (coordDistance) stCoord(abs(coord._y - ey), abs(coord._x - ex));

			if (-1 <= coordDistance->_x && coordDistance->_x <= 1 && -1 <= coordDistance->_y && coordDistance->_y <= 1) {
				break;
			}
		}
	}

	#ifdef SPEED_TEST
		sp->profileEnd("lineTo");
	#endif
	return true;
}
	
void CJumpPointSearch::nodeSkip() {

	#ifdef SPEED_TEST
		sp->profileBegin("nodeSkip");
	#endif

	for (iterator startNodeIter = pathBegin(); startNodeIter != pathEnd(); ++startNodeIter) {

		bool nodeFound = false;
		iterator findNodeIter;

		stNode* startNode = *startNodeIter;
		const stCoord* startCoord = startNode->_coord;

		for (iterator endNodeIter = startNodeIter + 1; endNodeIter != pathEnd(); ++endNodeIter) {

			stNode* endNode = *endNodeIter;

			const stCoord* endCoord = endNode->_coord;

			bool ableLine = lineTo(startCoord->_x, startCoord->_y, endCoord->_x, endCoord->_y, false, true);

			if (ableLine == true) {
				endNode->_parent = startNode;
				findNodeIter = endNodeIter;
				nodeFound = true;
			}

		}

		if (nodeFound == false) {
			continue;
		}

		stNode* findNode = *findNodeIter;
		stCoord* findCoord = findNode->_coord;

		lineTo(startCoord->_x, startCoord->_y, findCoord->_x, findCoord->_y);

		for (CLinkedList<stNode*>::iterator lineCoordIter = _line->begin(); lineCoordIter != _line->end(); ++lineCoordIter) {

			stCoord* coord = (*lineCoordIter)->_coord;
			stRGB* rgb = lineColor(coord->_y, coord->_x);

			rgb->red = 20;
			rgb->blue = 125;
			rgb->green = 125;

		}

		iterator deleteNodeIter = startNodeIter + 1;
		while (deleteNodeIter != findNodeIter) {
			_path->erase(deleteNodeIter._pathIter);
			deleteNodeIter = startNodeIter + 1;
		}
	}
	#ifdef SPEED_TEST
		sp->profileEnd("nodeSkip");
	#endif
}

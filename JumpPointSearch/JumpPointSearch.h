
#define SPEED_TEST

#ifdef SPEED_TEST
extern SimpleProfiler* sp;
#endif

class CJumpPointSearch {


public:
	
	enum class MAP_STATE {
		ROAD = 0,
		WALL = 1
	};
	enum class DIRECTION {
		NONE = -1,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		LEFT_UP,
		RIGHT_UP,
		LEFT_DOWN,
		RIGHT_DOWN
	};

	struct stRGB {
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};

	struct stReverseRGB {
		unsigned char blue;
		unsigned char green;
		unsigned char red;
	};

	struct stCoord {
		int _x;
		int _y;
		CJumpPointSearch::DIRECTION _dir;
		stCoord(int y = 0, int x = 0, DIRECTION dir = DIRECTION::NONE) {
			_x = x;
			_y = y;
			_dir = dir;
		}
	};

	struct stNode {
		int _moveCnt;
		stNode* _parent;
		int _distance;
		CJumpPointSearch::stCoord* _coord;

		stNode(stNode* parent, int moveCnt, int distance, stCoord* coord) {
			_coord = coord;
			_moveCnt = moveCnt;
			_parent = parent;
			_distance = distance;
		}
		~stNode() {
			delete(_coord);
		}
	};

	class iterator {
		friend class CJumpPointSearch;
	public:
		iterator(linkedList<stNode*>::iterator pathIter) {
			_pathIter = pathIter;
		}
		iterator() {}
		stNode* operator*() {
			return *_pathIter;
		}
		void operator++() {
			++_pathIter;
		}
		void operator=(iterator iter) {
			_pathIter = iter._pathIter;
		}
		bool operator==(iterator iter) {
			return *_pathIter == *iter;
		}
		bool operator!=(iterator iter) {
			return *_pathIter != *iter;
		}

		iterator operator+(int addNum) {
			iterator iter = *this;
			for (int addCnt = 0; addCnt < addNum; ++addCnt) {
				++iter;
			}
			return iter;
		}

	private:
		linkedList<stNode*>::iterator _pathIter;
	};

public:

	inline MAP_STATE* map(int y, int x) {
		return &_map[y * _width + x];
	}
	inline stNode* node(int y, int x) {
		return _nodeMap[y * _width + x];
	}

	inline stRGB* mapColor(int y, int x) {
		return &_mapColor[y * _width + x];
	}

	inline stRGB* lineColor(int y, int x) {
		return &_lineColor[y * _width + x];
	}

	CJumpPointSearch(int width, int height);

	iterator pathFind();
	void pathFindInit();
	stNode* pathFindSingleLoop();
	iterator makePath(stNode* endNode);
	void makeNode(stCoord* corner, stNode* parent);

	inline int width() {
		return _width;
	}
	inline int height() {
		return _height;
	}

	stCoord _start;
	stCoord _end;

	void listClear();

	static void test(const WCHAR* fileName);

	iterator lineTo(int sx, int sy, int ex, int ey, bool draw=false);

	void nodeSkip();
	void printToBitmap(const WCHAR* fileName, const int printRatio, iterator endNodeIter);

#ifdef _WINDOWS_

	void print(HDC hdc, int blockSize, iterator endNodeIter);

#endif

	inline linkedList<stNode*>::iterator pathEnd() {
		return _path->end();
	}

	inline linkedList<stNode*>::iterator pathBegin() {
		return _path->begin();
	}

	inline linkedList<stNode*>::iterator lineEnd() {
		return _line->end();
	}

	inline linkedList<stNode*>::iterator lineBegin() {
		return _line->begin();
	}

private:

	MAP_STATE* _map;
	stNode** _nodeMap;
	int _width;
	int _height;

	stRGB* _mapColor;
	stRGB* _lineColor;

	linkedList<stNode*>* _openList;
	linkedList<stNode*>* _closeList;
	linkedList<stNode*>* _path;
	linkedList<stNode*>* _line;

	linkedList<stNode*>::iterator* findMin(linkedList<stNode*>* list);

	stCoord* checkOrthogonal(DIRECTION dir, int y , int x, const stRGB* color);
	stCoord* checkDiagonal(DIRECTION dir, int y, int x, const stRGB* color);

	bool isNodeInList(stCoord* coord, linkedList<stNode*>* list);

};
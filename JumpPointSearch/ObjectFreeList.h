#ifndef __OBJECT_FREE_LIST__

class CAllocList;

#define __OBJECT_FREE_LIST__

#define allocObject() _allocObject(__FILEW__, __LINE__)
#define freeObject(x) _freeObject(x, __FILEW__, __LINE__)

template<typename T>
struct stAllocNode {
	stAllocNode() {
		nextNode = nullptr;
#ifdef _WIN64
		underFlowCheck = (void*)0xF9F9F9F9F9F9F9F9;
		overFlowCheck = (void*)0xF9F9F9F9F9F9F9F9;
#else
		underFlowCheck = (void*)0xF9F9F9F9;
		overFlowCheck = (void*)0xF9F9F9F9;
#endif
	}

	// f9로 초기화해서 언더플로우 체크합니다.
	void* underFlowCheck;

	// alloc 함수에서 리턴할 실제 데이터
	T data;

	// f9로 초기화해서 오버플로우 체크합니다.
	void* overFlowCheck;

	// alloc 함수에서 할당 빠르게 하기 위함
	stAllocNode<T>* nextNode;

	// allocList에 노드의 주소값이 저장되있는 배열의 주소값입니다.
	// 배열 수정해도 같이 수정되도록 배열의 주소를 저장합니다.
	// 최상단 비트가 1이면 alloc 함수를 통해 할당된 상태입니다.
	void** allocListPtr;

	// 소스 파일 이름
	const wchar_t* allocSourceFileName;
	const wchar_t* freeSourceFileName;

	// 소스 라인
	int allocLine;
	int freeLine;
};

template<typename T>
class CObjectFreeList
{
public:

	CObjectFreeList(int _capacity = 0);
	~CObjectFreeList();

	T* _allocObject(const wchar_t*, int);

	int _freeObject(T* data, const wchar_t*, int);

	inline unsigned int getCapacity() { return _capacity; }
	inline unsigned int getUsedCount() { return _usedCnt; }

private:


	// 사용 가능한 노드를 리스트의 형태로 저장합니다.
	// 할당하면 제거합니다.
	stAllocNode<T>* _freeNode;

	// 전체 노드 개수
	unsigned int _capacity;

	// 현재 할당된 노드 개수
	unsigned int _usedCnt;

	// 데이터 포인터(stNode->data)에 이 값을 더하면 노드 포인터(stNode)가 된다 !
	int _dataPtrToNodePtr;

	// 할당 받은 포인터를 저장하는 배열
	// 여기서 노드 하나 하나의 포인터를 관리합니다.
	CAllocList* _ptrList;


	// 메모리 정리용
	// 단순 리스트
	struct stSimpleListNode {
		void* ptr;
		stSimpleListNode* next;
	};

	// 아래 List는 new의 결과로 전달된 포인터들로만 구성되어 있습니다.
	// new stNode[100] 이면 node[0]만 저장되어있는 식입니다.
	// freeList 소멸자에서 메모리 정리용으로 사용합니다.

	// 배열로 new한 포인터들
	stSimpleListNode* arrAllocList;

	// 실제로 stNode 를 윈도우에서 할당받는 곳입니다.
	// 배열로 할당받습니다.
	stAllocNode<T>* actualArrayAlloc(int size);

	// 오브젝트를 2배로 할당합니다.
	void resize();
};

class CAllocList {

public:

	template<typename T>
	void push(void* data);

	inline void** begin() { return _arr; }
	inline void** end() { return _arr + _size; }

	// 무조건 2배로 증가
	template<typename T>
	void resize();

	CAllocList(unsigned int cap = 10) {
		_cap = 10;
		_size = 0;
		_arr = new void* [cap];

	}
	~CAllocList() {
		delete[] _arr;
	}


	template<typename T>
	friend class CObjectFreeList;

private:

	void** _arr;
	unsigned int _cap;
	unsigned int _size;

};

template<typename T>
void CAllocList::push(void* data) {
	if (_size >= _cap) {
		resize<T>();
	}
	_arr[_size] = data;
	((stAllocNode<T>*)data)->allocListPtr = &_arr[_size];
	_size += 1;
}

template<typename T>
void CAllocList::resize() {
	void** oldArr = _arr;

	_cap *= 2;
	_arr = new void* [_cap];

	for (int arrCnt = 0; arrCnt < _size; ++arrCnt) {
		_arr[arrCnt] = oldArr[arrCnt];

#if defined(_WIN64)
		unsigned __int64 nodePtr = (unsigned __int64)_arr[arrCnt];
		unsigned __int64 firstBit = nodePtr & 0x8000000000000000;
		nodePtr &= 0x7FFFFFFFFFFFFFFF;
		stAllocNode<T>* node = (stAllocNode<T>*)nodePtr;
		unsigned __int64 arrPtr = (unsigned __int64)&_arr[arrCnt];
#else
		unsigned int nodePtr = (unsigned int)_arr[arrCnt];
		unsigned int firstBit = nodePtr & 0x80000000;
		nodePtr &= 0x7FFFFFFF;
		stAllocNode<T>* node = (stAllocNode<T>*)nodePtr;
		unsigned int arrPtr = (unsigned int)&_arr[arrCnt];
#endif

		node->allocListPtr = (void**)(arrPtr);//(void**)(arrPtr | firstBit);
		*(node->allocListPtr) = (void*)(nodePtr | firstBit);

		//((stAllocNode<T>*)(_arr[arrCnt]))->allocListPtr = &_arr[arrCnt];
	}

	delete[](oldArr);
}

template <typename T>
CObjectFreeList<T>::CObjectFreeList(int size) {

	arrAllocList = nullptr;
	_freeNode = nullptr;

	_capacity = size;
	_usedCnt = 0;

	if (size == 0) {
		return;
	}

	_ptrList = new CAllocList(_capacity);

	actualArrayAlloc(_capacity);
	stAllocNode<T> node;
	_dataPtrToNodePtr = ((char*)&node) - ((char*)&node.data);

}

template <typename T>
CObjectFreeList<T>::~CObjectFreeList() {

	for (int allocCnt = 0; allocCnt < _ptrList->_size; ++allocCnt) {
		stAllocNode<T>* node = (stAllocNode<T>*)_ptrList->_arr[allocCnt];

#if defined(_WIN64)
		if ((unsigned __int64)node & 0x8000000000000000) {
			unsigned __int64 temp = (unsigned __int64)node;
			temp &= 0x7FFFFFFFFFFFFFF;
			node = (stAllocNode<T>*)temp;
			wprintf(L"메모리 누수\n ptr: 0x%016I64x\n allocFile: %s\n allocLine: %d\n", &node->data, node->allocSourceFileName, node->allocLine);
		}
#else
		if ((unsigned int)node & 0x80000000) {
			unsigned int temp = (unsigned int)node;
			temp &= 0x7FFFFFFF;
			node = (stAllocNode<T>*)temp;
			wprintf(L"메모리 누수\n ptr: 0x%08x\n allocFile: %s\n allocLine: %d\n", &node->data, node->allocSourceFileName, node->allocLine);
		}
#endif

	}


	while (arrAllocList != nullptr) {
		stSimpleListNode* nextNode = arrAllocList->next;
		delete[](stAllocNode<T>*)(arrAllocList->ptr);
		delete arrAllocList;
		arrAllocList = nextNode;
	}


}

template<typename T>
T* CObjectFreeList<T>::_allocObject(const wchar_t* fileName, int line) {

	_usedCnt += 1;
	if (_freeNode == nullptr) {

		resize();

		wprintf(L"ObjectFreeList, resize()\nfileName: %s\nline: %d\n", fileName, line);

	}

	stAllocNode<T>* allocNode = _freeNode;

#ifdef _WIN64
	unsigned __int64 ptrTemp = (unsigned __int64)(*(allocNode->allocListPtr));
	ptrTemp |= 0x8000000000000000;
#else
	unsigned int ptrTemp = (unsigned int)(*(_freeNode->allocListPtr));
	ptrTemp |= 0x80000000;
#endif

	(*(allocNode->allocListPtr)) = (void*)ptrTemp;

	allocNode->allocSourceFileName = fileName;
	allocNode->allocLine = line;

	_freeNode = _freeNode->nextNode;

	//new (&(allocNode->data)) T();

	return &(allocNode->data);
}

template <typename T>
int CObjectFreeList<T>::_freeObject(T* data, const wchar_t* fileName, int line) {

	stAllocNode<T>* usedNode = (stAllocNode<T>*)(((char*)data) + _dataPtrToNodePtr);

#ifdef _WIN64

	if (usedNode->underFlowCheck != (void*)0xF9F9F9F9F9F9F9F9) {
		// underflow
		wprintf(L"underflow detect\n fileName: %s\n line: %d\n", fileName, line);
		return -1;
	}
	if (usedNode->overFlowCheck != (void*)0xF9F9F9F9F9F9F9F9) {
		// overflow
		wprintf(L"overflow detect\n fileName: %s\n line: %d\n", fileName, line);
		return 1;
	}
	if (((unsigned __int64)(*(usedNode->allocListPtr)) & 0x8000000000000000) == 0) {
		// 중복 free
		wprintf(L"중복 free detect\n fileName: %s\n line: %d\n", fileName, line);
		return -2;
	}
	unsigned __int64 ptrTemp = (unsigned __int64)(*(usedNode->allocListPtr));
	ptrTemp &= 0x7FFFFFFFFFFFFFFF;
	*(usedNode->allocListPtr) = (void*)ptrTemp;
#else
	if (usedNode->underFlowCheck != (void*)0xF9F9F9F9) {
		// underflow
		wprintf(L"underflow detect\n fileName: %s\n line: %d\n", fileName, line);
		return -1;
	}
	if (usedNode->overFlowCheck != (void*)0xF9F9F9F9) {
		// overflow
		wprintf(L"overflow detect\n fileName: %s\n line: %d\n", fileName, line);
		return 1;
	}
	if (((unsigned int)(*(usedNode->allocListPtr)) & 0x80000000) == 0) {
		// 중복 free
		wprintf(L"중복 free detect\n fileName: %s\n line: %d\n", fileName, line);
		return -2;
	}
	unsigned int ptrTemp = (unsigned int)(*(usedNode->allocListPtr));
	ptrTemp &= 0x7FFFFFFF;
	*(usedNode->allocListPtr) = (void*)ptrTemp;
#endif

	usedNode->freeSourceFileName = fileName;
	usedNode->freeLine = line;

	usedNode->nextNode = _freeNode;

	_freeNode = usedNode;
	_usedCnt -= 1;

	//data->~T();

	return 0;
}

template<typename T>
stAllocNode<T>* CObjectFreeList<T>::actualArrayAlloc(int size) {

	stAllocNode<T>* nodeArr = new stAllocNode<T>[size];

	stSimpleListNode* arrAllocNode = new stSimpleListNode;
	arrAllocNode->ptr = nodeArr;
	arrAllocNode->next = arrAllocList;
	arrAllocList = arrAllocNode;

	stAllocNode<T>* nodeEnd = nodeArr + size;
	for (stAllocNode<T>* nodeIter = nodeArr;;) {

		stAllocNode<T>* node = nodeIter;
		++nodeIter;

		_ptrList->push<T>(node);

		if (nodeIter == nodeEnd) {
			break;
		}

		node->nextNode = nodeIter;
	}
	_freeNode = nodeArr;

	return nodeArr;
}

template<typename T>
void CObjectFreeList<T>::resize() {

	actualArrayAlloc(_capacity);

	_capacity <<= 1;

}

#endif
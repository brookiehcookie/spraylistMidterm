#include <iostream>
#include <vector>
#include <cmath>

// Really quick down-and dirty boolean
#define bool int
#define true (1)
#define false (0)

// Hey kids! Never do this in the real world!
#define maybe (rand()%2)

using namespace std;

/**************************************************/

template <typename T>
class SkipListNode {
public:
	// The data this node currently holds
	T data;
	// The height of this node
	int height;
	// A vector of the next pointers
	vector<SkipListNode*> next;

	SkipListNode(int height);
	~SkipListNode();
	SkipListNode<T> *getNext(int level);
	void setNext(int level, SkipListNode *newNext);
	void grow();
	bool maybeGrow();
	void trim(int height);
};

/**
	SkipListNode constructor adapted from skipListNodeCreate
*/
template <typename T>
SkipListNode<T>::SkipListNode(int _height) {
	height = _height;
	// Setup next vector: reserve space for `height` pointers
	next.reserve(height);
	// Fill with NULL references
	for (int i = 0; i < height; ++i)
		next.assign(i, NULL);
}

/**
	SkipListNode deconstructor adapted from skipListNodeDestroy
*/
template <typename T>
SkipListNode<T>::~SkipListNode() {
	// TODO: memory leaks here WRT vector?
}

/**
	Get the next SkipListNode<T> at given level
	Has bounds checking (using vector.at)
	Adapted from skipListNodeGetNext
*/
template <typename T>
SkipListNode<T> *SkipListNode<T>::getNext(int level) {
	return next.at(level);
}

/**
	Set the next SkipListNode<T> at given level
	Has bounds checking (using vector.assign)
	Adapted from skipListNodeSetNext
*/
template <typename T>
void SkipListNode<T>::setNext(int level, SkipListNode<T> *newNext) {
	next.assign(level, newNext);
}

/**
	Grow this node by one; set the new next pointer to null
	Adapted from skipListNodeGrow
*/
template <typename T>
void SkipListNode<T>::grow() {
	height++;
	next.reserve(height);
	next.assign(height - 1, NULL);
}

/**
	Randomly grow this node at 50% chance
	Adapted from skipListMaybeGrow
*/
template <typename T>
bool SkipListNode<T>::maybeGrow() {
	bool didGrow = rand() % 2;
	if (didGrow)
		this->grow();
	return didGrow;
}

/**
	Trim this node to given height
	This is a lot easier with C++ vectors than it is in C! :)
	Adapted from skipListNodeTrim
*/
template <typename T>
void SkipListNode<T>::trim(int newHeight) {
	height = newHeight;
	next.resize(height);
}

/**************************************************/

template <typename T>
class SkipList {
public:
	// The number of elements in the SkipList
	int size;
	// The head node of the SkipList (data is not meaningufl)
	// The height of the head node is the height of the skip list.
	SkipListNode<T> *head = NULL;

	static int getMaxHeight(int size);
	static int generateRandomHeight(int maxHeight);

	SkipList();
	SkipList(int height);
	~SkipList();
	void init(int height);
	int height();
	void add(T data);
	void addH(T data, int height);
	bool contains(T data);
	void remove(T data);
	void grow();
	void trim();
	void print();
};

template <typename T>
int SkipList<T>::getMaxHeight(int size) {
	if (size == 0)
		return 1;
	return 1 + log(size) / log(2);
}

template <typename T>
int SkipList<T>::generateRandomHeight(int maxHeight) {
	int h = 1;
	while (rand() % 2 == 0 && h < maxHeight)
		h++;
	return h;
}

/*
	Constructor for SkipList
	Adapted from skipLsitCreate
*/
template <typename T>
SkipList<T>::SkipList() {
	this->init(1);
}

/**
	Constructor for SkipList given specific height
	Adapted from skipLsitCreateH
*/
template <typename T>
SkipList<T>::SkipList(int height) {
	this->init(height);
}

/**
	Initialize a SkipList with head node of given height
*/
template <typename T>
void SkipList<T>::init(int height) {
	size = 0;
	head = new SkipListNode<T>(height);
}

/**
	Deconstructor for SkipList
*/
template <typename T>
SkipList<T>::~SkipList () {
	// TODO: memory leaks with SkipListNode<T> head
}


/**
	Get height of SkipList, which is defined to be
	the height of the head node
*/
template <typename T>
int SkipList<T>::height() {
	return head->height;
}

/**
	Add data to the SkipList
	Adapted from skipListInsert
*/
template <typename T>
void SkipList<T>::add(T data) {
	this->addH(data, SkipList<T>::generateRandomHeight(this->height()));
}

/**
	Add data to the SkipList in new node with given height
	Adapted from skipListInsertH
*/
template <typename T>
void SkipList<T>::addH(T data, int height) {
	// TODO: write (lock-free) SkipList insertion
}

/**
	Return whether or not the given data exists in the SkipList
	Adapted from skipListContains
*/
template <typename T>
bool SkipList<T>::contains(T data) {
	// TODO: write (lock-free) SkipList contains
	return maybe;
}

/**
	Remove given data from the SkipList
	Does not throw an error if given data does not exist
	Adapted from skipListDelete
*/
template <typename T>
void SkipList<T>::remove(T data) {
	// TODO: write (lock-free) SkipList removal
	// To be eventually turned into a SprayList?
	// Also consider logical deletion
}

/**
	Adapted from skipListGrow
*/
template <typename T>
void SkipList<T>::grow() {
	// TODO: write (lock-free) SkipList grow
}

/**
	Adapted from skipListTrim
*/
template <typename T>
void SkipList<T>::trim() {
	// TODO: write (lock-free) SkipList trim
}

/**
	Adapted from skipListPrint
*/
template <typename T>
void SkipList<T>::print() {
	// TODO: write (lock-free) SkipList print
}

/**************************************************/

/**
	Basic test driver for skip list stuff!
*/
int main() {
	SkipList<int> skippy;
	//cout << "A skiplist with 13 notes is of height " << SkipList<int>::getMaxHeight(13) << endl;
	cout << "My SkipList is of height " << skippy.height() << endl;
	skippy.add(1);

	cout << "Done" << endl;
	return 0;
}

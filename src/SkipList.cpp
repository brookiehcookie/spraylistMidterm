#include <iostream>
#include <vector>
#include <cmath>
#include <atomic>

// Really quick down-and dirty boolean
#define bool int
#define true (1)
#define false (0)

// Hey kids! Never do this in the real world!
#define maybe (rand()%2)

using namespace std;

/**************************************************/

/**
	Simulate Java's AtomicMarkableReference

	This uses the least significant bit of a pointer
	as the mark. This creates a restriction on the kinds
	of pointers you can use, but most of the time 
	it should be fine.

	References:
	http://www.cnblogs.com/zxh1210603696/p/3721460.html
	https://stackoverflow.com/questions/40247249/what-is-the-c-11-atomic-library-equivalent-of-javas-atomicmarkablereferencet
*/

template <class T>
class AtomicMarkableReference {
private:
	std::atomic<T> ref;
	const uintptr_t UNMARKED = 0;
	const uintptr_t MARKED = 1;
public:
	AtomicMarkableReference(T p, bool mark);
	AtomicMarkableReference(const AtomicMarkableReference &a);
	operator = (const AtomicMarkableReference &a);
	T getRef() const;
	bool isMarked() const;
	bool get(bool &b) const;
	bool compareAndSet(T expectedValue, T newValue, bool expectedMark, bool newMark);
	void set(T newValue, bool newMark);
	bool attemptMark(T expectedValue, bool newMark);
};

template <class T>
AtomicMarkableReference<T>::AtomicMarkableReference(T p, bool mark) {
	this->ref.store((T) ((uintptr_t) p | (uintptr_t) mark));
}

template <class T>
AtomicMarkableReference<T>::AtomicMarkableReference(const AtomicMarkableReference &a) {
	this->ref.store(a.ref.load());
}

template <class T>
AtomicMarkableReference<T>::operator = (const AtomicMarkableReference &a) {
	if (*this != a) {
		this->ref.store(a.ref.load());
		return *this;
	}
}

template <class T>
T AtomicMarkableReference<T>::getRef() const {
	T p = this->ref.load();
	return (bool) ((uintptr_t) p & MARKED) ? (T)((uintptr_t)(p) & ~MARKED) : p;
}

template <class T>
bool AtomicMarkableReference<T>::isMarked() const {
	T p = this->ref.load();
	return (bool) ((uintptr_t) p & MARKED);
}

template <class T>
bool AtomicMarkableReference<T>::get(bool &b) const {
	T p = this->ref.load();
	b = (bool) ((uintptr_t) p & MARKED);
	return b ? (T) ((uintptr_t)(p) & ~MARKED) : p;
}

template <class T>
bool AtomicMarkableReference<T>::compareAndSet(T expectedValue, T newValue, bool expectedMark, bool newMark) {
	T p = this->ref.load();
	bool b = (bool)((uintptr_t) p & MARKED);
	if (b == expectedMark) {
		expectedValue = (T)((uintptr_t) expectedValue | (uintptr_t) b);
		return this->ref.compare_exchange_strong(expectedValue, (T) ((uintptr_t) newValue | (uintptr_t) newMark));
	}
	return false;
}

template <class T>
void AtomicMarkableReference<T>::set(T newValue, bool newMark) {
	newValue = (T) ((uintptr_t) newValue | (uintptr_t) newMark);
	this->ref.exchange(newValue);
}

template <class T>
bool AtomicMarkableReference<T>::attemptMark(T expectedValue, bool newMark) {
	T newValue = (T) ((uintptr_t) expectedValue | (uintptr_t) newMark);
	expectedValue = isMarked() ? (T) ((uintptr_t) expectedValue | MARKED) : expectedValue;
	return this->ref.compare_exchange_strong(expectedValue, newValue);
}

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


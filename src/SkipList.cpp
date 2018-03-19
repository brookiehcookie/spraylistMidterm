#include <iostream>
#include <vector>
#include <cmath>
#include <atomic>
#include <climits>

#include <ctime>
#include <cstdlib>

#include <string>
#include <functional>

// Really quick down-and dirty boolean
#define bool int
#define true (1)
#define false (0)

// Hey kids! Never do this in the real world!
#define maybe (rand()%2)

using namespace std;

#define MAX_LEVEL 8

/**************************************************/

/**
	Very probably a really bad hash function
*/
uintptr_t hashy(void *p) {
	return ((uintptr_t) p) >> 3;
}

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
	T get(bool &b) const;
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
T AtomicMarkableReference<T>::get(bool &b) const {
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
	T value;
	int key;
	int topLevel;
	vector<AtomicMarkableReference<SkipListNode<T>*>*> next;
	int flag = 0;

	SkipListNode(int _key);
	SkipListNode(T x, int height);

	~SkipListNode();
	//SkipListNode<T> *getNext(int level);
	//void setNext(int level, SkipListNode *newNext);
	//void grow();
	//bool maybeGrow();
	//void trim(int height);

	char print() {
		if (flag == 1) {
			cout << "HEAD";
			return '\0';
		}
		if (flag == 2) {
			cout << "TAIL";
			return '\0';
		}
		/*if (value != NULL)
			cout << *value;
		else
			cout << "null";*/
		cout << value;
		return '\0';
	}
};

/**
	SkipListNode constructor for sentinel nodes
	//SkipListNode constructor adapted from skipListNodeCreate
*/
template <typename T>
SkipListNode<T>::SkipListNode(int _key) {
	//value = NULL;
	key = _key;
	//next = new vector<AtomicMarkableReference<SkipListNode<T>*>>();
	//next.reserve(MAX_LEVEL + 1);
	//next = new AtomicMarkableReference<SkipListNode<T>>[MAX_LEVEL + 1];
	for (int i = 0; i < MAX_LEVEL + 1; i++)
		next.push_back(new AtomicMarkableReference<SkipListNode<T>*>(NULL, false));
	topLevel = MAX_LEVEL;
}

/**
	SkipListNode constructor for ordinary nodes
*/
template <typename T>
SkipListNode<T>::SkipListNode(T x, int height) {
	value = x;
	key = std::hash<T>{}(x);
	//cout << "created node with value " << value << " and key " << key << " with height " << height << endl;
	for (int i = 0; i < height + 1; i++)
		next.push_back(new AtomicMarkableReference<SkipListNode<T>*>(NULL, false));
	topLevel = height;
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
/*template <typename T>
SkipListNode<T> *SkipListNode<T>::getNext(int level) {
	return next.at(level);
}*/

/**
	Set the next SkipListNode<T> at given level
	Has bounds checking (using vector.assign)
	Adapted from skipListNodeSetNext
*/
/*template <typename T>
void SkipListNode<T>::setNext(int level, SkipListNode<T> *newNext) {
	next.assign(level, newNext);
}*/

/**
	Grow this node by one; set the new next pointer to null
	Adapted from skipListNodeGrow
*/
/*template <typename T>
void SkipListNode<T>::grow() {
	height++;
	next.reserve(height);
	next.assign(height - 1, NULL);
}*/

/**
	Randomly grow this node at 50% chance
	Adapted from skipListMaybeGrow
*/
/*template <typename T>
bool SkipListNode<T>::maybeGrow() {
	bool didGrow = rand() % 2;
	if (didGrow)
		this->grow();
	return didGrow;
}*/

/**
	Trim this node to given height
	This is a lot easier with C++ vectors than it is in C! :)
	Adapted from skipListNodeTrim
*/
/*template <typename T>
void SkipListNode<T>::trim(int newHeight) {
	height = newHeight;
	next.resize(height);
}*/

/**************************************************/

template <typename T>
class SkipList {
public:
	// The number of elements in the SkipList
	int size;
	// The head node of the SkipList (data is not meaningufl)
	// The height of the head node is the height of the skip list.
	SkipListNode<T> *head = NULL;
	SkipListNode<T> *tail = NULL;

	static int getMaxHeight(int size);
	static int generateRandomHeight(int maxHeight);

	SkipList();
	SkipList(int height);
	~SkipList();
	void init(int height);
	int height();
	bool find(T x, SkipListNode<T> *preds[], SkipListNode<T> *succs[]);
	bool add(T x);
	bool addH(T x, int height);
	bool contains(T x);
	bool remove(T x);
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
	if (h == maxHeight) h--;
	return h;
}

/*
	Constructor for SkipList
	Adapted from skipLsitCreate
*/
template <typename T>
SkipList<T>::SkipList() {
	this->init(MAX_LEVEL);
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
	cout << "initialized with height = " << height << endl;
	size = 0;
	head = new SkipListNode<T>(height + 1);
	//head->value = NULL;
	head->flag = 1;
	head->key = INT_MIN;
	tail = new SkipListNode<T>(height + 1);
	//tail->value = NULL;
	tail->flag = 2;
	tail->key = INT_MAX;
	for (int level = 0; level < head->next.size(); level++)
		head->next.at(level)->set(tail, false);
}

/**
	Deconstructor for SkipList
*/
template <typename T>
SkipList<T>::~SkipList () {
	// TODO: memory leaks with head and tail
	//delete head;
	//delete tail;
	// probably also need to free all the other nodes
}


/**
	Get height of SkipList, which is defined to be
	the height of the head node
*/
template <typename T>
int SkipList<T>::height() {
	return head->topLevel;
}

/**
	The find algorithm
*/
template <typename T>
bool SkipList<T>::find(T x, SkipListNode<T> *preds[], SkipListNode<T> *succs[]) {
	int bottomLevel = 0;
	int key = std::hash<T>{}(x);
	//cout << "searching for key: " << key << endl;
	bool marked[] = {false};
	bool snip;
	SkipListNode<T> *pred = NULL;
	SkipListNode<T> *curr = NULL;
	SkipListNode<T> *succ = NULL;
	//cout << "starting find" << endl;
	// retry
	bool retry;
	while (true) {
		//cout << "loop0" << endl;
		retry = false;
		pred = head;
		for (int level = MAX_LEVEL - 1; level >= bottomLevel; level--) {
			//cout << "level = " << level << endl;
			curr = pred->next.at(level)->getRef();

			/*if (curr == NULL)
				cout << "THINKING EMOJI" << endl;
			if (curr->value == NULL)
				cout << "curr->value is null" << endl;*/

			while (true) {
				//cout << "loop1" << endl;
				succ = curr->next.at(level)->get(*marked);

				// Print state
				/*if (pred != NULL)
					cout << "pred: " << pred->print() << endl;
				else
					cout << "pred: NULL" << endl;

				if (curr != NULL)
					cout << "curr: " << curr->print() << endl;
				else
					cout << "curr: NULL" << endl;

				if (succ != NULL)
					cout << "succ: " << succ->print() << endl;
				else
					cout << "succ: NULL" << endl;*/


				while (marked[0]) {
					//cout << "loop2" << endl;
					snip = pred->next.at(level)->compareAndSet(curr, succ, false, false);
					if (!snip) {
						// jump to retry
						//cout << "retry" << endl;
						retry = true;
						break;
					}
					curr = pred->next.at(level)->getRef();
					succ = curr->next.at(level)->get(*marked);
				}
				if (retry) break;
				if (curr->key < key) {
					//cout << "move forward" << endl;
					pred = curr; curr = succ;
				} else {
					break;
				}
			}
			if (retry) break;
			preds[level] = pred;
			succs[level] = curr;
		}
		if (retry) continue;
		//cout << "making final comparison " << curr->key << " vs " << key << endl;
		return curr->key == key;
	}
}

/**
	Add data to the SkipList
	Adapted from skipListInsert
*/
template <typename T>
bool SkipList<T>::add(T x) {
	return this->addH(x, SkipList<T>::generateRandomHeight(this->height()));
}

/**
	Add data to the SkipList in new node with given height
	Adapted from skipListInsertH
*/
template <typename T>
bool SkipList<T>::addH(T x, int height) {
	int topLevel = height;
	int bottomLevel = 0;
	SkipListNode<T> *preds[MAX_LEVEL + 1];
	SkipListNode<T> *succs[MAX_LEVEL + 1];
	while (true) {
		bool found = find(x, preds, succs);
		if (found) {
			return false;
		} else {
			SkipListNode<T> *newNode = new SkipListNode<T>(x, topLevel);
			for (int level = bottomLevel; level <= topLevel; level++) {
				newNode->next.at(level)->set(succs[level], false);
			}
			SkipListNode<T> *pred = preds[bottomLevel];
			SkipListNode<T> *succ = succs[bottomLevel];
			newNode->next.at(bottomLevel)->set(succ, false);
			/*if (newNode->next.at(bottomLevel)->compareAndSet(succ, newNode, false, false)) {
				continue;
			}*/
			if (!pred->next.at(bottomLevel)->compareAndSet(succ, newNode, false, false)) {
				continue;
			}
			for (int level = bottomLevel+1; level <= topLevel; level++) {
				while (true) {
					pred = preds[level];
					succ = succs[level];
					if (pred->next.at(level)->compareAndSet(succ, newNode, false, false))
						break;
					find(x, preds, succs);
				}
			}
			return true;
		}
	}
}

/**
	Return whether or not the given data exists in the SkipList
	Adapted from skipListContains
*/
template <typename T>
bool SkipList<T>::contains(T x) {
	int bottomLevel = 0;
	int key = std::hash<T>{}(x);
	bool marked[] = {false};
	SkipListNode<T> *pred = head;
	SkipListNode<T> *curr = NULL;
	SkipListNode<T> *succ = NULL;
	for (int level = MAX_LEVEL; level >= bottomLevel; level--) {
		curr = pred->next.at(level)->getRef();
		while (true) {
			succ = curr->next.at(level)->get(*marked);
			while (marked[0]) {
				curr = pred->next.at(level)->getRef();
				succ = curr->next.at(level)->get(*marked);
			}
			if (curr->key < key) {
				pred = curr;
				curr = succ;
			} else {
				break;
			}
		}
	}
	return curr->key == key;
}

/**
	Remove given data from the SkipList
	Does not throw an error if given data does not exist
	Adapted from skipListDelete
*/
template <typename T>
bool SkipList<T>::remove(T x) {
	int bottomLevel = 0;
	SkipListNode<T> *preds[MAX_LEVEL + 1];
	SkipListNode<T> *succs[MAX_LEVEL + 1];
	SkipListNode<T> *succ;
	while (true) {
		bool found = find(x, preds, succs);
		if (!found) {
			return false;
		} else {
			SkipListNode<T> *nodeToRemove = succs[bottomLevel];
			for (int level = nodeToRemove->topLevel; level >= bottomLevel+1; level--) {
				bool marked[] = {false};
				succ = nodeToRemove->next.at(level)->get(*marked);
				while (!marked[0]) {
					nodeToRemove->next.at(level)->attemptMark(succ, true);
					succ = nodeToRemove->next.at(level)->get(*marked);
				}
			}
			bool marked[] = {false};
			succ = nodeToRemove->next.at(bottomLevel)->get(*marked);
			while (true) {
				bool iMarkedIt = nodeToRemove->next.at(bottomLevel)->compareAndSet(succ, succ, false, true);
				succ = succs[bottomLevel]->next.at(bottomLevel)->get(*marked);
				if (iMarkedIt) {
					find(x, preds, succs);
					return true;
				}
				else if (marked[0]) return false;
			}
		}
	}
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
	int i;
	SkipListNode<T> *nodey = head;
	SkipListNode<T> *nodeyNext;
	while (nodey != NULL) {
		cout << nodey->print();
		for (i = 0; i < nodey->topLevel; i++) {
			cout << "\t";
			nodeyNext = nodey->next.at(i)->getRef();
			if (nodeyNext != NULL) {
				/*if (nodeyNext->value != NULL) {
					cout << "(" << *nodeyNext->value << ")";
				} else {
					cout << "(NULL)";
				}*/
				cout << '(' << nodeyNext->print() << ')';
			} else {
				cout << "NULL";
			}
		}
		cout << endl;

		nodey = nodey->next.at(0)->getRef();
	}
}


/**************************************************/

/**
	Basic test driver for skip list stuff!
*/
int main() {
	srand(time(NULL));

	SkipList<int> *skippy = new SkipList<int>();

	SkipListNode<int>* preds[MAX_LEVEL + 1];
	SkipListNode<int>* succs[MAX_LEVEL + 1];

	//bool found = skippy.find(5, preds, succs);

	/*vector<AtomicMarkableReference<SkipListNode<int>*>*> next;
	for (int i = 0; i < MAX_LEVEL + 1; i++)
		next.push_back(new AtomicMarkableReference<SkipListNode<int>*>(NULL, false));
	*/
	
	/*int valueA = 42;
	int valueB = 69;
	int valueC = 420;

	SkipListNode<int> *nodeA = new SkipListNode<int>(valueA, 1);
	SkipListNode<int> *nodeB = new SkipListNode<int>(valueB, 2);
	SkipListNode<int> *nodeC = new SkipListNode<int>(valueC, 3);

	skippy->head->next.at(0)->set(nodeA, false);
	skippy->head->next.at(1)->set(nodeB, false);
	skippy->head->next.at(2)->set(nodeC, false);

	nodeA->next.at(0)->set(nodeB, false);

	nodeB->next.at(0)->set(nodeC, false);
	nodeB->next.at(1)->set(nodeC, false);

	nodeC->next.at(0)->set(skippy->tail, false);
	nodeC->next.at(1)->set(skippy->tail, false);
	nodeC->next.at(2)->set(skippy->tail, false);*/

	cout << "Setup done" << endl;
	skippy->print();
	cout << "Finding" << endl;
	
	for (int i = 1; i <= 16; i++)
		skippy->add(rand()%1024);

	skippy->print();

	int needle;
	cout << "Remove from this skiplist ";
	cin >> needle;
	if (skippy->remove(needle)) {
		cout << "REMOVED" << endl;
	} else {
		cout << "NOT REMOVED" << endl;
	}
	skippy->print();
	/*if (skippy->find(&needle, preds, succs)) {
		cout << "FOUND" << endl;
	} else {
		cout << "NOT found" << endl;
	}*/

	//SkipListNode<int> *nexty = nodeB->next.at(0)->get(marked);


	/*
	// Try to find 420
	bool found = skippy->find(420, preds, succs);
	if (found) {
		cout << "FOUND!" << endl;
	} else {
		cout << "NOT found" << endl;
	}*/


	return 0;
}


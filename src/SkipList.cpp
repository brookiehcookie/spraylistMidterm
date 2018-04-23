#include <iostream>
#include <vector>
#include <cmath>
#include <atomic>
#include <climits>
#include <ctime>
#include <cstdlib>
#include <string>
#include <functional>

#include <pthread.h>

// Really quick down-and dirty boolean
#define bool int
#define true (1)
#define false (0)

using namespace std;

#define MAX_LEVEL 12
const int LEVELS_TO_DESCEND = 1;
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
	void print();
	int getWalkLength(int threadCount);
	int Spray(int beginHeight, int walkLength, int descendAmount);
	
};

template <typename T>
int SkipList<T>::Spray(int beginHeight, int walkLength, int descendAmount) {
	SkipListNode<T> *node;
	node = head;
	int level = begin;
	
	while(level >= 0){
	
	}
}

template <typename T>
int SkipList<T>::getMaxHeight(int size) {
	if (size == 0)
		return 1;
	return 1 + log(size) / log(2);
}

template <typename T>
int SkipList<T>::getWalkLength(int threadCount) {
	int logOfThreads = floor(log(threadCount));
	int maxWalk = pow(logOfThreads,3);
	
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
	bool marked[] = {false};
	bool snip;
	SkipListNode<T> *pred = NULL;
	SkipListNode<T> *curr = NULL;
	SkipListNode<T> *succ = NULL;
	// retry
	bool retry;
	while (true) {
		retry = false;
		pred = head;
		for (int level = MAX_LEVEL - 1; level >= bottomLevel; level--) {
			curr = pred->next.at(level)->getRef();

			while (true) {
				succ = curr->next.at(level)->get(*marked);


				while (marked[0]) {
					snip = pred->next.at(level)->compareAndSet(curr, succ, false, false);
					if (!snip) {
						retry = true;
						break;
					}
					curr = pred->next.at(level)->getRef();
					succ = curr->next.at(level)->get(*marked);
				}
				if (retry) break;
				if (curr->key < key) {
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

#define THREADS_ADD 12
#define THREADS_REMOVE 12
#define NUMS_PER_THREAD 5000

typedef struct thread_data {
	int thread_id;
	SkipList<int> *skippy;
} thread_data;

void *skipListAdd(void *threadarg) {
	thread_data *my_data = (thread_data *) threadarg;
	SkipList<int> *skippy = my_data->skippy;

	// Generate nums from 0 to NUMS_PER_THREAD
	int nums[NUMS_PER_THREAD];
	for (int i = 0; i < NUMS_PER_THREAD; i++)
		nums[i] = i*THREADS_ADD + my_data->thread_id + 1;
	// Scramble
	int temp, j;
	for (int i = 0; i < NUMS_PER_THREAD; i++) {
		j = rand() % NUMS_PER_THREAD;
		temp = nums[i];
		nums[i] = nums[j];
		nums[j] = temp;
	}

	for (int i = 0; i < NUMS_PER_THREAD; i++) {
		//cout << "add " << nums[i] << endl;
		skippy->add(nums[i]);
		cout << '#';
	}

	pthread_exit(NULL);
}

void *skipListRemove(void *threadarg) {
	thread_data *my_data = (thread_data *) threadarg;
	SkipList<int> *skippy = my_data->skippy;

	// Generate nums from 0 to NUMS_PER_THREAD
	int nums[NUMS_PER_THREAD];
	for (int i = 0; i < NUMS_PER_THREAD; i++)
		nums[i] = i + my_data->thread_id*NUMS_PER_THREAD + 1;
	// Scramble
	int temp, j;
	for (int i = 0; i < NUMS_PER_THREAD; i++) {
		j = rand() % NUMS_PER_THREAD;
		temp = nums[i];
		nums[i] = nums[j];
		nums[j] = temp;
	}

	bool isRemoved[NUMS_PER_THREAD];
	for (int i = 0; i < NUMS_PER_THREAD; i++)
		isRemoved[i] = false;

	int i = 0;
	int count = 0;
	while (count < NUMS_PER_THREAD) {
		while (isRemoved[i]) i = ++i % NUMS_PER_THREAD;
		if (skippy->remove(nums[i])) {
			isRemoved[i] = true;
			count++;
			cout << ' ';
		}
		i++;
	}

	pthread_exit(NULL);
}

/**
	Basic test driver for skip list stuff!
*/
int main() {
	srand(time(NULL));

	SkipList<int> *skippy = new SkipList<int>();

	pthread_t threads_add[THREADS_ADD];
	thread_data threadarg_add[THREADS_ADD];


	pthread_t threads_rem[THREADS_REMOVE];
	thread_data threadarg_rem[THREADS_REMOVE];

	int rc;
	// Start add threads
	for (int i = 0; i < THREADS_ADD; i++) {
		threadarg_add[i].thread_id = i;
		threadarg_add[i].skippy = skippy;

		rc = pthread_create(&threads_add[i], NULL, skipListAdd, (void*) &threadarg_add[i]);
		if (rc) {
			cout << "Unable to create thread " << rc << endl;
			exit(1);
		}
	}
	// Start remove threads
	for (int i = 0; i < THREADS_REMOVE; i++) {
		threadarg_rem[i].thread_id = i;
		threadarg_rem[i].skippy = skippy;

		rc = pthread_create(&threads_rem[i], NULL, skipListRemove, (void*) &threadarg_rem[i]);
		if (rc) {
			cout << "Unable to create thread " << rc << endl;
			exit(1);
		}
	}

	for (int i = 0; i < THREADS_ADD; i++) {
		pthread_join(threads_add[i], NULL);
	}
	//cout << "All add threads complete" << endl;
	for (int i = 0; i < THREADS_REMOVE; i++) {
		pthread_join(threads_rem[i], NULL);
	}
	cout << endl;
	skippy->print();

	return 0;
}

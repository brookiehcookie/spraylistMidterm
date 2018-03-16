// "It's SprayList time, bitches!" - Dijkstra

#include <iostream>

// Really simple boolean implementation...
// there's probably an actual C++ bool type somewhere -> bool

#define bool int
#define true 1
#define false 0

int main() {
  SkipList<int> skippy;
  skippy.add(1);
  skippy.add(2);
  skippy.add(3);
  skippy.add(4);
  skippy.remove(3);
  
  return 0;
}

//*******************************************//

// Class skiplist (public final)
template <class T>
class SkipList {
  public:
    // constructor
    SkipList::SkipList(); 
	bool add(T const&); 
    bool remove(T const&);
}

template <class T>
SkipList::SkipList() {
  
}

template <class T>
bool SkipList::add(T x) 
{
  int topLevel = randomLevel(); // need random level method
  int bottomLevel = 0;
  Node<T>[] preds = (Node<T>[]) new Node[MAX_LEVEL +1]; // need C++ equiv
  Node<T>[] succs = (Node<T>[]) new Node[MAX_LEVEL +1]; // need C++ equiv
  while(true)
  {
    bool found = find(x,preds, succs); // need find method
    if (found)
      return false;
    else
    {
      Node<T> newNode = new Node(x, topLevel); // C++ equiv need
      for(int level = bottomLevel; level <= topLevel; level++)
      {
        Node<T> succ = succs[level]; // c++ equiv need
        newNode.next[level].set(succ,false); // c++ equiv need
        
        Node<t> pred = preds[bottomLevel];
        if(!pred.next[bottomLevel].compareAndSet(succ, newNode, false,false)) // need equiv. in C++
        {
          continue;
        }
        for(int level = bottomLevel+1; level <= topLevel; level++)
        {
         while(true)
         {
           pred = preds[level];
           succ = succ[level];
           if(pred.next[level].compareAndSet(succ,newNode, false, false)) // need equiv in c++
             break;
           find(x, preds, succ); // need find method
         }
        }
        return true;
      }
    }
  }
}

template <class T>
bool SkipList::remove(T x)
{
  int bottomLevel = 0;
  Node<T>[] preds = (Node<T>[]) new Node [MAX_LEVEL +1]; // c++ equiv need
  Node<T>[] succs = (Node<T>[]) new Node [MAX_LEVEL +1]; // C++ equiv need
  Node<T> succ;
  
  while(true)
  {
    bool found = find(x,preds, succs); // need find method
    if (!found)
      return false;
    else
    {
      Node<T>  nodeToRemove = succ[bottomLevel]; // need c++ equiv
      for(int level = nodeToRemove.topLevel; level >= bottomLevel+1; level--)
      {
        bool[] marked = (false);
        succ = nodeToRemove.next[level].get(marked); // get c++ equiv
        while(!marked[0])
        {
          nodeToRemove.next[level].compareAndSet(succ, succ, false, true); // need C++ equiv
          succ = nodeToRemove.next[level].get(marked);
        }
      }
      bool[] marked = {false};
      succ = nodeToRemove.next[bottomLevel].get(marked); // need C++ equiv
      
      while(true)
      {
        boolean iMarkedIt = nodeToRemove.next[bottomLevel.compareAndSet(succ, succ, false, true); // Need C++ Equiv
        succ = siccs[bottomLevel].compareAndSet(succ, succ, false, true);
        if(iMarketIt)
        {
          find(x,preds,succs); // need find method
          return true;
        }
        else if(marked[0])
          return false;
      }
    }
  }
 }
}

//*******************************************//

// Class Node (public, final, static)
template <class T>
class Node
{ 
    final T value;
  	final int key;
    final AtomicMarkableReference<Node<T>>[] next; // equivalent of this to look up
  	private int topLevel;
	public:
    // Constructor for sentinel Nodes
  	Node::Node(int key);
    // Constructor for regular Nodes
    Node:::Node(T x, int height);
}

 // Constructor for sentinel Nodes
 template <class T>
 class Node::Node(int key)
 {
   value = null; // null or NULL in c++...
   key = key;
   next = (AtomicMarkableReference>Node<T>>[])
   new AtomicMarkableReference(MAX_LEVEL + 1);   // need C++ Equivalent
   for (int i = 0; i < next.length; i++)   
   {
     next[i] = new AtomicMarkableReference<Node<T>:(null,false); // need C++ equivalent
   }
   topLevel = MAX_LEVEL;
 }

// Constructor for regular Nodes
template <class T>
Node:::Node(T x, int height)
{
  value = x;
  key = x.hashCode(); // need C++ equivalent
  next = (AtomicMarkableReference>Node<T>>[])
  new AtomicMarkableReference(height + 1);   // need C++ Equivalent
  for(int i = 0; i < next.length; i++)
  {
    next[i] = new AtomicMarkableReference<Node<T>>(null,false); // need C++ equivalent
  }
  topLevel = height;
}































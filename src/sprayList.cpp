

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

#pragma once

//STL은 RELEASE 모드의 최적화 컴파일 된다는 가정하에 쓰는거야. DEBUG 모드로는 너무 성능저하가 크다!!
// a

#include<iostream>

template <typename T>
class CList
{
public:
	typedef struct Node
	{
		T			 _data;
		struct Node* _next;
		struct Node* _prev;
	}Node;

	class iterator
	{
	private:
		Node* _node;
	public:
		iterator(Node* node = nullptr)
		{
			_node = node;
		}

		iterator& operator++()
		{
			_node = _node->_next;
			return *this;
		}

		const iterator operator++(int)
		{
			iterator temp(_node);
			_node = _node->_next;
			return temp;
		}

		iterator& operator--()
		{
			_node = _node->_prev;
			return *this;
		}

		const iterator operator--(int)
		{
			iterator temp(_node);
			_node = _node->_prev;
			return temp;
		}

		T& operator*()
		{
			return _node->_data;
		}

		iterator operator+(int cnt)
		{
			Node* node = _node;

			for (int i = 0; i < cnt; i++)
			{
				node = node->_next;
			}

			return  iterator(node);
		}

		bool operator==(const iterator& ref)
		{
			return _node == ref._node;
		}

		bool operator!=(const iterator& ref)
		{
			return !(*this == ref);
		}

		friend iterator CList<T>::erase(iterator);
		friend iterator CList<T>::insert(iterator, const T&);

		//swap를 위한
		friend void CList<T>::swap(iterator&, iterator&&);
	};

public:
	CList();
	~CList();

	iterator begin()
	{
		return iterator(head._next);
	}

	iterator end()
	{
		return iterator(&tail);
	}

	iterator erase(iterator iter)
	{
		iterator retIter(iter._node->_next);

		Node* dNode = iter._node;
		dNode->_next->_prev = dNode->_prev;
		dNode->_prev->_next = dNode->_next;

		delete dNode;
		_size -= 1;
		return retIter;
	}


	iterator insert(iterator iter, const T& data)
	{
		Node* newNode = new Node;
		newNode->_data = data;

		newNode->_next = iter._node;
		newNode->_prev = iter._node->_prev;

		iter._node->_prev->_next = newNode;
		iter._node->_prev = newNode;
		_size++;

		return iterator(newNode);
	}


	void push_front(T data);
	void push_back(T data);
	T pop_front();
	T pop_back();
	void clear();
	int size() { return _size; };
	bool empty()
	{
		if (head.next == tail)
			return true;
		else
			return false;
	};

	void swap(iterator& temp1, iterator&& temp2);

private:
	// 그냥 객체로 둔 이유?? : 더미노드를 둠으로써 구현의 편의성 확대
	Node head;
	Node tail;
	int _size = 0;
};

template<typename T>
CList<T>::CList()
{
	head._next = &tail;
	head._prev = NULL;

	tail._prev = &head;
	tail._next = NULL;
}

template<typename T>
CList<T>::~CList()
{
	clear();
}

template<typename T>
void CList<T>::push_front(T data)
{
	Node* newNode = new Node;
	newNode->_data = data;

	newNode->_next = head._next;
	newNode->_prev = &head;

	head._next->_prev = newNode;
	head._next = newNode;

	_size++;
}

template<typename T>
void CList<T>::push_back(T data)
{
	Node* newNode = new Node;
	newNode->_data = data;

	newNode->_next = &tail;
	newNode->_prev = tail._prev;

	tail._prev->_next = newNode;
	tail._prev = newNode;

	_size++;
}

template<typename T>
T CList<T>::pop_front()
{
	Node* dNode = head._next;
	T retData = dNode->_data;

	head._next = dNode->_next;
	dNode->_next->_prev = &head;

	_size--;

	delete dNode;
	return retData;
}



template<typename T>
T CList<T>::pop_back()
{
	Node* dNode = tail._prev;
	T retData = dNode->_data;

	tail._prev = dNode->_prev;
	dNode->_prev->_next = &tail;

	_size--;
	delete dNode;

	return retData;
}

template<typename T>
void CList<T>::clear()
{
	/*Node* dNode;
	Node* dNextNode = head._next;

	while (dNextNode != &tail)
	{
		dNode = dNextNode;
		dNextNode = dNextNode->_next;

		delete dNode;
	}*/

	while (_size > 0)
		pop_back();

	_size = 0;
	head._next = &tail;
	tail._prev = &head;
}

template<typename T>
void CList<T>::swap(iterator& temp1, iterator&& temp2)
{
	Node* tempNode;

	temp1._node->_prev->_next = temp2._node;
	temp1._node->_next = temp2._node->_next;

	temp2._node->_prev = temp1._node->_prev;
	temp2._node->_next->_prev = temp1._node;

	temp1._node->_prev = temp2._node;
	temp2._node->_next = temp1._node;
}
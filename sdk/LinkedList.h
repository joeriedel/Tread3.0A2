///////////////////////////////////////////////////////////////////////////////
// LinkedList.h
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Joe Riedel
// All rights reserved.
//
// Redistribution and use in source and binary forms, 
// with or without modification, are permitted provided 
// that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
//
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation and/or 
// other materials provided with the distribution. 
//
// Neither the name of the <ORGANIZATION> nor the names of its contributors may be 
// used to endorse or promote products derived from this software without specific 
// prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
// OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#define LL_HEAD		0x20000000
#define LL_TAIL		0x40000000
#define LL_CURRENT	0x80000000

#define LL_INS_BEFORE	0
#define LL_INS_AFTER	1

template <class T>
class CLinkedList
{
private:

	bool m_bAutoDelete;	// We may not always want the destructor to delete the nodes in the list.

public:
	typedef T DataType;

	CLinkedList();
	//CLinkedList(const CLinkedList&);
	//CLinkedList(const CLinkedList*);
	~CLinkedList();

	void SetAutoDelete(bool bAutoDelete) { m_bAutoDelete = bAutoDelete; }
	bool GetAutoDelete() { return m_bAutoDelete; }

	void MakeWrapperList(const CLinkedList& List);

	//add item to the list
	bool AddItem(T* pNode, unsigned int nPos = LL_TAIL);
	bool AddToHead(T* pNode);
	bool AddToTail(T* pNode);

	//
	// does not move LL_CURRENT
	// may change LL_HEAD, LL_TAIL
	//
	bool InsertItem( T* pNode, T* pPos, int pos = LL_INS_AFTER );

	T* ReplaceNode(T* pNode, bool bDelete = false);

	//remove the item but do not destroy
	T* RemoveItem(unsigned int nPos);
	bool RemoveItem(T* pNode);

	//delete the item
	bool DeleteItem(unsigned int nPos);
	bool DeleteItem(T* pNode);

	//Get a pointer to the item
	T* GetItem(unsigned int nPos);
	
	// Returns true if in the list.
	bool InList(T* pObject);

	// Helpers.
	void First() { SetPosition(LL_HEAD); }
	void Last() { SetPosition(LL_TAIL); }
	T* ResetPos() { First(); return m_pHead; }

	void SetPosition(unsigned int nPos);
	void SetPosition(T* pNode);

	T* GetNextItem();
	T* GetPrevItem();
	T* GetCurItem() const;
	int GetCount() const;
	bool IsEmpty() const;
	void DestroyList();
	int SizeOfList() const;

	T* ReleaseList();
	
	void Copy( const CLinkedList& list );

	//
	// return true from walkproc to cancel traversal.
	//
#define WALKLIST_STOP		1
#define WALKLIST_RESTART	2

	void WalkList( int (*WalkProc) (T* node, void* parm, void* parm2), void* parm = 0, void* parm2 = 0 );

private:

	T* m_pHead;
	T* m_pTail;
	T* m_pCur;
};

class CLLObject
{
public:
	
	void* prev, *next;

	CLLObject() { prev = next = 0; }
	CLLObject(const CLLObject& ObjectToCopy)
	{
		prev = 0;
		next = 0;
	}
	virtual ~CLLObject() {}
};

template <class T>
void CLinkedList<T>::WalkList( int (*WalkProc) (T* node, void* parm, void* parm2), void* parm, void* parm2 )
{
	T* node, *next;

	if( !WalkProc )
		return;

	next = 0;
	for( node = ResetPos(); node != 0; node = next )
	{
		SetPosition( node );
		next = GetNextItem();

		int r = WalkProc( node, parm, parm2 );
		
		if( r == WALKLIST_STOP )
			break;
		if( r == WALKLIST_RESTART )
		{
			next = ResetPos();
		}
	}
}

template <class T> 
CLinkedList<T>::CLinkedList()
{
	m_bAutoDelete = true;
	m_pHead = m_pTail = m_pCur = NULL;
}

//Destroy the linked list
template <class T> 
CLinkedList<T>::~CLinkedList()
{
	if(m_bAutoDelete)
		DestroyList();
}

//// Copies the list.
//template <class T>
//CLinkedList<T>::CLinkedList(const CLinkedList& ListToCopy)
//{
//	T* pNode, *pNodeCopy;
//
//	m_pHead = m_pTail = m_pCur = NULL;
//	for(pNode = ListToCopy.m_pHead; pNode; pNode = (T*)pNode->next)
//	{
//		pNodeCopy = new T(*pNode);
//		this->AddItem(pNodeCopy, LL_TAIL);
//	}
//
//	m_bAutoDelete = true;
//}
//
//// Copies the list.
//template <class T>
//CLinkedList<T>::CLinkedList(const CLinkedList* pListToCopy)
//{
//	T* pNode, *pNodeCopy;
//
//	m_pHead = m_pTail = m_pCur = NULL;
//	for(pNode = pListToCopy->m_pHead; pNode; pNode = (T*)pNode->next)
//	{
//		pNodeCopy = new T(pNode);
//		this->AddItem(pNodeCopy, LL_TAIL);
//	}
//
//	m_bAutoDelete = true;
//}
template <class T>
void CLinkedList<T>::Copy( const CLinkedList& ListToCopy )
{
	T* pNode, *pNodeCopy;

	if( m_bAutoDelete )
		DestroyList();
	else
		ReleaseList();

	m_pHead = m_pTail = m_pCur = NULL;

	for(pNode = ListToCopy.m_pHead; pNode; pNode = (T*)pNode->next)
	{
		pNodeCopy = new T(*pNode);
		this->AddItem(pNodeCopy, LL_TAIL);
	}

	m_bAutoDelete = true;
}

template <class T>
void CLinkedList<T>::MakeWrapperList(const CLinkedList& List)
{
	m_pHead = List.m_pHead;
	m_pTail = List.m_pTail;
	m_pCur  = List.m_pCur;
	m_bAutoDelete = false;
}

// Returns true if in the list.
template <class T>
bool CLinkedList<T>::InList(T* pObject)
{
	T* pNode;

	First();
	for(pNode = GetCurItem(); pNode != NULL; pNode = GetNextItem())
		if(pNode == pObject)
			return true;
	
	return false;
}

template <class T> 
bool CLinkedList<T>::InsertItem( T* pNode, T* pPos, int pos )
{
	if( pNode == 0 || pPos == 0 )
		return false;

	if( pos != LL_INS_BEFORE && pos != LL_INS_AFTER )
		return false;

	if( pos == LL_INS_BEFORE )
	{
		if( pPos->prev )
			((T*)pPos->prev)->next = pNode;
		pNode->prev = pPos->prev;
		pNode->next = pPos;
		pPos->prev = pNode;

		if( pPos == m_pHead )
			m_pHead = pNode;
	}
	else
	{
		if( pPos->next )
			((T*)pPos->next)->prev = pNode;
		pNode->prev = pPos;
		pNode->next = pPos->next;
		pPos->next = pNode;

		if( pPos == m_pTail )
			m_pTail = pNode;
	}

	return true;
}

//Adds an item to the list at the specified position
template <class T> 
bool CLinkedList<T>::AddItem(T* pNode, unsigned int nPos)
{
	if(pNode == NULL)
		return false;

	if(IsEmpty()) //Empty list
	{
		m_pHead = pNode;
		m_pTail = pNode;
		m_pCur = pNode;

		//set nodes pointers
		m_pHead->next = NULL;
		m_pHead->prev = NULL;
		return true;
	}

	//List is not empty
	switch(nPos)
	{
	case LL_HEAD:			//Add to head
		pNode->next = m_pHead;
		m_pHead->prev = pNode;
		m_pHead = m_pCur = pNode;
		pNode->prev = NULL;

		if(pNode->next == NULL)
			m_pTail = pNode;

		break;
	case LL_TAIL:			//Add to tail
		m_pTail->next = pNode;
		pNode->prev = m_pTail;
		m_pTail = m_pCur = pNode;
		
		pNode->next = NULL;
		break;
	case LL_CURRENT:			//Add to current position
		pNode->next = m_pCur->next;
		m_pCur->next = pNode;
		pNode->prev = m_pCur;
		m_pCur = pNode;		//track current node
			
		if(pNode->next == NULL)
			m_pTail = pNode;
		else
		{
			T* pNext = (T*)pNode->next;
			pNext->prev = pNode;
		}

		break;
	default:
		return false;
	}
	
	return true;
}

template <class T> 
bool CLinkedList<T>::AddToHead(T* pNode)
{
	if(pNode == NULL)
		return false;

	if(IsEmpty()) //Empty list
	{
		m_pHead = pNode;
		m_pTail = pNode;
		m_pCur = pNode;

		//set nodes pointers
		m_pHead->next = NULL;
		m_pHead->prev = NULL;
		return true;
	}

	//List is not empty
	pNode->next = m_pHead;
	m_pHead->prev = pNode;
	m_pHead = m_pCur = pNode;
	pNode->prev = NULL;

	if(pNode->next == NULL)
		m_pTail = pNode;
	
	return true;
}

template <class T> 
bool CLinkedList<T>::AddToTail(T* pNode)
{
	if(pNode == NULL)
		return false;

	if(IsEmpty()) //Empty list
	{
		m_pHead = pNode;
		m_pTail = pNode;
		m_pCur = pNode;

		//set nodes pointers
		m_pHead->next = NULL;
		m_pHead->prev = NULL;
		return true;
	}

	m_pTail->next = pNode;
	pNode->prev = m_pTail;
	m_pTail = m_pCur = pNode;
		
	pNode->next = NULL;
		
	return true;
}

template <class T> 
T* CLinkedList<T>::ReplaceNode(T* pNode, bool bDelete)
{
	if(m_pCur == NULL)
		return NULL;

	T* pTemp;
	T* pOldNode = m_pCur;

	pTemp = (T*)m_pCur->next;
	if(pTemp != NULL)
		pTemp->prev = pNode;
	pTemp = (T*)m_pCur->prev;
	if(pTemp != NULL)
		pTemp->next = pNode;

	if(m_pHead == m_pCur)
		m_pHead = pNode;
	if(m_pTail == m_pCur)
		m_pTail = pNode;

	pNode->next = m_pCur->next;
	pNode->prev = m_pCur->prev;
	m_pCur = pNode;

	pOldNode->prev = pOldNode->next = NULL;

	if(bDelete)
	{
		delete pOldNode;
		pOldNode = NULL;
	}

	return pOldNode;
}

//delete an item from the list at the specified position
template <class T> 
T* CLinkedList<T>::RemoveItem(unsigned int nPos)
{
	T* pTemp;

	if(IsEmpty())
		return NULL;

	switch(nPos)
	{
	case LL_HEAD:		//HEAD
		pTemp = m_pHead;	//track node to be deleted
		m_pHead = (T*)pTemp->next;	//redirect head node

		if(m_pHead != NULL)
			m_pHead->prev = NULL;	//head node previous should be NULL
		else
			m_pTail = NULL;

		if(m_pCur == pTemp)		//adjust current pointer if needed
			m_pCur = (T*)pTemp->next;

		break;
	case LL_TAIL:		//TAIL
		pTemp = m_pTail;
		m_pTail = (T*)pTemp->prev;

		if(m_pTail != NULL)
			m_pTail->next = NULL;
		else
			m_pHead = NULL;

		if(pTemp == m_pCur)
			m_pCur = (T*)pTemp->prev;
		
		break;
	case LL_CURRENT:		//CURRENT

		if(m_pCur == NULL)
			return false;

		pTemp = m_pCur;

		if(pTemp->next == NULL)
		{
			if(pTemp->prev == NULL)		//last item in list
				m_pHead = m_pTail = m_pCur = NULL;
			else
			{
				m_pTail = (T*)pTemp->prev;
				m_pTail->next = NULL;
				m_pCur = NULL;
			}
		}
		else
		{
			if(pTemp->prev == NULL)		//last item in list
			{
				m_pHead = (T*)pTemp->next;
				m_pHead->prev = NULL;
				m_pCur = m_pHead;
			}
			else
			{
				m_pCur = (T*)pTemp->next;
				m_pCur->prev = pTemp->prev;
				if(m_pCur->prev)
				{
					T* pPrev = (T*)m_pCur->prev;
					pPrev->next = m_pCur;
				}
			}
		}

		break;
	default:
		return NULL;
	}

	//delete pTemp;
	pTemp->next = pTemp->prev = NULL;
	return pTemp;

	//return true;
}

//delete an item by its pointer
template <class T> 
bool CLinkedList<T>::RemoveItem(T* pNode)
{
	m_pCur = pNode;
	T* pTemp = RemoveItem(LL_CURRENT);

	if(pTemp == NULL)
		return false;
	else
		return true;
}

template <class T> 
bool CLinkedList<T>::DeleteItem(unsigned int nPos)
{
	T* pTemp;

	if(IsEmpty())
		return false;

	switch(nPos)
	{
	case LL_HEAD:		//HEAD
		pTemp = m_pHead;	//track node to be deleted
		m_pHead = (T*)pTemp->next;	//redirect head node

		if(m_pHead != NULL)
			m_pHead->prev = NULL;	//head node previous should be NULL
		else
			m_pTail = NULL;

		if(m_pCur == pTemp)		//adjust current pointer if needed
			m_pCur = (T*)pTemp->next;

		break;
	case LL_TAIL:		//TAIL
		pTemp = m_pTail;
		m_pTail = (T*)pTemp->prev;

		if(m_pTail != NULL)
			m_pTail->next = NULL;
		else
			m_pHead = NULL;

		if(pTemp == m_pCur)
			m_pCur = (T*)pTemp->prev;
		
		break;
	case LL_CURRENT:		//CURRENT

		if(m_pCur == NULL)
			return false;

		pTemp = m_pCur;

		if(pTemp->next == NULL)
		{
			if(pTemp->prev == NULL)		//last item in list
				m_pHead = m_pTail = m_pCur = NULL;
			else
			{
				m_pTail = (T*)pTemp->prev;
				m_pTail->next = NULL;
				m_pCur = NULL;
			}
		}
		else
		{
			if(pTemp->prev == NULL)		//last item in list
			{
				m_pHead = (T*)pTemp->next;
				m_pHead->prev = NULL;
				m_pCur = m_pHead;
			}
			else
			{
				m_pCur = (T*)pTemp->next;
				m_pCur->prev = pTemp->prev;
				if(m_pCur->prev)
				{
					T* pPrev = (T*)m_pCur->prev;
					pPrev->next = m_pCur;
				}
			}
		}

		break;
	default:
		return false;
	}

	delete pTemp;
	
	return true;
}

template <class T> 
bool CLinkedList<T>::DeleteItem(T* pNode)
{
	m_pCur = pNode;
	return DeleteItem(LL_CURRENT);
}

//get the item at the requested location.
template <class T> 
T* CLinkedList<T>::GetItem(unsigned int nPos)
{
	T* pTemp;

	if(IsEmpty())
		return NULL;

	switch(nPos)
	{
	case LL_HEAD:		//HEAD
		return m_pHead;
		break;
	case LL_TAIL:		//TAIL
		return m_pTail;
		break;
	case LL_CURRENT:		//CURRENT
		return m_pCur;
		break;
	}

	SetPosition(LL_HEAD);

	for(UINT n = 0; n < nPos; n++)
	{
		pTemp = GetNextItem();
		if(pTemp == NULL)
			return NULL;
	}

	return m_pCur;
}

//Set the current pointer to the desired position
template <class T> 
void CLinkedList<T>::SetPosition(unsigned int nPos)
{
	T* pTemp;
	
	if(IsEmpty())
		return;

	if(nPos == LL_HEAD)
		m_pCur = m_pHead;
	else if(nPos == LL_TAIL)
		m_pCur = m_pTail;
	else
	{
		SetPosition(LL_HEAD);

		for(unsigned int n = 0; n < nPos; n++)
		{
			pTemp = GetNextItem();
			if(pTemp == NULL)
				return;
		}
	}
}

template <class T>
void CLinkedList<T>::SetPosition(T* pNode)
{
	if(IsEmpty())
		return;

	m_pCur = pNode;
}

// Get's the current item.
template <class T> 
T* CLinkedList<T>::GetCurItem() const
{
	return m_pCur;
}

//get the next item in the list
template <class T> 
T* CLinkedList<T>::GetNextItem()
{
	if(IsEmpty())
		return NULL;

	if(m_pCur)
		m_pCur = (T*)m_pCur->next;

	return m_pCur;
}

//get the previous item in the list
template <class T>
T* CLinkedList<T>::GetPrevItem()
{
	if(IsEmpty())
		return NULL;

	if(m_pCur)
		m_pCur = (T*)m_pCur->prev;

	return m_pCur;
}

//get the list count
template <class T> 
int CLinkedList<T>::GetCount() const
{
	int n = 0;

	if(IsEmpty())
		return 0;

	for(T* pTemp = m_pHead; pTemp != NULL; pTemp = (T*)pTemp->next)
		n++;

	return n;
}

//Check to see if list is empty
template <class T>  
bool CLinkedList<T>::IsEmpty() const
{
	if(m_pHead == NULL)
		return true;
	else
		return false;
}

//Destroys the list
template <class T> 
void CLinkedList<T>::DestroyList()
{
	T* pTemp = NULL;

	if(IsEmpty())
		return;

	for(; m_pTail != NULL; )
	{
		pTemp = m_pTail;	//point to node
		m_pTail = (T*)m_pTail->prev;	//step back

		delete pTemp;	//delete the node
	}
	
	m_pTail = m_pHead = m_pCur = NULL;
}

template <class T> 
T* CLinkedList<T>::ReleaseList()
{
	T* pTemp = m_pHead;

	m_pTail = m_pHead = m_pCur = NULL;

	return pTemp;
}

template <class T> 
int CLinkedList<T>::SizeOfList() const
{
	T* pTemp;
	int nSize = 0;

	if(IsEmpty())
		return 0;

	for(pTemp = m_pHead; pTemp != NULL; pTemp = (T*)pTemp->next)
	{
		nSize += sizeof(*pTemp);
	}

	return nSize;
}

#endif
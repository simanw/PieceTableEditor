/* AbiWord
 * Copyright (C) 2003 Joaquín Cuenca Abela
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */

#ifndef PT_PIECELIST_H
#define PT_PIECELIST_H

#define DEBUG 1

#include <stddef.h>
#ifdef DEBUG
#include <stdio.h>
#endif

struct pt_Piece;
typedef unsigned int pd_DocPosition;

class pt_PieceList
{
	struct Node;

public:
	class Iterator
	{
	public:
		Iterator() : m_pOwner(NULL), m_pNode(NULL) {}
		
		Iterator& operator++()
		{
			m_pNode = m_pOwner->_next(m_pNode);
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator tmp(*this);
			m_pNode = m_pOwner->_next(m_pNode);
			return tmp;
		}

		Iterator& operator--()
		{
			m_pNode = m_pOwner->_prev(m_pNode);
			return *this;
		}

		Iterator operator--(int)
		{
			Iterator tmp(*this);
			m_pNode = m_pOwner->_prev(m_pNode);
			return tmp;
		}

		bool operator==(const Iterator& other)
		{ return (m_pOwner == other.m_pOwner && m_pNode == other.m_pNode); }

		bool operator!=(const Iterator& other)
		{ return !(*this == other); }

		const pt_Piece* value() const;
		pt_Piece* value();
		
		bool is_valid() const
		{ return m_pNode != 0; }

	private:
		Iterator(pt_PieceList* owner, Node* node = 0) : m_pOwner(owner), m_pNode(node) {}
		const Node* getNode() const { return m_pNode; }
		Node* getNode() { return m_pNode; }

		pt_PieceList* m_pOwner;
		Node* m_pNode;
		friend class pt_PieceList;
	};

	pt_PieceList();
	~pt_PieceList();

	Iterator insertRoot(pt_Piece* new_piece); // throws std::bad_alloc (strong)
	Iterator insertLeft(pt_Piece* new_piece, Iterator& it); // throws std::bad_alloc (strong)
	Iterator insertRight(pt_Piece* new_piece, Iterator& it); // throws std::bad_alloc (strong)
	void erase(Iterator& it); // throws ()
	void fixSize(Iterator& it); // throws ()
	Iterator find(pd_DocPosition pos); // throws ()
	pd_DocPosition documentPosition(const Iterator& it) const; // throws ()

	Iterator begin() { return Iterator(this, _first()); }
	Iterator end() { return Iterator(this); }
	
	size_t size() const { return m_nSize; }
	pd_DocPosition sizeDocument() const;

#ifdef DEBUG
	void print() const;
	bool checkInvariants() const;
#endif

private:
	pt_PieceList(const pt_PieceList&);
	pt_PieceList& operator= (const pt_PieceList&);

	void _insertBST(Node* pn);
	void _insertFixup(Node* pn);
	void _eraseFixup(Node* pn);
	void _leftRotate(Node* x);
	void _rightRotate(Node* x);

#ifdef DEBUG
	int _countBlackNodes(const Iterator& it) const;
#endif

	void delete_tree(Node* node);

	const Node* _next(const Node* pn) const;
	const Node* _prev(const Node* pn) const;
	const Node* _first() const;
	const Node* _last() const;

	Node* _next(Node* pn);
	Node* _prev(Node* pn);
	Node* _first();
	Node* _last();

	Node* m_pLeaf;
	Node* m_pRoot;
	size_t m_nSize;
	friend class Iterator;
};

#endif // PT_PIECELIST_H

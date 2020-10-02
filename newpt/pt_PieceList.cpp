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

#include "pt_PieceList.hpp"

#include <assert.h>
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#include <stdarg.h>
#endif

#include "pt_Piece.hpp"

#define UT_ASSERT(st) assert(st);

///////////////////////////////////////////
// Node
///////////////////////////////////////////

struct pt_PieceList::Node
{
	pt_Piece* item;
	Node* left;
	Node* right;

	Node(pt_Piece* i = 0, Node* l = 0, Node* r = 0)
		: item(i), left(l), right(r) {}

#ifdef DEBUG
	void print()
	{
		if (!item)
			return;
				
		printf("(%i, %i)", item->size_left, item->size);

		if (right && right->item)
		{
			printf(" -> ");
			right->print();
		}
		else
			printf("\n");
	}
#endif
};

///////////////////////////////////////////
// Iterator
///////////////////////////////////////////

pt_Piece*
pt_PieceList::Iterator::value()
{
	return m_pNode->item;
}

///////////////////////////////////////////
// pt_PieceList
///////////////////////////////////////////
pt_PieceList::pt_PieceList()
	: m_pRoot(0),
	  m_nSize(0)
{
}

pt_PieceList::~pt_PieceList()
{
	Node* pn = m_pRoot;
	Node* old_pn = pn;

	while (pn != 0)
	{
		delete pn->item;
		pn = pn->right;
		delete old_pn;
		old_pn = pn;
	}		
}

pt_PieceList::Iterator
pt_PieceList::insertRoot(pt_Piece* new_piece)
{
	Iterator it_null(this, 0);
	return insertRight(new_piece, it_null);
}

pt_PieceList::Iterator
pt_PieceList::insertRight(pt_Piece* new_piece, Iterator& it)
{
	Node* pNode = it.getNode();
	Node* pNewNode;

	UT_ASSERT(m_nSize == 0 || it.is_valid());
	
	pNewNode = new Node(new_piece, 0, 0);

	++m_nSize;

	if (pNode == 0)
		pNode = m_pRoot;

	if (pNode != 0)
	{
		pNewNode->left = pNode;
		pNewNode->right = pNode->right;

		if (pNode->right != 0)
			pNode->right->left = pNewNode;

		pNode->right = pNewNode;
	}
	else
		m_pRoot = pNewNode;

	Iterator new_it(this, pNewNode);
	fixSize(new_it);

	return new_it;
}

pt_PieceList::Iterator
pt_PieceList::insertLeft(pt_Piece* new_piece, Iterator& it)
{
	Node* pNode = it.getNode();
	Node* pNewNode;

	UT_ASSERT(m_nSize == 0 || it.is_valid());
	
	pNewNode = new Node(new_piece, 0, 0);

	++m_nSize;

	if (pNode == 0)
		pNode = m_pRoot;

	if (pNode != 0)
	{
		pNewNode->right = pNode;
		pNewNode->left = pNode->left;

		if (pNode->left != 0)
			pNode->left->right = pNewNode;
		else
			m_pRoot = pNewNode;

		pNode->left = pNewNode;
	}
	else
		m_pRoot = pNewNode;

	Iterator new_it(this, pNewNode);
	fixSize(new_it);

	return Iterator(this, pNewNode);
}

void
pt_PieceList::erase(Iterator& it)
{
	if (!it.is_valid())
		return;

	Node* pNode = it.getNode();
	--m_nSize;

	UT_ASSERT(pNode);
	
	if (pNode == m_pRoot)
	{
		m_pRoot = pNode->right;

		if (m_pRoot != 0)
		{
			m_pRoot->left = 0;
			Iterator new_it(this, m_pRoot);
			fixSize(new_it);
		}
	}
	else if (pNode->right == 0)
	{
		pNode->left->right = 0;
	}
	else
	{
		pNode->left->right = pNode->right;
		pNode->right->left = pNode->left;
		Iterator new_it(this, pNode->left);
		fixSize(new_it);
	}

	delete pNode->item;
	delete pNode;
}

void
pt_PieceList::fixSize(Iterator& it)
{
	UT_ASSERT(it.is_valid());
	Node* pn = it.getNode();
	pt_Piece* item;

	while (pn != 0)
	{
		item = pn->item;

		if (pn->left)
			pn->item->size_left = pn->left->item->size_left + pn->left->item->size;
		else
			pn->item->size_left = 0;

		pn = pn->right;
	}
}

pt_PieceList::Iterator
pt_PieceList::find(pd_DocPosition pos)
{
	Node* pn = m_pRoot;

	while (pn != 0)
	{
		if (pn->item->size_left + pn->item->size > pos)
			return Iterator(this, pn);
		else
			pn = pn->right;
	}

	return Iterator(this, 0);
}

/* that's O(n), but it will be O(1) if we just put the root at the extreme right of the list */
pd_DocPosition
pt_PieceList::sizeDocument() const
{
	Node* pn = m_pRoot;

	if (pn == 0)
		return 0;

	while (pn->right != 0)
		pn = pn->right;

	return pn->item->size_left + pn->item->size;
}

pd_DocPosition
pt_PieceList::documentPosition(const Iterator& it) const
{
	UT_ASSERT(it.is_valid());

	const Node* pn = it.getNode();
	pd_DocPosition pos = pn->item->size_left;

	return pos;
}

const pt_PieceList::Node*
pt_PieceList::_prev(const Node* pn) const
{
	UT_ASSERT(pn != 0);
	return pn->left;
}

pt_PieceList::Node*
pt_PieceList::_prev(Node* pn)
{
	return const_cast<Node*> (_prev(const_cast<const Node*> (pn)));
}

const pt_PieceList::Node*
pt_PieceList::_next(const Node* pn) const
{
	UT_ASSERT(pn != 0);
	return pn->right;
}

pt_PieceList::Node*
pt_PieceList::_next(Node* pn)
{
	return const_cast<Node*> (_next(const_cast<const Node*> (pn)));
}

const pt_PieceList::Node*
pt_PieceList::_first() const
{
	return m_pRoot;
}

pt_PieceList::Node*
pt_PieceList::_first()
{
	return m_pRoot;
}

const pt_PieceList::Node*
pt_PieceList::_last() const
{
	Node* pn = m_pRoot;
	
	while(pn->right != 0)
		pn = pn->right;

	return pn;
}

pt_PieceList::Node*
pt_PieceList::_last()
{
	Node* pn = m_pRoot;
	
	while(pn->right != 0)
		pn = pn->right;

	return pn;
}

#ifdef DEBUG
void
pt_PieceList::print() const
{
	if (m_pRoot != 0)
		m_pRoot->print();
	else
		printf("empty set\n");
}

bool
pt_PieceList::checkInvariants() const
{
	Node* pn = m_pRoot;
	size_t nb_nodes = 0;
	pd_DocPosition offset = 0;

	while (pn != 0)
	{
		++nb_nodes;

		if (pn->item == 0)
			return false;

		if (pn->item->size_left != offset)
			return false;

		offset += pn->item->size;
		pn = pn->right;
	}

	if (nb_nodes != m_nSize)
		return false;

	return true;
}

#endif

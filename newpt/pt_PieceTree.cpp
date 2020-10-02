/* AbiWord
 * Copyright (C) 2001, 2002, 2003 Joaquín Cuenca Abela
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

#include "pt_PieceTree.hpp"

#include <assert.h>
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#include <stdarg.h>
#endif

#include "pt_Piece.hpp"

#define UT_ASSERT(st) assert(st);

#ifdef DEBUG
static size_t indent;

static void printnl(const char* st, ...)
{
	va_list args;
	size_t i;

	for (i = 0; i < indent; ++i)
		printf("  ");
	
	va_start(args, st);
	vprintf(st, args);
	va_end(args);
	
	printf("\n");
}
#endif

///////////////////////////////////////////
// Node
///////////////////////////////////////////

struct pt_PieceTree::Node
{
	enum Color { red, black } color;
	pt_Piece* item;
	Node* left;
	Node* right;
	Node* parent;

	Node(Color c = red, pt_Piece* i = 0, Node* l = 0, Node* r = 0, Node* p = 0)
		: color(c), item(i), left(l), right(r), parent(p) {}

#ifdef DEBUG
	void print()
	{
		if (!item)
			return;
				
		printnl("%c (%i, %i)", color == red ? 'R' : 'B', item->size_left, item->size);

		if (left && left->item)
		{
			printnl("--- left tree:");
			++indent;
			left->print();
			--indent;
		}

		if (right && right->item)
		{
			printnl("--- right tree:");
			++indent;
			right->print();
			--indent;
		}
	}
#endif
};

///////////////////////////////////////////
// Iterator
///////////////////////////////////////////

pt_Piece*
pt_PieceTree::Iterator::value()
{
	return m_pNode->item;
}

///////////////////////////////////////////
// pt_PieceTree
///////////////////////////////////////////
pt_PieceTree::pt_PieceTree()
	: m_pLeaf(new Node(Node::black)),
	  m_pRoot(m_pLeaf),
	  m_nSize(0),
	  m_nDocumentSize(0)
{
}

pt_PieceTree::~pt_PieceTree()
{
	if (m_pRoot != m_pLeaf)
		delete_tree(m_pRoot);

	delete m_pLeaf;
}

#if 0
void
pt_PieceTree::_insertBST(Node* pNewNode, pd_DocPosition pos)
{
	Node* pNode = m_pRoot;
	
	UT_ASSERT(pNewNode);

	while (pNode != m_pLeaf)
	{
		if (m_comp(pNewNode->item, pNode->item))
		{
			if (pNode->left != m_pLeaf)
				pNode = pNode->left;
			else
			{
				pNewNode->parent = pNode;
				pNode->left = pNewNode;
				break;
			}
		}
		else
		{
			if (pNode->right != m_pLeaf)
				pNode = pNode->right;
			else
			{
				pNewNode->parent = pNode;
				pNode->right = pNewNode;
				break;
			}
		}
	}

	if (pNode == m_pLeaf)
	{
		m_pRoot = pNewNode;
		m_pRoot->color = Node::black;
	}
}
#endif

void
pt_PieceTree::_insertFixup(Node* x)
{
	Node* y;
	UT_ASSERT(x);

	Iterator it(this, x);
	fixSize(it);
	
	while ((x != m_pRoot) && (x->parent->color == Node::red))
	{
		if (x->parent == x->parent->parent->left)
		{
			/* If x's parent is a left, y is x's right 'uncle' */
			y = x->parent->parent->right;
		    if (y && y->color == Node::red)
			{
				/*       bz          rz
				 *       / \         / \
				 *     rw  ry  =>  bw  by
				 *     /           /
				 *   rx          rx
				 */
				/* case 1 - change the colours */
				x->parent->color = Node::black;
				y->color = Node::black;
				x->parent->parent->color = Node::red;
				/* Move x up the tree */
				x = x->parent->parent;
			}
			else
			{
				/* y is a black node */
				if (x == x->parent->right)
				{
					/*       bz            bz
					 *       / \           / \
					 *     rw  by  =>    rx  by
					 *     / \           / \
					 *   b1  rx        rw  b3
					 *       / \       / \
 					 *     b2   b3   b1  b2
					 */
					/* and x is to the right */ 
					/* case 2 - move x up and rotate */
					x = x->parent;
					_leftRotate(x);
				}
				/* case 3 */
				x->parent->color = Node::black;
				x->parent->parent->color = Node::red;
				_rightRotate(x->parent->parent);
			}
		}
		else
		{
			/* If x's parent is a left, y is x's right 'uncle' */
			y = x->parent->parent->left;
			if (y && y->color == Node::red)
			{
				/* case 1 - change the colors */
				x->parent->color = Node::black;
				y->color = Node::black;
				x->parent->parent->color = Node::red;
				/* Move x up the tree */
				x = x->parent->parent;
			}
			else
			{
				/* y is a black node */
				if (x == x->parent->left)
				{
					/* and x is to the left */ 
					/* case 2 - move x up and rotate */
					x = x->parent;
					_rightRotate(x);
				}
				/* case 3 */
				x->parent->color = Node::black;
				x->parent->parent->color = Node::red;
				_leftRotate(x->parent->parent);
			}
		}
	}

	/* Colour the root black */
	m_pRoot->color = Node::black;
}

#if 0
Iterator
pt_PieceTree::insert(pt_Piece* new_piece)
{
	Node* pNode = it.getNode();
	Node* pNewNode = new Node(Node::red, new_piece, m_pLeaf, m_pLeaf, 0);

	++m_nSize;
	_insertBST(pNewNode);
	_insertFixup(pNewNode);

	return Iterator(this, pNewNode);
}
#endif

/**
 * Insert a new piece as the root of the tree.  The tree should be empty before performing
 * this operation.
 *
 * @param new_piece the piece to become root
 */
pt_PieceTree::Iterator
pt_PieceTree::insertRoot(pt_Piece* new_piece)
{
	Iterator it_null(this, 0);
	return insertRight(new_piece, it_null);
}

/**
 * Insert a new piece to the right of a given node.
 * This operation has the exceptional guarantee strong.
 *
 * @param new_piece the piece to insert
 * @param it iterator to the reference node
 */
pt_PieceTree::Iterator
pt_PieceTree::insertRight(pt_Piece* new_piece, Iterator& it)
{
	Node* pNode = it.getNode();

	UT_ASSERT(m_nSize == 0 || it.is_valid());
	
	Node* pNewNode = new Node(Node::red, new_piece, m_pLeaf, m_pLeaf, 0);

	new_piece->size_left = 0;
	
	++m_nSize;
	m_nDocumentSize += new_piece->size;

	if (!it.is_valid())
		m_pRoot = pNewNode;
	else if (pNode->right == m_pLeaf)
	{
		pNode->right = pNewNode;
		pNewNode->parent = pNode;
	}
	else
	{
		++it;
		pNode = it.getNode();

		UT_ASSERT(it.is_valid());
		UT_ASSERT(pNode->left == m_pLeaf);

		pNode->left = pNewNode;
		pNewNode->parent = pNode;
	}

	_insertFixup(pNewNode);

	return Iterator(this, pNewNode);
}

/**
 * Insert a new piece to the left of a given node.
 * This operation has the exceptional guarantee strong.
 *
 * @param new_piece the piece to insert
 * @param it iterator to the reference node
 */
pt_PieceTree::Iterator
pt_PieceTree::insertLeft(pt_Piece* new_piece, Iterator& it)
{
	Node* pNode = it.getNode();

	UT_ASSERT(m_nSize == 0 || it.is_valid());
	
	Node* pNewNode = new Node(Node::red, new_piece, m_pLeaf, m_pLeaf, 0);

	new_piece->size_left = 0;
	
	++m_nSize;
	m_nDocumentSize += new_piece->size;

	if (!it.is_valid())
		m_pRoot = pNewNode;
	else if (pNode->left == m_pLeaf)
	{
		pNode->left = pNewNode;
		pNewNode->parent = pNode;
	}
	else
	{
		--it;
		pNode = it.getNode();

		UT_ASSERT(it.is_valid());
		UT_ASSERT(pNode->right == m_pLeaf);

		pNode->right = pNewNode;
		pNewNode->parent = pNode;
	}

	_insertFixup(pNewNode);

	return Iterator(this, pNewNode);
}

/**
 * Erase a piece from the tree.
 * This operation has the exceptional guarantee "no throw".
 *
 * @param it iterator to the node to erase.
 */
void
pt_PieceTree::erase(Iterator& it)
{
	if (!it.is_valid())
		return;

	Node* pNode = it.getNode();
	UT_ASSERT(pNode);

	--m_nSize;
	m_nDocumentSize -= pNode->item->size;
	
	Node* y = (pNode->left == m_pLeaf || pNode->right == m_pLeaf) ?
		pNode :
		_prev(pNode);

	UT_ASSERT(y->left == m_pLeaf || y->right == m_pLeaf);
	Node* son = y->left != m_pLeaf ? y->left : y->right;
	UT_ASSERT(son);
	son->parent = y->parent;

	if (!y->parent)
		m_pRoot = son;
	else
		if (y->parent->left == y)
			y->parent->left = son;
		else
			y->parent->right = son;

	if (y != pNode)
	{
		delete pNode->item;
		pNode->item = y->item;
		Iterator it(this, pNode);
		fixSize(it);
	}

	if (son->parent)
	{
		/* and fix the whole left branch of the tree */
		Iterator it(this, son);
		fixSize(it);
	}
		
	if (y->color == Node::black)
		_eraseFixup(son);

	delete y;
}

/**
 * If the size of a tree's node changed, we should update the size_left field
 * of all its right parents.  This function does that fix.  You should pass
 * an iterator to the node that changed its size.  It assumes that the sizes
 * of the tree are all right except for a change in the size of the node passed
 * as argument.
 *
 * @param it Iterator to the node that changed its size
 */
void
pt_PieceTree::fixSize(Iterator& it)
{
	UT_ASSERT(it.is_valid());
	Node* pn = it.getNode();
	pt_Piece* item;
	int delta = 0;

	if (pn == m_pRoot)
		return;

	/* if the two sons of the left son are the same one, then they are
	 * two leafs, and maybe we've gone one node too high when we're climbing
	 * the tree (because pn was == pn->parent->right, but the change was in
	 * pn->parent->left).  We can't thus trust the return value of _calculateSize,
	 * and we should do this calculation at hand (it's just pn->left->item->size) */
	if (pn->parent->left == pn->parent->right && pn->parent->item)
	{
		pn = pn->parent;
		delta = - (int) pn->item->size_left;
		pn->item->size_left = 0;
	}

	if (delta == 0)
	{
		/* go up to the head of the first left subtree that contains the node that changed. */
		while (pn != m_pRoot && pn == pn->parent->right)
			pn = pn->parent;
	
		if (pn == m_pRoot)
			return;

		pn = pn->parent;
	
		/* Fix the size_left of this head */
		delta = _calculateSize(pn->left) - pn->item->size_left;
		pn->item->size_left += delta;
	}

	/* if the size_left of that head changed, propagate the change to our parents */
	while (pn != m_pRoot && delta != 0)
	{
		item = pn->item;
	
		if (pn->parent->left == pn)
			pn->parent->item->size_left += delta;

		pn = pn->parent;
	}
}

void
pt_PieceTree::_eraseFixup(Node* x)
{
	while (x != m_pRoot && x->color == Node::black)
	{
		if (x == x->parent->left)
		{
			Node *tmp = x->parent->right;
			if (tmp->color == Node::red)
			{
				tmp->color = Node::black;
				x->parent->color = Node::red;
				_leftRotate(x->parent);
				tmp = x->parent->right;
			}
			if (tmp->left->color == Node::black && tmp->right->color == Node::black)
			{
				tmp->color = Node::red;
				x = x->parent;
			}
			else
			{
				if (tmp->right->color == Node::black)
				{
					tmp->left->color = Node::black;
					tmp->color = Node::red;
					_rightRotate(tmp);
					tmp = x->parent->right;
				}
				tmp->color = x->parent->color;
				x->parent->color = Node::black;
				tmp->right->color = Node::black;
				_leftRotate(x->parent);
				x = m_pRoot;
			}
		}
		else
		{
			Node* tmp = x->parent->left;
			if (tmp->color == Node::red)
			{
				tmp->color = Node::black;
				x->parent->color = Node::red;
				_rightRotate(x->parent);
				tmp = x->parent->left;
			}
			if (tmp->right->color == Node::black && tmp->left->color == Node::black)
			{
				tmp->color = Node::red;
				x = x->parent;
            }
			else
			{
				if (tmp->left->color == Node::black)
				{
					tmp->right->color = Node::black;
					tmp->color = Node::red;
					_leftRotate(tmp);
					tmp = x->parent->left;
				}
				tmp->color = x->parent->color;
				x->parent->color = Node::black;
				tmp->left->color = Node::black;
				_rightRotate(x->parent);
				x = m_pRoot;
			}
		}
	}
	x->color = Node::black;
}

pt_PieceTree::Iterator
pt_PieceTree::find(pd_DocPosition pos)
{
	Node* x = m_pRoot;

	while (x != m_pLeaf)
	{
		pt_Piece* p = x->item;
		
		if (p->size_left > pos)
			x = x->left;
		else if (p->size_left + p->size > pos)
			return Iterator(this, x);
		else
		{
			pos -= p->size_left + p->size;
			x = x->right;
		}
	}

	UT_ASSERT(pos >= sizeDocument());
	return Iterator(this, 0);
}

pd_DocPosition
pt_PieceTree::documentPosition(const Iterator& it) const
{
	UT_ASSERT(it.is_valid());

	const Node* pn = it.getNode();
	pd_DocPosition pos = pn->item->size_left;
	while (pn != m_pRoot)
	{
		if (pn->parent->right == pn)
			pos += pn->parent->item->size_left + pn->parent->item->size;

		pn = pn->parent;
	}

	return pos;
}

void
pt_PieceTree::changeSize(int delta)
{
	UT_ASSERT(delta < 0 ? ((int) m_nDocumentSize >= -delta) : true);

	m_nDocumentSize += delta;
}

const pt_PieceTree::Node*
pt_PieceTree::_prev(const Node* pn) const
{
	UT_ASSERT(pn != NULL);

	if (pn != m_pLeaf)
	{
		if (pn->left != m_pLeaf)
		{
			pn = pn->left;

			while(pn->right != m_pLeaf)
				pn = pn->right;
		}
		else
		{
			while(pn->parent)
			{
				if (pn->parent->right == pn)
					return pn->parent;
				else
					pn = pn->parent;
			}

			return 0;
		}
	}

	return pn;
}

pt_PieceTree::Node*
pt_PieceTree::_prev(Node* pn)
{
	return const_cast<Node*> (_prev(const_cast<const Node*> (pn)));
}

const pt_PieceTree::Node*
pt_PieceTree::_next(const Node* pn) const
{
	UT_ASSERT(pn != NULL);

	if (pn != m_pLeaf)
	{
		if (pn->right != m_pLeaf)
		{
			pn = pn->right;

			while(pn->left != m_pLeaf)
				pn = pn->left;
		}
		else
		{
			while(pn->parent)
			{
				if (pn->parent->left == pn)
					return pn->parent;
				else
					pn = pn->parent;
			}

			return 0;
		}
	}

	return pn;
}

pt_PieceTree::Node*
pt_PieceTree::_next(Node* pn)
{
	return const_cast<Node*> (_next(const_cast<const Node*> (pn)));
}

const pt_PieceTree::Node*
pt_PieceTree::_first() const
{
	Node* pn = m_pRoot;
	
	if (pn == m_pLeaf)
		return 0;

	while(pn->left != m_pLeaf)
		pn = pn->left;

	return pn;
}

pt_PieceTree::Node*
pt_PieceTree::_first()
{
	Node* pn = m_pRoot;
	
	if (pn == m_pLeaf)
		return 0;

	while(pn->left != m_pLeaf)
		pn = pn->left;

	return pn;
}

const pt_PieceTree::Node*
pt_PieceTree::_last() const
{
	Node* pn = m_pRoot;
	
	if (pn == m_pLeaf)
		return 0;

	while(pn->right != m_pLeaf)
		pn = pn->right;

	return pn;
}

pt_PieceTree::Node*
pt_PieceTree::_last()
{
	Node* pn = m_pRoot;
	
	if (pn == m_pLeaf)
		return 0;

	while(pn->right != m_pLeaf)
		pn = pn->right;

	return pn;
}

/**
 * This method rotates a tree to the left.
 *
 * Let be 2 & 4 nodes of a tree.  Let be 1, 3, 5 subtrees.
 * This operation performs the rotation that appears in the
 * this ASCII picture:
 *
 *        2                4
 *       / \              / \
 *      1   4     =>     2   5
 *         / \          / \
 *        3   5        1   3
 *
 * @params x is the head of the tree (node 2 in the picture).
 */
void
pt_PieceTree::_leftRotate(Node* x)
{
 	Node* y;
	y = x->right;

	/* correct the sizes */
	y->item->size_left += x->item->size + x->item->size_left;
	
    /* Turn y's left sub-tree into x's right sub-tree */
    x->right = y->left;

    if (y->left != m_pLeaf)
        y->left->parent = x;

    /* y's new parent was x's parent */
    y->parent = x->parent;
    /* Set the parent to point to y instead of x */
    /* First see whether we're at the root */
    if (!x->parent)
		m_pRoot = y;
    else
        if (x == x->parent->left)
            /* x was on the left of its parent */
            x->parent->left = y;
        else
            /* x must have been on the right */
            x->parent->right = y;
    /* Finally, put x on y's left */
    y->left = x;
    x->parent = y;
}

/**
 * This method rotates a tree to the left.
 *
 * Let be 2 & 4 nodes of a tree.  Let be 1, 3, 5 subtrees.
 * This operation performs the rotation that appears in the
 * this ASCII picture:
 *
 *        4                2
 *       / \              / \
 *      2   5     =>     1   4
 *     / \                  / \
 *    1   3                3   5
 *
 * @params x is the head of the tree (node 4 in the picture).
 */
void
pt_PieceTree::_rightRotate(Node* x)
{
	Node* y;
	y = x->left;

	/* correct the sizes */
	x->item->size_left -= y->item->size + y->item->size_left;
	
	x->left = y->right;

	if (y->right != m_pLeaf)
		y->right->parent = x;

	y->parent = x->parent;

	if (!x->parent)
		m_pRoot = y;
	else
		if (x == x->parent->right)
			x->parent->right = y;
		else
			x->parent->left = y;

	y->right = x;
	x->parent = y;
}

/**
 * This method calculates the cumulated size of all the
 * nodes that are in the subtree that has "x" as head.
 *
 * This operation is performed in O(log(n)), where n is
 * the number of nodes in the subtree.
 *
 * @params x is the head of the subtree
 */
pd_DocPosition
pt_PieceTree::_calculateSize(Node* x)
{
	UT_ASSERT(x != NULL);

	if (x == m_pLeaf)
		return 0;
	
	return x->item->size_left + x->item->size + _calculateSize(x->right);
}

void
pt_PieceTree::delete_tree(Node* node)
{
	if (node->left != m_pLeaf)
		delete_tree(node->left);

	if (node->right != m_pLeaf)
		delete_tree(node->right);

	delete node->item;
	delete node;
}

#ifdef DEBUG
void
pt_PieceTree::print() const
{
	if (m_pRoot != m_pLeaf)
		m_pRoot->print();
	else
		printf("empty set\n");
}

// Iterator should be a ConstIterator
int
pt_PieceTree::_countBlackNodes(const Iterator& it) const
{
	int retval = 0;
	const Node* pn = it.getNode();
	UT_ASSERT(it.is_valid());

	do
	{
		if (pn->color == Node::black)
			++retval;
		else  // we're red
			if (pn->parent && pn->parent->color == Node::red) // and also our father!!
			{
				printf("ERROR: We've found a red node that has a red father.\n");
				return -1;
			}

		pn = pn->parent;
	}
	while (pn != 0);

	return retval;
}

bool
pt_PieceTree::checkInvariants() const
{
	int nb_blacks = 0;

	// These iterators should be ConstIterators
	// ConstIterator end(end());
	// ConstIterator it(begin());
	Iterator end(const_cast<pt_PieceTree* const> (this));
	Iterator it(const_cast<pt_PieceTree* const> (this), const_cast<Node*> (_first()));

	if (it != end)
		nb_blacks = _countBlackNodes(it++);

	if (nb_blacks < 0)
		return false;

	if (!checkSizeInvariant(m_pRoot, NULL))
		return false;
	
	for (; it != end; ++it)
	{
		Node *pn = it.getNode();
		
		if (pn->left == m_pLeaf && pn->right == m_pLeaf &&
			nb_blacks != _countBlackNodes(it))
		{
			printf("ERROR: We've found two paths that pass through a different number of black nodes. "
				   "The first path pass through %d nodes and the second one pass through %d nodes.\n", nb_blacks, _countBlackNodes(it));
			return false;
		}
	}

	return true;
}

bool
pt_PieceTree::checkSizeInvariant(const Node* pn, pd_DocPosition* size) const
{
	pd_DocPosition size_left, size_right;
	
	if (pn == m_pLeaf)
	{
		if (size)
			*size = 0;

		return true;
	}

	if (!checkSizeInvariant(pn->left, &size_left))
		return false;
	
	if (!checkSizeInvariant(pn->right, &size_right))
		return false;

	if (pn->item->size_left != size_left)
	{
		printf("A node reported as size_left [%u], but the real size of its left subtree is [%u].\n", pn->item->size_left, size_left);
		return false;
	}

	if (size)
		*size = pn->item->size + size_left + size_right;

	return true;
}

#endif

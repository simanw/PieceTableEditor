#include <assert.h>
#include <stdexcept>

#include "pt_PieceTree.hpp"
// #include "pt_PieceList.hpp"
// #define pt_PieceTree pt_PieceList

#include "pt_PieceTable.hpp"
#include "pt_Piece.hpp"

#define UT_ASSERT(cond) assert(cond)

pt_PieceTable::pt_PieceTable(const byte_t* initial_buffer /*, const pt_AttrProp* initial_attr_prop */)
	: m_InitialBuffer(initial_buffer) /*,
	  m_InitialAttrProp(initial_attr_prop) */
{
}

pt_PieceTable::~pt_PieceTable()
{
}

void
pt_PieceTable::insertCharacter(pd_DocPosition pos, char ch) // throws std::bad_alloc, std::out_of_range
{
	pd_DocPosition size_document = m_PieceTree.sizeDocument();

	if (size_document != 0)
	{
		if (pos > size_document)
			throw std::out_of_range("The position of the character to insert exceed the size of the document.");

		// Search the node that contains the position where we want to insert
		pt_PieceTree::Iterator it = m_PieceTree.find(pos == size_document ? pos - 1 : pos);
		pd_DocPosition doc_pos = m_PieceTree.documentPosition(it);
		if (!it.is_valid())
			throw std::logic_error("While looking for a valid position, pt_PieceTree was not able to find any piece.");

		pt_Piece* pPiece = it.value();

		// Insert the text into the modifiable buffer
		m_ChangeBuffer.push_back(ch);
			
		if (pPiece->buffer == CHANGE && pPiece->offset + pPiece->size == m_ChangeBuffer.size() - 1 &&
			doc_pos + pPiece->size == pos)
		{
			++pPiece->size;
			m_PieceTree.changeSize(1);
			m_PieceTree.fixSize(it);
		}
		else
		{
			pt_Piece* pNewPiece = new pt_Piece;

			pNewPiece->size = 1;
			pNewPiece->buffer = CHANGE;
			pNewPiece->offset = m_ChangeBuffer.size() - 1;
			pNewPiece->size_left = 0;

			/* TODO: put the pNewPiece on a auto_ptr, or the function will have a sub-basic exception guarantee */

			if (doc_pos == pos)
			{
				/* first case: */
				m_PieceTree.insertLeft(pNewPiece, it);
			}
			else if (pos < doc_pos + pPiece->size)
			{
				/* second case: */
				pt_Piece* pRightPiece = new pt_Piece;

				pRightPiece->size = doc_pos + pPiece->size - pos;
				pRightPiece->buffer = pPiece->buffer;
				pRightPiece->offset = pPiece->offset + pos - doc_pos;
				pRightPiece->size_left = 0;

				pPiece->size -= pRightPiece->size;
				m_PieceTree.changeSize(-pRightPiece->size);

				m_PieceTree.insertRight(pRightPiece, it);
				m_PieceTree.insertRight(pNewPiece, it);
			}
			else if (doc_pos + pPiece->size == pos)
			{
				/* third case: */
				m_PieceTree.insertRight(pNewPiece, it);
			}
			else
			{
				delete pNewPiece;
				throw std::logic_error("The position where we want "
					"to insert is outside the positions covered by the "
					"piece that we extracted from the piece table.");
			}
		}
	}
	else
	{
		// That's only possible if we have a void piece table
		if (pos != 0)
			throw std::logic_error("The piece table is empty, but we were asked to insert "
				"a character at a position != 0.");
			
		// Insert the text into the modifiable buffer
		m_ChangeBuffer.push_back(ch);
			
		pt_Piece* pNewPiece = new pt_Piece;

		pNewPiece->size = 1;
		pNewPiece->buffer = CHANGE;
		pNewPiece->offset = 0;
		pNewPiece->size_left = 0;

		m_PieceTree.insertRoot(pNewPiece);
	}
}

void
pt_PieceTable::deleteCharacter(pd_DocPosition pos) // throws std::bad_alloc
{
	pd_DocPosition end = m_PieceTree.sizeDocument();

	// We should not try to erase a character that doesn't exist
	if (pos >= end)
		throw std::out_of_range("The position of the character to delete exceed the size of the document.");

	// Search the node that contains the position where we want to delete
	pt_PieceTree::Iterator itNode = m_PieceTree.find(pos);
	pd_DocPosition doc_pos = m_PieceTree.documentPosition(itNode);
	UT_ASSERT(itNode.is_valid());
	pt_Piece* pPiece = itNode.value();

	// We have 3 posibilities.  The character that we want to erase can be
	// 1) at the beginning of the piece
	// 2) in the middle of the piece
	// 3) at the end of the piece
	if (doc_pos == pos)
	{
		// case 1
		++pPiece->offset;
		--pPiece->size;
		m_PieceTree.changeSize(-1);

		// Now, we have to update the size_left or right of our parents
		if (pPiece->size == 0)
			m_PieceTree.erase(itNode);
		else
			m_PieceTree.fixSize(itNode);

	}
	else if (doc_pos + pPiece->size == pos + 1)
	{
		// case 3
		--pPiece->size;
		m_PieceTree.changeSize(-1);

		// Now, we have to update the size_left or right of our parents
		if (pPiece->size == 0)
			m_PieceTree.erase(itNode);
		else
			m_PieceTree.fixSize(itNode);
	}
	else
	{
		pt_Piece* pNewPiece = new pt_Piece;

		// case 2
		pd_DocPosition old_size = pPiece->size;
		pd_DocPosition pos_deleted_char = pos - doc_pos;

		pPiece->size = pos_deleted_char;
		m_PieceTree.changeSize(pPiece->size - old_size);
		m_PieceTree.fixSize(itNode);

		pNewPiece->size = old_size - pPiece->size - 1;
		pNewPiece->buffer = pPiece->buffer;
		pNewPiece->offset = pPiece->offset + pPiece->size + 1;
		pNewPiece->size_left = 0;

		m_PieceTree.insertRight(pNewPiece, itNode);
	}
}

char pt_PieceTable::lookupCharacter(pd_DocPosition pos, pt_Piece* hint) const
{
	pd_DocPosition end = m_PieceTree.sizeDocument();

	// We should not try to lookup a character that doesn't exist
	UT_ASSERT(pos < end);

	if (hint == NULL)
	{
		// Search the node that contains the character we want
		// We don't yet have a const iterator, thus I should const_cast the piece tree
		pt_PieceTree& pt = const_cast<pt_PieceTree&> (m_PieceTree);
		pt_PieceTree::Iterator it = pt.find(pos);
		UT_ASSERT(it.is_valid());
		pt_Piece* pPiece = it.value();

		if (pPiece->buffer == INITIAL)
			return m_InitialBuffer[pPiece->offset + pos - pt.documentPosition(it)];
		else
			return m_ChangeBuffer[pPiece->offset + pos - pt.documentPosition(it)];
	}
	else
	{
		// TODO
		fprintf(stderr, "Not yet implemented.");
		return 0;
	}

	return 0;
}

pd_DocPosition
pt_PieceTable::size() const
{
	return m_PieceTree.sizeDocument();
}

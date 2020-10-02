#ifndef PT_PIECETABLE_HPP
#define PT_PIECETABLE_HPP

#include <deque>
#include "pt_PieceTree.hpp"

// #include "pt_PieceList.hpp"
// #define pt_PieceTree pt_PieceList

typedef unsigned char byte_t;
typedef unsigned int pd_DocPosition;
struct pt_AttrProp;
class pt_PieceTree;

class pt_PieceTable
{
public:
	pt_PieceTable(const byte_t* initial_buffer = NULL /*, const pt_AttrProp* initial_attr_prop = NULL */);
	~pt_PieceTable();
	
	/* This function has the exceptional safety guarantee strong.
	 * (FIXME: Right now it don't even meets the basic guarantee) */
	void insertCharacter(pd_DocPosition pos, char ch); // throws std::bad_alloc, std::logic_error

	/* This function has the exceptional safety guarantee strong.
	 * (FIXME: Right now it don't even meets the basic guarantee)
	 * It may seem surprising that the "delete" operation is strong
	 * instead of no throw, but you should take in account that a
	 * delete operation on a piece table can cause a split of a piece,
	 * and thus we may need to add a new node to the tree. */
	void deleteCharacter(pd_DocPosition pos); // throws std::bad_alloc, std::logic_error

	/* This function has the exceptional safety guarantee "no throw" */
	char lookupCharacter(pd_DocPosition pos, pt_Piece* hint = NULL) const; // throws ()

	/* This function has the exceptional safety guarantee "no throw" */
	pd_DocPosition size() const; // throws ()

#ifdef DEBUG
	bool checkInvariants() const { return m_PieceTree.checkInvariants(); } // throws ()
	void print() const { m_PieceTree.print(); } // throws ()
	size_t sizeInternalTree() const { return m_PieceTree.size(); } // throws ()
#endif

private:
	enum Table
	{
		INITIAL,
		CHANGE
	};

	const byte_t* m_InitialBuffer;

	/* const pt_AttrProp* m_InitialAttrProp */;

	std::deque<byte_t> m_ChangeBuffer;
	/* std::deque<pt_AttrProp> m_ChangeAttrProp */;

	pt_PieceTree m_PieceTree;

	pt_PieceTable& operator= (const pt_PieceTable&);
	pt_PieceTable(const pt_PieceTable&);
};

#endif // PT_PIECETABLE_HPP

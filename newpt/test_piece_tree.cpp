#include <stdlib.h>
#include <stdexcept>
#include <list>
#include "pt_PieceTree.hpp"
#include "pt_PieceTable.hpp"
#include "pt_Piece.hpp"

#define for if (0) ; else for

pt_Piece*
new_piece()
{
	pd_DocPosition size = static_cast<pd_DocPosition> (rand() * 1.0 / RAND_MAX * 15 + 1);
	pt_Piece* p = new pt_Piece;
	p->size = size;
	p->size_left = 0;
	
	return p;
}

enum Direction { LEFT, RIGHT };

static Direction
new_direction()
{
	if (rand() > RAND_MAX / 2)
		return RIGHT;
	else
		return LEFT;
}

bool
test_insertion()
{
	pt_PieceTree pt;
	pt_PieceTree::Iterator it;

	it = pt.insertRoot(new_piece());
	// printf("STAGE 0 ===================\n");
	// printf("Insert root (%d)\n", it.value()->size);
	// pt.print();
	// printf("===========================\n\n");
	
	for (int i = 0; i < 1000; ++i)
	{
		pt_Piece* p = new_piece();

		// printf("STAGE %i ===================\n", i + 1);

		if (new_direction() == LEFT)
		{
			// printf("Insert left (%d)\n", p->size);
			it = pt.insertLeft(p, it);
		}
		else
		{
			// printf("Insert right (%d)\n", p->size);
			it = pt.insertRight(p, it);
		}

		// pt.print();
		// printf("===========================\n\n");

		if (!pt.checkInvariants())
		{
			pt.print();
			return false;
		}
	}

	return true;
}

bool
test_delete()
{
	const size_t nb_insertions = 1000;
	pt_PieceTree pt;
	pt_PieceTree::Iterator it;

	it = pt.insertRoot(new_piece());
	for (size_t i = 0; i < nb_insertions; ++i)
	{
		pt_Piece* p = new_piece();
		
		if (new_direction() == LEFT)
			it = pt.insertLeft(p, it);
		else
			it = pt.insertRight(p, it);
	}

	if (!pt.checkInvariants())
	{
		pt.print();
		return false;
	}

	// pt.print();
		
	for (size_t i = 0; i < nb_insertions; ++i)
	{
		size_t node_nb = static_cast<size_t> (rand() * 1.0 / RAND_MAX * pt.size());
		it = pt.begin();

		for (size_t j = 0; j < node_nb; ++j)
			++it;

		// printf("erase node %d (size %d).\n", node_nb, it.value()->size);

		pt.erase(it);

		if (!pt.checkInvariants())
		{
			printf("Test delete failed with i = %d\n", i);
			pt.print();
			return false;
		}
	}

	return true;
}

bool
checkPieceTable(const pt_PieceTable& pt, size_t nb_pieces, const char *st)
{
	if (!pt.checkInvariants())
	{
		printf("Error: the piece table don't meets its invariants.\n");
		pt.print();
		return false;
	}

	size_t s;
	if ((s = pt.sizeInternalTree()) != nb_pieces)
	{
		printf("The piece table has %d nodes, but it should have %d.\n", s, nb_pieces);
		pt.print();
		return false;
	}

	const size_t length_st = strlen(st);
	if ((s = pt.size()) != length_st)
	{
		printf("The piece table has a size of %d when it should be %d.\n", s, length_st);
		pt.print();
		return false;
	}

	for (size_t i = 0; i < length_st; ++i)
	{
		char ch;

		if ((ch = pt.lookupCharacter(i)) != st[i])
		{
			char wanted_char = st[i];
			char* prev = new char[i + 1];

			strncpy(prev, st, i);
			prev[i] = 0;

			printf("The piece table should contain the text \"%s%c\", but it contains instead \"%s%c\".\n",
				   prev, wanted_char, prev, ch);

			delete[] prev;
			pt.print();
			return false;
		}
	}

	return true;
}

bool
test_void_piece_table()
{
	pt_PieceTable pt;
	return checkPieceTable(pt, 0, "");
}

bool
test_insertion_ptable1()
{
	pt_PieceTable pt;

	try
	{
		pt.insertCharacter(0, 'H');
	}
	catch (const std::logic_error& e)
	{
		printf("%s\n", e.what());
		return false;
	}

	return checkPieceTable(pt, 1, "H");
}

bool
test_insertion_ptable2()
{
	pt_PieceTable pt;

	try
	{
		pt.insertCharacter(0, 'H');
		pt.insertCharacter(1, 'i');
	}
	catch (const std::logic_error& e)
	{
		printf("%s\n", e.what());
		return false;
	}

	return checkPieceTable(pt, 1, "Hi");
}

bool
test_insertion_ptable3()
{
	pt_PieceTable pt;

	try
	{
		pt.insertCharacter(0, 'i');
		pt.insertCharacter(0, 'H');
	}
	catch (const std::logic_error& e)
	{
		printf("%s\n", e.what());
		return false;
	}

	return checkPieceTable(pt, 2, "Hi");
}

bool
test_insertion_ptable4()
{
	pt_PieceTable pt;

	try
	{
		pt.insertCharacter(0, 'H');
		pt.insertCharacter(1, '!');
		pt.insertCharacter(1, 'i');
	}
	catch (const std::logic_error& e)
	{
		printf("%s\n", e.what());
		return false;
	}

	return checkPieceTable(pt, 3, "Hi!");
}

bool
test_insertion_ptable5()
{
	pt_PieceTable pt;

	try
	{
		pt.insertCharacter(0, 'H');
		pt.insertCharacter(1, 'y');
		pt.insertCharacter(2, '!');
		pt.insertCharacter(1, 'e');
	}
	catch (const std::logic_error& e)
	{
		printf("%s\n", e.what());
		return false;
	}

	return checkPieceTable(pt, 3, "Hey!");
}

bool
test_delete_ptable()
{
	pt_PieceTable pt;

	pt.insertCharacter(0, 'H');

	try
	{
		pt.deleteCharacter(0);
	}
	catch (const std::logic_error& e)
	{
		printf("%s\n", e.what());
		return false;
	}

	return checkPieceTable(pt, 0, "");
}

bool
test_delete_ptable1()
{
	pt_PieceTable pt;

	pt.insertCharacter(0, 'H');
	pt.insertCharacter(1, 'i');

	try
	{
		pt.deleteCharacter(0);
	}
	catch (const std::logic_error& e)
	{
		printf("%s\n", e.what());
		return false;
	}

	return checkPieceTable(pt, 1, "i");
}

bool
test_delete_ptable2()
{
	pt_PieceTable pt;

	pt.insertCharacter(0, 'H');
	pt.insertCharacter(1, 'i');

	pt.deleteCharacter(1);
	return checkPieceTable(pt, 1, "H");
}

bool
test_delete_ptable3()
{
	pt_PieceTable pt;

	pt.insertCharacter(0, 'H');
	pt.insertCharacter(1, 'i');
	pt.insertCharacter(1, '!');

	pt.deleteCharacter(1);
	return checkPieceTable(pt, 2, "Hi");
}

bool
test_insertion_ptable_random1()
{
	pt_PieceTable pt;
	unsigned int i, j, r;
	char ch;
	char buffer[1000];

	/* insert random characters at random locations, until we fill the piece table
	 * with 1000 characters.  Also, insert the same characters on a buffer at the
	 * same positions. */
	for (i = 0; i < 1000; ++i)
	{
		r = (unsigned int) ((rand() * 1.0 / RAND_MAX) * i);
		ch = (char) ((rand() * 1.0 / RAND_MAX) * ('z' - 'a')) + 'a';

		pt.insertCharacter(r, ch);

		for (j = i; j != r; --j)
			buffer[j] = buffer[j - 1];
		
		buffer[r] = ch;

		if (!pt.checkInvariants())
		{
			printf("Error: the piece table doesn't meets its invariants.\n");
			pt.print();
			return false;
		}
	}

	/* Check that the characters in the piece table are the same that the characters
	 * in the buffer */
	for (i = 0; i < 1000; ++i)
	{
		char ch;

		if ((ch = pt.lookupCharacter(i)) != buffer[i])
		{
			printf("Error: The piece table returned '%c' at the position '%d', when '%c' was expected.\n", ch, i, buffer[i]);
			return false;
		}
	}

	return true;
}

bool
test_delete_ptable_random1()
{
	const size_t nb_deletes = 1000;
	pt_PieceTable pt;
	unsigned int i, j;

	for (i = 0; i < nb_deletes; ++i)
		pt.insertCharacter(i, 'a' + i % ('z' - 'a'));

	if (!pt.checkInvariants())
	{
		printf("Error: the piece table doesn't meets its invariants.\n");
		pt.print();
		return false;
	}

	for (i = nb_deletes - 1; i != 0; --i)
	{
		j = (unsigned int) ((rand() * 1.0 / RAND_MAX) * i);
		pt.deleteCharacter(j);

		if (!pt.checkInvariants())
		{
			printf("Error: the piece table doesn't meets its invariants.\n");
			pt.print();
			printf("%d - %d.\n", i, j);
			return false;
		}
	}

	return true;
}

typedef bool (*TestFn) ();

void
test(TestFn fn, const char* desc)
{
	if (fn())
		printf("%s test passed OK.\n", desc);
	else
		printf("%s test FAILED.\n", desc);
}

#if 1
int
main()
{
	test(test_insertion, "Insertion");
	test(test_delete, "Delete");
	test(test_void_piece_table, "Creation of a piece table");
	test(test_insertion_ptable1, "Insertion on a void piece table");
	test(test_insertion_ptable2, "Insertion on the right of a piece");
	test(test_insertion_ptable3, "Insertion on the left of a piece");
	test(test_insertion_ptable4, "Insertion on the middle of a piece with 2 chars");
	test(test_insertion_ptable5, "Insertion on the middle of a piece");
	test(test_insertion_ptable_random1, "Insertion randomly of a piece");
	test(test_delete_ptable, "Delete the only character on the piece table");
	test(test_delete_ptable1, "Delete the first character on the piece table");
	test(test_delete_ptable2, "Delete the last character on the piece table");
	test(test_delete_ptable3, "Delete on the piece table");
	test(test_delete_ptable_random1, "Delete randomly");

	return 0;
}
#endif

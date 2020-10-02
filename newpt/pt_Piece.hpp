#ifndef PT_PIECE_HPP
#define PT_PIECE_HPP

struct pt_Piece
{
	char buffer; // MSVC always uses 4 bytes for an enum, so we should do it with a char...
	pd_DocPosition offset;
	pd_DocPosition size;
	pd_DocPosition size_left;
};

#endif // PT_PIECE_HPP

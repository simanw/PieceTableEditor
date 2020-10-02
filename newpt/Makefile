$CXXFLAGS="-g -O0"

regression: pt_PieceTable.o pt_PieceTree.o test_piece_tree.o
	$(CXX) $(CXXFLAGS) -o regression pt_PieceTable.o pt_PieceTree.o test_piece_tree.o

profile: pt_PieceTable.o pt_PieceTree.o profile_piece_tree.o
	$(CXX) $(CXXFLAGS) -o profile pt_PieceTable.o pt_PieceTree.o profile_piece_tree.o

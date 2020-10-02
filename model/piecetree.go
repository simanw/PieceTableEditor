package model

import (
	"errors"
	"fmt"
)

type PieceDescriptor_Tree struct {
	Buffer    bool
	Offset    int
	Size      int
	Size_left int // Size of left subtree is sufficient
}

type Node struct {
	Left   *Node
	Right  *Node
	Parent *Node
	Piece  *PieceDescriptor_Tree
}

type PieceTree struct {
	Root         *Node
	NumOfNode    int
	LengthOfFile int
}

type PieceTable_Tree struct {
	Tree       *PieceTree
	FileBuffer []byte
	AddBuffer  []byte
}

// Error
//var outOfBoundsError = errors.New("Index out of bounds")
var offsetError = errors.New("Invalide piece")

// func NewPiece(buffer bool, size int) *PieceDescriptor_Tree {
// 	piece := PieceDescriptor_Tree{}
// 	piece.Buffer = buffer
// 	piece.Size = Size
// 	piece.Offset = Offset

// }

func NewNode(piece *PieceDescriptor_Tree) *Node {
	node := Node{}
	node.Piece = piece
	node.Left = nil
	node.Right = nil
	node.Parent = nil
	return &node
}

func NewPieceTree(initialLength int) *PieceTree {
	tree := PieceTree{}
	initialPiece := &PieceDescriptor_Tree{FILE_BUFFER, 0, initialLength, 0}
	tree.Root = NewNode(initialPiece)
	tree.Root.Left = nil
	tree.Root.Right = nil
	tree.Root.Parent = nil
	tree.NumOfNode = 1
	tree.LengthOfFile = initialLength
	return &tree
}

func NewPieceTable(file string) PieceTable_Tree {
	table := PieceTable_Tree{}
	table.FileBuffer = []byte(file)
	table.AddBuffer = []byte{}
	table.Tree = NewPieceTree(len(file))

	return table
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
 * Correct the size of node 4
 * @params x is the head of the tree (node 2 in the picture).
 */
func (t *PieceTree) leftRotation(x *Node) {
	y := x.Right
	// Uplift the right subtree if x
	if y != nil {
		x.Right = y.Left
		if y.Left != nil {
			y.Left.Parent = x
		}
		y.Parent = x.Parent
		// Correct the size of y
		y.Piece.Size_left += x.Piece.Size_left + x.Piece.Size
	}
	// Update the parent of x
	if x.Parent == nil {
		t.Root = y
	} else if x == x.Parent.Left {
		x.Parent.Left = y
	} else {
		x.Parent.Right = y
	}
	// Don't foget x itself!
	if y != nil {
		y.Left = x
	}
	x.Parent = y
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
 * Correct the size of node 2
 * @params x is the head of the tree (node 4 in the picture).
 */
func (t *PieceTree) rightRotation(x *Node) {
	y := x.Left
	if y != nil {
		x.Left = y.Right
		if y.Right != nil {
			y.Right.Parent = x
		}
		y.Parent = x.Parent
		// Correct the size of x
		x.Piece.Size_left = t.calculateSize(x.Left)
	}

	if x.Parent == nil {
		t.Root = y
	} else if x.Parent.Left == x {
		x.Parent.Left = y
	} else {
		x.Parent.Right = y
	}
	if y != nil {
		y.Right = x
	}
	x.Parent = y
}

// func (t *PieceTree) splay(x *Node) {
// 	for x.Parent != nil {
// 		if x.Parent.Parent != nil {
// 			if x.Parent.Left == x {
// 				t.rightRotation(x.Parent)
// 			} else {
// 				t.leftRotation(x.Parent)
// 			}
// 		} else if x.Parent.Left == x && x.Parent.Parent.Left == x.Parent {
// 			t.rightRotation(x.Parent.Parent)
// 			t.rightRotation(x.Parent)
// 		} else if x.Parent.Right == x && x.Parent.Parent.Right == x.Parent {
// 			t.leftRotation(x.Parent.Parent)
// 			t.leftRotation(x.Parent.Parent)
// 		} else if x.Parent.Left == x && x.Parent.Parent.Right == x.Parent {
// 			t.rightRotation(x.Parent)
// 			t.leftRotation(x.Parent)
// 		} else {
// 			t.leftRotation(x.Parent)
// 			t.rightRotation(x.Parent)
// 		}
// 	}
// }
func (t *PieceTree) splay(n *Node) {

	t.fixSize(n)
	for n.Parent != nil {
		p := n.Parent
		gp := p.Parent
		if gp == nil { // zig step: parent is the root
			if n == p.Left {
				t.rightRotation(p)
			} else {
				t.leftRotation(p)
			}
		} else if (n == p.Left && p == gp.Left) || (n == p.Right && p == gp.Right) { // zig-zig step: p is not the root and x and p are either both right children or are both left children
			if n == p.Left {
				t.rightRotation(gp)
				t.rightRotation(n.Parent)
			} else {
				t.leftRotation(gp)
				t.leftRotation(n.Parent)
			}
		} else { // zig-zag step
			for i := 0; i < 2; i++ {
				p := n.Parent
				if n == p.Left {
					t.rightRotation(p)
				} else {
					t.leftRotation(p)
				}
			}
		}
	}
}

func (t *PieceTree) replace(u, v *Node) {
	if u.Parent == nil {
		t.Root = v
	} else if u == u.Parent.Left {
		u.Parent.Left = v
	} else {
		u.Parent.Right = v
	}
	if v != nil {
		v.Parent = u.Parent
	}
}

// Get the leftmost node in the subtree rooted at x
func subtree_minimum(x *Node) *Node {

	for x.Left != nil {
		x = x.Left
	}
	return x
}

// Get the rightmost node in the subtree of rooted at x
func subtree_maximum(x *Node) *Node {

	for x.Right != nil {
		x = x.Right
	}
	return x
}

/*
* Get the next piece
* Amortized time O(1)
 */
func (p *PieceTree) next(x *Node) *Node {

	if x.Right != nil {
		x = x.Right
		for x.Left != nil {
			x = x.Left
		}
		return x
	} else {
		for x.Parent != nil {
			if x.Parent.Left == x {
				return x.Parent
			} else {
				x = x.Parent
			}
		}
	}

	return nil
}

/*
* Get the previous piece
 */
func (t *PieceTree) prev(x *Node) *Node {

	if x != nil {
		// case 1: the rightmost node of its left subtree
		if x.Left != nil {
			x = x.Left
			for x.Right != nil {
				x = x.Right
			}
			return x
		} else {
			for x.Parent != nil {
				if x.Parent.Right == x {
					return x.Parent
				}
				x = x.Parent
			}
		}
	}
	return nil
}

/**
 * Insert a new piece to the left of a given node.
 * This operation has the exceptional guarantee strong.
 *
 * @param newPiece: the node containing a new piece to insert
 * @param x: the given node
 */
func (t *PieceTree) insertLeft(newPiece *PieceDescriptor_Tree, x *Node) {

	newNode := NewNode(newPiece)

	if x.Left == nil {
		x.Left = newNode
		newNode.Parent = x
	} else {
		prevNode := t.prev(x)
		if prevNode != nil {
			prevNode.Right = newNode
			newNode.Parent = prevNode
		} else {
			fmt.Println("Error: fail to find the prev")
		}
	}
	//t.fixSize(newNode.Parent)
	t.splay(newNode)
}

func (t *PieceTree) insertRight(newPiece *PieceDescriptor_Tree, x *Node) {

	newNode := NewNode(newPiece)

	if x.Right == nil {
		x.Right = newNode
		newNode.Parent = x
	} else {
		nextNode := t.next(x)
		if nextNode != nil {
			nextNode.Left = newNode
			newNode.Parent = nextNode
		} else {
			fmt.Println("Error: fail to find the next")
		}
	}
	//t.fixSize(newNode.Parent)
	t.splay(newNode)
}

func (t *PieceTree) deleteNode() {

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
func (t *PieceTree) calculateSize(x *Node) int {

	if x == nil {
		return 0
	}

	return x.Piece.Size_left + x.Piece.Size + t.calculateSize(x.Right)
}

/**
 * If the size of a tree's node changed, we should update the size_left field
 * of all its right parents.  This function does that fix. You should pass
 * the node that changed its size.  It assumes that the sizes
 * of the tree are all right except for a change in the size of the node passed
 * as argument.
 *
 * @param x: the node that changed its size
 */
func (t *PieceTree) fixSize(x *Node) {

	var delta = 0

	if x.Parent == nil || x == nil {
		return
	}

	/* if the two sons of the left son are the same one, then they are
	 * two leafs, and maybe we've gone one node too high when we're climbing
	 * the tree (because pn was == pn->parent->right, but the change was in
	 * pn->parent->left).  We can't thus trust the return value of _calculateSize,
	 * and we should do this calculation at hand (it's just pn->left->item->size)
	 */
	// if x.Parent.Left == x.Parent.Right {
	// 	x = x.Parent
	// 	delta = -x.Piece.Size_left
	// 	x.Piece.Size_left = 0
	// }

	if delta == 0 {
		/* go up to the head of the first left subtree that contains the node that changed. */
		for x.Parent != nil && x == x.Parent.Right {
			x = x.Parent
		}

		if x.Parent == nil {
			return
		}

		x = x.Parent

		/* Fix the size_left of this head */
		delta = t.calculateSize(x.Left) - x.Piece.Size_left
		x.Piece.Size_left += delta
	}

	/* if the size_left of that head changed, propagate the change to our parents */
	for x.Parent != nil && delta != 0 {
		if x == x.Parent.Left {
			x.Parent.Piece.Size_left += delta
		}
		x = x.Parent
	}
}

/*
* Search the node that contains the position where we want to insert
* @param: the logical index
 */
func (t *PieceTree) find(index int) *Node {

	x := t.Root
	for x != nil {
		if index < x.Piece.Size_left {
			x = x.Left
		} else if index <= x.Piece.Size_left+x.Piece.Size {
			//fmt.Println("Successfully find the affected node:", x.Piece)
			return x
		} else {
			index -= x.Piece.Size + x.Piece.Size_left
			x = x.Right
		}
	}
	// If it reaches this, the tree is empty
	return nil
}

/*
* Get the logical offset of a piece
* @param x: the node containing the piece
 */
func (t *PieceTree) node2LogicalOffset(x *Node) (int, error) {

	if x == nil {
		//fmt.Println("************* node2logicaloffset(): The input node is nil!! **************")
		return -1, offsetError
	}
	offset := x.Piece.Size_left
	for x.Parent != nil {
		if x.Parent.Right == x {
			offset += x.Parent.Piece.Size + x.Parent.Piece.Size_left
		}
		x = x.Parent
	}
	//fmt.Println("Successfully transfer node to logical offset:", offset)
	return offset, nil
}

func (t *PieceTable_Tree) Insert(str []byte, index int) {

	// Search the node that contains the position where we want to insert
	affectedNode := t.Tree.find(index)
	logicalOffset, err := t.Tree.node2LogicalOffset(affectedNode)
	if err != nil {
		fmt.Println("Error: in finding logical offset of a given node, current file length: ", t.Tree.LengthOfFile)
	}

	// If the piece points to the end of the add buffer, and we are inserting at its end, simply increase its length
	if affectedNode.Piece.Buffer == ADD_BUFFER && affectedNode.Piece.Offset+affectedNode.Piece.Size == len(t.AddBuffer) && index == logicalOffset+affectedNode.Piece.Size {
		affectedNode.Piece.Size += len(str)
		t.Tree.fixSize(affectedNode)
	} else {
		newPiece := PieceDescriptor_Tree{ADD_BUFFER, len(t.AddBuffer), len(str), 0}

		if index == logicalOffset {
			//fmt.Println("Start to insert newPiece to the left:", newPiece)
			t.Tree.insertLeft(&newPiece, affectedNode)
			t.Tree.NumOfNode += 1
		} else if index < logicalOffset+affectedNode.Piece.Size {
			rightPiece := PieceDescriptor_Tree{}
			rightPiece.Buffer = affectedNode.Piece.Buffer
			rightPiece.Offset = affectedNode.Piece.Offset + (index - logicalOffset)
			rightPiece.Size = affectedNode.Piece.Size - (index - logicalOffset)
			rightPiece.Size_left = 0

			affectedNode.Piece.Size -= rightPiece.Size
			//fmt.Println("Start to insert rightPiece:", rightPiece)
			t.Tree.insertRight(&rightPiece, affectedNode)
			//fmt.Println("Start to insert newPiece:", newPiece)
			t.Tree.insertRight(&newPiece, affectedNode)
			t.Tree.NumOfNode += 2
		} else if logicalOffset+affectedNode.Piece.Size == index {
			//fmt.Println("Start to insert newPiece:", newPiece)
			t.Tree.insertRight(&newPiece, affectedNode)
			t.Tree.NumOfNode += 1
		}
	}

	t.Tree.LengthOfFile += len(str)
	t.AddBuffer = append(t.AddBuffer, []byte(str)...)
	return
}

func (t *PieceTable_Tree) Delete(i, j int) {

}

/**
* Gets a string of a particular length from the piece table at a particular offset
 */
func (t *PieceTable_Tree) StringAt(i, length int) *[]byte {

	var buf []byte
	initialNode := t.Tree.find(i)
	finalNode := t.Tree.find(i + length)
	str := []byte{}
	curr := initialNode

	initialLogicalOffset, err := t.Tree.node2LogicalOffset(initialNode)
	if err != nil {
		fmt.Printf("Error in StringAt(): %v\n", err)
	}
	finalLogicalOffset, err := t.Tree.node2LogicalOffset(finalNode)
	if err != nil {
		fmt.Printf("Error in StringAt(): %v\n", err)
	}

	if initialNode.Piece.Buffer {
		buf = t.AddBuffer
	} else {
		buf = t.FileBuffer
	}
	remainingPieceLength := initialNode.Piece.Size - (i - initialLogicalOffset)
	start := initialNode.Piece.Offset + (i - initialLogicalOffset)
	// String lies in the inital node
	if initialNode == finalNode {
		str = append(str, buf[start:start+length]...)
	} else {
		str = append(str, buf[start:start+remainingPieceLength]...)
		// Iterate through remaining pieces
		for true {
			curr = t.Tree.next(curr)
			if curr.Piece.Buffer {
				buf = t.AddBuffer
			} else {
				buf = t.FileBuffer
			}

			// If this is the final piece, only add the remaining length to the string
			if curr == finalNode {
				str = append(str, buf[curr.Piece.Offset:curr.Piece.Offset+(i+length-finalLogicalOffset)]...)
				break
			} else { // Otherwise, add the whole piece to the string
				str = append(str, buf[curr.Piece.Offset:curr.Piece.Offset+curr.Piece.Size]...)
			}
		}
	}
	return &str
}

/*
* Concatenate all pieces via traversing the tree in in-order
 */
func (t *PieceTable_Tree) GetFile() string {

	var curr = t.Tree.Root
	file := []byte{}
	stack := []*Node{}

	for len(stack) > 0 || curr != nil {

		for curr != nil {
			stack = append(stack, curr)
			curr = curr.Left
		}
		curr = stack[len(stack)-1]
		stack = stack[:len(stack)-1]

		if curr.Piece.Buffer == ADD_BUFFER {
			file = append(file, t.AddBuffer[curr.Piece.Offset:curr.Piece.Offset+curr.Piece.Size]...)
		} else {
			file = append(file, t.FileBuffer[curr.Piece.Offset:curr.Piece.Offset+curr.Piece.Size]...)
		}

		curr = curr.Right
	}
	return string(file)
}

func (t *PieceTree) PrintTree() {
	var curr = t.Root
	var width = 0
	queue := []*Node{curr}

	for len(queue) > 0 {
		width = len(queue)
		for i := 0; i < width; i++ {
			curr = queue[0]
			if curr != t.Root {
				fmt.Print(curr.Piece, curr.Parent.Piece, "\t")
			} else {
				fmt.Print(curr.Piece, "\t")
			}

			queue = queue[1:len(queue)]
			if curr.Left != nil {
				queue = append(queue, curr.Left)
			}
			if curr.Right != nil {
				queue = append(queue, curr.Right)
			}
		}
		fmt.Println("\n -----------------------")
	}
}

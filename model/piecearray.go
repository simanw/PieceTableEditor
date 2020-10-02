package model

import (
	"errors"
	"fmt"
)

const (
	// Indicate the piece points to the original file buffer
	FILE_BUFFER = false
	// Indicate the piece points to the add buffer
	ADD_BUFFER = true
)

// Error
var outOfBoundsError = errors.New("Index out of bounds")

// Error
var deleteEmptyString = errors.New("Delete an empty string")

type PieceDescriptor struct {
	Buffer bool
	Offset int
	Length int
}

type PieceTable struct {
	FileBuffer   []byte
	AddBuffer    []byte
	Table        []PieceDescriptor
	LengthOfFile int
}

func (p *PieceTable) InitPieceTable(file string) {
	p.Table = append(p.Table, PieceDescriptor{
		Buffer: FILE_BUFFER,
		Offset: 0,
		Length: len(file),
	})
	p.AddBuffer = []byte{}
	p.FileBuffer = []byte(file)
	p.LengthOfFile = len(file)
}

/**
* Returns the index in the piece table of the piece that contains the character at offset,
* and the offset into that piece's buffer corresponding to the offset
 */
func (p *PieceTable) LogicalOffset2PieceIndexAndBufferOffset(offset int) (int, int, error) {

	if offset < 0 {
		return -1, -1, outOfBoundsError
	}
	remainingOffset := offset
	for i := 0; i < len(p.Table); i++ {
		piece := p.Table[i]
		if remainingOffset <= piece.Length {
			return i, piece.Offset + remainingOffset, nil
		}
		remainingOffset -= piece.Length
	}
	// If this is reached, the offset is greater than the sequence length
	return -1, -1, outOfBoundsError

}
func filter(pieces []PieceDescriptor) []PieceDescriptor {
	ans := []PieceDescriptor{}
	for _, p := range pieces {
		if p.Length > 0 {
			ans = append(ans, p)
		}
	}
	return ans
}

/**
* Inserts a string into the piece table
 */
func (p *PieceTable) Insert(str []byte, offset int) {
	if len(str) == 0 {
		return
	}

	addBufferOffset := len(p.AddBuffer)
	p.AddBuffer = append(p.AddBuffer, str...)
	pieceIndex, bufferOffset, err := p.LogicalOffset2PieceIndexAndBufferOffset(offset)
	if err != nil {
		fmt.Printf("Error in insert(): %v\n", err)
	}
	originalPiece := p.Table[pieceIndex]
	// If the piece points to the end of the add buffer, and we are inserting at its end, simply increase its length
	if originalPiece.Buffer && bufferOffset == originalPiece.Offset+originalPiece.Length && originalPiece.Offset+originalPiece.Length == addBufferOffset {
		originalPiece.Length += len(str)
		p.LengthOfFile += len(str)
		return
	}

	insertPieces := []PieceDescriptor{
		PieceDescriptor{
			Buffer: originalPiece.Buffer,
			Offset: originalPiece.Offset,
			Length: bufferOffset - originalPiece.Offset,
		},
		PieceDescriptor{
			Buffer: ADD_BUFFER,
			Offset: addBufferOffset,
			Length: len(str),
		},
		PieceDescriptor{
			Buffer: originalPiece.Buffer,
			Offset: bufferOffset,
			Length: originalPiece.Length - (bufferOffset - originalPiece.Offset),
		},
	}
	filtered := []PieceDescriptor{}
	for _, piece := range insertPieces {
		if piece.Length > 0 {
			filtered = append(filtered, piece)
		}
	}

	newPieces := make([]PieceDescriptor, pieceIndex+1)
	copy(newPieces, p.Table[:pieceIndex])
	newPieces = append(newPieces, filtered...)
	p.Table = append(newPieces, p.Table[pieceIndex+1:]...)

	p.LengthOfFile += len(str)
}

/**
* Deletes a string from the piece table
 */
func (p *PieceTable) Delete(offset int, length int) error {
	if length == 0 {
		return deleteEmptyString
	}
	if length < 0 {
		p.Delete(offset+length, -length)
	}
	if offset < 0 {
		return outOfBoundsError
	}

	// First, find the affected pieces, since a delete can span multipe pieces
	initialAffectedPieceIndex, initialBufferOffset, err := p.LogicalOffset2PieceIndexAndBufferOffset(offset)
	if err != nil {
		fmt.Printf("Error in delete(): %v\n", err)
	}
	finalAffectedPieceIndex, finalBufferOffset, err := p.LogicalOffset2PieceIndexAndBufferOffset(offset + length)
	if err != nil {
		fmt.Printf("Error in delete(): %v\n", err)
	}
	// If the delete occurs at the end or the beginning of a single piece, simply adjust the window
	if initialAffectedPieceIndex == finalAffectedPieceIndex {
		piece := p.Table[initialAffectedPieceIndex]
		// Is the delete at the beginning of the piece?
		if initialBufferOffset == piece.Offset {
			piece.Offset += length
			piece.Length -= length
			p.LengthOfFile -= length
			return nil
		} else if finalBufferOffset == piece.Offset+piece.Length { // Or at the end of the piece?
			piece.Length -= length
			p.LengthOfFile -= length
			return nil
		}
	}
	deletePieces := []PieceDescriptor{
		PieceDescriptor{
			Buffer: p.Table[initialAffectedPieceIndex].Buffer,
			Offset: p.Table[initialAffectedPieceIndex].Offset,
			Length: initialBufferOffset - p.Table[initialAffectedPieceIndex].Offset,
		},
		PieceDescriptor{
			Buffer: p.Table[finalAffectedPieceIndex].Buffer,
			Offset: finalBufferOffset,
			Length: p.Table[finalAffectedPieceIndex].Length - (finalBufferOffset - p.Table[finalAffectedPieceIndex].Offset),
		},
	}
	filtered := []PieceDescriptor{}
	for _, piece := range deletePieces {
		if piece.Length > 0 {
			filtered = append(filtered, piece)
		}
	}

	newPieces := append(filtered, p.Table[finalAffectedPieceIndex-initialAffectedPieceIndex+1:]...)
	p.Table = append(p.Table[:initialAffectedPieceIndex], newPieces...)
	p.LengthOfFile -= length

	return nil
}

/**
* Gets the sequence as a string
* @returns {string} The sequence
 */
func (p *PieceTable) GetFile() string {
	str := []byte{}

	for _, piece := range p.Table {
		if piece.Buffer {
			str = append(str, p.AddBuffer[piece.Offset:piece.Offset+piece.Length]...)
		} else {
			str = append(str, p.FileBuffer[piece.Offset:piece.Offset+piece.Length]...)
		}
	}
	return string(str)
}

/**
* Gets a string of a particular length from the piece table at a particular offset
 */
func (p *PieceTable) StringAt(offset int, length int) *[]byte {
	if length < 0 {
		return p.StringAt(offset+length, -length)
	}

	str := []byte{}
	var buf []byte
	initialPieceIndex, initialBufferOffset, err := p.LogicalOffset2PieceIndexAndBufferOffset(offset)
	if err != nil {
		fmt.Printf("Error in StringAt() - initial: %v\n", err)
	}
	finalPieceIndex, finalBufferOffset, err := p.LogicalOffset2PieceIndexAndBufferOffset(offset + length)
	if err != nil {
		fmt.Printf("Error in StringAt() - final: %v\n", err)
	}

	piece := p.Table[initialPieceIndex]
	if piece.Buffer {
		buf = p.AddBuffer
	} else {
		buf = p.FileBuffer
	}
	remainingPieceLength := piece.Length - (initialBufferOffset - piece.Offset)
	//fmt.Println("StringAt(): inital piece index ", initialPieceIndex, " final piece index ", finalPieceIndex, "buffer offset: ", initialBufferOffset, "remaining string length ", remainingPieceLength)
	if initialPieceIndex == finalPieceIndex {
		str = append(str, buf[initialBufferOffset:initialBufferOffset+length]...)
	} else {
		str = append(str, buf[initialBufferOffset:initialBufferOffset+remainingPieceLength]...)
		// Iterate through remaining pieces
		for i := initialPieceIndex + 1; i <= finalPieceIndex; i++ {
			piece = p.Table[i]
			if piece.Buffer {
				buf = p.AddBuffer
			} else {
				buf = p.FileBuffer
			}
			// If this is the final piece, only add the remaining length to the string
			if i == finalPieceIndex {
				str = append(str, buf[piece.Offset:finalBufferOffset]...)
			} else { // Otherwise, add the whole piece to the string
				str = append(str, buf[piece.Offset:piece.Offset+piece.Length]...)
			}
		}
	}
	return &str
}

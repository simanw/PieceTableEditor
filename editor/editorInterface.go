package editor

// TextEditor provides functionality for manipulating and analyzing text documents.
type TextEditor interface {
	// Removes characters [i..j) from the document and places them in the clipboard.
	// Previous clipboard contents is overwritten.
	Cut(i, j int)
	// Places characters [i..j) from the document in the clipboard.
	// Previous clipboard contents is overwritten.
	Copy(i, j int)
	// Inserts the contents of the clipboard into the document starting at position i.
	// Nothing is inserted if the clipboard is empty.
	Paste(i int)
	// Returns the document as a string.
	GetText() string
	// Returns the number of misspelled words in the document. A word is considered misspelled
	// if it does not appear in /usr/share/dict/words or any other dictionary (of comparable size)
	// that you choose.
	Misspellings() int
	// Inserts a string at position i
	Insert(str string, i int)
	// Deletes
	Delete(i, j int)
}

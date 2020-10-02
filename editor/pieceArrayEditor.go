package editor

import (
	"bufio"
	"os"
	"pieceTableEditor/model"
	"strings"
)

type MyEditor struct {
	OriginalFile string
	Pt           model.PieceTable
	Trie         *model.Trie
	Clipboard    *[]byte
}

func (e *MyEditor) InitMyEditor(file string) {
	// On windows, the dictionary can often be found at:
	// C:/Users/{username}/AppData/Roaming/Microsoft/Spelling/en-US/default.dic
	fileHandle, _ := os.Open("/usr/share/dict/words")
	defer fileHandle.Close()
	e.Trie = model.InitTrie()
	scanner := bufio.NewScanner(fileHandle)
	for scanner.Scan() {
		e.Trie.Insert(scanner.Text())
	}
	e.OriginalFile = file
	e.Pt.InitPieceTable(file)
	e.Clipboard = &[]byte{}
}

func (e *MyEditor) Cut(i, j int) {
	//fmt.Println("output table in Cut() ", e.Pt.Table)
	e.Clipboard = e.Pt.StringAt(i, j-i)
	e.Pt.Delete(i, j-i)
}

func (e *MyEditor) Copy(i, j int) {
	e.Clipboard = e.Pt.StringAt(i, j-i)
	//fmt.Println("Copying clipboard", e.Clipboard)
}

func (e *MyEditor) Paste(i int) {
	if len(*e.Clipboard) == 0 {
		return
	}
	//fmt.Println("output table in Paste() ", e.Pt.Table)
	//fmt.Println("Pastinging ", e.Clipboard)
	e.Pt.Insert(*e.Clipboard, i)
	e.Clipboard = &[]byte{}
}

func (e *MyEditor) GetText() string {
	return e.Pt.GetFile()
}

func (e *MyEditor) Misspellings() int {
	result := 0
	for _, word := range strings.Fields(e.OriginalFile) {
		if !e.Trie.Find(word) {
			result++
		}
	}
	return result
}

func (e *MyEditor) Insert(str string, i int) {
	e.Pt.Insert([]byte(str), i)
}

func (e *MyEditor) Delete(i, j int) {
	e.Pt.Delete(i, j-i)
}

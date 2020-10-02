package editor

import (
	"bufio"
	"os"
	"strings"
)

type SimpleEditor struct {
	Document   string
	dictionary map[string]bool
	pasteText  string
}

func NewSimpleEditor(document string) SimpleEditor {
	// On windows, the dictionary can often be found at:
	// C:/Users/{username}/AppData/Roaming/Microsoft/Spelling/en-US/default.dic
	fileHandle, _ := os.Open("/usr/share/dict/words")
	defer fileHandle.Close()
	dict := make(map[string]bool)
	scanner := bufio.NewScanner(fileHandle)
	for scanner.Scan() {
		dict[scanner.Text()] = true
	}
	return SimpleEditor{Document: document, dictionary: dict}
}

func (s *SimpleEditor) Cut(i, j int) {
	s.pasteText = s.Document[i:j]
	s.Document = s.Document[:i] + s.Document[j:]
}

func (s *SimpleEditor) Copy(i, j int) {
	s.pasteText = s.Document[i:j]
}

func (s *SimpleEditor) Paste(i int) {
	s.Document = s.Document[:i] + s.pasteText + s.Document[i:]
}

func (s *SimpleEditor) GetText() string {
	return s.Document
}

func (s *SimpleEditor) Misspellings() int {
	result := 0
	for _, word := range strings.Fields(s.Document) {
		if !s.dictionary[word] {
			result++
		}
	}
	return result
}

func (s *SimpleEditor) Insert(str string, i int) {
	s.Document = s.Document[:i] + str + s.Document[i:]
}

func (s *SimpleEditor) Delete(i, j int) {
	if len(s.Document) <= j-i {
		return
	}
	s.Document = s.Document[:i] + s.Document[j:]
}

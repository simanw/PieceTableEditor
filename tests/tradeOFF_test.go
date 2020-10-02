package tests

import (
	"bufio"
	"os"
	"strings"
	"testing"
)

const ALBHABET_SIZE = 27

type TrieNode struct {
	childrens [27]*TrieNode
	isWordEnd bool
}
type Trie struct {
	root *TrieNode
}

func InitTrie() *Trie {
	return &Trie{
		root: &TrieNode{},
	}
}

func (t *Trie) Insert(word string) {
	var index int
	curr := t.root
	for i := 0; i < len(word); i++ {
		switch {
		case 'a' <= word[i] && word[i] <= 'z':
			index = int(word[i]) - 'a'
		case word[i] == '-':
			index = 26
		}
		if curr.childrens[index] == nil {
			curr.childrens[index] = &TrieNode{}
		}
		curr = curr.childrens[index]
	}
	curr.isWordEnd = true
}

func (t *Trie) Find(word string) bool {

	var index int
	curr := t.root
	for i := 0; i < len(word); i++ {
		switch {
		case 'a' <= word[i] && word[i] <= 'z':
			index = int(word[i]) - 'a'
		case word[i] == '-':
			index = 26
		}
		if curr.childrens[index] == nil {
			return false
		}
		curr = curr.childrens[index]
	}
	if curr.isWordEnd {
		return true
	}
	return false
}

type TrieNode53 struct {
	childrens [53]*TrieNode53
	isWordEnd bool
}
type Trie53 struct {
	root *TrieNode53
}

func InitTrie53() *Trie53 {
	return &Trie53{
		root: &TrieNode53{},
	}
}

func (t *Trie53) Insert(word string) {
	var index int
	curr := t.root
	for i := 0; i < len(word); i++ {
		switch {
		case 'a' <= word[i] && word[i] <= 'z':
			index = int(word[i]) - 'a'
		case 'A' <= word[i] && word[i] <= 'Z':
			index = int(word[i]) - 'A' + 26
		case word[i] == '-':
			index = 52
		}
		if curr.childrens[index] == nil {
			curr.childrens[index] = &TrieNode53{}
		}
		curr = curr.childrens[index]
	}
	curr.isWordEnd = true
}

func (t *Trie53) Find(word string) bool {

	var index int
	curr := t.root
	for i := 0; i < len(word); i++ {
		switch {
		case 'a' <= word[i] && word[i] <= 'z':
			index = int(word[i]) - 'a'
		case 'A' <= word[i] && word[i] <= 'Z':
			index = int(word[i]) - 'A' + 26
		case word[i] == '-':
			index = 52
		}
		if curr.childrens[index] == nil {
			return false
		}
		curr = curr.childrens[index]
	}
	if curr.isWordEnd {
		return true
	}
	return false
}

func getMap() map[string]bool {
	file, _ := os.Open("/usr/share/dict/words")
	defer file.Close()

	dict := make(map[string]bool)
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		dict[strings.ToLower(scanner.Text())] = true
	}
	return dict
}

func getTrie53() *Trie53 {
	fileHandle, _ := os.Open("/usr/share/dict/words")
	defer fileHandle.Close()
	trie := InitTrie53()
	scanner := bufio.NewScanner(fileHandle)
	for scanner.Scan() {
		trie.Insert(scanner.Text())
	}
	return trie

}
func getTrie() *Trie {
	fileHandle, _ := os.Open("/usr/share/dict/words")
	defer fileHandle.Close()
	trie := InitTrie()
	scanner := bufio.NewScanner(fileHandle)
	for scanner.Scan() {
		trie.Insert(scanner.Text())
	}
	return trie

}
func BenchmarkMapOrTrie(b *testing.B) {
	b.Run("MapObject", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			getMap()
		}
	})
	b.Run("TrieOfSize53", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			getTrie53()
		}
	})
	b.Run("TrieOfSize27", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			getTrie()
		}
	})
	dict := getMap()
	trie := getTrie()
	trie53 := getTrie53()
	b.Run("SearchMap", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			if dict["foo"] {
				continue
			}
		}
	})

	b.Run("SearchTrieOfSize53", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			if trie53.Find("foo") {
				continue
			}
		}
	})
	b.Run("SearchTrieOfSize27", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			if trie.Find(strings.ToLower("Foo")) {
				continue
			}
		}
	})
}

func BenchmarkByteOrString(b *testing.B) {
	var bytes []byte
	var str string
	b.Run("Btye", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			bytes = append(bytes, "ab"...)
		}
	})

	b.Run("String", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			str += "ab"
		}
	})
}

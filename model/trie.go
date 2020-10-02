package model

const (
	//ALBHABET_SIZE is the total number of characters in english alphabet, plus hyphen
	ALBHABET_SIZE = 53
)

type TrieNode struct {
	childrens [ALBHABET_SIZE]*TrieNode
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
		case 'A' <= word[i] && word[i] <= 'Z':
			index = int(word[i]) - 'A' + 26
		case word[i] == '-':
			index = 52
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

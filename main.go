package main

import (
	"fmt"
	"math/rand"
	"pieceTableEditor/editor"
	"strings"
	"time"
)

func main() {
	file := strings.Repeat("0123456789", 5)

	editor := editor.MyPieceTreeEditor{}
	editor.InitMyPieceTreeEditor(file)
	rand.Seed(time.Now().UnixNano())
	i := 0
	j := 0

	for n := 0; n < 1; n++ {
		str := strings.Repeat("y", 2)
		tmp := rand.Intn(editor.Pt.Tree.LengthOfFile)
		fmt.Println("Randomly insert a string ", str, " at position ", tmp)
		editor.Insert(str, tmp)
		fmt.Println("Get file: ", editor.GetText())

		i = rand.Intn(editor.Pt.Tree.LengthOfFile)
		j = rand.Intn(editor.Pt.Tree.LengthOfFile-i) + i
		editor.Copy(i, j)
		editor.Paste(0)
		fmt.Println("Randomly copy a string from ", i, " to ", j, " then paste it in the front")
		//editor.Pt.Tree.PrintTree()
		fmt.Println("Get file: ", editor.GetText())
	}

}

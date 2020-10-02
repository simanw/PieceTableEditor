package main

import (
	"fmt"
	"math/rand"
	"pieceTableEditor/editor"
	"strings"
	"time"
)

func main() {
	file := strings.Repeat("0123456789", 1)

	editor := editor.MyPieceTreeEditor{}
	editor.InitMyPieceTreeEditor(file)
	rand.Seed(time.Now().UnixNano())
	//i := 0
	//j := 0
	for n := 0; n < 100; n++ {
		str := strings.Repeat("y", 2)
		tmp := rand.Intn(editor.Pt.Tree.LengthOfFile)
		fmt.Println("Random position: Tring to insert string at position", tmp)
		editor.Insert(str, tmp)

		// copy() 没问题了
		// i = rand.Intn(editor.Pt.Tree.LengthOfFile)
		// j = rand.Intn(editor.Pt.Tree.LengthOfFile-i) + i
		// editor.Copy(i, j)
		// res := editor.Clipboard
		editor.Pt.Tree.PrintTree()
		res := editor.GetText()
		//fmt.Println("Get string at ", i, "-", j, string(res))
		fmt.Println("Get file: ", res)
	}

}

package test

import (
	"math/rand"
	"pieceTableEditor/editor"
	"strconv"
	"strings"
	"testing"
	"time"
)

func BenchmarkInsert(b *testing.B) {
	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		{strings.Repeat("Neeva is awesome!", 100)},
	}

	for _, tc := range cases {

		editor := editor.MyPieceTreeEditor{}
		editor.InitMyPieceTreeEditor(tc.data)
		//fmt.Println("----------- Initialization done ----------")
		b.Run("MyPieceTreeEditor"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			rand.Seed(time.Now().UnixNano())
			for i := 0; i < b.N; i++ {
				if rand.Intn(10) <= 7 {
					str := strings.Repeat("a", rand.Intn(20))
					editor.Insert(str, 1)
					//fmt.Println("--------- Fixed position: Successfully insert string at fixed position")
				} else {
					str := strings.Repeat("a", rand.Intn(20))
					tmp := rand.Intn(editor.Pt.Tree.LengthOfFile)
					//fmt.Println("--------- Random position: Tring to insert string at position", tmp)
					editor.Insert(str, tmp)
				}
			}
		})
	}

}

func BenchmarkCopy(b *testing.B) {
	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		{strings.Repeat("Neeva is awesome!", 100)},
	}

	for _, tc := range cases {
		editor := editor.MyPieceTreeEditor{}
		editor.InitMyPieceTreeEditor(tc.data)
		b.Run("MyPieceTreeEditor"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			rand.Seed(time.Now().UnixNano())
			i := 0
			j := 0
			for n := 0; n < b.N; n++ {
				if rand.Intn(10) <= 7 {
					editor.Copy(2, 10)
				} else {
					i = rand.Intn(editor.Pt.Tree.LengthOfFile)
					j = rand.Intn(editor.Pt.Tree.LengthOfFile-i) + i
					editor.Copy(i, j)
				}
			}
		})
	}
}

func BenchmarkClipboard(b *testing.B) {
	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		{strings.Repeat("Neeva is awesome!", 100)},
	}

	for _, tc := range cases {
		b.Run("FixedClipboard"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			editor := editor.MyPieceTreeEditor{}
			editor.InitMyPieceTreeEditor(tc.data)
			for n := 0; n < b.N; n++ {
				if n&1 == 0 {
					editor.Copy(1, 3)
				} else {
					editor.Paste(2)
				}
			}
		})

	}
}

func BenchmarkParallelClipboard(b *testing.B) {
	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		//{strings.Repeat("Neeva is awesome!", 100)},
	}

	for _, tc := range cases {
		b.RunParallel(func(pb *testing.PB) {
			editor := editor.MyPieceTreeEditor{}
			editor.InitMyPieceTreeEditor(tc.data)
			i := 0
			j := 0
			n := 0
			for pb.Next() {
				if n&1 == 0 {
					i = rand.Intn(editor.Pt.Tree.LengthOfFile)
					j = rand.Intn(editor.Pt.Tree.LengthOfFile-i) + i
					editor.Copy(i, j)
				} else {
					pos := rand.Intn(editor.Pt.Tree.LengthOfFile)
					editor.Paste(pos)
				}
				n += 1
			}
		})

	}

}

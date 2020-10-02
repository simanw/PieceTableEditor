package tests

import (
	"math/rand"
	"pieceTableEditor/model"
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
		s := model.NewSimpleEditor(tc.data)
		b.Run("NaiveEditorInsert"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			rand.Seed(time.Now().UnixNano())
			for n := 0; n < b.N; n++ {
				str := strings.Repeat("a", rand.Intn(20))
				s.Insert(str, rand.Intn(len(s.Document)))
			}
		})
		var e = model.MyEditor{}
		e.InitMyEditor(tc.data)
		b.Run("MyEditorInsert"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			rand.Seed(time.Now().UnixNano())
			for n := 0; n < b.N; n++ {
				str := strings.Repeat("a", rand.Intn(20))
				e.Insert(str, rand.Intn(e.Pt.LengthOfFile))
			}
		})

	}
}

func BenchmarkDelete(b *testing.B) {
	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		{strings.Repeat("Neeva is awesome!", 100)},
	}

	for _, tc := range cases {
		s := model.NewSimpleEditor(tc.data)
		b.Run("NaiveEditorDelete"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				rand.Seed(time.Now().UnixNano())
				i := rand.Intn(len(s.Document))
				j := rand.Intn(len(s.Document)-i) + i
				s.Delete(i, j)
			}
		})
		var e = model.MyEditor{}
		e.InitMyEditor(tc.data)
		b.Run("MyEditorDelete"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				i := rand.Intn(e.Pt.LengthOfFile)
				j := rand.Intn(e.Pt.LengthOfFile-i) + i
				e.Delete(i, j)
			}
		})

	}
}

func BenchmarkParallelNaiveClipboard(b *testing.B) {
	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		{strings.Repeat("Neeva is awesome!", 100)},
	}
	for _, tc := range cases {
		//fmt.Println("NaiveEditorCopyPaste" + strconv.Itoa(len(tc.data)))
		b.RunParallel(func(pb *testing.PB) {
			n := 0
			s := model.NewSimpleEditor(tc.data)
			for pb.Next() {
				rand.Seed(time.Now().UnixNano())
				i := 0
				j := 0
				if n%2 == 0 {
					i = rand.Intn(len(s.Document))
					j = rand.Intn(len(s.Document)-i) + i
					s.Copy(i, j)
				} else {
					pos := rand.Intn(len(s.Document) - (j - i))
					s.Paste(pos)
				}
				n += 1
			}
		})
	}
}
func BenchmarkParallelPieceTableClipboard(b *testing.B) {

	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		//{strings.Repeat("Neeva is awesome!", 100)},
	}
	for _, tc := range cases {
		b.RunParallel(func(pb *testing.PB) {
			n := 0
			var s2 = model.MyEditor{}
			s2.InitMyEditor(tc.data)
			for pb.Next() {
				rand.Seed(time.Now().UnixNano())
				i := 0
				j := 0
				if n%2 == 0 {
					i = rand.Intn(s2.Pt.LengthOfFile)
					j = rand.Intn(s2.Pt.LengthOfFile-i) + i
					s2.Copy(i, j)
				} else {
					pos := rand.Intn(s2.Pt.LengthOfFile)
					s2.Paste(pos)
				}
				n += 1
			}
		})
	}
	// 	fmt.Println("MyEditorCopyPaste" + strconv.Itoa(len(tc.data)))

}

func BenchmarkClipboard(b *testing.B) {
	cases := []struct {
		data string
	}{
		{strings.Repeat("Neeva is awesome!", 10)},
		{strings.Repeat("Neeva is awesome!", 100)},
	}
	for _, tc := range cases {

		s := model.NewSimpleEditor(tc.data)
		b.Run("NaiveEditorCutPaste"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				rand.Seed(time.Now().UnixNano())
				i := 0
				j := 0
				if n%2 == 0 {
					i = rand.Intn(len(s.Document))
					j = rand.Intn(len(s.Document)-i) + i
					s.Cut(i, j)
				} else {
					pos := rand.Intn(len(s.Document) - (j - i))
					s.Paste(pos)
				}
			}
		})
		var s1 = model.MyEditor{}
		s1.InitMyEditor(tc.data)
		b.Run("MyEditorCutPaste"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				rand.Seed(time.Now().UnixNano())
				i := 0
				j := 0
				if n%2 == 0 {
					i = rand.Intn(s1.Pt.LengthOfFile)
					j = rand.Intn(s1.Pt.LengthOfFile-i) + i
					s1.Cut(i, j)
				} else {
					pos := rand.Intn(s1.Pt.LengthOfFile)
					s1.Paste(pos)
				}
			}
		})
		s = model.NewSimpleEditor(tc.data)
		b.Run("NaiveEditorGetText"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				_ = s.GetText()
			}
		})
		var s3 = model.MyEditor{}
		s3.InitMyEditor(tc.data)
		b.Run("MyEditorGetText"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				_ = s3.GetText()
			}
		})

		s = model.NewSimpleEditor(tc.data)
		b.Run("NaiveEditorMisspellings"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				_ = s.Misspellings()
			}
		})
		var s4 = model.MyEditor{}
		s4.InitMyEditor(tc.data)
		b.Run("MyEditorMisspellings"+strconv.Itoa(len(tc.data)), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				_ = s4.Misspellings()
			}
		})

	}
}

package views

import (
	"os"
	"path/filepath"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
)

func NewFileSystemViewer(
	rootPath string,
	onFileSelected func(string),
) *widget.Tree {

	tree := widget.NewTree(

		func(uid string) []string {

			files, err := os.ReadDir(uid)

			if err != nil {
				return []string{}
			}

			children := []string{}

			for _, file := range files {

				children = append(
					children,
					filepath.Join(uid, file.Name()),
				)
			}

			return children
		},

		func(uid string) bool {

			info, err := os.Stat(uid)

			if err != nil {
				return false
			}

			return info.IsDir()
		},

		func(branch bool) fyne.CanvasObject {
			return widget.NewLabel("template")
		},

		func(uid string, branch bool, obj fyne.CanvasObject) {

			label := obj.(*widget.Label)

			label.SetText(filepath.Base(uid))
		},
	)

	tree.Root = rootPath

	tree.OnSelected = func(uid string) {

		println(uid)

		onFileSelected(uid)

	}

	return tree
}

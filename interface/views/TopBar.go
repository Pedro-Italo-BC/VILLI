package views

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

func NewTopBar() *fyne.Container {

	return container.NewHBox(

		widget.NewButton(
			"image1",
			func() {},
		),

		widget.NewButton(
			"image2",
			func() {},
		),

		widget.NewButton(
			"file.txt",
			func() {},
		),
	)
}

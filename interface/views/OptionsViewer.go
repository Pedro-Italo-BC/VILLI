package views

import (
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

func NewOptionsViewer() *widget.Card {

	content := container.NewVBox(

		widget.NewCheck(
			"Enable Method A",
			func(bool) {},
		),

		widget.NewCheck(
			"Enable Method B",
			func(bool) {},
		),

		widget.NewButton(
			"Process",
			func() {},
		),
	)

	return widget.NewCard(
		"Options",
		"",
		content,
	)
}

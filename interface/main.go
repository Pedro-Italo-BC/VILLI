package main

import (
	"fyne.io/fyne/v2/app"

	"villi-interface/views"
)

func main() {

	a := app.New()

	w := a.NewWindow("VILLI")

	w.SetContent(
		views.NewMainLayout(),
	)

	w.Resize(
		views.DefaultWindowSize(),
	)

	w.ShowAndRun()
}

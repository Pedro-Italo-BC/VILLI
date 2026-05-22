package views

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

func NewMainLayout() fyne.CanvasObject {

	// FILE VIEWER
	fileViewerPreview := NewFileViewer()

	// FILE SYSTEM
	fileViewer := NewFileSystemViewer(

		"./filePlace",

		func(path string) {

			fileViewerPreview.Update(path)
		},
	)

	// PREVIEW PANEL
	preview := container.NewBorder(

		container.NewHBox(

			widget.NewButton(
				"+",
				func() {
					fileViewerPreview.ZoomIn()
				},
			),

			widget.NewButton(
				"-",
				func() {
					fileViewerPreview.ZoomOut()
				},
			),
		),

		nil,
		nil,
		nil,

		fileViewerPreview,
	)

	// OPTIONS
	options := NewOptionsViewer()

	// RIGHT PANEL
	rightPanel := container.NewBorder(
		options,
		nil,
		nil,
		nil,
		fileViewer,
	)

	// BODY
	body := container.NewHSplit(
		preview,
		rightPanel,
	)

	body.Offset = 0.75

	// TOP BAR
	topBar := NewTopBar()

	// FINAL LAYOUT
	return container.NewBorder(
		topBar,
		nil,
		nil,
		nil,
		body,
	)
}

func DefaultWindowSize() fyne.Size {
	return fyne.NewSize(1400, 900)
}

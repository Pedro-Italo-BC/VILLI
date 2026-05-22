package views

import (
	"fmt"
	"image/color"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

type FileViewer struct {
	widget.BaseWidget

	Content *fyne.Container

	Image *canvas.Image

	CurrentFile string

	Zoom float64

	OffsetX float32
	OffsetY float32
}

func NewFileViewer() *FileViewer {

	bg := canvas.NewRectangle(
		color.RGBA{
			R: 25,
			G: 25,
			B: 25,
			A: 255,
		},
	)

	bg.Resize(fyne.NewSize(5000, 5000))

	content := container.NewWithoutLayout(bg)

	viewer := &FileViewer{
		Content: content,
		Zoom:    1.0,
	}

	viewer.ExtendBaseWidget(viewer)

	return viewer
}

func (v *FileViewer) CreateRenderer() fyne.WidgetRenderer {

	return widget.NewSimpleRenderer(v.Content)
}

func (v *FileViewer) Update(path string) {

	v.CurrentFile = path

	ext := strings.ToLower(
		filepath.Ext(path),
	)

	switch ext {

	case ".svg":
		v.renderSVG(path)

	case ".png",
		".jpg",
		".jpeg",
		".gif",
		".bmp",
		".webp":

		v.renderImage(path)

	case ".txt",
		".json",
		".xml",
		".csv",
		".md",
		".go",
		".py",
		".java",
		".c",
		".cpp":

		v.renderText(path)
	}
}

func (v *FileViewer) renderSVG(path string) {

	output := "./temp/render.png"

	size := int(1500 * v.Zoom)

	cmd := exec.Command(
		"resvg",
		path,
		output,
		"--width",
		fmt.Sprintf("%d", size),
	)

	err := cmd.Run()

	if err != nil {

		println(err.Error())

		return
	}

	v.renderImage(output)
}

func (v *FileViewer) renderImage(path string) {

	img := canvas.NewImageFromFile(path)

	img.FillMode = canvas.ImageFillContain

	size := fyne.NewSize(
		float32(1000*v.Zoom),
		float32(1000*v.Zoom),
	)

	img.Resize(size)

	img.Move(
		fyne.NewPos(
			v.OffsetX,
			v.OffsetY,
		),
	)

	v.Image = img

	bg := canvas.NewRectangle(
		color.RGBA{
			R: 25,
			G: 25,
			B: 25,
			A: 255,
		},
	)

	bg.Resize(fyne.NewSize(5000, 5000))

	v.Content.Objects = []fyne.CanvasObject{
		bg,
		img,
	}

	v.Refresh()
}

func (v *FileViewer) renderText(path string) {

	data, err := os.ReadFile(path)

	if err != nil {
		return
	}

	text := widget.NewMultiLineEntry()

	text.SetText(string(data))

	scroll := container.NewScroll(text)

	v.Content.Objects = []fyne.CanvasObject{
		scroll,
	}

	v.Refresh()
}

func (v *FileViewer) ZoomIn() {

	v.Zoom *= 1.2

	if v.CurrentFile != "" {
		v.Update(v.CurrentFile)
	}
}

func (v *FileViewer) ZoomOut() {

	v.Zoom /= 1.2

	if v.Zoom < 0.1 {
		v.Zoom = 0.1
	}

	if v.CurrentFile != "" {
		v.Update(v.CurrentFile)
	}
}

func (v *FileViewer) Dragged(
	ev *fyne.DragEvent,
) {

	v.OffsetX += ev.Dragged.DX
	v.OffsetY += ev.Dragged.DY

	if v.CurrentFile != "" {
		v.Update(v.CurrentFile)
	}
}

func (v *FileViewer) DragEnd() {}

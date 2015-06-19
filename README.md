# ComponentPrinter
QML Component for printing (or saving images of) a component (and children therein).

---

### Declaration:


```
import com.foxmoxie.Printer 1.0

Printer {
    id: myPrinter
}
```


### Properties:


* bool antialias
  * enable or disable antialiasing

* int dpi
  * specify the dpi to print (auto-set by 'setup' method)

* string filepath
  * the filepath (on some platforms) to print to when "print to file" is selected

* object item
  * the qml component you want to print

* string printerName
  * the name of the destination printer (auto-set by 'setup' method)


* int printHeight
  * the page height in millimeters (auto-set by 'setup' method)

* int printWidth
  * the page width in millimeters (auto-set by 'setup' method)



### Methods:


* bool print()
  * print the item using predeclared parameters.

* bool saveImage(string fileName, string fileFormat, int quality)
  * save an image of the component to an image file.

* bool setup()
  * display a print dialogue and configure our printer from it. returns true if the user clicks 'print', false otherwise.

* setMargins( double top, double right, double bottom, double left )
  * sets the page margins (auto-set by 'setup' method)

* QRectF getMargins()
  * returns a QRectF representing the currently set margins. 'x', 'y', 'width', and 'height' are the pertinent properties of the returned object.


### Signals:


* printComplete
  * when the component is printer this signal will be emitted.

* antialiasChanged, dpiChanged, filePathChanged, itemChanged, marginsChanged, printerNameChanged, printHeightChanged, printWidthChanged
  * when a property changes the respective signal is emitted.


### Example:


```
import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import com.foxmoxie.Printer 1.0

Window {
    width: 640
    height: 640
    visible: true

    Rectangle {
        id: myAmazingComponent
	color: 'red'

	width: 300
	height: 300
	anchors.centerIn: parent

	Rectangle {
	    id: alsoAmazing
	    color: 'blue'

	    width: 150
	    height: 150
	    anchors.centerIn: parent
	}
    }


    Printer {
        id: printer
        item: myAmazingComponent
        antialias: false
    }

    Button {
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }
        text: 'Print'
        onClicked: {
            if( printer.setup() )
                printer.print();
            //printer.saveImage("test.png", 'png', 100);
        }
    }
}
```

# ComponentPrinter
QML Component for printing (or saving images of) a component (and children therein).

Please note that the API has changed significantly since the 1.0 version.

---

### Declaration:


```
import com.foxmoxie.Printer 1.1

Printer {
    id: myPrinter
}
```


### Properties:


* bool antialias
  * enable or disable antialiasing

* int resolution
  * the dpi to print at

* string filepath
  * the filepath (on some platforms) to print to when "print to file" is selected

* object item
  * the qml component you want to print

* QRectF pageRect (read-only, call setPageSize to change)
  * a QRectF object representing the page dimensions in device pixels. for other units, see getPaperRect() method. this is usually smaller than the paperRect() since the page normally has margins between its borders and the paper.  'x', 'y', 'width', and 'height' are the pertinent properties of the returned object.

* QRectF paperRect (read-only, call setPageSize to change)
  * a QRectF object representing the paper dimensions in device pixels. for other units, see getPaperRect() method.  'x', 'y', 'width', and 'height' are the pertinent properties of the returned object.

* object paperSizes (read-only)
  * an array of strings representing the known standard paper sizes configurable via the 'setPageSize(string)' method.

* string printerName
  * the name of the destination printer

* Status status
  * the printer's current status (if supported). unfortunately this property has no 'changed' signal. can be one of:
    * Printer::Idle
    * Printer::Active
    * Printer::Aborted
    * Printer::Error
    * Printer::Unknown


### Methods:


* bool print()
  * print the item using predeclared parameters.

* bool saveImage(string fileName, string fileFormat, int quality)
  * save an image of the component to an image file.

* PrinterOptions setup()
  * display a print dialogue. returns a PrinterOptions object for use with 'setOptions' if the user clicks 'print', null otherwise.

* setMargins( double top, double right, double bottom, double left )
  * sets the page margins (auto-set by 'setup' method)

* QRectF getMargins()
  * returns a QRectF representing the currently set margins. 'x', 'y', 'width', and 'height' are the pertinent properties of the returned object.

* void setOptions( PrinterOptions options )
  * configure to print using the specified options object.

* bool setPageSize( int width, int height, Unit unit )
  * configure the page size to a custom size. valid units are:
    * Printer::Millimeter
    * Printer::Point
    * Printer::Inch
    * Printer::Pica
    * Printer::Didot
    * Printer::Cicero
    * Printer::DevicePixel

* bool setPageSize( string paperSize )
  * configure the page size to a standard size (one found in the 'paperSizes' property or via getPaperSizes)

* QRectF getPageRect( Unit units ) (default unit used is DevicePixel)
  * fetch the page size in the specified units of measurement. 'x', 'y', 'width', and 'height' are the pertinent properties of the returned object. valid units are:
    * Printer::Millimeter
    * Printer::Point
    * Printer::Inch
    * Printer::Pica
    * Printer::Didot
    * Printer::Cicero
    * Printer::DevicePixel

* QRectF getPaperRect( Unit units ) (Default unit used is DevicePixel)
  * fetch the paper size in the specified units of measurement. 'x', 'y', 'width', and 'height' are the pertinent properties of the returned object. valid units are:
    * Printer::Millimeter
    * Printer::Point
    * Printer::Inch
    * Printer::Pica
    * Printer::Didot
    * Printer::Cicero
    * Printer::DevicePixel

* Status getStatus()
  * the printer's current status (if supported).  can be one of:
    * Printer::Idle
    * Printer::Active
    * Printer::Aborted
    * Printer::Error
    * Printer::Unknown

#### Property Hooks: (See the respective property documentation for details.)


* void setAntialias( bool toggle )
* void setFilePath( string filepath )
* void setItem( object item )
* void setPrinterName( string printerName )
* void setResolution( int dpi )

* bool getAntialias()
* string getFilePath()
* object getItem()
* object getPaperSizes()
* string getPrinterName()
* int getResolution()
* Status getStatus()


### Signals:


* printComplete
  * when the component is printer this signal will be emitted.

* antialiasChanged
  * antialiasing setting changed

* filePathChanged
  * the target print to file path has changed

* itemChanged
  * the target component has changed

* marginsChanged
  * somehow the margins were adjusted, by hand or by print dialogue

* printerNameChanged
  * a new printer (name) was selected

* resolutionChanged
  * the printer resolution has changed

* sizeChanged
  * target printing size has changed


### Example:


```
import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import com.foxmoxie.Printer 1.1

Window {
    width: 640
    height: 640
    visible: true

    Rectangle {
        id: pageRect
        color: 'white'
        width: printer.pageRect.width // When one sets a size using the 'setPageSize' method, this changes.
        height: printer.pageRect.height // When one sets a size using the 'setPageSize' method, this changes.
        visible: false // Can be true or false, still works!

        Rectangle {
            id: myAmazingComponent
            color: 'red'

            width: 300
            height: 300
            anchors.centerIn: parent // Or position it anywhere on your page.

            Rectangle {
                id: alsoAmazing
                color: 'blue'

                width: 150
                height: 150
                anchors.centerIn: parent
            }
        }
    }

    Printer {
        id: printer
        item: pageRect
        antialias: false

        Component.onCompleted: {
            console.log( "Sizes: " );
            var sz = printer.paperSizes;
            for( var x=0; x < sz.length; x++ )
                console.log(' - ' + sz[x]);

            // To use a standard size:
            printer.setPageSize( 'Letter / ANSI A' );

            // Or custom:
            // printer.setPageSize( 640, 480, Printer.DevicePixel );
            // Valid "units" are Millimeter, Point, Inch, Pica, Didot, Cicero, and DevicePixel.
            // These are resolution dependent (except DevicePixel) so I suggest having your resolution configured in advance.
        }
    }

    Button {
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }
        text: 'Print'
        onClicked: {
            var opts = printer.setup();
            if( opts )
            {
                // If you don't do this any changes to the settings (in the dialogue) will be ignored:
                printer.setOptions( opts );
                // However when you call setOptions any previously specified parameters may be replaced.

                printer.print();
            }

            //printer.saveImage("test.png", 'png', 100);
        }
    }
}
```

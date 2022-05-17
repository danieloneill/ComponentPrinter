# ComponentPrinter
QML Component for printing (or saving images of) a component (and children therein).

Please note that the API has changed significantly since the 1.0 version.

---

### Declaration:


```
import com.foxmoxie.Printer 1.3  // For Qt 5.x
//import com.foxmoxie.Printer    // for Qt 6.x

Printer {
    id: myPrinter
}
```


### Properties:


* bool antialias
  * enable or disable antialiasing

* bool monochrome
  * enable or disable monochrome printing (eg, thermal printers)

* int copyCount
  * specify how many copies a call to 'print()' will yield

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

* bool grabImage(string fileFormat, [int quality], [function(ByteArray data) callback])
  * provides an image of the item via provided callback function as a byte array.

* bool saveImage(string fileName, string fileFormat, int quality, [function(bool success) callback])
  * save an image of the item to an image file, optionally with a callback upon completion.

* bool open()
  * open a printing session (start a new print job). *This MUST be called before print*
  
* bool print([function(bool success) callback])
  * print the item using predeclared parameters, optionally with a callback upon completion. (Note that this method will usually return before printing is complete, so calling *close* immediately will likely break your entire world. Instead wait for printComplete, printError, or close in your provided callback function.)

* bool newPage()
  * begins a new page. this does not need to be called for the first page. doing so will result in your first page being blank.

* bool close()
  * close (and submit) the open printing session. *This "mUsT" be called after printing is complete*
  
* bool abort()
  * attempt to abort the current print job, and close the session.
  
* bool setup()
  * display a print dialogue. configures the printer context and returns true if the user clicks 'print', false otherwise.

* setMargins( double top, double right, double bottom, double left )
  * sets the page margins (auto-set by 'setup' method)

* QRectF getMargins()
  * returns a QRectF representing the currently set margins. 'x', 'y', 'width', and 'height' are the pertinent properties of the returned object.

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
* void setMonochrome( bool monochrome )
* void setCopyCount( int count )

* bool getAntialias()
* string getFilePath()
* object getItem()
* object getPaperSizes()
* string getPrinterName()
* int getResolution()
* bool getMonochrome()
* int getCopyCount()

### Signals:


* printComplete
  * when the component successfully prints this signal will be emitted.

* printError
  * when the component unsuccessfully prints this signal will be emitted.

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

* monochromeChanged
  * flag for printing to monochrome has changed

* copyCountChanged
  * copy count has changed

### Example:

**NOTE**
*In order to execute the example below (or most of the included example files) you must launch with a QApplication initialised due to the usage of QPrintDialog which requires this!*

```
$ qml -a widget ./singlepage.qml
```


```
import QtQuick
import QtQuick.Controls
import QtQuick.Window
import com.foxmoxie.Printer

Window {
    width: 640
    height: 640
    visible: true

    id: topWindow

    Rectangle {
        id: pageContainer
        color: 'white'
        width: printer.pageRect.width // When one sets a size using the 'setPageSize' method, this changes.
        height: printer.pageRect.height // When one sets a size using the 'setPageSize' method, this changes.
        visible: false // Can be true or false, still works!


        Rectangle {
            id: myAmazingComponent
            color: 'red'

            width: parent.width * 0.25
            height: parent.width * 0.25
            anchors.centerIn: parent // Or position it anywhere on your page.

            Rectangle {
                id: alsoAmazing
                color: 'blue'

                width: parent.width * 0.5
                height: parent.width * 0.5
                anchors.centerIn: parent

                Text {
                    anchors.centerIn: parent
                    font.pixelSize: parent.height * 0.5
                    color: 'white'
                    text: "😄"
                }
            }
        }
    }

    Printer {
        id: printer
        antialias: false
        monochrome: false
        item: pageContainer

        onPrintComplete: console.log("Print complete.");
        onPrintError: console.log("Print error!");

        Component.onCompleted: scanPaperSizes();

        function scanPaperSizes()
        {
            console.log( "Sizes: " );
            printer.paperSizes.forEach( function(sz) {
                console.log(' - ' + sz);
            } );

            // To use a standard size:
            printer.setPageSize( 'Letter / ANSI A' );

            // Or custom:
            // printer.setPageSize( 640, 480, Printer.DevicePixel );
            // Valid "units" are Millimeter, Point, Inch, Pica, Didot, Cicero, and DevicePixel.
            // These are resolution dependent (except DevicePixel) so I suggest having your resolution configured in advance.
        }

        function printPage()
        {
            // Open a print job, print the component (item), then close/submit the print job.
            printer.open();
            printer.print();
            printer.close();
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
            if( printer.setup() )
                printer.printPage();
        }
    }
}

```

/////////////////////////////////////////////////////////////////////////////
// This example uses QPrintDialog which requires a QApplication instance.
// If running from QML runtime utility, specify the apptype:
//  $ qml -a widget imagecomponents.qml
/////////////////////////////////////////////////////////////////////////////


/*******************
 * Assets included:
 *   https://commons.wikimedia.org/wiki/File:Methane_venting_offshore_Virginia.jpg - Background Image
 *   NOAA, Public domain, via Wikimedia Commons
 *
 *   https://rustacean.net/ - Ferris the Crab
 *   Karen Rustad Tölva, Public Domain
 *******************/

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
        visible: true // Can be true or false, still works!


        Image {
            id: myAmazingComponent
            source: '1024px-Methane_venting_offshore_Virginia.jpg'
            anchors.centerIn: parent // Or position it anywhere on your page.

            Image {
                id: alsoAmazing

                width: parent.width * 0.5
                height: parent.width * 0.5
                anchors.centerIn: parent

                source: 'rustacean-flat-happy.svg'
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
    }

    Button {
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }
        text: 'Save'
        onClicked: {
            printer.saveImage("test.png", 'png', 100);
            console.log("Saved to 'test.png'.");
        }
    }
}

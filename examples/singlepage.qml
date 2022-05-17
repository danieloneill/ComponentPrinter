/////////////////////////////////////////////////////////////////////////////
// This example uses QPrintDialog which requires a QApplication instance.
// If running from QML runtime utility, specify the apptype:
//  $ qml -a widget singlepage.qml
/////////////////////////////////////////////////////////////////////////////

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
    }

    Button {
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }
        text: 'Print'
        onClicked: {
            if( !printer.setup() )
            {
                console.log("Cancelled in Print Setup.");
                return;
            }

            // Open a print job, print the component (item), then close/submit the print job.
            if( !printer.open() )
            {
                console.log("Failed to open printer!");
                return;
            }

            console.log("Okay, now we print!");

            // This doesn't happen immediately, so the close will happen when the component
            // sends either the printComplete or printError signal:
            printer.print( function() {
                console.log("Job complete, closing printer context.");
                printer.close();
            } );
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// This example uses QPrintDialog which requires a QApplication instance.
// If running from QML runtime utility, specify the apptype:
//  $ qml -a widget multipage.qml
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

    Component {
        id: pageComponent

        Rectangle {
            id: pageContainer
            color: 'white'
            width: printer.pageRect.width // When one sets a size using the 'setPageSize' method, this changes.
            height: printer.pageRect.height // When one sets a size using the 'setPageSize' method, this changes.
            visible: false // Can be true or false, still works!

            property string text: 'Unset'

            // Any regular QML/Qt Quick can be inside your container.
            // It will be printed 'as seen on screen', with layers rendered
            // in the same order as on screen.
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
                        text: pageContainer.text
                    }
                }
            }
        }
    }

    Printer {
        id: printer
        antialias: false
        monochrome: false

        // Just for our sake to track which page number we're on.
        // In this example, it's also displayed in the box printed.
        property int page: 1

        onPrintComplete: nextPage();
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

        // First page, we don't new a 'newPage'.
        function start()
        {
            printer.page = 1;

            // Open a session:
            printer.open();

            // Generate a page and print it:
            printer.printPage();
        }

        function generatePage()
        {
            // Subsequent pages will start on a new page.
            printer.newPage();

            // Generate the next page, and print it:
            printer.printPage();
        }

        function printPage()
        {
            let newPageObject = pageComponent.createObject(printer, { text: printer.page });
            if( !newPageObject )
            {
                console.log("Failed to generate page #"+printer.page);
                return;
            }

            printer.item = newPageObject;
            printer.print();
        }

        function nextPage()
        {
            console.log('Page '+page+' printed.');
            if( printer.page < 4 )
            {
                // Print the next one.
                printer.page++;
                generatePage();
            }
            else
                // Done! Calling printer.close() completes this print job and submits it to the printing subsystem.
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
            printer.page = 1;

            if( printer.setup() )
                printer.start();
        }
    }
}

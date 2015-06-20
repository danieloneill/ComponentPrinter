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

        onPrintComplete: {
            console.log('Print success.');
        }

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

                delete opts; // Or don't.
                printer.print();
            }

            //printer.saveImage("test.png", 'png', 100);
        }
    }
}

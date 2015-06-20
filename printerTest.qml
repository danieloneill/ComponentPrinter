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
        //visible: false // Can be true or false, still works!

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
                    text: ''+printer.page
                }
            }
        }
    }

    Printer {
        id: printer
        item: pageRect
        antialias: false

        property int page: 1

        onPrintComplete: {
            console.log('Page '+page+' printed.');
            if( printer.page < 4 )
            {
                // Pretend we're printing multiple pages here...
                printer.page++;
                printer.print();
            }
        }
        onPrintError: {
            console.log("Print error!");
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
            if( printer.setup() )
                printer.print();

            //printer.saveImage("test.png", 'png', 100);
        }
    }
}

#include "componentprinter_plugin.h"
#include "printer.h"

#include <qqml.h>

void ComponentPrinterPlugin::registerTypes(const char *uri)
{
    // @uri com.foxmoxie.Printer
    qmlRegisterType<Printer>(uri, 1, 1, "Printer");
    qmlRegisterUncreatableType<PrinterOptions>(uri, 1, 0, "PrinterOptions", "Instantiated by Printer::setup()");
}



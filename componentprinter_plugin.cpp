#include "componentprinter_plugin.h"
#include "printer.h"

#include <qqml.h>

void ComponentPrinterPlugin::registerTypes(const char *uri)
{
    // @uri com.foxmoxie.Printer
    qmlRegisterType<Printer>(uri, 1, 3, "Printer");
}



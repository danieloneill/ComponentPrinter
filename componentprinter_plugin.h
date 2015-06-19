#ifndef COMPONENTPRINTER_PLUGIN_H
#define COMPONENTPRINTER_PLUGIN_H

#include <QQmlExtensionPlugin>

class ComponentPrinterPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // COMPONENTPRINTER_PLUGIN_H


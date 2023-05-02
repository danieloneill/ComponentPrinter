#include "printer.h"

#include <QBuffer>
#include <QFileInfo>
#include <QPainter>
#ifndef QT_NO_PRINTER
# include <QPrintEngine>
#endif
#include <QQuickItemGrabResult>

// Just for converting QByteArray:
#include <QQmlEngine>

Printer::Printer(QQuickItem *parent):
    QQuickItem(parent)
{
#ifndef QT_NO_PRINTER
    m_printDialogue = nullptr;
    m_printer = new QPrinter(QPrinter::ScreenResolution);
    m_pagePrinted = false;
    m_sessionOpen = false;
    m_copyCount = 1;
    m_painter = nullptr;
    m_antialias = true;
    m_monochrome = false;
    m_margins = QRectF(0, 0, 0, 0);
    m_filepath.clear();
#endif

    m_mode = Printer::GrabOnly;
    m_item = NULL;

    m_fileDest.clear();
    m_fileType.clear();
    m_fileQuality = 0;
}

Printer::~Printer()
{
#ifndef QT_NO_PRINTER
    delete m_printer;
#endif
}

#ifndef QT_NO_PRINTER
bool Printer::print(QJSValue callback)
{
    m_mode = Printer::Print;
    m_callback = callback;
    return grab();
}
#endif

bool Printer::grabImage(const QString &fileFormat, int quality, QJSValue callback)
{
    m_mode = Printer::GrabOnly;
    m_callback = callback;
    m_fileType = fileFormat;
    m_fileQuality = quality;
    return grab();
}

bool Printer::saveImage(const QString &fileName, const QString &fileFormat, int quality, QJSValue callback)
{
    m_mode = Printer::PrintToFile;
    m_callback = callback;
    m_fileDest = fileName;
    m_fileType = fileFormat;
    m_fileQuality = quality;
    return grab();
}

#ifndef QT_NO_PRINTER
bool Printer::printImage(const QImage &img)
{
    QMarginsF margins( m_margins.left(), m_margins.top(), m_margins.right(), m_margins.bottom() );
    if( !m_printer->setPageMargins( margins, QPageLayout::Millimeter ) )
    {
        qWarning() << tr("Printer: Failed to set page margin (in mm) as configured.");
        return false;
    }

    if( !m_sessionOpen )
    {
        qWarning() << tr("Printer: Attempt to print without first calling Printer::open(). (This behaviour changed in 1.2)");;
        return false;
    }

    if( m_monochrome )
        // Convert to monochrome, no dithering:
        m_painter->drawImage( m_printer->paperRect(QPrinter::DevicePixel), img.convertToFormat(QImage::Format_Mono, Qt::MonoOnly | Qt::ThresholdDither) );
    else
        m_painter->drawImage( m_printer->paperRect(QPrinter::DevicePixel), img );

    return true;
}

bool Printer::printImageData(const QByteArray &data)
{
    return printImage( QImage::fromData(data) );
}

bool Printer::setup()
{
    m_printer->setOutputFormat(QPrinter::NativeFormat);

    m_printDialogue = new QPrintDialog(m_printer);
    if( m_printDialogue->exec() == QDialog::Accepted )
    {
        m_printDialogue->deleteLater();
        return true;
    }

    delete m_printDialogue;

    return false;
}

bool Printer::open()
{
    if( m_sessionOpen )
    {
        qWarning() << tr("Printer::open called while already in a multipage session. (Call 'close' first.)");
        return false;
    }

    m_painter = new QPainter();
    if( !m_painter )
    {
        qWarning() << tr("Printer::open failed to instantiate new QPainter. (Are you out of memory?)");
        return false;
    }

    if( !m_painter->begin(m_printer) )
    {
        qWarning() << tr("Failed to initialise QPainter to QPrintDevice.");
        return false;
    }

    m_painter->setRenderHint(QPainter::Antialiasing, m_antialias);
    m_painter->setRenderHint(QPainter::TextAntialiasing, m_antialias);
    m_painter->setRenderHint(QPainter::SmoothPixmapTransform, m_antialias);

    m_sessionOpen = true;
    return true;
}

bool Printer::close()
{
    if( !m_sessionOpen )
    {
        qWarning() << tr("Printer::close called while not in multipage session.");
        return false;
    }

    delete m_painter;
    m_painter = nullptr;
    m_sessionOpen = false;

    return true;
}

bool Printer::newPage() const
{
    if( !m_sessionOpen )
    {
        qWarning() << tr("Printer::newPage called while not in a multipage session. (Call Printer::open first.)");
        return false;
    }

    return m_printer->newPage();
}

bool Printer::abort()
{
    if( m_sessionOpen )
        close();

    return m_printer->abort();
}

void Printer::setMonochrome(bool toggle)
{
    if( m_monochrome == toggle )
        return;

    m_monochrome = toggle;
    emit monochromeChanged();
}

void Printer::setAntialias(bool toggle)
{
    if( m_antialias == toggle )
        return;

    m_antialias = toggle;
    emit antialiasChanged();
}

void Printer::setFilePath(const QString &filepath)
{
    if( m_filepath == filepath )
        return;

    m_filepath = filepath;
    emit filePathChanged();
}
#endif

void Printer::setItem(QQuickItem *item)
{
     if( m_item == item )
         return;

     m_item = item;
     emit itemChanged();
}

#ifndef QT_NO_PRINTER
void Printer::setMargins(double top, double right, double bottom, double left)
{
    QRectF m( left, top, right-left, bottom-top );
    if( m_margins == m )
        return;

    m_margins = m;
    emit marginsChanged();
}

bool Printer::setPageSize( const QString &paperSize )
{
    QPageSize size;
    // Run through each..
    for( int x=0; x < QPageSize::LastPageSize; x++ )
    {
        size = QPageSize((QPageSize::PageSizeId)x);
        if( size.name() == paperSize )
        {
            bool result = m_printer->setPageSize( size );
            emit sizeChanged();
            return result;
        }
    }

    qWarning() << tr("Unknown paper size: ") << paperSize << tr(" (Refer to 'paperSizes()' for valid options.)");
    return false;
}

bool Printer::setPageSize( qreal width, qreal height, Unit unit )
{
    QSizeF szf(width, height);
    QPageSize size;

    switch( unit )
    {
    case DevicePixel:
        // Fanagle from DPI:
        szf /= m_printer->resolution();
        size = QPageSize(szf, QPageSize::Inch);
        break;
    default:
        size = QPageSize(szf, (QPageSize::Unit)unit);
        break;
    }

    bool result = m_printer->setPageSize(size);
    emit sizeChanged();
    return result;
}

void Printer::setPrinterName(const QString &printerName)
{
    if( m_printer->printerName() == printerName )
        return;

    m_printer->setPrinterName( printerName );
    emit printerNameChanged();
}

void Printer::setResolution(int dpi)
{
    if( m_printer->resolution() == dpi )
        return;

    m_printer->setResolution( dpi );
    emit resolutionChanged();
}

void Printer::setCopyCount(int count)
{
    if( m_printer->copyCount() == count )
        return;

    m_printer->setCopyCount( count );
    emit copyCountChanged();
}

QRectF Printer::getPageRect(Unit unit) const
{
    return m_printer->pageRect( (QPrinter::Unit)unit );
}

QRectF Printer::getPaperRect(Unit unit) const
{
    return m_printer->paperRect( (QPrinter::Unit)unit );
}

QStringList Printer::getPaperSizes() const
{
    QStringList results;
    QPageSize size;
    // Run through each..
    for( int x=0; x < QPageSize::LastPageSize; x++ )
    {
        size = QPageSize((QPageSize::PageSizeId)x);
        results.append( size.name() );
    }
    return results;
}

Printer::Status Printer::getStatus() const
{
    QPrinter::PrinterState state = m_printer->printEngine()->printerState();
    return (Printer::Status)state;
}
#endif

bool Printer::grab()
{
    if( !m_item )
    {
        qWarning() << tr("Printer::grab: No item source specified. (Set it with the 'item' property.)");
        return false;
    }

    QSharedPointer<QQuickItemGrabResult> res = m_item->grabToImage();
    if( !res )
    {
        qWarning() << tr("Printer::grab: Grab failed for some reason. (Is the item loaded and rendered?)");
        return false;
    }

    connect( res.data(), SIGNAL(ready()), this, SLOT(grabbed()) );
    m_result = res;

    return true;
}

void Printer::grabbed()
{
    const QImage img = m_result.data()->image();
    m_result.clear();

    QQmlEngine *jse = qmlEngine(this);
    jse->collectGarbage();

    bool ret = true;

    if( m_mode == Printer::PrintToFile )
    {
        ret = img.save(m_fileDest, m_fileType.toStdString().c_str(), m_fileQuality);
        if( m_callback.isCallable() )
        {
            QJSValueList args;
            args << ret;
            m_callback.call(args);
        }
    }
#ifndef QT_NO_PRINTER
    else if( m_mode == Printer::Print )
    {
        if( !m_filepath.isEmpty() )
            m_printer->setOutputFileName(m_filepath);

        ret = printImage(img);
        if( m_callback.isCallable() )
        {
            QJSValueList args;
            args << ret;
            m_callback.call(args);
        }
    }
#endif
    else if( m_callback.isCallable() )
    {
        QImage image;
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        ret = img.save(&buffer, m_fileType.toStdString().c_str(), m_fileQuality);
        buffer.close();

        if( ret )
        {
            QJSValueList args;
            args << jse->toScriptValue<QByteArray>(ba);
            m_callback.call( args );
        }
    }

    m_callback = QJSValue();

    if( ret )
        emit printComplete();
    else
        emit printError();
}

#ifndef QT_NO_PRINTER
QString Printer::getPrinterName() const
{
    return m_printer->printerName();
}
#endif

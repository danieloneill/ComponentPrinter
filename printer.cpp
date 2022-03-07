#include "printer.h"

#include <QFileInfo>
#include <QPainter>
#include <QPrintEngine>
#include <QQuickItemGrabResult>

Printer::Printer(QQuickItem *parent):
    QQuickItem(parent)
{
    m_printDialogue = nullptr;
    m_printer = new QPrinter(QPrinter::ScreenResolution);
    m_pagePrinted = false;
    m_sessionOpen = false;
    m_savingToFile = true;
    m_copyCount = 1;
    m_painter = nullptr;
    m_antialias = true;
    m_monochrome = false;
    m_filepath.clear();
    m_item = NULL;
    m_margins = QRectF(0, 0, 0, 0);

    m_fileDest.clear();
    m_fileType.clear();
    m_fileQuality = 0;
}

Printer::~Printer()
{
    delete m_printer;
}

bool Printer::print()
{
    m_savingToFile = false;
    return grab();
}

bool Printer::saveImage(const QString &fileName, const QString &fileFormat, int quality)
{
    m_savingToFile = true;
    m_fileDest = fileName;
    m_fileType = fileFormat;
    m_fileQuality = quality;
    return grab();
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

QPrinter::Unit qprinterUnitFromPrinterUnit(Printer::Unit unit)
{
    QPrinter::Unit punit;
    switch( unit )
    {
    case Printer::Millimeter:
        punit = QPrinter::Millimeter;
        break;
    case Printer::Point:
        punit = QPrinter::Point;
        break;
    case Printer::Pica:
        punit = QPrinter::Pica;
        break;
    case Printer::Inch:
        punit = QPrinter::Inch;
        break;
    case Printer::Didot:
        punit = QPrinter::Didot;
        break;
    case Printer::Cicero:
        punit = QPrinter::Cicero;
        break;
    case Printer::DevicePixel:
        punit = QPrinter::DevicePixel;
        break;
    default:
        punit = (QPrinter::Unit)unit;
    }
    return punit;
}

QRectF Printer::getPageRect(Unit unit) const
{
    return m_printer->pageRect( qprinterUnitFromPrinterUnit(unit) );
}

QRectF Printer::getPaperRect(Unit unit) const
{
    return m_printer->paperRect( qprinterUnitFromPrinterUnit(unit) );
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

    qWarning() << "Unknown paper size: " << paperSize << " (Refer to 'paperSizes()' for valid options.)";
    return false;
}

bool Printer::setPageSize( qreal width, qreal height, Unit unit )
{
    QSizeF szf(width, height);
    QPageSize size;

    switch( unit )
    {
    case Millimeter:
        size = QPageSize(szf, QPageSize::Millimeter);
        break;
    case Point:
        size = QPageSize(szf, QPageSize::Point);
        break;
    case Inch:
        size = QPageSize(szf, QPageSize::Inch);
        break;
    case Pica:
        size = QPageSize(szf, QPageSize::Pica);
        break;
    case Didot:
        size = QPageSize(szf, QPageSize::Didot);
        break;
    case Cicero:
        size = QPageSize(szf, QPageSize::Cicero);
        break;
    case DevicePixel:
        // Fanagle from DPI:
        szf /= m_printer->resolution();
        size = QPageSize(szf, QPageSize::Inch);
        break;
    }

    bool result = m_printer->setPageSize(size);
    emit sizeChanged();
    return result;
}

Printer::Status Printer::getStatus() const
{
    QPrinter::PrinterState state = m_printer->printEngine()->printerState();
    switch( state )
    {
        case QPrinter::Idle:
            return Idle;
        case QPrinter::Active:
            return Active;
        case QPrinter::Aborted:
            return Aborted;
        case QPrinter::Error:
            return Error;
    }
    return Unknown;
}

bool Printer::grab()
{
    if( !m_item )
    {
        qWarning() << "Printer::grab: No item source specified. (Set it with the 'item' property.)";
        return false;
    }

    QSharedPointer<QQuickItemGrabResult> res = m_item->grabToImage();
    if( !res )
    {
        qWarning() << "Printer::grab: Grab failed for some reason. (Is the item loaded and rendered?)";
        return false;
    }

    connect( res.data(), SIGNAL(ready()), this, SLOT(grabbed()) );
    m_result = res;

    return true;
}

bool Printer::open()
{
    if( m_sessionOpen )
    {
        qWarning() << tr("Printer::open called while already in a multipage session. (Call 'close' first.)");
        return false;
    }

    m_sessionOpen = true;
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

bool Printer::printGrab(const QImage &img)
{
    QMarginsF margins( m_margins.left(), m_margins.top(), m_margins.right(), m_margins.bottom() );
    if( !m_printer->setPageMargins( margins, QPageLayout::Millimeter ) )
    {
        qWarning() << tr("Printer: Failed to set page margin (in mm) as configured.");
        return false;
    }

    // Less work if we're in a multipage session:
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

void Printer::grabbed()
{
    const QImage img = m_result.data()->image();
    m_result.clear();

    bool ret;

    if( m_savingToFile )
        ret = img.save(m_fileDest, m_fileType.toStdString().c_str(), m_fileQuality);
    else
    {
        if( !m_filepath.isEmpty() )
            m_printer->setOutputFileName(m_filepath);

        ret = printGrab(img);
    }

    if( ret )
        emit printComplete();
    else
        emit printError();
}

void Printer::setItem(QQuickItem *item)
{
     if( m_item == item )
         return;

     m_item = item;
     emit itemChanged();
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

void Printer::setMargins(double top, double right, double bottom, double left)
{
    QRectF m( left, top, right-left, bottom-top );
    if( m_margins == m )
        return;

    m_margins = m;
    emit marginsChanged();
}

void Printer::setPrinterName(const QString &printerName)
{
    if( m_printer->printerName() == printerName )
        return;

    m_printer->setPrinterName( printerName );
    emit printerNameChanged();
}

QString Printer::getPrinterName() const
{
    return m_printer->printerName();
}

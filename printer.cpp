#include "printer.h"

#include <QFileInfo>
#include <QPainter>
#include <QPrintEngine>
#include <QQuickItemGrabResult>

Printer::Printer(QQuickItem *parent):
    QQuickItem(parent)
{
    m_printer = new QPrinter();
    m_antialias = true;
    m_monochrome = false;
    m_item = NULL;
    m_savingToFile = true;
    m_margins = QRectF(0, 0, 0, 0);
    m_copyCount = 1;
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

QRectF Printer::getPageRect(Unit unit)
{
    return m_printer->pageRect( qprinterUnitFromPrinterUnit(unit) );
}

QRectF Printer::getPaperRect(Unit unit)
{
    return m_printer->paperRect( qprinterUnitFromPrinterUnit(unit) );
}

QStringList Printer::getPaperSizes()
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

Printer::Status Printer::getStatus()
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
        qWarning() << "Printer::saveImage: Grab failed for some reason. (Is the item visible and rendered?)";
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

    bool ret;

    if( m_savingToFile )
        ret = img.save(m_fileDest, m_fileType.toStdString().c_str(), m_fileQuality);
    else
    {
        if( !m_filepath.isEmpty() )
            m_printer->setOutputFileName(m_filepath);

        m_printer->setPageMargins( m_margins.left(), m_margins.top(), m_margins.right(), m_margins.bottom(), QPrinter::Millimeter );

        QPainter painter;
        painter.begin(m_printer);
        painter.setRenderHint(QPainter::Antialiasing, m_antialias);
        painter.setRenderHint(QPainter::TextAntialiasing, m_antialias);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, m_antialias);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, m_antialias);

        painter.fillRect( m_printer->paperRect(), qRgba(255, 255, 255, 255) );

        if( m_monochrome )
            // Convert to monochrome, no dithering:
            painter.drawImage( m_printer->paperRect(), img.convertToFormat(QImage::Format_Mono, Qt::MonoOnly | Qt::ThresholdDither) );
        else
            painter.drawImage( m_printer->paperRect(), img.convertToFormat(QImage::Format_Mono, Qt::MonoOnly | Qt::ThresholdDither) );

        painter.end();
        ret = true;
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
//#ifdef Q_OS_WIN32
    m_printer->setOutputFormat(QPrinter::NativeFormat);
//#endif

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

QString Printer::getPrinterName()
{
    return m_printer->printerName();
}

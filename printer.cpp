#include "printer.h"

#include <QFileInfo>
#include <QPainter>
#include <QQuickItemGrabResult>

Printer::Printer(QQuickItem *parent):
    QQuickItem(parent)
{
    m_antialias = false;
    m_dpi = 600;
    m_item = NULL;
    m_savingToFile = true;
    m_margins = QRectF(0, 0, 0, 0);
}

Printer::~Printer()
{
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

bool Printer::grab()
{
    if( !m_item )
    {
        qDebug() << "Printer::grab: No item source specified. (Set it with the 'item' property.)";
        return false;
    }

    QSharedPointer<QQuickItemGrabResult> res = m_item->grabToImage();
    if( !res )
    {
        qDebug() << "Printer::saveImage: Grab failed for some reason. (Is the item visible and rendered?)";
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
            m_printer.setOutputFileName(m_filepath);

        m_printer.setResolution(m_dpi);
        m_printer.setPaperSize( QSizeF( m_printwidth, m_printheight ), QPrinter::Millimeter );
        m_printer.setPageMargins( m_margins.left(), m_margins.top(), m_margins.right(), m_margins.bottom(), QPrinter::Millimeter );

        QPainter painter;
        painter.begin(&m_printer);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setRenderHint(QPainter::TextAntialiasing, false);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, false);

        painter.fillRect( m_printer.paperRect(), qRgba(255, 255, 255, 255) );
        painter.drawImage( m_printer.paperRect(), img );

        painter.end();
        ret = true;
    }

    if( ret )
        qDebug() << "Print Success.";
    else
        qDebug() << "Print Failed.";

    emit printComplete();
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
#ifdef Q_OS_WIN32
    m_printer.setOutputFormat(QPrinter::NativeFormat);
#endif

    if( m_printDialogue.exec() == QDialog::Accepted )
    {
        setPrinterName(m_printDialogue.printer()->printerName());
        setFilePath(m_printDialogue.printer()->outputFileName());
        setDpi(m_printDialogue.printer()->resolution());
        setPrintHeight(m_printDialogue.printer()->pageSizeMM().height());
        setPrintWidth(m_printDialogue.printer()->pageSizeMM().width());
        return true;
    }

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
    if( m_printer.printerName() == printerName )
        return;

    m_printer.setPrinterName( printerName );
    emit printerNameChanged();
}

QString Printer::getPrinterName()
{
    return m_printer.printerName();
}

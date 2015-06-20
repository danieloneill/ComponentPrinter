#ifndef PRINTER_H
#define PRINTER_H

#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QQuickItem>

class PrinterOptions : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PrinterOptions)

public:
    PrinterOptions(QObject *parent=NULL) : QObject(parent) {}

    QPrinterInfo opts;
};

class Printer : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(Printer)

    QSharedPointer<QQuickItemGrabResult> m_result;
    QPrintDialog    m_printDialogue;
    QPrinter        *m_printer;
    bool            m_savingToFile;

    bool m_antialias;
    QString m_filepath;
    QQuickItem *m_item;
    QRectF m_margins;

    QString m_fileDest;
    QString m_fileType;
    int m_fileQuality;

    Q_PROPERTY(bool antialias READ getAntialias WRITE setAntialias NOTIFY antialiasChanged)
    Q_PROPERTY(int resolution READ getResolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(QString filepath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QQuickItem* item READ getItem WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(QRectF pageRect READ getPageRect NOTIFY sizeChanged)
    Q_PROPERTY(QRectF paperRect READ getPaperRect NOTIFY sizeChanged)
    Q_PROPERTY(QStringList paperSizes READ getPaperSizes)
    Q_PROPERTY(QString printerName READ getPrinterName WRITE setPrinterName NOTIFY printerNameChanged)
    Q_PROPERTY(Status status READ getStatus)

public:
    Printer(QQuickItem *parent = 0);
    ~Printer();

    typedef enum {
        Millimeter = 0,
        Point,
        Inch,
        Pica,
        Didot,
        Cicero,
        DevicePixel
    } Unit;
    Q_ENUMS(Unit)

    typedef enum {
        Idle,
        Active,
        Aborted,
        Error,
        Unknown
    } Status;
    Q_ENUMS(Status)

public slots:
    bool print();
    bool saveImage(const QString &fileName, const QString &fileFormat, int quality);
    PrinterOptions *setup();

    // Property Hooks:
    void setAntialias(bool toggle) { if( m_antialias == toggle ) return; m_antialias = toggle; emit antialiasChanged(); }
    void setFilePath(const QString &filepath) { if( m_filepath == filepath ) return; m_filepath = filepath; emit filePathChanged(); }
    void setItem( QQuickItem *item );
    void setMargins(double top, double right, double bottom, double left);
    void setOptions( PrinterOptions *p );
    bool setPageSize( qreal width, qreal height, Unit unit );
    bool setPageSize( const QString &paperSize );
    void setPrinterName(const QString &printerName);
    void setResolution(int dpi) { if( m_printer->resolution() == dpi ) return; m_printer->setResolution( dpi ); emit resolutionChanged(); }

    bool getAntialias() { return m_antialias; }
    QString getFilePath() { return m_filepath; }
    QQuickItem *getItem() { return m_item; }
    QRectF getMargins() { return m_margins; }
    QRectF getPageRect(Unit unit=DevicePixel);
    QRectF getPaperRect(Unit unit=DevicePixel);
    QStringList getPaperSizes();
    QString getPrinterName();
    int getResolution() { return m_printer->resolution(); }
    Status getStatus();

private slots:
    bool grab();
    void grabbed();

signals:
    void antialiasChanged();
    void filePathChanged();
    void itemChanged();
    void marginsChanged();
    void printComplete();
    void printError();
    void printerNameChanged();
    void resolutionChanged();
    void sizeChanged();
};

#endif // PRINTER_H


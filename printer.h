#ifndef PRINTER_H
#define PRINTER_H

#ifndef QT_NO_PRINTER
# include <QPrinter>
# include <QPrintDialog>
#endif
#include <QQuickItem>
#include <QJSValue>

class Printer : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_DISABLE_COPY(Printer)

public:
    typedef enum { Print, PrintToFile, GrabOnly } GrabMode;
    Q_ENUMS(GrabMode);

private:
    QSharedPointer<QQuickItemGrabResult> m_result;
    QQuickItem *m_item;
#ifndef QT_NO_PRINTER
    QPrintDialog    *m_printDialogue;
    QPrinter        *m_printer;
    bool            m_pagePrinted;
    bool            m_sessionOpen;
    int             m_copyCount;
    QPainter        *m_painter;

    bool    m_antialias;
    bool    m_monochrome;
    QString m_filepath;
    QRectF  m_margins;
#endif

    GrabMode    m_mode;
    QString     m_fileDest;
    QString     m_fileType;
    int         m_fileQuality;
    QJSValue    m_callback;

    Q_PROPERTY(QQuickItem* item READ getItem WRITE setItem NOTIFY itemChanged)
#ifndef QT_NO_PRINTER
    Q_PROPERTY(QString filepath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool antialias READ getAntialias WRITE setAntialias NOTIFY antialiasChanged)
    Q_PROPERTY(bool monochrome READ getMonochrome WRITE setMonochrome NOTIFY monochromeChanged)
    Q_PROPERTY(int resolution READ getResolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(int copyCount READ getCopyCount WRITE setCopyCount NOTIFY copyCountChanged)
    Q_PROPERTY(QRectF pageRect READ getPageRect NOTIFY sizeChanged)
    Q_PROPERTY(QRectF paperRect READ getPaperRect NOTIFY sizeChanged)
    Q_PROPERTY(QStringList paperSizes READ getPaperSizes)
    Q_PROPERTY(QString printerName READ getPrinterName WRITE setPrinterName NOTIFY printerNameChanged)
    Q_PROPERTY(Status status READ getStatus)
#endif

public:
    Printer(QQuickItem *parent = 0);
    ~Printer();

#ifndef QT_NO_PRINTER
    typedef enum {
        Millimeter = QPageSize::Millimeter,
        Point = QPageSize::Point,
        Inch = QPageSize::Inch,
        Pica = QPageSize::Pica,
        Didot = QPageSize::Didot,
        Cicero = QPageSize::Cicero,
        DevicePixel
    } Unit;
    Q_ENUMS(Unit)

    typedef enum {
        Idle = QPrinter::Idle,
        Active = QPrinter::Active,
        Aborted = QPrinter::Aborted,
        Error = QPrinter::Error,
        Unknown
    } Status;
    Q_ENUMS(Status)
#endif

public slots:
#ifndef QT_NO_PRINTER
    bool print(QJSValue callback=QJSValue());
    bool setup();
    bool open();
    bool close();
    bool newPage() const;
    bool abort();
#endif

    bool grabImage(const QString &fileFormat, int quality=100, QJSValue callback=QJSValue());
    bool saveImage(const QString &fileName, const QString &fileFormat, int quality, QJSValue callback=QJSValue());
#ifndef QT_NO_PRINTER
    bool printImage(const QImage &img);
    bool printImageData(const QByteArray &img);
#endif

    // Property Hooks:
    void setItem( QQuickItem *item );
#ifndef QT_NO_PRINTER
    void setFilePath(const QString &filepath);
    void setMonochrome(bool toggle);
    void setAntialias(bool toggle);
    void setMargins(double top, double right, double bottom, double left);
    bool setPageSize( qreal width, qreal height, Unit unit );
    bool setPageSize( const QString &paperSize );
    void setPrinterName(const QString &printerName);
    void setResolution(int dpi);
    void setCopyCount(int count);
#endif

    QQuickItem *getItem() const { return m_item; }
#ifndef QT_NO_PRINTER
    QString getFilePath() const { return m_filepath; }
    bool getMonochrome() const { return m_monochrome; }
    bool getAntialias() const { return m_antialias; }
    QRectF getMargins() const { return m_margins; }
    QRectF getPageRect(Unit unit=DevicePixel) const;
    QRectF getPaperRect(Unit unit=DevicePixel) const;
    QStringList getPaperSizes() const;
    QString getPrinterName() const;
    int getResolution() const { return m_printer->resolution(); }
    int getCopyCount() const { return m_printer->copyCount(); }
    Status getStatus() const;
#endif

private slots:
    bool grab();
    void grabbed();

signals:
    void itemChanged();
    void frameGrabbed(const QByteArray &imageData);
    void sizeChanged();
    void printComplete();
    void printError();
#ifndef QT_NO_PRINTER
    void filePathChanged();
    void monochromeChanged();
    void antialiasChanged();
    void marginsChanged();
    void printerNameChanged();
    void resolutionChanged();
    void copyCountChanged();
#endif
};

#endif // PRINTER_H


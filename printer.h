#ifndef PRINTER_H
#define PRINTER_H

#include <QPrinter>
#include <QPrintDialog>
#include <QQuickItem>

class Printer : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(Printer)

    QSharedPointer<QQuickItemGrabResult> m_result;
    QPrintDialog    *m_printDialogue;
    QPrinter        *m_printer;
    bool            m_pagePrinted;
    bool            m_sessionOpen;
    bool            m_savingToFile;
    int             m_copyCount;
    QPainter        *m_painter;

    bool m_antialias;
    bool m_monochrome;
    QString m_filepath;
    QQuickItem *m_item;
    QRectF m_margins;

    QString m_fileDest;
    QString m_fileType;
    int m_fileQuality;

    Q_PROPERTY(bool antialias READ getAntialias WRITE setAntialias NOTIFY antialiasChanged)
    Q_PROPERTY(bool monochrome READ getMonochrome WRITE setMonochrome NOTIFY monochromeChanged)
    Q_PROPERTY(int resolution READ getResolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(int copyCount READ getCopyCount WRITE setCopyCount NOTIFY copyCountChanged)
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
    bool setup();

    bool open();
    bool close();
    bool newPage() const;
    bool abort();

    // Property Hooks:
    void setMonochrome(bool toggle) { if( m_monochrome == toggle ) return; m_monochrome = toggle; emit monochromeChanged(); }
    void setAntialias(bool toggle) { if( m_antialias == toggle ) return; m_antialias = toggle; emit antialiasChanged(); }
    void setFilePath(const QString &filepath) { if( m_filepath == filepath ) return; m_filepath = filepath; emit filePathChanged(); }
    void setItem( QQuickItem *item );
    void setMargins(double top, double right, double bottom, double left);
    bool setPageSize( qreal width, qreal height, Unit unit );
    bool setPageSize( const QString &paperSize );
    void setPrinterName(const QString &printerName);
    void setResolution(int dpi) { if( m_printer->resolution() == dpi ) return; m_printer->setResolution( dpi ); emit resolutionChanged(); }
    void setCopyCount(int count) { if( m_printer->copyCount() == count ) return; m_printer->setCopyCount( count ); emit copyCountChanged(); }

    bool getMonochrome() const { return m_monochrome; }
    bool getAntialias() const { return m_antialias; }
    QString getFilePath() const { return m_filepath; }
    QQuickItem *getItem() const { return m_item; }
    QRectF getMargins() const { return m_margins; }
    QRectF getPageRect(Unit unit=DevicePixel) const;
    QRectF getPaperRect(Unit unit=DevicePixel) const;
    QStringList getPaperSizes() const;
    QString getPrinterName() const;
    int getResolution() const { return m_printer->resolution(); }
    int getCopyCount() const { return m_printer->copyCount(); }
    Status getStatus() const;

private slots:
    bool grab();
    void grabbed();

private:
    bool printGrab(const QImage &img);

signals:
    void monochromeChanged();
    void antialiasChanged();
    void filePathChanged();
    void itemChanged();
    void marginsChanged();
    void printComplete();
    void printError();
    void printerNameChanged();
    void resolutionChanged();
    void copyCountChanged();
    void sizeChanged();
};

#endif // PRINTER_H


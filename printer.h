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
    QPrintDialog    m_printDialogue;
    QPrinter        m_printer;
    bool            m_savingToFile;

    bool m_antialias;
    int m_dpi;
    QString m_filepath;
    QQuickItem *m_item;
    QRectF m_margins;
    int m_printheight;
    int m_printwidth;

    QString m_fileDest;
    QString m_fileType;
    int m_fileQuality;

    Q_PROPERTY(bool antialias READ getAntialias WRITE setAntialias NOTIFY antialiasChanged)
    Q_PROPERTY(int dpi READ getDpi WRITE setDpi NOTIFY dpiChanged)
    Q_PROPERTY(QString filepath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QQuickItem* item READ getItem WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(QString printerName READ getPrinterName WRITE setPrinterName NOTIFY printerNameChanged)
    Q_PROPERTY(int printHeight READ getPrintHeight WRITE setPrintHeight NOTIFY printHeightChanged)
    Q_PROPERTY(int printWidth READ getPrintWidth WRITE setPrintWidth NOTIFY printWidthChanged)

public:
    Printer(QQuickItem *parent = 0);
    ~Printer();

public slots:
    bool print();
    bool saveImage(const QString &fileName, const QString &fileFormat, int quality);
    bool setup();

    // Property Hooks:
    void setAntialias(bool toggle) { if( m_antialias == toggle ) return; m_antialias = toggle; emit antialiasChanged(); }
    void setDpi(int dpi) { if( m_dpi == dpi ) return; m_dpi = dpi; emit dpiChanged(); }
    void setFilePath(const QString &filepath) { if( m_filepath == filepath ) return; m_filepath = filepath; emit filePathChanged(); }
    void setItem( QQuickItem *item );
    void setMargins(double top, double right, double bottom, double left);
    void setPrinterName(const QString &printerName);
    void setPrintHeight(int height) { if( height == m_printheight ) return; m_printheight = height; emit printHeightChanged(); }
    void setPrintWidth(int width) { if( width == m_printwidth ) return; m_printwidth = width; emit printWidthChanged(); }

    bool getAntialias() { return m_antialias; }
    int getDpi() { return m_dpi; }
    QString getFilePath() { return m_filepath; }
    QQuickItem *getItem() { return m_item; }
    QRectF getMargins() { return m_margins; }
    QString getPrinterName();
    int getPrintHeight() { return m_printheight; }
    int getPrintWidth() { return m_printwidth; }

private slots:
    bool grab();
    void grabbed();

signals:
    void antialiasChanged();
    void dpiChanged();
    void filePathChanged();
    void itemChanged();
    void marginsChanged();
    void printerNameChanged();
    void printHeightChanged();
    void printWidthChanged();

    void printComplete();
};

#endif // PRINTER_H


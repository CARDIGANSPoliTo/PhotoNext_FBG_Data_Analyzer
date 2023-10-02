#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>

class ColorPicker : public QWidget
{
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = nullptr);
    void setColor(const QColor &, const QColor &, bool set = true);

private:
    QColor m_color;

    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void colorChanged(const QColor &color);
};

#endif // COLORPICKER_H

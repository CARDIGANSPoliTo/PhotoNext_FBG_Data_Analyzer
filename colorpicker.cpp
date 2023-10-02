#include "colorpicker.h"
#include <QColorDialog>

ColorPicker::ColorPicker(QWidget *parent)
    : QWidget{parent}
    , m_color{QColor::Invalid}
{
}

void ColorPicker::setColor(const QColor &color, const QColor &master, bool set)
{
    if(master != m_color)
    {
        setAutoFillBackground(true);
        setPalette(QPalette(color));
        if(set)
            m_color = master;
        emit colorChanged(master);
    }
    else
    {
        setAutoFillBackground(true);
        setPalette(QPalette(color));
    }
}

void ColorPicker::mouseReleaseEvent(QMouseEvent *event)
{
    QColorDialog diag(m_color);

    if(diag.exec() == QColorDialog::Accepted)
    {
        setColor(diag.currentColor(), diag.currentColor(), false);
    }
}

#ifndef FFTVIEW_H
#define FFTVIEW_H

#include "fftoptionsdialog.h"
#include <QWidget>

class FFTView : public QWidget
{
    Q_OBJECT
public:
    explicit FFTView(QWidget *parent = nullptr);

private:
    FFTOptionsDialog m_optionsDialog;

};

#endif // FFTVIEW_H

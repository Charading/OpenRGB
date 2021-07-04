/*-------------------------------------------------------------------*\
|  Swatches.cpp                                                       |
|                                                                     |
|  Custom Colour Swatch widget that allows for adding user colours    |
|                                                                     |
|  Chris M (Dr_No)          23rd April 2021                           |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "swatches.h"
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>

Swatches::Swatches(QWidget *parent) :
    QWidget(parent),
    initSize(size_inc_margin * minColumns, size_inc_margin * minRows)
{
    setBaseSize(initSize);
    setSizeIncrement(size_inc_margin, size_inc_margin);
    add_swatch.color.setRgb( 0, 0, 0, 0); //transparent

    swatch black_swatch;
    swatch_ary.push_back(black_swatch);
    swatch white_swatch;
    white_swatch.color.setRgb(255, 255, 255, 255);
    swatch_ary.push_back(white_swatch);
    swatch red_swatch;
    red_swatch.color.setRgb(255, 0, 0, 255);
    swatch_ary.push_back(red_swatch);
    swatch yellow_swatch;
    yellow_swatch.color.setRgb(255, 255, 0, 255);
    swatch_ary.push_back(yellow_swatch);
    swatch green_swatch;
    green_swatch.color.setRgb( 0, 255, 0, 255);
    swatch_ary.push_back(green_swatch);
    swatch cyan_swatch;
    cyan_swatch.color.setRgb( 0, 255, 255, 255);
    swatch_ary.push_back(cyan_swatch);
    swatch blue_swatch;
    blue_swatch.color.setRgb( 0, 0, 255, 255);
    swatch_ary.push_back(blue_swatch);
    swatch magenta_swatch;
    magenta_swatch.color.setRgb(255, 0, 255, 255);
    swatch_ary.push_back(magenta_swatch);

    min_swatches = swatch_ary.size();
}

QColor Swatches::color()
{
    return swatch_ary[selected].color.toRgb();
}

QSize Swatches::sizeHint () const
{
    return QSize(width(), height());
}

QSize Swatches::minimumSizeHint () const
{
    return baseSize();
}

void Swatches::currentColorInput(const QColor &color)
{
    if(color == picker_color)
    {
        return;
    }

    picker_color = color;
}

void Swatches::addCustomSwatch(const QColor &color)
{
    swatch new_swatch;
    new_swatch.color = picker_color;
    swatch_ary.push_back(new_swatch);
    update();
}

void Swatches::resizeEvent(QResizeEvent *event)
{
    unsigned int size = 0;

    if(event->size().width() < event->size().height())
    {
        size = event->size().width();
    }
    else
    {
        size = event->size().height();
    }

    swatch_pixmap   = QPixmap(event->size());
    swatch_pixmap.fill(Qt::transparent);
    drawSwatches(event->size());
    update();
}

void Swatches::mousePressEvent(QMouseEvent *event)
{
    mouseDown = true;
}

void Swatches::mouseReleaseEvent(QMouseEvent *event)
{
    if(!mouseDown)
    {
        return;
    }

    /*-----------------------------------------------------*\
    | Clear mouse down and in-region flags                  |
    \*-----------------------------------------------------*/
    mouseDown   = false;

    if(add_swatch.region.contains(event->pos()))
    {
        addCustomSwatch(picker_color);
    }
    else
    {
        int swatch_count = swatch_ary.size();
        for(std::size_t i = 0; i < swatch_count; i++)
        {
            if(swatch_ary[i].region.contains(event->pos()))
            {
                emit swatchChanged(swatch_ary[i].color.toRgb());
                break;
            }
        }
    }
}

void Swatches::drawSwatches(const QSize &newSize)
{
    /*-----------------------------------------------------*\
    | Create image canvas & paint background transparent    |
    \*-----------------------------------------------------*/
    swatch_image = QImage(newSize, QImage::Format_ARGB32_Premultiplied);
    swatch_image.fill(Qt::transparent);

    /*-----------------------------------------------------*\
    | Set up painter with antialiasing                      |
    \*-----------------------------------------------------*/
    QPainter painter(&swatch_image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(border_pen);

    /*-----------------------------------------------------*\
    | Paint the swatch cluster                              |
    \*-----------------------------------------------------*/
    int size            = qMin(size_inc_margin, (newSize.height() / minRows) - (margin * 2));
    size_inc_margin     = size + (margin * 2);
    int swatch_count    = swatch_ary.size();

    QPoint pointNewSwatch(margin, margin);
    for(std::size_t i = 0; i < swatch_count; i++)
    {
        QBrush brush(swatch_ary[i].color, Qt::SolidPattern);
        swatch_ary[i].region.setSize(QSize(size, size));
        swatch_ary[i].region.marginsAdded(QMargins(margin, margin, margin, margin));
        swatch_ary[i].region.moveTo((i / minRows * size_inc_margin), (i % minRows * size_inc_margin));

        painter.setBrush(brush);
        painter.drawRect(swatch_ary[i].region);
    }

    QBrush brush(add_swatch.color);
    add_swatch.region.setSize(QSize(size, size));
    add_swatch.region.marginsAdded(QMargins(margin, margin, margin, margin));
    add_swatch.region.moveTo((swatch_count / minRows * size_inc_margin), (swatch_count % minRows * size_inc_margin));

    painter.setBrush(brush);
    painter.drawRect(add_swatch.region);
    painter.drawText(add_swatch.region, Qt::AlignCenter, QString("+"));

    swatch_pixmap = QPixmap().fromImage(swatch_image);
}


void Swatches::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    drawSwatches(this->size());             //This is the main draw function
    painter.drawPixmap(0,0,swatch_pixmap);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Created between 2005 and 2012 by The Voreen Team                   *
 * as listed in CREDITS.TXT <http://www.voreen.org>                   *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

/*
 * This code is based on a ColorPicker class from voreen.
 * For more information about voree see http://www.voreen.org/
 */

#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

#include <QFrame>

class QColor;
class QMouseEvent;
class QPaintEvent;

/**
 * A widget that allows the user to select a color. Therefore a rect with all possible colors and
 * a cross indicating the current selected color are drawn. You can change the current color by
 * a click on the desired color.
 */
class ColorPicker : public QFrame
{
    Q_OBJECT

  public:
    /**
     * Constructor
     *
     * @param parent the parent widget
     */
    ColorPicker(QWidget *parent = 0);

    /**
     * Destructor
     */
    ~ColorPicker(void);

  public slots:
    /**
     * Slot that is called when the color of the selected key in the mapping canvas changes.
     *
     * @param h new hue value
     * @param s new saturation value
     */
    void setCol(int h, int s);

    /**
     * Slot that is called when the color of the selected key in the mapping canvas changes.
     *
     * @param c new color
     */
    void setCol(const QColor & c);

  signals:
    /**
     * This signal is emitted when the color was changed by the user.
     */
    void newCol(int h, int s);

    /**
     * This signal is emitted when the user changes the color. It will switch
     * the coarseness mode of volume rendering on and off.
     *
     * @param on should coarseness mode switched on or off?
     */
    void toggleInteractionMode(bool b);

  protected:
    /**
     * Paints a rect with all selectable colors and a black cross that is centered at
     * the current selected color.
     *
     * @param event the paint event
     */
    void paintEvent(QPaintEvent *event);

    /**
     * Sets the current color to the color at the clicked position.
     * The center of the black cross is also set to this position.
     *
     * @param event the mouse event
     */
    void mouseMoveEvent(QMouseEvent *event);

    /**
     * Sets the current color to the color at the clicked position.
     * The center of the black cross is also set to this position.
     *
     * @param event the mouse event
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * Only emits the toggleInteractionMode signal, so that coarseness mode is siwtched off.
     *
     * @param event the mouse event
     */
    void mouseReleaseEvent(QMouseEvent *event);

  private:
    /**
     * Converts the current color to a point so that the color at the point
     * equals the current color.
     *
     * @return point in the widget that equals the current color
     */
    QPoint colPt(void);

    /**
     * Converts the position of the given point to the corresponding hue value.
     *
     * @param pt position where the hue value is querried
     * @return hue value at the given position
     */
    int huePt(const QPoint & pt);

    /**
     * Converts the position of the given point to the corresponding saturation value.
     *
     * @param pt position where the saturation value is querried
     * @return saturation value at the given position
     */
    int satPt(const QPoint & pt);

    /**
     * Sets the current color to the color at the given position.
     *
     * @param pt position
     */
    void setCol(const QPoint & pt);

    int hue_; ///< hue value of current color
    int sat_; ///< saturation value of current color

    QPixmap *pix_; ///< pixmap with all selectable colors
};

 #endif // COLOR_PICKER_H

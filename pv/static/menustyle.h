
#ifndef MENUSTYLE_H
#define MENUSTYLE_H

#include <QProxyStyle>

class MenuStyle : public QProxyStyle
{
public:
    int styleHint(StyleHint stylehint, const QStyleOption *opt, const QWidget *widget, QStyleHintReturn *returnData) const override;
};

#endif // MENUSTYLE_H

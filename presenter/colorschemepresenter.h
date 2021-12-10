#ifndef PRESENTER_COLORCHEMEPRESENTER_H
#define PRESENTER_COLORCHEMEPRESENTER_H

#include <QFile>
#include <QObject>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonArray>
#include <QApplication>
#include <QJsonDocument>

#include "structs/colorscheme/colorranges.h"

using MapOfColorRanges = QMap<QString, ColorRanges>;

class ColorSchemePresenter : public QObject
{
    Q_OBJECT
public:
    explicit ColorSchemePresenter(QObject *parent);
    ~ColorSchemePresenter();

    const QList<QString> GetAllGradientsNames();
    const QVector<QColor> GetColorVector(const QString &gradientName);
    const QVector<int> GetRanges(const QString &gradientName);
    const QVector<QPoint> GetPoints(const QString &gradientName);
    const ColorRanges &GetColorRanges(const QString &gradientName);

    void DeleteGradientByName(const QString &gradientName);
    void SaveGradient(const QString &gradientName, const QVector<int> &ranges, const QVector<QColor> &colors);
public Q_SLOTS:
    void OnSchemeEdit(const QString &gradientName, ColorRanges &range);
public:
    void GetPresetFromFile();
private:
    void SetDefaultColors();
    void ParseJsonDocument(const QJsonObject &array, QStringList &namesList);
    void NameAttributeAppedToList(QStringList &list, const QJsonObject &object);
    void RangeAttributeAppedToColorRanges(ColorRanges &ranges, const QJsonObject &object);
    void ColorAttributeAppedToColorRanges(ColorRanges &ranges, const QJsonObject &object);
    void PointAttributeAppedToColorRanges(ColorRanges &ranges, const QJsonObject &object);
private:
    void SavePresetToFile();
    QJsonObject GetJsonObjectFromColorRange(const QString &colorName, const ColorRanges &range);
    QJsonObject GetJsonObjectFromRangeData(const ColorRanges &ranges, int &index) const;

private:
    const QString m_dspSettingFilePath;

    MapOfColorRanges m_mapOfColorRanges;

private:
    const QString m_globalJsonTagName="gradients";
    const QString m_nameJsonTagName="name";
    const QString m_rangesJsonTagName="ranges";
    const QString m_rangeTagName="range";
    const QString m_rangeColorTagName="rgb";
    const QString m_xTagName="x";
    const QString m_yTagName="y";
};

#endif // PRESENTER_COLORCHEMEPRESENTER_H

#include "presenter/colorscheme/colorschemepresenter.h"
#include <QDebug>

ColorSchemePresenter::ColorSchemePresenter(QObject *parent)
    : QObject(parent)
    , m_dspSettingFilePath(QApplication::applicationDirPath() +  "/setting.json")
{
    GetPresetFromFile();
}

ColorSchemePresenter::~ColorSchemePresenter()
{
    SavePresetToFile();
}

const QList<QString> ColorSchemePresenter::GetAllGradientsNames()
{
    return m_mapOfColorRanges.keys();
}

const QVector<QColor> ColorSchemePresenter::GetColorVector(const QString &gradientName)
{
    if (m_mapOfColorRanges.contains(gradientName))
    {
        return m_mapOfColorRanges[gradientName].colorsForGradientLabel;
    }
    else
    {
        qFatal("ColorChemePresenter::GetColorVector Ключа нет");
    }
}

const QVector<int> ColorSchemePresenter::GetRanges(const QString &gradientName)
{
    if (m_mapOfColorRanges.contains(gradientName))
    {
        return m_mapOfColorRanges[gradientName].rangesForRangeSlider;
    }
    else
    {
        qFatal("ColorChemePresenter::GetRanges Ключа нет");
    }
}

const QVector<QPoint> ColorSchemePresenter::GetPoints(const QString &gradientName)
{
    if (m_mapOfColorRanges.contains(gradientName))
    {
        return m_mapOfColorRanges[gradientName].pointsForCirclePalette;
    }
    else
    {
        qFatal("ColorChemePresenter::GetPoints Ключа нет");
    }
}

const ColorRanges &ColorSchemePresenter::GetColorRanges(const QString &gradientName)
{
    if (m_mapOfColorRanges.contains(gradientName))
    {
        return m_mapOfColorRanges[gradientName];
    }
    else
    {
        qFatal("ColorChemePresenter::GetPoints Ключа нет");
    }
}

void ColorSchemePresenter::DeleteGradientByName(const QString &gradientName)
{
    if (m_mapOfColorRanges.contains(gradientName))
    {
        m_mapOfColorRanges.remove(gradientName);
    }
    else
    {
        qFatal("ColorChemePresenter::DeleteSpectorByName Ключа нет");
    }
}

void ColorSchemePresenter::SaveGradient(const QString &gradientName, const QVector<int> &ranges, const QVector<QColor> &colors)
{
    m_mapOfColorRanges[gradientName].rangesForRangeSlider=ranges;
    m_mapOfColorRanges[gradientName].colorsForGradientLabel=colors;
}

void ColorSchemePresenter::OnSchemeEdit(const QString &gradientName, ColorRanges &range)
{
    m_mapOfColorRanges[gradientName]=range;
}

void ColorSchemePresenter::GetPresetFromFile()
{
    QFile settingFile(m_dspSettingFilePath);
    if (settingFile.exists())
    {
        if (!settingFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(nullptr, "ColorChemePresenter::GetPresetFromFile - Не можем открыть файл", settingFile.errorString());
            return;
        }

        QByteArray saveData = settingFile.readAll();
        settingFile.close();
        QJsonDocument settingDoc(QJsonDocument::fromJson(saveData));

        const QJsonObject &json = settingDoc.object();
        if (json.contains(m_globalJsonTagName) && json[m_globalJsonTagName].isArray())
        {
            QStringList gradientsNameList;
            ParseJsonDocument(json, gradientsNameList);
        }
        else
        {
            QMessageBox::critical(nullptr, "ColorChemePresenter::GetPresetFromFile" , "Тэг неверный, должен быть " + m_globalJsonTagName + " или это не массив должны быть тэг:[]");
        }
    }
    else
    {
        SetDefaultColors();
    }
}

void ColorSchemePresenter::SetDefaultColors()
{
    ColorRanges colorRanges;
    colorRanges.rangesForRangeSlider={0, 67, 100};
    colorRanges.colorsForGradientLabel={Qt::green, Qt::blue, Qt::red};
    m_mapOfColorRanges.insert("Базовый цвет", colorRanges);
}

void ColorSchemePresenter::ParseJsonDocument(const QJsonObject &array, QStringList &namesList)
{
    const QJsonArray &gradientsArray = array[m_globalJsonTagName].toArray();
    for (QJsonValue gradient : gradientsArray)
    {
        const QJsonObject &gradientObject = gradient.toObject();
        NameAttributeAppedToList(namesList, gradientObject);
        ColorRanges colorRanges;
        if (gradientObject.contains(m_rangesJsonTagName) && gradientObject[m_rangesJsonTagName].isArray())
        {
            const QJsonArray &rangesArray = gradientObject[m_rangesJsonTagName].toArray();
            for (QJsonValue rangeValue : rangesArray)
            {
                const QJsonObject &rangeObject = rangeValue.toObject();
                RangeAttributeAppedToColorRanges(colorRanges, rangeObject);
                ColorAttributeAppedToColorRanges(colorRanges, rangeObject);
                PointAttributeAppedToColorRanges(colorRanges, rangeObject);
            }
        }
        m_mapOfColorRanges.insert(namesList.last(), colorRanges);
    }
}

void ColorSchemePresenter::NameAttributeAppedToList(QStringList &namesList, const QJsonObject &object)
{

    if (object.contains(m_nameJsonTagName) && object[m_nameJsonTagName].isString()) {
        namesList.push_back(object[m_nameJsonTagName].toString());
    }
}

void ColorSchemePresenter::RangeAttributeAppedToColorRanges(ColorRanges &ranges, const QJsonObject &object)
{
    if (object.contains(m_rangeTagName) && object[m_rangeTagName].isDouble()) {
        ranges.rangesForRangeSlider.push_back(object[m_rangeTagName].toInt());
    }
}

void ColorSchemePresenter::ColorAttributeAppedToColorRanges(ColorRanges &ranges, const QJsonObject &object)
{
    QColor col(object[m_rangeColorTagName].toString());
    qDebug()<<col;
    if (object.contains(m_rangeColorTagName) && object[m_rangeColorTagName].isString()) {
        ranges.colorsForGradientLabel.push_back(QColor(object[m_rangeColorTagName].toString()));
    }
}

void ColorSchemePresenter::PointAttributeAppedToColorRanges(ColorRanges &ranges, const QJsonObject &object)
{
    QPoint xy;
    if (object.contains(m_xTagName) && object[m_xTagName].isDouble()) {
        xy.setX(object[m_xTagName].toInt());
    }
    if (object.contains(m_yTagName) && object[m_yTagName].isDouble()) {
        xy.setY(object[m_yTagName].toInt());
    }
    ranges.pointsForCirclePalette.push_back(xy);
}

void ColorSchemePresenter::SavePresetToFile()
{
    QFile settingFile(m_dspSettingFilePath);
    if (!settingFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "ColorChemePresenter::SavePresetToFile - Не можем открыть файл", settingFile.errorString());
        return;
    }
    else
    {
        QJsonDocument jsonDocument;
        QJsonObject globalJsonObject;
        QJsonArray gradientsArray;
        for (MapOfColorRanges::const_iterator gradient=m_mapOfColorRanges.cbegin(); gradient!=m_mapOfColorRanges.cend(); ++gradient)
        {
            gradientsArray.push_back(GetJsonObjectFromColorRange(gradient.key(), *gradient));
        }
        globalJsonObject[m_globalJsonTagName]=gradientsArray;
        jsonDocument.setObject(globalJsonObject);
        settingFile.write(jsonDocument.toJson());
        settingFile.flush();
        settingFile.close();
    }
}

QJsonObject ColorSchemePresenter::GetJsonObjectFromColorRange(const QString &colorName,const ColorRanges &range)
{
    QJsonObject gradientObject;
    gradientObject[m_nameJsonTagName]=colorName;
    QJsonArray rangesArray;
    if (range.rangesForRangeSlider.count()==range.colorsForGradientLabel.count())
    {
        for (int i=0; i<range.rangesForRangeSlider.count(); i++)
        {
            rangesArray.append(GetJsonObjectFromRangeData(range, i));
        }
        gradientObject[m_rangesJsonTagName]=rangesArray;
        return gradientObject;
    }
    else
    {
        qFatal("Колличество точек ColorChemePresenter::SavePresetToFile не совпадает");
    }
}

QJsonObject ColorSchemePresenter::GetJsonObjectFromRangeData(const ColorRanges &ranges, int &index) const
{
    QJsonObject rangeObject;
    rangeObject[m_rangeTagName]=ranges.rangesForRangeSlider.at(index);
    rangeObject[m_rangeColorTagName]=ranges.colorsForGradientLabel.at(index).name();
    //    rangeObject[m_xTagName]=ranges.pointsForCirclePalette.at(index).x();
    //    rangeObject[m_yTagName]=ranges.pointsForCirclePalette.at(index).y();
    return rangeObject;
}


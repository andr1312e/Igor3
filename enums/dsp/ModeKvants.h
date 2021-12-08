#ifndef MODEKVANTS_H
#define MODEKVANTS_H
#include <QtGlobal>

enum TYPE_MODES // типы режимов // должны быть от 0 до Count_shift-1
{
    TYPE_MODE_QM        = 0, // БЗ
    TYPE_MODE_MM        = 1, // СЗ
    TYPE_MODE_RM        = 2, // РЗ
    TYPE_MODE_LFM       = 3, // ЛЧМ
    TYPE_MODE_RI_1      = 4, // РИ_1
    TYPE_MODE_RI_2      = 5 // РИ_2
};

static const int count_job_point = 30; // сколько свего рабочих точек // точки идут от 0-й по 29-ю

struct Mode
{
    uint8_t type_mode;
    double dist; // м
    double vr[count_job_point]; // м/с
    double blind_vr[count_job_point]; // м/с // 2.5 кванта (половина 5)
    double time_mode; // мсек
    Mode()
        : type_mode(0)
        , dist(0)
        , time_mode(0)
    {

    }
    Mode(uint8_t Type_mode, double Dist)
        : type_mode(Type_mode)
        , dist(Dist)
    {

    }
};

static const quint8 Count_all_modes = 63;
static const Mode MODE[Count_all_modes] = { // использую для создания массива режимов для разных рабочих точек (кванты, none, время импульса (мкс), количество импульсов)
    Mode(TYPE_MODE_QM, 131), // кванты
    Mode(TYPE_MODE_QM, 147),
    Mode(TYPE_MODE_QM, 163),
    Mode(TYPE_MODE_QM, 179),
    Mode(TYPE_MODE_QM, 195),
    Mode(TYPE_MODE_QM, 211),
    Mode(TYPE_MODE_QM, 227),
    Mode(TYPE_MODE_QM, 243),
    Mode(TYPE_MODE_QM, 195), // с 8 по 15 не действительны
    Mode(TYPE_MODE_QM, 203),
    Mode(TYPE_MODE_QM, 211),
    Mode(TYPE_MODE_QM, 219),
    Mode(TYPE_MODE_QM, 227),
    Mode(TYPE_MODE_QM, 235),
    Mode(TYPE_MODE_QM, 243),
    Mode(TYPE_MODE_QM, 251),

    Mode(TYPE_MODE_MM, 1310),
    Mode(TYPE_MODE_MM, 1390),
    Mode(TYPE_MODE_MM, 1470),
    Mode(TYPE_MODE_MM, 1550),
    Mode(TYPE_MODE_MM, 1630),
    Mode(TYPE_MODE_MM, 1710),
    Mode(TYPE_MODE_MM, 1790),
    Mode(TYPE_MODE_MM, 1870),
    Mode(TYPE_MODE_MM, 1950),
    Mode(TYPE_MODE_MM, 2030),
    Mode(TYPE_MODE_MM, 2110),
    Mode(TYPE_MODE_MM, 2190),
    Mode(TYPE_MODE_MM, 2270),
    Mode(TYPE_MODE_MM, 2350),
    Mode(TYPE_MODE_MM, 2430),
    Mode(TYPE_MODE_MM, 2510),

    Mode(TYPE_MODE_MM, 1310),
    Mode(TYPE_MODE_MM, 1390),
    Mode(TYPE_MODE_MM, 1470),
    Mode(TYPE_MODE_MM, 1550),
    Mode(TYPE_MODE_MM, 1630),
    Mode(TYPE_MODE_MM, 1710),
    Mode(TYPE_MODE_MM, 1790),
    Mode(TYPE_MODE_MM, 1870),
    Mode(TYPE_MODE_MM, 1950),
    Mode(TYPE_MODE_MM, 2030),
    Mode(TYPE_MODE_MM, 2110),
    Mode(TYPE_MODE_MM, 2190),
    Mode(TYPE_MODE_MM, 2270),
    Mode(TYPE_MODE_MM, 2350),
    Mode(TYPE_MODE_MM, 2430),
    Mode(TYPE_MODE_MM, 2510),

    Mode(TYPE_MODE_RM, 4848),
    Mode(TYPE_MODE_RM, 5252),
    Mode(TYPE_MODE_RM, 5656),

    Mode(TYPE_MODE_LFM, 4848),
    Mode(TYPE_MODE_LFM, 5352),
    Mode(TYPE_MODE_LFM, 5856),
    Mode(TYPE_MODE_LFM, 6360),

    Mode(TYPE_MODE_RI_1, 4848),
    Mode(TYPE_MODE_RI_1, 5352),
    Mode(TYPE_MODE_RI_1, 5856),
    Mode(TYPE_MODE_RI_1, 6360),

    Mode(TYPE_MODE_RI_2, 4848),
    Mode(TYPE_MODE_RI_2, 5352),
    Mode(TYPE_MODE_RI_2, 5856),
    Mode(TYPE_MODE_RI_2, 6360)
};
#endif // MODEKVANTS_H

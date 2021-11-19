#include "linearcolormapindexed.h"

LinearColorMapIndexed::LinearColorMapIndexed()
    : QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::Indexed )
{
    addColorStop( 0.1, Qt::red );
    addColorStop( 0.5, Qt::yellow );
    addColorStop( 0.0, Qt::darkBlue );
}

LinearColorMapIndexed::~LinearColorMapIndexed()
{

}

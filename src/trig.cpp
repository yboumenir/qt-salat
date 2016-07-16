#include "trig.hpp"
#include "math.h"

TrigHelper::TrigHelper()
{

}

double TrigHelper::dsin(double d)
{
    return sin(deg2rad(d));
}

double TrigHelper::dcos(double d)
{
    return cos(deg2rad(d));
}

double TrigHelper::dtan(double d)
{
    return tan(deg2rad(d));
}

double TrigHelper::darcsin(double x)
{
    return rad2deg(asin(x));
}

double TrigHelper::darccos(double x)
{
    return rad2deg(acos(x));
}

double TrigHelper::darctan(double x)
{
    return rad2deg(atan(x));
}

double TrigHelper::darctan2(double y, double x)
{
    return rad2deg(atan2(y, x));
}

double TrigHelper::darccot(double x)
{
    return rad2deg(atan(1.0 / x));
}

double TrigHelper::deg2rad(double d)
{
    return d * M_PI / 180.0;
}

double TrigHelper::rad2deg(double r)
{
    return r * 180.0 / M_PI;
}

double TrigHelper::fix_angle(double a)
{
    a = a - 360.0 * floor(a / 360.0);
    a = a < 0.0 ? a + 360.0 : a;
    return a;
}

double TrigHelper::fix_hour(double a)
{
    a = a - 24.0 * floor(a / 24.0);
    a = a < 0.0 ? a + 24.0 : a;
    return a;
}

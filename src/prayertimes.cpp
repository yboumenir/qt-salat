/*-------------------------- In the name of God ----------------------------*\
 
    PrayerTimes 0.3
    Islamic prayer times calculator
    Based on PrayTimes 1.1 JavaScript library

----------------------------- Copyright Block --------------------------------

Copyright (C) 2007-2010 PrayTimes.org

Developed By: Mohammad Ebrahim Mohammadi Panah <ebrahim at mohammadi dot ir>
Based on a JavaScript Code By: Hamid Zarrabi-Zadeh

License: GNU LGPL v3.0

TERMS OF USE:
    Permission is granted to use this code, with or
    without modification, in any website or application
    provided that credit is given to the original work
    with a link back to PrayTimes.org.

This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY.

PLEASE DO NOT REMOVE THIS COPYRIGHT BLOCK.

------------------------------------------------------------------------------

User's Manual:
http://praytimes.org/manual

Calculating Formulas:
http://praytimes.org/calculation

Code Repository:
http://code.ebrahim.ir/prayertimes/

\*--------------------------------------------------------------------------*/


#include <ctime>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

#include "prayertimes.hpp"
#include "trig.hpp"

PrayerTimes::PrayerTimes(Parameters::CalculationMethod calc_method, Parameters::JuristicMethod asr_juristic, Parameters::AdjustingMethod adjust_high_lats, double dhuhr_minutes)
    : calc_method(calc_method)
    , asr_juristic(asr_juristic)
    , adjust_high_lats(adjust_high_lats)
    , dhuhr_minutes(dhuhr_minutes)
{
    method_params[Parameters::Jafari]  = Parameters::MethodConfig(16.0, false, 4.0, false, 14.0);	// Jafari
    method_params[Parameters::Karachi] = Parameters::MethodConfig(18.0, true,  0.0, false, 18.0);	// Karachi
    method_params[Parameters::ISNA]    = Parameters::MethodConfig(15.0, true,  0.0, false, 15.0);	// ISNA
    method_params[Parameters::MWL]     = Parameters::MethodConfig(18.0, true,  0.0, false, 17.0);	// MWL
    method_params[Parameters::Makkah]  = Parameters::MethodConfig(19.0, true,  0.0, true,  90.0);	// Makkah
    method_params[Parameters::Egypt]   = Parameters::MethodConfig(19.5, true,  0.0, false, 17.5);	// Egypt
    method_params[Parameters::Custom]  = Parameters::MethodConfig(18.0, true,  0.0, false, 17.0);	// Custom
}

PrayerTimes::~PrayerTimes()
{

}

void PrayerTimes::get_prayer_times(int year, int month, int day, double _latitude, double _longitude, double _timezone, double times[])
{
    latitude = _latitude;
    longitude = _longitude;
    timezone = _timezone;
    julian_date = get_julian_date(year, month, day) - longitude / (double) (15 * 24);
    compute_day_times(times);
}

void PrayerTimes::get_prayer_times(time_t date, double latitude, double longitude, double timezone, double times[])
{
    tm* t = localtime(&date);
    get_prayer_times(1900 + t->tm_year, t->tm_mon + 1, t->tm_mday, latitude, longitude, timezone, times);
}

void PrayerTimes::set_calc_method(Parameters::CalculationMethod method_id)
{
    calc_method = method_id;
}

void PrayerTimes::set_asr_method(Parameters::JuristicMethod method_id)
{
    asr_juristic = method_id;
}

void PrayerTimes::set_high_lats_adjust_method(Parameters::AdjustingMethod method_id)
{
    adjust_high_lats = method_id;
}

void PrayerTimes::set_fajr_angle(double angle)
{
    method_params[Parameters::Custom].fajr_angle = angle;
    calc_method = Parameters::Custom;
}

void PrayerTimes::set_maghrib_angle(double angle)
{
    method_params[Parameters::Custom].maghrib_is_minutes = false;
    method_params[Parameters::Custom].maghrib_value = angle;
    calc_method = Parameters::Custom;
}

void PrayerTimes::set_isha_angle(double angle)
{
    method_params[Parameters::Custom].isha_is_minutes = false;
    method_params[Parameters::Custom].isha_value = angle;
    calc_method = Parameters::Custom;
}

void PrayerTimes::set_dhuhr_minutes(double minutes)
{
    dhuhr_minutes = minutes;
}

void PrayerTimes::set_maghrib_minutes(double minutes)
{
    method_params[Parameters::Custom].maghrib_is_minutes = true;
    method_params[Parameters::Custom].maghrib_value = minutes;
    calc_method = Parameters::Custom;
}

void PrayerTimes::set_isha_minutes(double minutes)
{
    method_params[Parameters::Custom].isha_is_minutes = true;
    method_params[Parameters::Custom].isha_value = minutes;
    calc_method = Parameters::Custom;
}

void PrayerTimes::get_float_time_parts(double time, int &hours, int &minutes)
{
    time = TrigHelper::fix_hour(time + 0.5 / 60);		// add 0.5 minutes to round
    hours = floor(time);
    minutes = floor((time - hours) * 60);
}

std::string PrayerTimes::float_time_to_time24(double time)
{
    if (std::isnan(time))
        return std::string();
    int hours, minutes;
    get_float_time_parts(time, hours, minutes);
    return two_digits_format(hours) + ':' + two_digits_format(minutes);
}

std::string PrayerTimes::float_time_to_time12(double time, bool no_suffix)
{
    if (std::isnan(time))
        return std::string();
    int hours, minutes;
    get_float_time_parts(time, hours, minutes);
    const char* suffix = hours >= 12 ? " PM" : " AM";
    hours = (hours + 12 - 1) % 12 + 1;
    return int_to_string(hours) + ':' + two_digits_format(minutes) + (no_suffix ? "" : suffix);
}

std::string PrayerTimes::float_time_to_time12ns(double time)
{
    return float_time_to_time12(time, true);
}

double PrayerTimes::get_effective_timezone(time_t local_time)
{
    tm* tmp = localtime(&local_time);
    tmp->tm_isdst = 0;
    time_t local = mktime(tmp);
    tmp = gmtime(&local_time);
    tmp->tm_isdst = 0;
    time_t gmt = mktime(tmp);
    return (local - gmt) / 3600.0;
}

double PrayerTimes::get_effective_timezone(int year, int month, int day)
{
    tm date = { 0 };
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    date.tm_isdst = -1;		// determine it yourself from system
    time_t local = mktime(&date);		// seconds since midnight Jan 1, 1970
    return get_effective_timezone(local);
}

PrayerTimes::DoublePair PrayerTimes::sun_position(double jd)
{
    double d = jd - 2451545.0;
    double g = TrigHelper::fix_angle(357.529 + 0.98560028 * d);
    double q = TrigHelper::fix_angle(280.459 + 0.98564736 * d);
    double l = TrigHelper::fix_angle(q + 1.915 * TrigHelper::dsin(g) + 0.020 * TrigHelper::dsin(2 * g));

    // double r = 1.00014 - 0.01671 * dcos(g) - 0.00014 * dcos(2 * g);
    double e = 23.439 - 0.00000036 * d;

    double dd = TrigHelper::darcsin(TrigHelper::dsin(e) * TrigHelper::dsin(l));
    double ra = TrigHelper::darctan2(TrigHelper::dcos(e) * TrigHelper::dsin(l), TrigHelper::dcos(l)) / 15.0;
    ra = TrigHelper::fix_hour(ra);
    double eq_t = q / 15.0 - ra;

    return DoublePair(dd, eq_t);
}

double PrayerTimes::equation_of_time(double jd)
{
    return sun_position(jd).second;
}

double PrayerTimes::sun_declination(double jd)
{
    return sun_position(jd).first;
}

double PrayerTimes::compute_mid_day(double _t)
{
    double t = equation_of_time(julian_date + _t);
    double z = TrigHelper::fix_hour(12 - t);
    return z;
}

double PrayerTimes::compute_time(double g, double t)
{
    double d = sun_declination(julian_date + t);
    double z = compute_mid_day(t);
    double v = 1.0 / 15.0 * TrigHelper::darccos((-TrigHelper::dsin(g) - TrigHelper::dsin(d) * TrigHelper::dsin(latitude)) / (TrigHelper::dcos(d) * TrigHelper::dcos(latitude)));
    return z + (g > 90.0 ? - v :  v);
}

double PrayerTimes::compute_asr(int step, double t)  // Shafii: step=1, Hanafi: step=2
{
    double d = sun_declination(julian_date + t);
    double g = -TrigHelper::darccot(step + TrigHelper::dtan(fabs(latitude - d)));
    return compute_time(g, t);
}

void PrayerTimes::compute_times(double times[])
{
    day_portion(times);

    times[Parameters::Fajr]    = compute_time(180.0 - method_params[calc_method].fajr_angle, times[Parameters::Fajr]);
    times[Parameters::Sunrise] = compute_time(180.0 - 0.833, times[Parameters::Sunrise]);
    times[Parameters::Dhuhr]   = compute_mid_day(times[Parameters::Dhuhr]);
    times[Parameters::Asr]     = compute_asr(1 + asr_juristic, times[Parameters::Asr]);
    times[Parameters::Sunset]  = compute_time(0.833, times[Parameters::Sunset]);
    times[Parameters::Maghrib] = compute_time(method_params[calc_method].maghrib_value, times[Parameters::Maghrib]);
    times[Parameters::Isha]    = compute_time(method_params[calc_method].isha_value, times[Parameters::Isha]);
}

void PrayerTimes::compute_day_times(double times[])
{
    double default_times[] = { 5, 6, 12, 13, 18, 18, 18 };		// default times
    for (int i = 0; i < Parameters::TimesCount; ++i)
        times[i] = default_times[i];

    for (int i = 0; i < NUM_ITERATIONS; ++i)
        compute_times(times);

    adjust_times(times);
}

void PrayerTimes::adjust_times(double times[])
{
    for (int i = 0; i < Parameters::TimesCount; ++i)
        times[i] += timezone - longitude / 15.0;
    times[Parameters::Dhuhr] += dhuhr_minutes / 60.0;		// Dhuhr
    if (method_params[calc_method].maghrib_is_minutes)		// Maghrib
        times[Parameters::Maghrib] = times[Parameters::Sunset] + method_params[calc_method].maghrib_value / 60.0;
    if (method_params[calc_method].isha_is_minutes)		// Isha
        times[Parameters::Isha] = times[Parameters::Maghrib] + method_params[calc_method].isha_value / 60.0;

    if (adjust_high_lats != Parameters::None)
        adjust_high_lat_times(times);
}

void PrayerTimes::adjust_high_lat_times(double times[])
{
    double night_time = time_diff(times[Parameters::Sunset], times[Parameters::Sunrise]);		// sunset to sunrise

    // Adjust Fajr
    double fajr_diff = night_portion(method_params[calc_method].fajr_angle) * night_time;
    if (std::isnan(times[Parameters::Fajr]) || time_diff(times[Parameters::Fajr], times[Parameters::Sunrise]) > fajr_diff)
        times[Parameters::Fajr] = times[Parameters::Sunrise] - fajr_diff;

    // Adjust Isha
    double isha_angle = method_params[calc_method].isha_is_minutes ? 18.0 : method_params[calc_method].isha_value;
    double isha_diff = night_portion(isha_angle) * night_time;
    if (std::isnan(times[Parameters::Isha]) || time_diff(times[Parameters::Sunset], times[Parameters::Isha]) > isha_diff)
        times[Parameters::Isha] = times[Parameters::Sunset] + isha_diff;

    // Adjust Maghrib
    double maghrib_angle = method_params[calc_method].maghrib_is_minutes ? 4.0 : method_params[calc_method].maghrib_value;
    double maghrib_diff = night_portion(maghrib_angle) * night_time;
    if (std::isnan(times[Parameters::Maghrib]) || time_diff(times[Parameters::Sunset], times[Parameters::Maghrib]) > maghrib_diff)
        times[Parameters::Maghrib] = times[Parameters::Sunset] + maghrib_diff;
}

double PrayerTimes::night_portion(double angle)
{
    switch (adjust_high_lats)
    {
    case Parameters::AngleBased:
        return angle / 60.0;
    case Parameters::MidNight:
        return 1.0 / 2.0;
    case Parameters::OneSeventh:
        return 1.0 / 7.0;
    default:
        // Just to return something!
        // In original library nothing was returned
        // Maybe I should throw an exception
        // It must be impossible to reach here
        return 0;
    }
}

void PrayerTimes::day_portion(double times[])
{
    for (int i = 0; i < Parameters::TimesCount; ++i)
        times[i] /= 24.0;
}

double PrayerTimes::time_diff(double time1, double time2)
{
    return TrigHelper::fix_hour(time2 - time1);
}

std::string PrayerTimes::int_to_string(int num)
{
    char tmp[16];
    tmp[0] = '\0';
    sprintf(tmp, "%d", num);
    return std::string(tmp);
}

std::string PrayerTimes::two_digits_format(int num)
{
    char tmp[16];
    tmp[0] = '\0';
    sprintf(tmp, "%2.2d", num);
    return std::string(tmp);
}

double PrayerTimes::get_julian_date(int year, int month, int day)
{
    if (month <= 2)
    {
        year -= 1;
        month += 12;
    }

    double a = floor(year / 100.0);
    double b = 2 - a + floor(a / 4.0);

    return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + b - 1524.5;
}

double PrayerTimes::calc_julian_date(int year, int month, int day)
{
    double j1970 = 2440588.0;
    tm date = { 0 };
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    date.tm_isdst = -1;		// determine it yourself from system
    time_t ms = mktime(&date);		// seconds since midnight Jan 1, 1970
    double days = floor(ms / (double) (60 * 60 * 24));
    return j1970 + days - 0.5;
}

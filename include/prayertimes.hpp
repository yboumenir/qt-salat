/*-------------------------- In the name of God ----------------------------*\

    libprayertimes 1.0
    Islamic prayer times calculator library
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

#ifndef PRAYERTIMES_H
#define PRAYERTIMES_H

#include <cstdio>
#include <cmath>
#include <string>
#include <QObject>
/* -------------------- PrayerTimes Class --------------------- */

struct Parameters{

    enum
    {
        VERSION_MAJOR = 1,
        VERSION_MINOR = 0,
    };

    // Calculation Methods
    enum CalculationMethod
    {
        Jafari, 	// Ithna Ashari
        Karachi,	// University of Islamic Sciences, Karachi
        ISNA,   	// Islamic Society of North America (ISNA)
        MWL,    	// Muslim World League (MWL)
        Makkah, 	// Umm al-Qura, Makkah
        Egypt,  	// Egyptian General Authority of Survey
        Custom, 	// Custom Setting

        CalculationMethodsCount
    };

    // Juristic Methods
    enum JuristicMethod
    {
        Shafii,    // Shafii (standard)
        Hanafi,    // Hanafi
    };

    // Adjusting Methods for Higher Latitudes
    enum AdjustingMethod
    {
        None,      	// No adjustment
        MidNight,  	// middle of night
        OneSeventh,	// 1/7th of night
        AngleBased,	// angle/60th of night
    };

    // Time IDs
    enum TimeID
    {
        Fajr,
        Sunrise,
        Dhuhr,
        Asr,
        Sunset,
        Maghrib,
        Isha,

        TimesCount
    };

//private:
    struct MethodConfig
    {
        MethodConfig()
        {
        }

        MethodConfig(double fajr_angle,
                     bool maghrib_is_minutes,
                     double maghrib_value,
                     bool isha_is_minutes,
                     double isha_value)
            : fajr_angle(fajr_angle)
            , maghrib_is_minutes(maghrib_is_minutes)
            , maghrib_value(maghrib_value)
            , isha_is_minutes(isha_is_minutes)
            , isha_value(isha_value)
        {
        }

        double fajr_angle;
        bool   maghrib_is_minutes;
        double maghrib_value;		// angle or minutes
        bool   isha_is_minutes;
        double isha_value;		// angle or minutes
    };



};

class PrayerTimes : public QObject
{
    Q_OBJECT
public:
    /* --------------------- User Interface ----------------------- */
    /*
    PrayerTimes(CalculationMethod calc_method = Jafari,
            JuristicMethod asr_juristic = Shafii,
            AdjustingMethod adjust_high_lats = MidNight,
            double dhuhr_minutes = 0)

    get_prayer_times(date, latitude, longitude, timezone, &times)
    get_prayer_times(year, month, day, latitude, longitude, timezone, &times)

    set_calc_method(method_id)
    set_asr_method(method_id)
    set_high_lats_adjust_method(method_id)		// adjust method for higher latitudes

    set_fajr_angle(angle)
    set_maghrib_angle(angle)
    set_isha_angle(angle)
    set_dhuhr_minutes(minutes)		// minutes after mid-day
    set_maghrib_minutes(minutes)		// minutes after sunset
    set_isha_minutes(minutes)		// minutes after maghrib

    get_float_time_parts(time, &hours, &minutes)
    float_time_to_time24(time)
    float_time_to_time12(time)
    float_time_to_time12ns(time)
*/

    /* -------------------- Interface Functions -------------------- */

    PrayerTimes(Parameters::CalculationMethod calc_method = Parameters::CalculationMethod::Jafari,
                     Parameters::JuristicMethod asr_juristic = Parameters::JuristicMethod::Shafii,
                     Parameters::AdjustingMethod adjust_high_lats = Parameters::AdjustingMethod::MidNight,
                     double dhuhr_minutes = 0);
    ~PrayerTimes();

    /* return prayer times for a given date */
    void get_prayer_times(int year, int month, int day, double _latitude, double _longitude, double _timezone, double times[]);

    /* return prayer times for a given date */
    void get_prayer_times(time_t date, double latitude, double longitude, double timezone, double times[]);

    /* set the calculation method  */
    void set_calc_method(Parameters::CalculationMethod method_id);

    /* set the juristic method for Asr */
    void set_asr_method(Parameters::JuristicMethod method_id);

    /* set adjusting method for higher latitudes */
    void set_high_lats_adjust_method(Parameters::AdjustingMethod method_id);

    /* set the angle for calculating Fajr */
    void set_fajr_angle(double angle);

    /* set the angle for calculating Maghrib */
    void set_maghrib_angle(double angle);

    /* set the angle for calculating Isha */
    void set_isha_angle(double angle);

    /* set the minutes after mid-day for calculating Dhuhr */
    void set_dhuhr_minutes(double minutes);

    /* set the minutes after Sunset for calculating Maghrib */
    void set_maghrib_minutes(double minutes);

    /* set the minutes after Maghrib for calculating Isha */
    void set_isha_minutes(double minutes);

    /* get hours and minutes parts of a float time */
    static void get_float_time_parts(double time, int& hours, int& minutes);

    /* convert float hours to 24h format */
    static std::string float_time_to_time24(double time);

    /* convert float hours to 12h format */
    static std::string float_time_to_time12(double time, bool no_suffix = false);

    /* convert float hours to 12h format with no suffix */
    static std::string float_time_to_time12ns(double time);

    /* ---------------------- Time-Zone Functions ----------------------- */

    /* compute local time-zone for a specific date */
    static double get_effective_timezone(time_t local_time);

    /* compute local time-zone for a specific date */
    static double get_effective_timezone(int year, int month, int day);

    /* ---------------------- Calculation Functions ----------------------- */

    /* References: */
    /* http://www.ummah.net/astronomy/saltime   */
    /* http://aa.usno.navy.mil/faq/docs/SunApprox.html */

    typedef std::pair<double, double> DoublePair;

    /* compute declination angle of sun and equation of time */
    DoublePair sun_position(double jd);

    /* compute equation of time */
    double equation_of_time(double jd);

    /* compute declination angle of sun */
    double sun_declination(double jd);

    /* compute mid-day (Dhuhr, Zawal) time */
    double compute_mid_day(double _t);

    /* compute time for a given angle G */
    double compute_time(double g, double t);

    /* compute the time of Asr */
    double compute_asr(int step, double t);

    /* ---------------------- Compute Prayer Times ----------------------- */

    // array parameters must be at least of size TimesCount

    /* compute prayer times at given julian date */
    void compute_times(double times[]);


    /* compute prayer times at given julian date */
    void compute_day_times(double times[]);


    /* adjust times in a prayer time array */
    void adjust_times(double times[]);

    /* adjust Fajr, Isha and Maghrib for locations in higher latitudes */
    void adjust_high_lat_times(double times[]);


    /* the night portion used for adjusting times in higher latitudes */
    double night_portion(double angle);

    /* convert hours to day portions  */
    void day_portion(double times[]);

    /* ---------------------- Misc Functions ----------------------- */

    /* compute the difference between two times  */
    static double time_diff(double time1, double time2);

    static std::string int_to_string(int num);

    /* add a leading 0 if necessary */
    static std::string two_digits_format(int num);

    /* ---------------------- Julian Date Functions ----------------------- */

    /* calculate julian date from a calendar date */
    double get_julian_date(int year, int month, int day);

    /* convert a calendar date to julian date (second method) */
    double calc_julian_date(int year, int month, int day);


private:
    /* ---------------------- Private Variables -------------------- */
    Parameters m_params;

    Parameters::MethodConfig method_params[Parameters::CalculationMethodsCount];

    Parameters::CalculationMethod calc_method;		// caculation method
    Parameters::JuristicMethod asr_juristic;		// Juristic method for Asr
    Parameters::AdjustingMethod adjust_high_lats;	// adjusting method for higher latitudes
    double dhuhr_minutes;		// minutes after mid-day for Dhuhr

    double latitude;
    double longitude;
    double timezone;
    double julian_date;

    /* --------------------- Technical Settings -------------------- */

    static const int NUM_ITERATIONS = 1;		// number of iterations needed to compute times
};

#endif

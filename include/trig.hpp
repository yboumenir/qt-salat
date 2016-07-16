#ifndef TRIGHELPER_H
#define TRIGHELPER_H


class TrigHelper{

public:
    TrigHelper();
    /* ---------------------- Trigonometric Functions ----------------------- */

    /* degree sin */
    static double dsin(double d);

    /* degree cos */
    static double dcos(double d);

    /* degree tan */
    static double dtan(double d);

    /* degree arcsin */
    static double darcsin(double x);

    /* degree arccos */
    static double darccos(double x);

    /* degree arctan */
    static double darctan(double x);

    /* degree arctan2 */
    static double darctan2(double y, double x);

    /* degree arccot */
    static double darccot(double x);

    /* degree to radian */
    static double deg2rad(double d);

    /* radian to degree */
    static double rad2deg(double r);

    /* range reduce angle in degrees. */
    static double fix_angle(double a);

    /* range reduce hours to 0..23 */
    static double fix_hour(double a);

};

#endif

#include "Troposphere.h"

#include <math.h>

#include "Constants.h"
#include "Common.h"

// ������ ������������ �������� GCAT (GPSCode Analysis Tool)
// H - ������ �������� ��� ������� ����, ��
// El - ���� ���������� �������� ��� ����������, ���
// ������������ �������� - ������������ ��������, �
double GCAT(double H, double El)
{
	double Zd, Zw;
	if(H < -5.0 || H > ht)
	{
		return 0.0;
	}
	Zd = 2.3 * exp(-0.116E-3 * H);
	Zw = 0.1;
	return (Zd + Zw) * 1.001 / sqrt(0.002001 + sqr(sin(El)));
}

// ������ ���������� ������������� ����������� ���������
// H - ������ ��� ������� ����, �
// *Weather - ��������� �� ��������� � ��������������� �����������:
// p - ����������� ��������, ����
// T - �����������, �
// e - ����������� �������� �������� ����, ����
void StandartAtmosphere(double H, struct Weather *Weather)
{
	if(H >= -5000.0 && H <= ht * 1.0E+3)
	{
		Weather->p = p0 * pow((1.0 - 2.2557E-5 * H), 5.2568);
		Weather->T = t0 + Beta * 1.0E-3 * H + 273.15;
		Weather->e = 6.108 * r0 / 100.0 * exp((17.15 * Weather->T - 4684.0) /
					 (Weather->T - 38.45));
	}
}

// ������ ������������ �������� ������������� (�� ������ ������ 1972 � 1973 ����)
// H - ������ �������� ��� ������� ����, ��
// El - ���� ���������� �������� ��� ����������, ���
// *Weather - ��������� �� ��������� � �����������:
// p - ����������� ��������, ����
// T - �����������, �
// e - ����������� �������� �������� ����, ����
// ������������ �������� - �������� ������������ ��������, �
double Saastamoinen(double H, double El, struct Weather *Weather)
{
	double H0[13] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.5,
	                 2.0, 2.5, 3.0, 4.0, 5.0, 6.0};
	double B0[13] = {1.16, 1.13, 1.10, 1.07, 1.04, 1.01, 0.94,
	                 0.88, 0.82, 0.76, 0.66, 0.57, 0.49};
	int i;
	double z, dz, B;
	if(H < -5.0 || H > ht)
	{
		return 0.0;
	}
	z = M_PI_2 - El;
	dz = 16.0 / 3600.0 * DegToRad * tan(z) / Weather->T *
		 (Weather->p + 4800.0 * Weather->e / Weather->T) -
		 0.07 / 3600.0 * DegToRad * (pow(tan(z), 3.0) + tan(z)) *
		 (Weather->p / 1000.0);
	z -= dz;
	if(z * RadToDeg > 80.0)
	{
		return 0.0;
	}
	B = Neville(B0, H0, H, 13);
	return 0.002277 / cos(z) * (Weather->p + (1255.0 / Weather->T + 0.05) *
	       Weather->e - B * sqr(tan(z)));
}

// ������ ������������ �������� ������ �� ������ ������ �������������
// � ��������� ����������� CfA-2.2
// B - �������������� ������ ��������� �����, �
// H - ������ ��������� ����� ��� ������� ����, ��
// El - ���� ���������� �������� ��� ����������, ���
// *Weather - ��������� �� ��������� � �����������:
// p - ����������� ��������, ����
// T - �����������, �
// e - ����������� �������� �������� ����, ����
// ������������ �������� - �������� ������������ ��������, �
double SaastamoinenDavis(double B, double H, double El, struct Weather *Weather)
{
	double Zd, Zw, a, b, c1, m;
    if(H < -5.0 || H > ht)
	{
		return 0.0;
	}
	Zd = 0.0022768 * Weather->p / (1.0 - 0.00266 * cos(2.0 * B * DegToRad) -
		 0.00028 * H);
	Zw = 0.0022768 * (1255.0 / Weather->T + 0.05) * Weather->e;
	a = 0.001185 * (1.0 + 0.6071E-4 * (Weather->p - 1000.0) - 0.1471E-3 *
		Weather->e + 0.3072E-2 * (Weather->t - 20.0) + 0.1965E-1 *
		(Beta + 6.5) - 0.5645E-2 * (ht - 11.231));
	b = 0.001144 * (1.0 + 0.1164E-4 * (Weather->p - 1000.0) + 0.2795E-3 *
		Weather->e + 0.3109E-2 * (Weather->t - 20.0) + 0.3038E-1 *
		(Beta + 6.5) - 0.1217E-1 * (ht - 11.231));
	c1 = -0.0090;
	m = 1.0 / (sin(El) + a / (tan(El) + b / (sin(El) + c1)));
	return (Zd + Zw) * m;
}

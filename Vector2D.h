//Copyright (C) 2011 Valdemart, 2012 Valdemart & TheMaxx (SFML, ostream, var. improv.)
#ifndef _INC_VECTOR2D_H
#define _INC_VECTOR2D_H
#pragma once
#include <cmath>


typedef class Vector2D Vc2D;

class Vector2D final
{
private:
	double junk;
public:
	double x,y;

	Vector2D(void){ junk = x = y = 0.0; }
	Vector2D(double _x, double _y){x=_x;y=_y; junk=0.0;}
	Vector2D(const Vector2D & v){x=v.x;y=v.y; junk=0.0;}
	~Vector2D(void){}

	double Length ()
	{
		return sqrt(x*x+y*y);
	}

	double GetXnY ()
	{
		return fabs(x) + fabs(y);
	}

	double GetDistance (Vector2D v){
		double tx=(x-v.x),ty=(y-v.y);
		return sqrt((tx*tx) + (ty*ty));
	}

	double GetDistance (double xx, double yy){
		double tx=(x-xx),ty=(y-yy);
		return sqrt((tx*tx) + (ty*ty));
	}

	Vector2D& Normalize ()
	{
		if(Length()==0.0)
			return *this;

		double length = Length ();
		x = x/length;
		y = y/length;
		return *this;
	}

	Vector2D& NormalizeMath ()
	{
		if(Length()==0.0)
			return *this;

		if(fabs(x)>fabs(y))
		{
			y /= fabs(x);
			x /= fabs(x);
		}
		else
		{
			x /= fabs(y);
			y /= fabs(y);
		}
		return *this;
	}

	double DotProduct (Vector2D v) //= |this||v|*cos(<(this,v))
	{
		return x*v.x+y*v.y;
	}

	double PerpDotProduct (Vector2D v) //= |this||v|*sin(<(this,v))
	{
		return x*v.y-v.x*y;
	}

	double Angle (Vector2D v)
	{
		if((Length ()==0.0)||(v.Length ()==0.0))
			return 0.0;

		return atan ((x*v.y-v.x*y)/(x*v.x+y*v.y));
	}

	Vector2D Add(double xx, double yy)
	{
		x+=xx;
		y+=yy;
		return *this;
	}

	Vector2D AddTemp(double xx, double yy)
	{
		return Vector2D (x+xx, y+yy);
	}

	Vector2D operator + (Vector2D v)
	{
		return Vector2D (x+v.x, y+v.y);
	}

	Vector2D& operator += (Vector2D v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2D operator - (Vector2D v)
	{
		return Vector2D (x-v.x, y-v.y);
	}

	Vector2D& operator -= (Vector2D v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2D operator - ()
	{
		return Vector2D (-x, -y);
	}

	Vector2D operator * (double k)
	{
		return Vector2D (k*x, k*y);
	}

	Vector2D operator * (Vector2D v)
	{
		return Vector2D (x*v.x, y*v.y);
	}

	Vector2D& operator *= (double k)
	{
		x *= k;
		y *= k;
		return *this;
	}

	Vector2D operator / (double k)
	{
		return Vector2D (x/k, y/k);
	}

	Vector2D& operator /= (double k)
	{
		x /= k;
		y /= k;
		return *this;
	}

	Vector2D& operator = (Vector2D v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	Vector2D& operator = (double k)
	{
		x = y = k;
		return *this;
	}

	bool operator == (Vector2D v)
	{
		if (((this->x)==v.x) && ((this->y)==v.y))
			return true;
		return false;
	}

	bool operator != (Vector2D v)
	{
		return !((*this)==v);
	}

	Vector2D Hadamard (Vector2D v) //iloczyn parami wspolrzednych
	{
		return Vector2D (x*v.x, y*v.y);
	}

	double& operator [] (const int ind)
	{
		return (ind==0?x:(ind==1?y:junk));
	}

#ifdef SFML_VECTOR2_HPP
	static sf::Vector2f ToSFML(const Vector2D v)
	{
		return sf::Vector2f(v.x,v.y);//(sfVector2f_trash = sf::Vector2f(v.x,v.y));
	}
	static sf::Vector2f ToSFML(const double vx, const double vy)
	{
		return sf::Vector2f(vx,vy);//(sfVector2f_trash = sf::Vector2f(v.x,v.y));
	}
	static Vector2D FromSFML(const sf::Vector2f v)
	{
		return Vector2D(v.x,v.y);
	}
#endif

};

#ifdef _GLIBCXX_OSTREAM
	static std::ostream& operator<< (std::ostream& str, const Vector2D v)
	{
		return (str << v.x << ":" << v.y);
	}
#endif

#endif
#ifndef _COMPLEX_
#define _COMPLEX_

//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	Complex.cpp
// Author(s)    |	Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	Complex
// Description:
//		A Class which stores real and imaginary float variables for a
//		complex number. Includes methods to Add, Multiply or compare 
//		complex numbers.
//

#include <cmath>

class Complex
{
public:
	Complex() { m_R = 0; m_I = 0; };
	Complex(float _r, float _i) { m_R = _r; m_I = _i; };

	~Complex() {};

	//Complex + Complex
	void Plus(Complex& other)
	{
		m_R += other.GetReal();
		m_I += other.GetImaginary();
	}

	//Complex * Complex
	void Multi(Complex& other)
	{
		//(Ar + Ai)(Br + Bi)
		float R = (m_R * other.GetReal()) - (m_I * other.GetImaginary());
		float I = (other.GetReal() * m_I) + (m_R * other.GetImaginary());
		m_R = R;
		m_I = I;
	}

	float Moduli()
	{
		return sqrtf(powf(m_R, 2) + powf(m_I, 2));
	}

	//Compares with an offset
	bool Compare(Complex& other, float accuracy)
	{
		//Calculate the Modulus:
		float ourM = this->Moduli();
		float otherM = other.Moduli();

		//Return true if the difference is less than the accuracy provided.
		return (abs(ourM - otherM) < accuracy);
	}

	float GetReal() { return m_R; };
	float GetImaginary() { return m_I; };

protected:
	
private:

	//Member Data:
public:

protected:

private:
	float m_R; //Real
	float m_I; //Imaginary
};

#endif //_COMPLEX_
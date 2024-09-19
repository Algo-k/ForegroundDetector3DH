
#pragma once

#include <math.h>
#include <string.h>
#include "FDColor.h"
#include <vector>

using namespace System::Runtime::InteropServices;

namespace CppLibImaging
{
	public ref class ForegroundDetector3DH
	{

	private:

		int _red_dim;
		int _green_dim;
		int _blue_dim;
		double _red_div;
		double _green_div;
		double _blue_div;
		double _red_mul;
		double _green_mul;
		double _blue_mul;
		double* _h3ddensity;
		double* _h3dinvert;
		double* _h3dblunted;
		double _p_norm = 2;
		double _dist_pow = 1;
		double _color_sharpness = 0.3;

	public:

		void Create(int red_dim, int green_dim, int blue_dim)
		{
			_red_dim = red_dim;
			_green_dim = green_dim;
			_blue_dim = blue_dim;
			_red_div = 256.0 / red_dim;
			_green_div = 256.0 / green_dim;
			_blue_div = 256.0 / blue_dim;
			_red_mul = 255.0 / (red_dim - 1);
			_green_mul = 255.0 / (green_dim - 1);
			_blue_mul = 255.0 / (blue_dim - 1);
			_h3ddensity = new double[_red_dim * _green_dim * _blue_dim];
			_h3dinvert = new double[_red_dim * _green_dim * _blue_dim];
			_h3dblunted = new double[_red_dim * _green_dim * _blue_dim];
			memset(_h3ddensity, 0, _red_dim * _green_dim * _blue_dim * sizeof(double));
		}

		void SetColorSharpness(double sharpness)
		{
			_color_sharpness = sharpness;
		}

		void SetPNorm(double p_norm)
		{
			_p_norm = p_norm;
		}

		void SetDistancePow(double power)
		{
			_dist_pow = power;
		}

		void SetLightEffect(double effect)
		{
			_light_effect = effect;
		}

		void AddColor(int red, int green, int blue)
		{
			int r = red / _red_div;
			int g = green / _green_div;
			int b = blue / _blue_div;
			_h3ddensity[r * (_green_dim * _blue_dim) + g * _blue_dim + b] += 1;
		}

		void AddColor(int alpha, int red, int green, int blue)
		{
			int r = red / _red_div;
			int g = green / _green_div;
			int b = blue / _blue_div;
			auto index = r * (_green_dim * _blue_dim) + g * _blue_dim + b;
			this->_h3ddensity[index] += alpha / 255.0;
		}

	private:

		void CalculateBlunted()
		{
			memset(_h3dblunted, 0, _red_dim * _green_dim * _blue_dim * sizeof(double));
			for (int i = 0; i < _red_dim * _green_dim * _blue_dim - 1; i++)
			{
				_h3dblunted[i] = pow(_h3ddensity[i], _color_sharpness);
			}
		}

		double CalculateDistance(int i, int j)
		{
			int ib = i % _blue_dim;
			i /= _blue_dim;
			int ig = i % _green_dim;
			i /= _green_dim;
			int ir = i;

			int jb = j % _blue_dim;
			j /= _blue_dim;
			int jg = j % _green_dim;
			j /= _green_dim;
			int jr = j;

			return CalculateDistance(ir, jr, ig, jg, ib, jb);
		}

		double _light_effect = 0.5;

		double CalculateDistance(int ir, int jr, int ig, int jg, int ib, int jb)
		{
			int r = jr - ir;
			int g = jg - ig;
			int b = jb - ib;
			int l = (jr + jg + jb) - (ir + ig + ib);
			int light_dim = _red_dim + _green_dim + _blue_dim;
			double d1 =
				(1 - _light_effect) * (
					pow(((double)abs(r)) / _red_dim, _p_norm) +
					pow(((double)abs(g)) / _green_dim, _p_norm) +
					pow(((double)abs(b)) / _blue_dim, _p_norm)
					) +
				_light_effect * pow(((double)abs(l)) / light_dim, _p_norm);
				d1 = pow(d1, 1.0 / _p_norm);
			return pow(d1, _dist_pow);
		}

		void CalculateInvert()
		{
			memset(_h3dinvert, 0, _red_dim * _green_dim * _blue_dim * sizeof(double));
			double c = 1.0 / (_red_dim * _green_dim * _blue_dim);
			for (int i = 0; i < _red_dim * _green_dim * _blue_dim - 1; i++)
			{
				for (int j = i + 1; j < _red_dim * _green_dim * _blue_dim; j++)
				{
					auto d = CalculateDistance(i, j);
					_h3dinvert[j] += _h3dblunted[i] * d * c;
					_h3dinvert[i] += _h3dblunted[j] * d * c;
				}
			}
		}

		void IndexToRGB(int index, int* r, int* g, int* b)
		{
			*b = index % _blue_dim;
			index /= _blue_dim;
			*g = index % _green_dim;
			index /= _green_dim;
			*r = index;
		}

		void FindMaximum(int* r, int* g, int* b)
		{
			double max = -1;
			int index = -1;
			for (int i = 0; i < _red_dim * _green_dim * _blue_dim; i++)
			{
				if (_h3dinvert[i] > max)
				{
					max = _h3dinvert[i];
					index = i;
				}
			}
			IndexToRGB(index, r, g, b);
		}

		int* FindMaximum(int number)
		{
			if (number > _red_dim * _green_dim * _blue_dim)
			{
				throw gcnew System::Exception("The provided number exceeds the maximum allowable limit.");
			}

			int* results = new int[number];
			char* selected = new char[_red_dim * _green_dim * _blue_dim];
			memset(selected, 0, _red_dim * _green_dim * _blue_dim);

			for (int itter = 0; itter < number; itter++)
			{
				double max = -1;
				int index = -1;
				for (int i = 0; i < _red_dim * _green_dim * _blue_dim; i++)
				{
					if (selected[i] == 0)
					{
						if (_h3dinvert[i] > max)
						{
							max = _h3dinvert[i];
							index = i;
						}
					}
				}
				selected[index] = 1;
				results[itter] = index;
			}
			delete selected;
			return results;
		}

		!ForegroundDetector3DH()
		{
			delete[] _h3ddensity;
			delete[] _h3dinvert;
		}

	public:

		void Detect([Out] unsigned char% r, [Out] unsigned char% g, [Out] unsigned char% b)
		{
			CalculateBlunted();
			CalculateInvert();
			int mr, mg, mb;
			FindMaximum(&mr, &mg, &mb);
			r = mr * _red_mul;
			g = mg * _green_mul;
			b = mb * _blue_mul;
		}		
		
		FDColor Detect()
		{
			CalculateBlunted();
			CalculateInvert();
			int mr, mg, mb;
			FindMaximum(&mr, &mg, &mb);
			FDColor color;
			color.R = mr * _red_mul;
			color.G = mg * _green_mul;
			color.B = mb * _blue_mul;
			return color;
		}

		array<FDColor>^ DetectMultiple(int number)
		{
			CalculateBlunted();
			CalculateInvert();
			auto results = FindMaximum(number);
			array<FDColor>^ colors = gcnew array<FDColor>(number);
			for (int i = 0; i < number; i++)
			{
				int r, g, b;
				IndexToRGB(results[i], &r, &g, &b);
				colors[i].R = r * _red_mul;
				colors[i].G = g * _green_mul;
				colors[i].B = b * _blue_mul;
			}
			delete[] results;
			return colors;
		}		
		
		FDColor DetectNth(int n)
		{
			CalculateBlunted();
			CalculateInvert();
			auto results = FindMaximum(n + 1);
			FDColor color;

			int r, g, b;
			IndexToRGB(results[n], &r, &g, &b);
			color.R = r * _red_mul;
			color.G = g * _green_mul;
			color.B = b * _blue_mul;

			delete[] results;
			return color;
		}

	};
}


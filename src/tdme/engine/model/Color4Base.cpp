#include <tdme/engine/model/Color4Base.h>

#include <algorithm>
#include <array>

#include <tdme/math/Math.h>
#include <tdme/math/MathTools.h>

using std::array;
using std::begin;
using std::copy;
using std::end;

using tdme::engine::model::Color4Base;
using tdme::math::Math;
using tdme::math::MathTools;

Color4Base::Color4Base() 
{
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 1.0f;
}

Color4Base::Color4Base(const Color4Base& color)
{
	data = color.data;
}

Color4Base::Color4Base(const array<float, 4>& color)
{
	this->data = color;
}

Color4Base::Color4Base(float r, float g, float b, float a) 
{
	data[0] = r;
	data[1] = g;
	data[2] = b;
	data[3] = a;
}

void Color4Base::set(const array<float, 4>& color)
{
	this->data = color;
}

void Color4Base::set(float r, float g, float b, float a)
{
	data[0] = r;
	data[1] = g;
	data[2] = b;
	data[3] = a;
}

void Color4Base::set(const Color4Base& color)
{
	data = color.data;
}

void Color4Base::add(float r, float g, float b, float a)
{
	data[0] += r;
	data[1] += g;
	data[2] += b;
	data[3] += a;
}

float Color4Base::getRed() const
{
	return data[0];
}

void Color4Base::setRed(float red)
{
	data[0] = red;
}

float Color4Base::getGreen() const
{
	return data[1];
}

void Color4Base::setGreen(float green)
{
	data[1] = green;
}

float Color4Base::getBlue() const
{
	return data[2];
}

void Color4Base::setBlue(float blue)
{
	data[2] = blue;
}

float Color4Base::getAlpha() const
{
	return data[3];
}

void Color4Base::setAlpha(float alpha)
{
	data[3] = alpha;
}

array<float, 4>& Color4Base::getArray() const
{
	return (array<float, 4>&)data;
}

bool Color4Base::equals(const Color4Base& c) const
{
	return (this == &c) || (
		Math::abs(data[0] - c.data[0]) < MathTools::EPSILON &&
		Math::abs(data[1] - c.data[1]) < MathTools::EPSILON &&
		Math::abs(data[2] - c.data[2]) < MathTools::EPSILON &&
		Math::abs(data[3] - c.data[3]) < MathTools::EPSILON
	);
}

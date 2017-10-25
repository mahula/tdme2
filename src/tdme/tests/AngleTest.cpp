#include <tdme/tests/AngleTest.h>

#include <string>

#include <tdme/math/Vector3.h>
#include <tdme/utils/Console.h>

using std::string;
using std::to_string;

using tdme::tests::AngleTest;
using tdme::math::Vector3;
using tdme::utils::Console;

void AngleTest::main()
{
	auto a = (Vector3(0.0f, 0.0f, 1.0f)).normalize();
	auto b = (Vector3(-1.0f, 0.0f, +1.0f)).normalize();
	auto n = Vector3(0.0f, 1.0f, 0.0f);
	Console::println(to_string(Vector3::computeAngle(a, b, n)));
}

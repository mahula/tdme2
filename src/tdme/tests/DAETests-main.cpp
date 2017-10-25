#include <iostream>
#include <string>

#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/Console.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/fileio/models/DAEReader.h>

using std::wcout;
using std::cout;
using std::endl;
using std::find;

using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::engine::fileio::models::DAEReader;
using tdme::engine::model::Model;
using tdme::engine::model::Group;
using tdme::engine::model::Animation;
using tdme::utils::Console;
using tdme::utils::Key;

void printMatrix(Matrix4x4& matrix) {
	for (auto value: matrix.getArray()) {
		cout << value;
		cout << " ";
	}
	cout << endl;
}

void printVector(Vector3& vector) {
	for (auto value: vector.getArray()) {
		cout << value;
		cout << " ";
	}
	cout << endl;
}

int main(int argc, char** argv)
{
    cout << "DAETest" << endl;
    Model* model = DAEReader::read("./resources/tests/models/test", "cube.dae");
    for (auto it: *model->getSubGroups()) {
    		Group* group = it.second;
    		cout << it.first << endl;
    		cout << "--" << endl;
    		for (auto& vector: *group->getVertices()) {
    			printVector(vector);
    		}
    		cout << "--" << endl;
    		printMatrix(group->getTransformationsMatrix());
    		cout << "--" << endl;
    		Animation* animation = group->getAnimation();
    		for (auto& matrix: *animation->getTransformationsMatrices()) {
    			printMatrix(matrix);
    		}
    }
}

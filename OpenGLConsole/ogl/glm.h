#pragma once

#include <gl/glm/glm.hpp>		//��� 3D ����������
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
//#include <GLM/gtc/type_ptr.hpp>

//���� ������
//	vec3 EulerAngles(RotationAroundXInRadians, RotationAroundYInRadians, RotationAroundZInRadians);

//�����������
//	x = RotationAxis.x * sin(RotationAngle / 2)
//	y = RotationAxis.y * sin(RotationAngle / 2)
//	z = RotationAxis.z * sin(RotationAngle / 2)
//	w = cos(RotationAngle / 2)
//		RotationAngle is in radians

//��� ������� ���������� �� C++ ?
//	#include <glm/gtc/quaternion.hpp> and <glm/gtx/quaternion.hpp>
//	Creates an identity quaternion (no rotation)
//		glm::quat MyQuaternion;
//	Direct specification of the 4 components
//		MyQuaternion = quat(w, x, y, z);
//	Conversion from Euler angles (in radians) to Quaternion
//		vec3 EulerAngles(90, 45, 0);
//		MyQuaternion = quat(EulerAngles);
//	Conversion from axis-angle
//	In GLM the angle must be in degrees here, so convert it.
//		MyQuaternion = gtx::quaternion::angleAxis(degrees(RotationAngle), RotationAxis);

//��� ������������� ���������� � �������?
//		mat4 RotationMatrix = quaternion::toMat4(quaternion);
//	������ �� ������ ������������ ��� ��� �������� ����� ������ ��� ������ :
//		mat4 RotationMatrix = quaternion::toMat4(quaternion);
//	...
//		mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;
// You can now use ModelMatrix to build the MVP matrix

//������ � ����, ��� ��� ����������� ������ ?
//	��� ������������� ������� �������� ������� ���� ������� ���� ����� ����� ���������.���� ��� �������� ����� 1, �� ������� ��������� � ����� �����������.
//	� ������������� ��� ����� �� �� ����� :
//		float matching = quaternion::dot(q1, q2);
//		if (abs(matching - 1.0) < 0.001) {
//			// q1 and q2 are similar
//		}
//	�� ����� ������ �������� ���� ����� q1 � q2, ���� acos() ����� ��������� ��������.

//��� ��������� ������� � �����?
//	�� ������ ������� ��������� :
//		rotated_point = orientation_quaternion *  point;
//	... �� ���� �� ������ ��������� ���� ��������� �������, �� ������, ������ �����, ������������� �� � �������.
//	�������� ��������, ��� ����� �������� ������ �������� �������.���� �� ������ ��������� ������ ������ ����� :
//		rotated_point = origin + (orientation_quaternion * (point - origin));

//��� ��������������� ����� ����� ������������� ?
//	��� ���������� SLERP : Spherical Linear intERPolation.� GLM �� ������ ������� ��� � ������� mix :
//		glm::quat interpolatedquat = quaternion::mix(quat1, quat2, 0.5f); // or whatever factor

//��� ����������� 2 ������� ?
//	������ !������ �������� ��� ����������� ������. ������� ����� ��, ��� ��� ������, �.�.�������� :
// 		quat combined_rotation = second_rotation * first_rotation;

//��� ����� ���������� gluLookAt. ��� � ���������� ������ �� �����?
//	����������� RotationBetweenVectors!
//	Find the rotation between the front of the object (that we assume towards +Z,
//	but this depends on your model) and the desired direction
//		quat rot1 = RotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);
//	������ �� ����� ������ ��������� ���� ������ ������ ����������� :
//	Recompute desiredUp so that it's perpendicular to the direction
//	You can skip that part if you really want to force desiredUp
//		vec3 right = cross(direction, desiredUp);
//		desiredUp = cross(right, direction);
//	Because of the 1rst rotation, the up is probably completely screwed up.
//	Find the rotation between the "up" of the rotated object, and the desired up
//		vec3 newUp = rot1 * vec3(0.0f, 1.0f, 0.0f);
//		quat rot2 = RotationBetweenVectors(newUp, desiredUp);
//	������ ���������� �� :
//		quat targetOrientation = rot2 * rot1; // remember, in reverse order.
//	�������������, ������������, ��, �����������, � �� ������� ���������!�� �� ������ ������ ��������� ����������� : targetPos - currentPos.
//	����� � ��� ���� ��� ������� ����������, ��, ��������, �������� ��������������� ����� startOrientation � targetOrientation.

//��� ������������ LookAt, �� ���������� �������� � ������������ ��������� ?
//	�������� ���� - ������� SLERP(= use glm::mix), �� ������ � ���������������� ���������, ����� ���� �� �������� ��������� �������� :
//		float mixFactor = maxAllowedAngle / angleBetweenQuaternions;
//		quat result = glm::gtc::quaternion::mix(q1, q2, mixFactor);
//	��� ����� ������ ����������, ������� �������� ������ ������ �������.�������� ��������, ��� �� �� ���������� mix() ��������������� � �������� �����������.
/*glm::quat RotateTowards(glm::quat q1, glm::quat q2, float maxAngle) {

	if (maxAngle < 0.001f) {
		// No rotation allowed. Prevent dividing by 0 later.
		return q1;
	}

	float cosTheta = glm::dot(q1, q2);

	// q1 and q2 are already equal.
	// Force q2 just to be sure
	if (cosTheta > 0.9999f) {
		return q2;
	}

	// Avoid taking the long path around the sphere
	if (cosTheta < 0) {
		q1 = q1*-1.0f;
		cosTheta *= -1.0f;
	}

	float angle = glm::acos(cosTheta);

	// If there is only a 2&deg; difference, and we are allowed 5&deg;,
	// then we arrived.
	if (angle < maxAngle) {
		return q2;
	}

	float fT = maxAngle / angle;
	angle = maxAngle;

	glm::quat res = (glm::sin((1.0f - fT) * angle) * q1 + glm::sin(fT * angle) * q2) / glm::sin(angle);
	res = glm::normalize(res);
	return res;

}*/
//	�� ������ ������������ ��� ��� :
//		CurrentOrientation = RotateTowards(CurrentOrientation, TargetOrientation, 3.14f * deltaTime);

//������� ����� 2�� ���������
/*glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);

}*/
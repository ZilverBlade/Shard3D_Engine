#include "../../s3dpch.h"
#include "components.h"

#include "../asset/assetmgr.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Shard3D {
	namespace Components {

		struct MatrixCalculator {
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix

			// Matrix corresponds to translate * Rz * Rx * Ry * scale transformation
	// Rotation convention = ZXY tait-bryan angles
			static inline glm::mat4 ZXY(TransformComponent& transform) {
				const float c3 = glm::cos(transform.rotation.z);
				const float s3 = glm::sin(transform.rotation.z);
				const float c2 = glm::cos(transform.rotation.x);
				const float s2 = glm::sin(transform.rotation.x);
				const float c1 = glm::cos(transform.rotation.y);
				const float s1 = glm::sin(transform.rotation.y);

				return glm::mat4(
					{
						transform.scale.x * (-c2 * s1),
						transform.scale.x * (c1 * c2),
						transform.scale.x * (s2),
						0.0f,
					},
					{
						transform.scale.y * (c1 * s3 + c3 * s1 * s2),
						transform.scale.y * (s1 * s3 - c1 * c3 * s2),
						transform.scale.y * (c2 * c3),
						0.0f,
					},
					{
						transform.scale.z * (c1 * c3 - s1 * s2 * s3),
						transform.scale.z * (c3 * s1 + c1 * s2 * s3),
						transform.scale.z * (-c2 * s3),
						0.0f,
					},
					{ transform.translation.x, transform.translation.y, transform.translation.z, 1.0f }
				);
			}
			// Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
	// Rotation convention = YXZ tait-bryan angles
			static inline glm::mat4 YXZ(TransformComponent& transform) {
				const float c3 = glm::cos(transform.rotation.z);
				const float s3 = glm::sin(transform.rotation.z);
				const float c2 = glm::cos(transform.rotation.x);
				const float s2 = glm::sin(transform.rotation.x);
				const float c1 = glm::cos(transform.rotation.y);
				const float s1 = glm::sin(transform.rotation.y);
				return glm::mat4(
					{
						transform.scale.x * (c1 * c3 + s1 * s2 * s3),
						transform.scale.x * (c2 * s3),
						transform.scale.x * (c1 * s2 * s3 - c3 * s1),
						0.0f,
					},
					{
						transform.scale.y * (c3 * s1 * s2 - c1 * s3),
						transform.scale.y * (c2 * c3),
						transform.scale.y * (c1 * c3 * s2 + s1 * s3),
						0.0f,
					},
					{
						transform.scale.z * (c2 * s1),
						transform.scale.z * (-s2),
						transform.scale.z * (c1 * c2),
						0.0f,
					},
					{ transform.translation.x, transform.translation.y, transform.translation.z, 1.0f }
				);
			}
		};

		void TransformComponent::decompose(const glm::mat4& transform, glm::vec3* outTranslation, glm::vec3* outRotationEulerXYZ, glm::vec3* outScale) {
			// From glm::decompose in matrix_decompose.inl

			using namespace glm;
			using T = float;

			mat4 LocalMatrix(transform);

			if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
				return;

			// First, isolate perspective.  This is the messiest.
			if (
				epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
			{
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of translation (easy).
			*outTranslation = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			outScale->x = length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			outScale->y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			outScale->z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
#if 0
			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}
#endif

#if 0
			
			

			outRotationEulerXYZ->y = asin(-Row[0][2]);
			if (cos(outRotationEulerXYZ->y) != 0) {
				outRotationEulerXYZ->x = atan2(Row[1][2], Row[2][2]);
				outRotationEulerXYZ->z = atan2(Row[0][1], Row[0][0]);
			}
			else {
				outRotationEulerXYZ->x = atan2(-Row[2][0], Row[1][1]);
				outRotationEulerXYZ->z = 0;
		}

#endif
			

			outRotationEulerXYZ->y = asin(-Row[1][2]);
			if (cos(outRotationEulerXYZ->y) != 0) {
				outRotationEulerXYZ->x = atan2(Row[0][2], Row[2][2]);
				outRotationEulerXYZ->z = atan2(Row[1][0], Row[1][1]);
			}
			else {
				outRotationEulerXYZ->x = atan2(-Row[2][1], Row[0][0]);
				outRotationEulerXYZ->z = 0;
			}

		}

		glm::mat4 TransformComponent::calculateMat4() {
			return MatrixCalculator::YXZ(*this);
		}

		glm::mat3 TransformComponent::calculateNormalMatrix() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);

			const glm::vec3 invScale = 1.0f / scale;
			return glm::mat3{
				{
					invScale.x * (c1 * c3 + s1 * s2 * s3),
					invScale.x * (c2 * s3),
					invScale.x * (c1 * s2 * s3 - c3 * s1),
				},
				{
					invScale.y * (c3 * s1 * s2 - c1 * s3),
					invScale.y * (c2 * c3),
					invScale.y * (c1 * c3 * s2 + s1 * s3),
				},
				{
					invScale.z * (c2 * s1),
					invScale.z * (-s2),
					invScale.z * (c1 * c2),
				}, };
		}
		void Mesh3DComponent::validate() {
			auto& model = ResourceHandler::retrieveMesh(asset);
			if (model->materialSlots.size() == materials.size()) return;
			SHARD3D_WARN("Mesh3DComponent validation failed! Resetting materials...");
			materials = model->materials;
		}
		Mesh3DComponent::Mesh3DComponent(const AssetID& mdl) : asset(mdl.getFile()) {
			auto& model = ResourceHandler::retrieveMesh(mdl);
			materials = model->materials;
		}
		BillboardComponent::BillboardComponent(const AssetID& tex) : asset(tex.getFile()) {}

}
}
#include "Camera.h"

void CameraCreate(Camera &camera, float x, float y, float z)
{
	// зададим позицию камеры и еденичную матрицу проекции
	camera.position   = vec3(x, y, z);
	camera.projection = mat4_identity;
}

void CameraPerspective(Camera &camera, float fov, float aspect, float zNear, float zFar)
{
	// расчет перспективной матрицы проекции
	camera.projection = GLPerspective(fov, aspect, zNear, zFar);
}

void CameraOrthographic(Camera &camera, float left, float right, float bottom, float top, float zNear, float zFar)
{
	// расчет перспективной матрицы проекции
	camera.projection = GLOrthographic(left, right, bottom, top, zNear, zFar);
}

void CameraRotate(Camera &camera, float x, float y, float z)
{
	// увеличение углов вращения камеры
	camera.rotation += vec3(x, y, z);
}

void CameraMove(Camera &camera, float x, float y, float z)
{
	// сначала нам надо перевести вектор направления в локальные координаты камеры
	// для этого нам надо инвертировать матрицу вращения камеры и умножить ее на вектор
	// однако для матрицы вращения транспонирование дает тот же результат что инвертирование
	vec3 move = transpose(mat3(GLRotation(camera.rotation))) * vec3(x, y, z);

	camera.position += move;
}

void CameraSetup(GLuint program, const Camera &camera)
{
	static GLint viewProjectionLocation = glGetUniformLocation(program, "transform.viewProjection");
	// расчитаем необходимые матрицы

	mat4 view           = GLRotation(camera.rotation) * GLTranslation(-camera.position);
	/*mat4 view = mat4(-0.707107,
					-0.408248,
					-0.57735,
					0,
					0,
					0.816497,
					-0.57735,
					0,
					0.707107,
					-0.408248,
					-0.57735,
					0,
					0,
					0,
					-1.73205,
					1
				);*/


	mat4 viewProjection = camera.projection * view;
	// передаем матрицы в шейдерную программу
	glUniformMatrix4fv(viewProjectionLocation, 1, GL_TRUE, viewProjection.m);
}

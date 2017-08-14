#include "stdafx.h"
#include "ogl/ogl.h"
#include "ogl/Light.hpp"
#include "ogl/ShaderManager.hpp"
#include "ogl/TextureManager.hpp"
#include "ogl/MaterialManager.hpp"
#include "ogl/ModelManager.hpp"
#include "ogl/Camera.hpp"
#include "ogl/Primitives.hpp"
#include "ogl/FrameBuffer.hpp"
#include "ogl/Font.hpp"
#include "ogl/Fog.hpp"
#include "ogl/geometry.h"
#include "app.h"

void drawNanosuit() {
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	model = glm::scale(model, glm::vec3(ogl::rotat, ogl::rotat, ogl::rotat));	// It's a bit too big for our scene, so scale it down
	ogl::getShaderManager()->getShader(_T("main"))->bind(_T("model"), model);
	ogl::getModelManager()->getModel(_T("nanosuit"))->Draw(ogl::getShaderManager()->getShader(_T("main")));
}
void drawSample2() {
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));	// It's a bit too big for our scene, so scale it down
	model = glm::translate(model, glm::vec3(0.0f, -2.35, 0.0f)); // Translate it down a bit so it's at the center of the scene
	model = glm::rotate(model, 4.7f, glm::vec3(1.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	ogl::getShaderManager()->getShader(_T("main"))->bind(_T("model"), model);
	ogl::getModelManager()->getModel(_T("sample2"))->Draw(ogl::getShaderManager()->getShader(_T("main")));
}

namespace ogl {
	std::shared_ptr<TexMaterial> texMaterial;
	GLuint skyboxTex;

	DirLight *dirLight;
	PointLight *pointLight;
	SpotLight *spotLight;

	Cube *cube;
	Tor *tor1;
	Tor *tor2;
	Plain *plain;
	Plain *ground;
	Cube *skybox;
	Pyramide *lightCube;
	Cube *mirrorbox;
	Cube *glassbox;

	Font* font1;
	FogParameters fog;
	FrameBuffer *fbo;
}

using namespace ogl;

void refreshLightPosition() {
	spotLight->Position(getFlyCamera()->m_position);
	spotLight->Direction(getFlyCamera()->m_target);
}
void renderSkybox() {
	getShaderManager()->getShader("Skybox")->activate()
		->bind("vp", app.Projection() * getFlyCamera()->View());
	skybox->DrawSkybox(getShaderManager()->getShader("Skybox"), skyboxTex);
}
void renderText() {
	getShaderManager()->getShader("Font")->activate()
		->bind(_T("projection"), app.Ortho());
	font1->Print(_T("FPS: ") + to_tstring(app.getFPS())
		+ "\nCamera position: " + to_tstring(getFlyCamera()->m_position.x) + "; " + to_tstring(getFlyCamera()->m_position.y) + "; " + to_tstring(getFlyCamera()->m_position.z)
		+ "\nCamera front: " + to_tstring(getFlyCamera()->m_target.x) + "; " + to_tstring(getFlyCamera()->m_target.y) + "; " + to_tstring(getFlyCamera()->m_target.z)
		+ "\nCamera up: " + to_tstring(getFlyCamera()->m_up.x) + "; " + to_tstring(getFlyCamera()->m_up.y) + "; " + to_tstring(getFlyCamera()->m_up.z)
		+ "\nm_value: " + to_tstring(app.m_value)
		, 0, app.Height() - 20);
}
void renderNormals(std::list<Primitives*> lst) {
	getShaderManager()->getShader("NormalVectors")->activate()
		->bind("view", getFlyCamera()->View())
		->bind("projection", app.Projection())
		->bind("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	for each (auto obj in lst)
		obj->Draw(getShaderManager()->getShader("NormalVectors"));
}
void renderColor(std::list<Primitives*> lst) {
	getShaderManager()->getShader("Colored")->activate()
		->bind("view", getFlyCamera()->View())
		->bind("projection", app.Projection())
		->bind("color", glm::vec4(0.76f, 0.55f, 0.33f, 1.0f));
	for each (auto obj in lst)
		obj->Draw(getShaderManager()->getShader("Colored"));
}
void renderSimpleLight(std::list<Primitives*> lst) {
	getShaderManager()->getShader("SimpleLight")->activate()
		->bind("vp", app.Projection() * getFlyCamera()->View())
		->bind("viewPosition", getFlyCamera()->m_position)
		->bind("fogOn", app.m_fogOn)
		->bind("spotOn", app.m_spotOn)
		->bind("depthOn", app.m_depthOn)
		->bind("shadowOn", app.m_shadowOn)
		->bind("gamma", app.m_value / 10.0f)
		;
	//dirLight->bindWithShadow(getShaderManager()->getShader("SimpleLight"), 2, "sun");
	pointLight->bindWithShadow(getShaderManager()->getShader("SimpleLight"), 3, "pointLight");
	//spotLight->bind(getShaderManager()->getShader("SimpleLight"));
	fog.bind(getShaderManager()->getShader("SimpleLight"), "fog");
	getTextureManager()->getTexture("snow")
		->bind(getShaderManager()->getShader("SimpleLight"), "diffuse", false, 0, 0);
	getTextureManager()->getTexture("container_specular")
		->bind(getShaderManager()->getShader("SimpleLight"), "specular", false, 0, 1);
	for each (auto obj in lst)
		obj->Draw(getShaderManager()->getShader("SimpleLight"));
	
}
void renderLightDepth(std::list<Primitives*> lst) {
	dirLight->RenderDepth(getShaderManager()->getShader("SunDepth"), lst);
	pointLight->RenderDepth(getShaderManager()->getShader("SunDepth"), lst);
}
void renderLightDepthFromScreen(std::list<Primitives*> lst) {
	pointLight->RenderDepthToScreen(getShaderManager()->getShader("SunDepth"), lst, 0, 0, 250, 250);
}
void renderTesselationObjects(std::list<Primitives*> lst) {
	getShaderManager()->getShader("Tesselation")->activate()
		->bind("view", getFlyCamera()->View())
		->bind("projection", app.Projection())
		->bind("tessLevel", app.m_tessLevel)
		->bind("viewPosition", getFlyCamera()->m_position)
		->bind("fogOn", app.m_fogOn)
		->bind("spotOn", app.m_spotOn);
	dirLight->bind(getShaderManager()->getShader("Tesselation"), "sun");
	pointLight->bind(getShaderManager()->getShader("Tesselation"));
	spotLight->bind(getShaderManager()->getShader("Tesselation"));
	fog.bind(getShaderManager()->getShader("Tesselation"), "fog");
	ogl::getTextureManager()->getTexture("container")
		->bind(getShaderManager()->getShader("Tesselation"), "diffuse", false, 0, 0);
	ogl::getTextureManager()->getTexture("container_specular")
		->bind(getShaderManager()->getShader("Tesselation"), "specular", false, 0, 1);
	ogl::getTextureManager()->getTexture("heightmap")
		->bind(getShaderManager()->getShader("Tesselation"), "heightmap", false, 0, 2);
	for each (auto obj in lst)
		obj->DrawPath(getShaderManager()->getShader("Tesselation"));
}
void renderExplosive(std::list<Primitives*> lst) {
	getShaderManager()->getShader("Explosive")->activate()
		->bind("view", getFlyCamera()->View())
		->bind("projection", app.Projection())
		->bind("viewPosition", getFlyCamera()->m_position)
		->bind("time", (float)glfwGetTime());
	dirLight->bind(getShaderManager()->getShader("Explosive"), "sun");
	pointLight->bind(getShaderManager()->getShader("Explosive"));
	spotLight->bind(getShaderManager()->getShader("Explosive"));
	ogl::getTextureManager()->getTexture("container")
		->bind(getShaderManager()->getShader("Explosive"), "diffuse", false, 0, 0);
	ogl::getTextureManager()->getTexture("container_specular")
		->bind(getShaderManager()->getShader("Explosive"), "specular", false, 0, 1);
	fog.bind(getShaderManager()->getShader("Explosive"), "fog");
	for each (auto obj in lst)
		obj->Draw(getShaderManager()->getShader("Explosive"));
}
void renderToFramebuffer(std::list<Primitives*> lst) {
	fbo->bind();
	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	renderSkybox();
	// Set Drawing buffers
	getShaderManager()->getShader("main")->activate()
		->bind(_T("view"), getFlyCamera()->View())
		->bind(_T("projection"), getFlyCamera()->Projection(app.m_window))
		->bind(_T("viewPos"), getFlyCamera()->m_position);
	dirLight->bind(getShaderManager()->getShader("main"));
	pointLight->bind(getShaderManager()->getShader("main"));
	spotLight->bind(getShaderManager()->getShader("main"));
	texMaterial->bind(getShaderManager()->getShader("main"));
	for each (auto obj in lst)
		obj->Draw(getShaderManager()->getShader("main"));
	texMaterial->unbind();
	renderText();
	fbo->unbind();
}
void renderFromFramebuffer(GLuint texture, Primitives* obj) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	getShaderManager()->getShader("test1")->activate()
		->bind("view", getFlyCamera()->View())
		->bind("projection", getFlyCamera()->Projection(app.m_window))
		->bind("diffuse", 0);
	obj->Draw(getShaderManager()->getShader("test1"));
}
void renderObjModels() {
	getShaderManager()->getShader("main")->activate()
		->bind(_T("viewPos"), getFlyCamera()->m_position)
		->bind(_T("view"), getFlyCamera()->View())
		->bind(_T("projection"), getFlyCamera()->Projection(app.m_window));
	//Add Light
	dirLight->bind(getShaderManager()->getShader("main"));
	pointLight->bind(getShaderManager()->getShader("main"));
	spotLight->bind(getShaderManager()->getShader("main"));
	//Add Material
	texMaterial->bind(getShaderManager()->getShader("main"));
	getTextureManager()->getTexture(skyboxTex).get()
		->bind(getShaderManager()->getShader("main"), "skybox", 0, 3);
	drawNanosuit();
	drawSample2();
	getTextureManager()->getTexture(skyboxTex).get()->unbind();
	texMaterial->unbind();
}
void renderMirrors(std::list<Primitives*> lst) {
	getShaderManager()->getShader("Mirror")->activate()
		->bind("viewPos", getFlyCamera()->m_position)
		->bind("view", getFlyCamera()->View())
		->bind("projection", getFlyCamera()->Projection(app.m_window));
	for each (auto obj in lst)
		obj->DrawMirror(getShaderManager()->getShader("Mirror"), skyboxTex);
}
void renderGlass(std::list<Primitives*> lst, GlassType glassType) {
	getShaderManager()->getShader("Glass")->activate()
		->bind("viewPos", getFlyCamera()->m_position)
		->bind("view", getFlyCamera()->View())
		->bind("projection", getFlyCamera()->Projection(app.m_window));
	for each (auto obj in lst)
		obj->DrawGlass(getShaderManager()->getShader("Glass"), skyboxTex, ogl::getGlass(glassType));
}
void ogl::initScene() {
	getFlyCamera()->Projection(app.Projection());

	texMaterial = ogl::getMaterialManager()->getMaterial("container");
	ogl::getTextureManager()->getTexture("awesomeface")->Type(Texture::TYPE::DIFFUSE);
	texMaterial->addTexture(ogl::getTextureManager()->getTexture("awesomeface"));
	skyboxTex = ogl::getTextureManager()->getTexture("jajlands1")->Id();

	dirLight = new DirLight(glm::vec3(-0.78f, -0.45f, 0.42f));
	
	pointLight = new PointLight(glm::vec3(0.0f, 4.0f, 0.0f));
	pointLight->CreatePointLight(PointLight::DISTANCE::D600);
	pointLight->Color(1.0f, 1.0f, 1.0f);

	spotLight = new SpotLight(getFlyCamera()->m_position, getFlyCamera()->m_target);
	spotLight->Color(1.0f, 0.0f, 1.0f);
	
	cube = new Cube(glm::vec3(1.0f), glm::vec3(0.0f, -0.45f, 0.0f));
	auto animation = std::make_unique<ogl::Animation>(new ogl::Animation);
	animation->Rotate = std::move(std::make_unique<ogl::iRotateAnimation>(
		new ogl::iRotateAnimation(glm::vec3(5.0f, 5.0f, 0.0f), true)));
	cube->Animation = std::move(animation);

	tor1 = new Tor(1.0f, 0.4f, 50, 100, glm::vec3(1.0f), glm::vec3(0.0f, -0.45f, 0.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	animation = std::make_unique<ogl::Animation>(new ogl::Animation);
	animation->Rotate = std::move(std::make_unique<ogl::iRotateAnimation>(
		new ogl::iRotateAnimation(glm::vec3(0.0f, 0.0f, 1.0f), true)));
	tor1->Animation = std::move(animation);

	ground = new Plain(glm::vec3(100.0f), glm::vec3(0.0f, -1.5f, 0.0f));

	animation = std::make_unique<ogl::Animation>(new ogl::Animation);
	animation->Rotate = std::move(std::make_unique<ogl::iRotateAnimation>(
		new ogl::iRotateAnimation(glm::vec3(0.0f, -5.0f, 0.0f), true)));
	plain = new Plain(glm::vec3(1.2f), glm::vec3(0.0f, -1.001f, 0.0f));
	plain->Animation = std::move(animation);

	tor2 = new Tor(1.0f, 0.4f, 50, 100, glm::vec3(1.0f), glm::vec3(0.0f, -1.5001f, 0.0f));
	animation = std::make_unique<ogl::Animation>(new ogl::Animation);
	animation->Rotate = std::move(std::make_unique<ogl::iRotateAnimation>(
		new ogl::iRotateAnimation(glm::vec3(0.0f, 0.0f, 1.0f), true)));
	tor2->Animation = std::move(animation);

	lightCube = new Pyramide(glm::vec3(0.1f), glm::vec3(0.0f, 3.0f, 0.0f));
	skybox = new Cube(glm::vec3(250.0f));
	mirrorbox = new Cube(glm::vec3(2.0f), glm::vec3(10.0f, -0.5f, 0.0f));
	glassbox = new Cube(glm::vec3(2.0f), glm::vec3(11.0f, 1.5f, -1.0f));
	
	font1 = new Font(FONTS_DIR + "sample2.ttf");
	font1->Color(glm::vec4(0.0f, 0.0f, 1.0f, 0.25f))->Shader(getShaderManager()->getShader("Font"));
	
	fbo = new FrameBuffer(app.Width(), app.Height(), FrameBuffer::FRAMETYPE::FULL);
}
void ogl::renderScene() {
	//pointLight->Position(glm::vec3(0, app.m_value, 0));
	if (app.m_poligonMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_FRAMEBUFFER_SRGB);
	//Устанавливаем направление фанарика
	refreshLightPosition();
	//Построение карты теней от направленного источника освещения
	renderLightDepth(std::list<Primitives*>{
		ground
		, lightCube
		, cube 
		//, tor1
		, plain
		, glassbox
		, mirrorbox
	});
	
	glViewport(0, 0, app.Width(), app.Height());
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Визуализируем скайбокс
	renderSkybox();
	//Визуализируем объекты с простым освещением
	renderSimpleLight(std::list<Primitives*>{
		ground
		, lightCube
		, cube
		//, tor1
		, plain
		, glassbox
		, mirrorbox
	});
	//Визуализируем нормали
	//renderNormals(std::list<Primitives*>{ (Primitives*)lightCube, (Primitives*)cube, (Primitives*)plain });
	//Визуализируем цветные объекты
	//renderColor(std::list<Primitives*>{ (Primitives*)glassbox });
	//Визуализируем объекты с простым освещением
	//renderDepthPointLight1(std::list<Primitives*>{ /*(Primitives*)lightCube,*/ (Primitives*)cube, /*(Primitives*)ground, */(Primitives*)plain });
	//Визуализируем объекты с тессиляцией
	//renderTesselationObjects(std::list<Primitives*>{ (Primitives*)plain });
	//Шейдер взрыва
	//renderExplosive(std::list<Primitives*>{ (Primitives*)lightCube, (Primitives*)tor1 });
	//Вывод в фреймбуфер
	//renderToFramebuffer(std::list<Primitives*>{ (Primitives*)ground, (Primitives*)cube });
	//Вывод из фреймбуфера на объект
	//renderFromFramebuffer(depthPointLight1->m_depthTexture, (Primitives*)mirrorbox);
	//Эксперементальная визуализация
	/*
	{
		getShaderManager()->getShader("main")->activate()
			->bind("viewPos", getFlyCamera()->m_position)
			->bind("view", getFlyCamera()->View())
			->bind("projection", app.Projection());
		dirLight->bind(getShaderManager()->getShader("main"));
		pointLight->bind(getShaderManager()->getShader("main"));
		spotLight->bind(getShaderManager()->getShader("main"));
		texMaterial->bind(getShaderManager()->getShader("main"));
		getTextureManager()->getTexture(skyboxTex).get()
			->bind(getShaderManager()->getShader("main"), "skybox", 0, 3);
		tor2->Draw(getShaderManager()->getShader("main"));
		getTextureManager()->getTexture(skyboxTex).get()->unbind();
		texMaterial->unbind();
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		glDepthMask(GL_FALSE);
		glClear(GL_STENCIL_BUFFER_BIT);
		plain->Draw(getShaderManager()->getShader("main"));
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		getShaderManager()->getShader("Colored")->activate()
			->bind("view", getFlyCamera()->View())
			->bind("projection", app.Projection())
			->bind("color", glm::vec4(0.7f, 0.7f, 0.0f, 0.2f));
		plain->Scale(1.2f);
		plain->Draw(getShaderManager()->getShader("Colored"));
		plain->Scale(1.0f);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDepthMask(GL_TRUE);

		getShaderManager()->getShader("main")->activate()
			->bind("viewPos", getFlyCamera()->m_position)
			->bind("view", getFlyCamera()->View())
			->bind("projection", app.Projection())
			->bind("objColor", glm::vec3(-0.03f, -0.03f, -0.03f));
		texMaterial->bind(getShaderManager()->getShader("main"));
		getTextureManager()->getTexture(skyboxTex).get()
			->bind(getShaderManager()->getShader("main"), "skybox", 0, 3);
		tor2->Draw(getShaderManager()->getShader("main"));
		getTextureManager()->getTexture(skyboxTex).get()->unbind();
		texMaterial->unbind();
		glDisable(GL_STENCIL_TEST);
	}
	*/
	//визуализация obj моделей
	//renderObjModels()
	//визуализация зеркала
	//renderMirrors(std::list<Primitives*>{ (Primitives*)mirrorbox });
	//Визуализация стекла 
	//renderGlass(std::list<Primitives*>{ (Primitives*)glassbox }, GlassType::AIR );
	renderLightDepthFromScreen(std::list<Primitives*>{
		ground
		//lightCube
		, cube
		//, tor1
		, plain
		, glassbox
		, mirrorbox
	});
	//Пишем текст
	renderText();
	app.swapBuffers();
}
void ogl::releaseScene() {
	texMaterial.reset();

	delete dirLight;
	delete spotLight;
	delete pointLight;

	delete tor1;
	delete tor2;
	delete skybox;
	delete lightCube;
	delete mirrorbox;
	delete glassbox;

	delete font1;
	delete fbo;

	getShaderManager()->unloadShaders();
	getTextureManager()->unloadTextures();
	getMaterialManager()->unloadMaterials();
	getModelManager()->unloadModels();
}

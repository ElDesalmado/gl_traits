﻿#include "helpers.hpp"

#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glt_Common.h"
#include "glt_CommonValidate.h"


constexpr const char a[] = "StringA",
a1[] = "StringA";


int main(int argc, char * argv[])
{
    
    fsys::path exePath{ argv[0] };

	std::cout << exePath.generic_string() << std::endl;

	SmartGLFW glfw{ 3, 3 };
	SmartGLFWwindow window{ SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL" };

	glfw.MakeContextCurrent(window);
	glfw.LoadOpenGL();

	glEnable(GL_DEPTH_TEST);


	// build and compile our shader program
	// ------------------------------------
	Shader ourShader{ (exePath.generic_string() + "vshader.vs").c_str(),
		(exePath.generic_string() + "fshader.fs").c_str() };

	auto posCoords = glm_cube_positions();
	auto texCoords = glm_cube_texCoords();	// for batched 1st array


	glt::VAO<glm::vec3, glm::vec2> vao{};
	vao.Bind();

	glt::Buffer<glm::vec3> vboPos{};
    glt::Buffer<glm::vec2> vboTex{};

	vboPos.Bind(glt::BufferTarget::array);
	vboPos.AllocateMemory(posCoords.size(), glt::BufUsage::static_draw);
 
    // TODO: use operator() here
    glt::Sequence<glm::vec3>& sVec3 = vboPos;
/*    sVec3.SubData(posCoords.data(), posCoords.size());

    vao.AttributePointer(glt::tag_s<0>(), sVec3);
   
    vboTex.Bind(glt::BufferTarget::array);
    vboTex.AllocateMemory(texCoords.size(), glt::BufUsage::static_draw);
    vboTex.SeqN().SubData(texCoords.data(), texCoords.size());

    vao.AttributePointer(glt::tag_s<1>(), vboTex.SeqN());
    vboTex.UnBind();

    // TODO: use EnablePointers()
	vao.EnablePointer(0);
	vao.EnablePointer(1);

	
	/////////////////////////////////////////////////////////////////////
	// The rest part is identical to other use cases
	/////////////////////////////////////////////////////////////////////

	glEnable(GL_DEPTH_TEST);

	// load and create textures 
	unsigned int texture1, texture2;
	{
		Image tex1{ (exePath.generic_string() + "resources/textures/container.jpg") },
			tex2{ (exePath.generic_string() + "resources/textures/awesomeface.png") };

		assert(tex1.Data() && tex2.Data());

		// texture 1
		// ---------
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1.Width(), tex1.Height(),
			0, GL_RGB, GL_UNSIGNED_BYTE, tex1.Data());
		glGenerateMipmap(GL_TEXTURE_2D);

		// texture 2
		// ---------
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex2.Width(), tex2.Height(),
			0, GL_RGBA, GL_UNSIGNED_BYTE, tex2.Data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader
		ourShader.use();

		// create transformations
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// retrieve the matrix uniform locations
		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		ourShader.setMat4("projection", projection);

		// render box
		vao.Bind();
		// glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    */
	return 0;

}
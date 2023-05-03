#include "framework.h"
#include "mainController.h"


void mainController::drawGL()
{
	// set viewport
	glClearColor(0.7, 0.7, 0.7, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);


	// get mouse position ( to Rotate view )
	this->calc_mouse_position();

	// setup matrix *********************************************
	// model matrix
	GLfloat eyeVec[3] = { 0.0, 13.0, 35.0 };
	GLfloat headVec[3] = { 0.0, 1.0, 0.0 };

	modelMat_obj->initMatrix();
	modelMat_obj->rotate_Ydeg(act_angle[0]);
	modelMat_obj->rotate_Xdeg(act_angle[1]);

	//normMat_obj->calculateVec3(eyeVec);
	//normMat_obj->calculateVec3(headVec);

	float view_ratio = (float)VIEW_WIDTH / (float)VIEW_HEIGHT;
	matrix_obj->initMatrix();
	/*
	matrix_obj->lookAt(35.0 * cos(act_radian), 13.0, 35.0 * sin(act_radian), // eyePoint
		0.0, -2.0, 0.0,	// targetPoint
		0.0, 1.0, 0.0); // headVec
	*/
	// look point is fixed *****
	matrix_obj->lookAt(eyeVec[0], eyeVec[1], eyeVec[2],
			0.0, -2.0, 0.0,
			headVec[0], headVec[1], headVec[2]);

	matrix_obj->perspective(30.0, view_ratio, 25.0, 50.0);






	//*****************************************************************
	// draw grid line *************************************************
	//*****************************************************************
	// change shader

	glUseProgram(NORMAL_SH[2]);
	glUniformMatrix4fv(UNF_NORMAL_mvpMatrix, 1, GL_FALSE, matrix_obj->getMatrix());
	glUniformMatrix4fv(UNF_NORMAL_modelMatrix, 1, GL_FALSE, modelMat_obj->getMatrix());

	// bind VAO
	glBindVertexArray(VAO_BASEGRID_name);

	// draw array
	int numP = ((GRID_X + 1) + (GRID_Z + 1) + 8) * 2;
	glDrawArrays(GL_LINES, 0, numP);

	//*****************************************************************
	// draw flat tickmark **************************************
	//*****************************************************************

	GLfloat flatCol[4];
	flatCol[0] = 1.0;	flatCol[1] = 1.0;	flatCol[2] = 1.0;	flatCol[3] = 1.0;

	GLfloat g_shift[3];
	g_shift[0] = 0.0;	g_shift[1] = 0.0;	g_shift[2] = 0.0;

	glUseProgram(FLAT_SH[2]);
	glUniformMatrix4fv(UNF_FLAT_mvpMatrix, 1, GL_FALSE, matrix_obj->getMatrix());
	glUniformMatrix4fv(UNF_FLAT_modelMatrix, 1, GL_FALSE, modelMat_obj->getMatrix());
	glUniform4fv(UNF_FLAT_flatColor, 1, flatCol);
	glUniform3fv(UNF_FLAT_global_shift, 1, g_shift);

	// bind VAO
	glBindVertexArray(VAO_ORIGIN_name);

	// draw triangle tickmark
	numP = (GRID_Y) * 3;
	glDrawArrays(GL_TRIANGLES, 0, numP);


	// ************************************************************
	// Draw Cubes *************************************************
	// ************************************************************

	// setup cubes vertex ( DYNAMIC_DRAW )
	if (NUM_CUBE != 0)
	{
		// change & setup shader ****************************
		GLfloat base_color[4] = { 1.0, 1.0, 1.0, 1.0 };

		glUseProgram(SHADE_SH[2]);
		glUniformMatrix4fv(UNF_SHADE_mvpMatrix, 1, GL_FALSE, matrix_obj->getMatrix());
		glUniformMatrix4fv(UNF_SHADE_normMatrix, 1, GL_FALSE, modelMat_obj->getMatrix());
		glUniform4fv(UNF_SHADE_global_shift, 1, g_shift);
		glUniform4fv(UNF_SHADE_baseColor, 1, base_color);


		// update vertex ***************************************
		glBindVertexArray(VAO_CUBES_name);

		long dataSize = sizeof(GLfloat) * NUM_CUBE * 36 * 4;

		glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBES_name[0]);
		glBufferData(GL_ARRAY_BUFFER, dataSize, cubes_ptr, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBES_name[1]);
		glBufferData(GL_ARRAY_BUFFER, dataSize, cubes_norm_ptr, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, NUM_CUBE * 36);
		
		// change color
		base_color[0] = 0.0;	base_color[1] = 0.0;	base_color[2] = 0.0;
		glUniform4fv(UNF_SHADE_baseColor, 1, base_color);
		glLineWidth(4.0);
		glDrawArrays(GL_LINES, 0, NUM_CUBE * 36);
		glLineWidth(1.0);
	}

	// ****************************************************************
	// Draw Support cube ****************************************************
	// ****************************************************************
	if (NUM_POINT_SUPPORT != 0)
	{
		GLfloat support_color[4] = {1.0, 0.0, 1.0, 1.0};
		glUniform4fv(UNF_SHADE_baseColor, 1, support_color);


		glBindVertexArray(VAO_CUBES_name);

		long supportSize = sizeof(GLfloat) * NUM_POINT_SUPPORT * 4;

		glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBES_name[0]);
		glBufferData(GL_ARRAY_BUFFER, supportSize, support_ptr, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBES_name[1]);
		glBufferData(GL_ARRAY_BUFFER, supportSize, support_norm_ptr, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, NUM_POINT_SUPPORT);
	}

	// ****************************************************************
	// Draw Cursor ****************************************************
	// ****************************************************************

	this->drawCursor();



	glFlush();
	SwapBuffers(gl_obj->OpenGL_HDC);
}




void mainController::drawCursor()
{

	//*****************************************
	// draw cursor cube ***********************
	//*****************************************

	GLfloat shift[3];
	GLfloat g_shift[3];

	shift[0] = (GLfloat)CURSOR_XYZ[0];
	shift[1] = (GLfloat)CURSOR_XYZ[1];
	shift[2] = (GLfloat)CURSOR_XYZ[2];

	g_shift[0] = -(GRID_X/2);
	g_shift[1] = -(GRID_Y/2);
	g_shift[2] = -(GRID_Z/2);


	// **************************************
	// draw vertical plane ******************
	// **************************************
	GLfloat pCol[4] = { 1.0, 1.0, 0.0, 0.7 };
	GLfloat pZ[3];
	pZ[0] = 0.0;
	pZ[1] = shift[1] - (GRID_Y/2) + 0.01; // +0.01 to reduce z jaggy
	pZ[2] = 0.0;

	///////////////////
	act_plane_shift[1] += (pZ[1] - act_plane_shift[1]) * 0.3;
	///////////////////

	glUseProgram(FLAT_SH[2]);

	glUniformMatrix4fv(UNF_FLAT_mvpMatrix, 1, GL_FALSE, matrix_obj->getMatrix());
	glUniformMatrix4fv(UNF_FLAT_modelMatrix, 1, GL_FALSE, modelMat_obj->getMatrix());
	glUniform4fv(UNF_FLAT_flatColor, 1, pCol);
	glUniform3fv(UNF_FLAT_global_shift, 1, act_plane_shift);

	glBindVertexArray(VAO_ZPLANE_name);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


	// *****************************************
	// draw cursor cube ************************
	//******************************************
	
	// change cursor color
	GLfloat lineCol[4];
	int IDX = CURSOR_XYZ[0];
	int IDY = CURSOR_XYZ[1];
	int IDZ = CURSOR_XYZ[2];

	if (canPut[IDX][IDY][IDZ])
	{
		lineCol[0] = 1.0;
		lineCol[1] = 1.0;
		lineCol[2] = 1.0;
		lineCol[3] = 1.0;
		glLineWidth(2.0);
	}
	else
	{
		lineCol[0] = 1.0;
		lineCol[1] = 0.0;
		lineCol[2] = 0.0;
		lineCol[3] = 1.0;
		glLineWidth(1.0);
	}


	// change shader
	// clear depth buffer to overwrite cursor
	glClear(GL_DEPTH_BUFFER_BIT);
	//glPolygonOffset(10.0, 15.0); // z += (z*factor + r*unit)

	glUseProgram(CURSOR_SH[2]);
	glUniformMatrix4fv(UNF_CURSOR_mvpMatrix, 1, GL_FALSE, matrix_obj->getMatrix());
	glUniformMatrix4fv(UNF_CURSOR_modelMatrix, 1, GL_FALSE, modelMat_obj->getMatrix());
	glUniform4fv(UNF_CURSOR_lineColor, 1, lineCol);
	glUniform3fv(UNF_CURSOR_shift, 1, shift);
	glUniform3fv(UNF_CURSOR_global_shift, 1, g_shift);

	// set line width
	//glLineWidth(2.0);
	glBindVertexArray(VAO_CURSOR_name);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0); // offset of Indice(last attr)
}
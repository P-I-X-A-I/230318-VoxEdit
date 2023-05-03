
#include "framework.h"
#include "mainController.h"



void mainController::gl_setup_sampler()
{
	// gen sampler
	glGenSamplers(1, &Sampler_name);
	// setup Sampler
	glSamplerParameteri(Sampler_name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(Sampler_name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(Sampler_name, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(Sampler_name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// get num texture
	GLint numTex;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTex);

	for (int i = 0; i < numTex; i++)
	{
		glBindSampler(i, Sampler_name);
	}
}


void mainController::gl_setup_shader()
{
	gl_obj->loadShaderSource_And_CompileShader("SHADER/NORMAL_VS.txt", 0, &NORMAL_SH[0]);
	gl_obj->loadShaderSource_And_CompileShader("SHADER/NORMAL_FS.txt", 2, &NORMAL_SH[1]);
	gl_obj->createProgram_And_AttachShader(&NORMAL_SH[2], &NORMAL_SH[0], NULL, &NORMAL_SH[1]);
	gl_obj->getUniformLocation(&NORMAL_SH[2], &UNF_NORMAL_mvpMatrix, "mvpMatrix");
	gl_obj->getUniformLocation(&NORMAL_SH[2], &UNF_NORMAL_modelMatrix, "modelMatrix");

	gl_obj->loadShaderSource_And_CompileShader("SHADER/FLAT_VS.txt", 0, &FLAT_SH[0]);
	gl_obj->loadShaderSource_And_CompileShader("SHADER/FLAT_FS.txt", 2, &FLAT_SH[1]);
	gl_obj->createProgram_And_AttachShader(&FLAT_SH[2], &FLAT_SH[0], NULL, &FLAT_SH[1]);
	gl_obj->getUniformLocation(&FLAT_SH[2], &UNF_FLAT_mvpMatrix, "mvpMatrix");
	gl_obj->getUniformLocation(&FLAT_SH[2], &UNF_FLAT_modelMatrix, "modelMatrix");
	gl_obj->getUniformLocation(&FLAT_SH[2], &UNF_FLAT_flatColor, "flatColor");
	gl_obj->getUniformLocation(&FLAT_SH[2], &UNF_FLAT_global_shift, "global_shift");

	gl_obj->loadShaderSource_And_CompileShader("SHADER/CURSOR_VS.txt", 0, &CURSOR_SH[0]);
	gl_obj->loadShaderSource_And_CompileShader("SHADER/CURSOR_FS.txt", 2, &CURSOR_SH[1]);
	gl_obj->createProgram_And_AttachShader(&CURSOR_SH[2], &CURSOR_SH[0], NULL, &CURSOR_SH[1]);
	gl_obj->getUniformLocation(&CURSOR_SH[2], &UNF_CURSOR_mvpMatrix, "mvpMatrix");
	gl_obj->getUniformLocation(&CURSOR_SH[2], &UNF_CURSOR_modelMatrix, "modelMatrix");
	gl_obj->getUniformLocation(&CURSOR_SH[2], &UNF_CURSOR_lineColor, "lineColor");
	gl_obj->getUniformLocation(&CURSOR_SH[2], &UNF_CURSOR_shift, "cursor_shift");
	gl_obj->getUniformLocation(&CURSOR_SH[2], &UNF_CURSOR_global_shift, "global_shift");

	gl_obj->loadShaderSource_And_CompileShader("SHADER/SHADE_VS.txt", 0, &SHADE_SH[0]);
	gl_obj->loadShaderSource_And_CompileShader("SHADER/SHADE_FS.txt", 2, &SHADE_SH[1]);
	gl_obj->createProgram_And_AttachShader(&SHADE_SH[2], &SHADE_SH[0], NULL, &SHADE_SH[1]);
	gl_obj->getUniformLocation(&SHADE_SH[2], &UNF_SHADE_mvpMatrix, "mvpMatrix");
	gl_obj->getUniformLocation(&SHADE_SH[2], &UNF_SHADE_normMatrix, "modelMatrix");
	gl_obj->getUniformLocation(&SHADE_SH[2], &UNF_SHADE_global_shift, "global_shift");
	gl_obj->getUniformLocation(&SHADE_SH[2], &UNF_SHADE_baseColor, "baseColor");
}



void mainController::gl_setup_VAO_VBO()
{

	// BASE GRID array
	glGenVertexArrays(1, &VAO_BASEGRID_name);
	glGenBuffers(2, VBO_BASEGRID_name);

	glBindVertexArray(VAO_BASEGRID_name);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// supply data
	// line x
	int numL_X = GRID_X + 1;
	int numL_Y = GRID_Y + 1;
	int numL = numL_X + numL_Y + 8;

	// alloc memory
	int lineDataSize = sizeof(GLfloat) * numL * 2 * 4;
	GLfloat* grid_v_ptr = (GLfloat*)malloc(lineDataSize);
	GLfloat* grid_c_ptr = (GLfloat*)malloc(lineDataSize);

	GLfloat* w_v = grid_v_ptr;
	GLfloat* w_c = grid_c_ptr;

	GLfloat startX = (float)(GRID_X / 2);
	GLfloat startY = (float)(GRID_Y / 2);
	GLfloat startZ = (float)(GRID_Z / 2);

	for (int i = 0; i < (GRID_X + 1); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			*w_v = (i * 1.0) - startX;		w_v++;
			*w_v = -startY;					w_v++;
			*w_v = (j * GRID_Z) - startZ;	w_v++;
			*w_v = 1.0;						w_v++;

			*w_c = 0.0;		w_c++;
			*w_c = 0.0;		w_c++;
			*w_c = 0.0;		w_c++;
			*w_c = 1.0;		w_c++;
		}
	}

	for (int i = 0; i < (GRID_Z + 1); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			*w_v = (j * GRID_X) - startX;	w_v++;
			*w_v = -startY;					w_v++;
			*w_v = (i * 1.0) - startZ;		w_v++;
			*w_v = 1.0;						w_v++;

			*w_c = 0.0;		w_c++;
			*w_c = 0.0;		w_c++;
			*w_c = 0.0;		w_c++;
			*w_c = 1.0;		w_c++;
		}
	}


	// 4 corners
	GLfloat vx[4];
	GLfloat vz[4];
	vx[0] = -startX;	vz[0] = -startZ;
	vx[1] = startX;		vz[1] = -startZ;
	vx[2] = startX;		vz[2] = startZ;
	vx[3] = -startX;	vz[3] = startZ;

	// vertical line
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			*w_v = vx[i];					w_v++;
			*w_v = (j * GRID_Y) - startY;	w_v++;
			*w_v = vz[i];					w_v++;
			*w_v = 1.0;						w_v++;

			*w_c = 0.0;		w_c++;
			*w_c = 0.0;		w_c++;
			*w_c = 0.0;		w_c++;
			*w_c = 1.0;		w_c++;
		}
	}

	// ceil
	GLfloat cx[4][2];
	GLfloat cz[4][2];

	cx[0][0] = vx[0];	cz[0][0] = vz[0];
	cx[0][1] = vx[1];	cz[0][1] = vz[1];

	cx[1][0] = vx[1];	cz[1][0] = vz[1];
	cx[1][1] = vx[2];	cz[1][1] = vz[2];

	cx[2][0] = vx[2];	cz[2][0] = vz[2];
	cx[2][1] = vx[3];	cz[2][1] = vz[3];

	cx[3][0] = vx[3];	cz[3][0] = vz[3];
	cx[3][1] = vx[0];	cz[3][1] = vz[0];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			*w_v = cx[i][j];	w_v++;
			*w_v = startY;		w_v++;
			*w_v = cz[i][j];	w_v++;
			*w_v = 1.0;			w_v++;

			*w_c = 0.0;			w_c++;
			*w_c = 0.0;			w_c++;
			*w_c = 0.0;			w_c++;
			*w_c = 1.0;			w_c++;
		}
	}


	int dataSize = sizeof(GLfloat) * numL * 2 * 4;
	glBindBuffer(GL_ARRAY_BUFFER, VBO_BASEGRID_name[0]); // vertex
	glBufferData(GL_ARRAY_BUFFER, dataSize, grid_v_ptr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_BASEGRID_name[1]); // color
	glBufferData(GL_ARRAY_BUFFER, dataSize, grid_c_ptr, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);


	// ********************************
	// origin circle
	//********************************

	glGenVertexArrays(1, &VAO_ORIGIN_name);
	glGenBuffers(1, &VBO_ORIGIN_name);

	glBindVertexArray(VAO_ORIGIN_name);
	glEnableVertexAttribArray(0); // vertex

	// set vertex( for triangle)
	GLfloat tick_v[GRID_Y][3][4];

	GLfloat xVal[3];
	GLfloat zVal[3];

	// make triangle tickmark
	xVal[0] = -startX;			zVal[0] = startZ;
	xVal[1] = -0.28 - startX;	zVal[1] = startZ;
	xVal[2] = -startX;			zVal[2] = 0.28 + startZ;

	for (int y = 0; y < GRID_Y; y++)
	{
		GLfloat yVal = (y * 1.0) - startY;

		for (int t = 0; t < 3; t++)
		{
			tick_v[y][t][0] = xVal[t];
			tick_v[y][t][1] = yVal;
			tick_v[y][t][2] = zVal[t];
			tick_v[y][t][3] = 1.0;
		}
	}

	dataSize = sizeof(GLfloat) * GRID_Y * 3 * 4;
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ORIGIN_name);
	glBufferData(GL_ARRAY_BUFFER, dataSize, tick_v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);


	// Cursor **********************************************************

	glGenVertexArrays(1, &VAO_CURSOR_name);
	glGenBuffers(2, VBO_CURSOR_name);
	glBindVertexArray(VAO_CURSOR_name);
	glEnableVertexAttribArray(0); // vertex

	GLfloat CV[8][4]; // 4 corner
	GLint CE[24];

	CV[0][0] = 0.0;		CV[0][1] = 0.0;		CV[0][2] = 0.0;		CV[0][3] = 1.0;
	CV[1][0] = 1.0;		CV[1][1] = 0.0;		CV[1][2] = 0.0;		CV[1][3] = 1.0;
	CV[2][0] = 1.0;		CV[2][1] = 1.0;		CV[2][2] = 0.0;		CV[2][3] = 1.0;
	CV[3][0] = 0.0;		CV[3][1] = 1.0;		CV[3][2] = 0.0;		CV[3][3] = 1.0;
	CV[4][0] = 0.0;		CV[4][1] = 0.0;		CV[4][2] = 1.0;		CV[4][3] = 1.0;
	CV[5][0] = 1.0;		CV[5][1] = 0.0;		CV[5][2] = 1.0;		CV[5][3] = 1.0;
	CV[6][0] = 1.0;		CV[6][1] = 1.0;		CV[6][2] = 1.0;		CV[6][3] = 1.0;
	CV[7][0] = 0.0;		CV[7][1] = 1.0;		CV[7][2] = 1.0;		CV[7][3] = 1.0;

	CE[0] = 0;	CE[1] = 1;
	CE[2] = 1;	CE[3] = 2;
	CE[4] = 2;	CE[5] = 3;
	CE[6] = 3;	CE[7] = 0;

	CE[8] = 4;	CE[9] = 5;
	CE[10] = 5;	CE[11] = 6;
	CE[12] = 6;	CE[13] = 7;
	CE[14] = 7;	CE[15] = 4;

	CE[16] = 0;	CE[17] = 4;
	CE[18] = 1;	CE[19] = 5;
	CE[20] = 2;	CE[21] = 6;
	CE[22] = 3;	CE[23] = 7;

	dataSize = sizeof(GLfloat) * 8 * 4;
	glBindBuffer(GL_ARRAY_BUFFER, VBO_CURSOR_name[0]);
	glBufferData(GL_ARRAY_BUFFER, dataSize, CV, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	dataSize = sizeof(GLint) * 24;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_CURSOR_name[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, CE, GL_STATIC_DRAW);


	//********************************
	// Z Plane ***********************
	//********************************

	glGenVertexArrays(1, &VAO_ZPLANE_name);
	glGenBuffers(1, &VBO_ZPLANE_name);
	glBindVertexArray(VAO_ZPLANE_name);
	glEnableVertexAttribArray(0); // position

	GLfloat zP[4][4];
	GLfloat pX = (GRID_X / 2);
	GLfloat pZ = (GRID_Z / 2);

	zP[0][0] = -pX;		zP[0][1] = 0.0;		zP[0][2] = -pZ;		zP[0][3] = 1.0;
	zP[1][0] = -pX;		zP[1][1] = 0.0;		zP[1][2] = pZ;		zP[1][3] = 1.0;
	zP[2][0] = pX;		zP[2][1] = 0.0;		zP[2][2] = -pZ;		zP[2][3] = 1.0;
	zP[3][0] = pX;		zP[3][1] = 0.0;		zP[3][2] = pZ;		zP[3][3] = 1.0;

	dataSize = sizeof(GLfloat) * 4 * 4;
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ZPLANE_name);
	glBufferData(GL_ARRAY_BUFFER, dataSize, zP, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);


	//*****************************************************
	// cubes **********************************************
	//*****************************************************

	glGenVertexArrays(1, &VAO_CUBES_name);
	glGenBuffers(2, VBO_CUBES_name);
	glBindVertexArray(VAO_CUBES_name);
	glEnableVertexAttribArray(0); // vertex
	glEnableVertexAttribArray(1); // normal
}
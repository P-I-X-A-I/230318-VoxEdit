#pragma once

#include "WindowGUI_Manager_Class.h"
#include "OpenGL_Manager_Class.h"
#include "matrixClass.h"

#include "SaveLoadClass.h"

#include <iostream>

#define VIEW_WIDTH 800
#define VIEW_HEIGHT 640

// must be even number ***
#define GRID_X 12
#define GRID_Y 12 // z in STL coordinate
#define GRID_Z 12 // y in STL coordinate
//************************

class mainController
{
public:

	// objects
	WindowGUI_Manager_Class* winGUI_obj;
	OpenGL_Manager_Class* gl_obj;
	matrixClass* matrix_obj;
	matrixClass* modelMat_obj;
	SaveLoadClass* save_load_obj;

	// handler
	HWND h_mainWin;
	HWND h_glView;

	// variable
	double mousePos[2];
	double prevPos[2];
	bool isMouseDown = false;
	//double camera_radian = 1.3;
	//float act_radian = 1.3;
	double camera_angle[2] = { 0.0, 0.0 };
	double act_angle[2] = { 0.0, 0.0 };

	GLfloat act_plane_shift[3] = { 0.0, 0.0, 0.0 };

	int ANGLE_MODE = 0;
	int CURSOR_XYZ[3];
	GLfloat cursor_shift[3];

	// cube variable
	int isCube[GRID_X][GRID_Y][GRID_Z];
	int isSupport[GRID_X][GRID_Y][GRID_Z];
	bool canPut[GRID_X][GRID_Y][GRID_Z];
	GLfloat* cubes_ptr;
	GLfloat* cubes_norm_ptr;
	int NUM_CUBE;
	GLfloat SPV[18][4];
	GLfloat* support_ptr;
	GLfloat* support_norm_ptr;
	int NUM_POINT_SUPPORT;

	// OpenGL variables
	GLuint Sampler_name;

	// shader --------
	GLuint NORMAL_SH[3]; //0-VS, 1-FS, 2-PRG
	GLint UNF_NORMAL_mvpMatrix;
	GLint UNF_NORMAL_modelMatrix;

	GLuint FLAT_SH[3];
	GLint UNF_FLAT_mvpMatrix;
	GLint UNF_FLAT_modelMatrix;
	GLint UNF_FLAT_flatColor;
	GLint UNF_FLAT_global_shift;

	GLuint CURSOR_SH[3];
	GLint UNF_CURSOR_mvpMatrix;
	GLint UNF_CURSOR_modelMatrix;
	GLint UNF_CURSOR_lineColor;
	GLint UNF_CURSOR_shift;
	GLint UNF_CURSOR_global_shift;

	GLuint SHADE_SH[3];
	GLint UNF_SHADE_mvpMatrix;
	GLint UNF_SHADE_normMatrix;
	GLint UNF_SHADE_global_shift;
	GLint UNF_SHADE_baseColor;

	// VAO, VBO -------
	GLuint VAO_BASEGRID_name;
	GLuint VBO_BASEGRID_name[2]; // vert, color

	GLuint VAO_ORIGIN_name; // tickmark
	GLuint VBO_ORIGIN_name; // vert

	GLuint VAO_CURSOR_name;
	GLuint VBO_CURSOR_name[2]; // vert, element

	GLuint VAO_ZPLANE_name;
	GLuint VBO_ZPLANE_name;

	GLuint VAO_CUBES_name;
	GLuint VBO_CUBES_name[2]; // vert, normal
	///////////////////////////////////
	///////////////////////////////////
	///////////////////////////////////


	mainController();
	~mainController();

	void calc_mouse_position();

	// setup function
	void gl_setup_sampler();
	void gl_setup_shader();
	void gl_setup_VAO_VBO();

	// from WindowGUI manager
	void mouseFlag(bool yn);
	void arrowKey_hit(int keyNum, int shiftFlag);
	void spaceKey_hit(int keyNum);
	void deleteKey_hit(int keyNum);

	void save_button();

	// draw function
	void drawGL();
	void drawCursor();

	// utility function
private:
	void update_canPut_flag();
	void delete_cube_by_flag(int yLayer);
	void setup_cubes();
	GLfloat* set_cube_vertex(int xID, int yID, int zID, GLfloat* ptr);
	GLfloat* set_cube_normal(int xID, int yID, int zID, GLfloat* nPtr);
	void setup_support_block();
	int check_support_pattern(int xID, int yID, int zID);
	GLfloat* set_support_vertex(int xID, int yID, int zID, int sID, GLfloat* ptr);
	void culc_support_normal();
};


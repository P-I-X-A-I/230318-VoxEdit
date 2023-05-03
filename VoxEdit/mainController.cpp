
#include "framework.h"
#include "mainController.h"

mainController* this_obj;

VOID CALLBACK drawTimer(HWND hWnd, UINT msg, UINT idEvent, DWORD dwTime)
{
	this_obj->drawGL();
}

mainController::mainController()
{
	this_obj = this;

	// Class initialization ///////////////////
	winGUI_obj = new WindowGUI_Manager_Class();
	winGUI_obj->setMainController(this);

	gl_obj = new OpenGL_Manager_Class();
	matrix_obj = new matrixClass();
	modelMat_obj = new matrixClass();

	save_load_obj = new SaveLoadClass();
	// ////////////////////////////////////////


	// create main window
	h_mainWin = winGUI_obj->createNewWindow(L"mainWin", L"Vox Edit", 4);
	winGUI_obj->set_window_size(h_mainWin, 1024, 640);

	// add GUI (button..slider....)
	winGUI_obj->create_button(h_mainWin, L"save STL", 830, 550, 150, 30, 22);

	// add OpenGL View(textView)
	h_glView = winGUI_obj->create_textfield(h_mainWin, NULL, 0, 0, VIEW_WIDTH, VIEW_HEIGHT, 111);

	// init opengl
	HWND dumWin = winGUI_obj->create_borderless_window(L"dummy");
	gl_obj->init_OpenGL_functions(dumWin, h_glView);
	winGUI_obj->close_window(dumWin);
	gl_obj->set_OpenGL_status();


	/// setup functions ************************
	this->gl_setup_sampler();
	this->gl_setup_shader();
	this->gl_setup_VAO_VBO();
	//******************************************


	// timer
	UINT timerID = SetTimer(NULL, 100, 31, (TIMERPROC)drawTimer);

	// init variables
	mousePos[0] = 0.0;
	mousePos[1] = 0.0;
	prevPos[0] = 0.0;
	prevPos[1] = 0.0;
	isMouseDown = false;

	CURSOR_XYZ[0] = 0;
	CURSOR_XYZ[1] = 0;
	CURSOR_XYZ[2] = 0;
	cursor_shift[0] = 0.0;
	cursor_shift[1] = 0.0;
	cursor_shift[2] = 0.0;

	// alloc memory for maximum number of cubes ( 36 vertices per 1 cube )
	long dataSize = sizeof(GLfloat) * GRID_X * GRID_Y * GRID_Z * 36 * 4;
	cubes_ptr = (GLfloat*)malloc(dataSize);
	cubes_norm_ptr = (GLfloat*)malloc(dataSize);
	//*******************************************************************
	// alloc memory for support block
	dataSize = sizeof(GLfloat) * GRID_X * GRID_Y * GRID_Z * 36 * 4; // size tekito-
	support_ptr = (GLfloat*)malloc(dataSize);
	support_norm_ptr = (GLfloat*)malloc(dataSize);
	//*******************************************************************


	NUM_CUBE = 0;

	for (int x = 0; x < GRID_X; x++)
	{
		for (int y = 0; y < GRID_Y; y++)
		{
			for (int z = 0; z < GRID_Z; z++)
			{
				isCube[x][y][z] = 0;

				if (y == 0)
				{
					canPut[x][y][z] = true;
				}
				else
				{
					canPut[x][y][z] = false;
				}
			}
		}
	}
}


mainController::~mainController()
{

}


void mainController::calc_mouse_position()
{
	POINT mP;
	GetCursorPos(&mP);
	ScreenToClient(h_glView, &mP);

	mousePos[0] = mP.x;
	mousePos[1] = mP.y;

	double delta[2];
	delta[0] = prevPos[0] - mousePos[0];
	delta[1] = prevPos[1] - mousePos[1];

	// rotate camera angle
	
	if (isMouseDown == true)
	{

		camera_angle[0] -= delta[0] * 0.1;
		camera_angle[1] -= delta[1] * 0.1;
		

		// limit angle range *********************************************
		//if (camera_angle[0] >= 170.0) { camera_angle[0] = 170.0; }
		//else if (camera_angle[0] <= -170.0) { camera_angle[0] = -170.0; }

		if (camera_angle[1] >= 35.0) { camera_angle[1] = 35.0; }
		else if (camera_angle[1] <= -25.0) { camera_angle[1] = -25.0; }
		//*****************************************************************
	}

	act_angle[0] += (camera_angle[0] - act_angle[0]) * 0.2;
	act_angle[1] += (camera_angle[1] - act_angle[1]) * 0.2;

	// check angle to control key movement
	double originRad = 3.14159265 / 2.0; // 1.57
	double quaterRad = 6.2831853 / 8.0;

	
	//////////////////////////////////////////////////////////////
	double act_sin = sin(act_angle[0] * 0.0174532925);
	double act_cos = cos(act_angle[0] * 0.0174532925);

	double root1_2 = 1.0 / 1.4142;

	if ( act_cos > root1_2 ) // -45 ~ 45 deg
	{
		ANGLE_MODE = 0;
	}
	else if (act_cos < -root1_2)
	{
		ANGLE_MODE = 3;
	}
	else
	{
		if (act_sin >= 0.0)
		{
			ANGLE_MODE = 2;
		}
		else
		{
			ANGLE_MODE = 1;
		}
	}

	/*
	if (act_angle[0] <= 45.0 && act_angle[0] >= -45.0)
	{
		ANGLE_MODE = 0;
	}
	else if (act_angle[0] > 45.0 && act_angle[0] < 135.0)
	{
		ANGLE_MODE = 2;
	}
	else if (act_angle[0] > -135.0 && act_angle[0] < -45.0)
	{
		ANGLE_MODE = 1;
	}
	else
	{
		ANGLE_MODE = 3;
	}
	*/


	// record prev pos
	prevPos[0] = mousePos[0];
	prevPos[1] = mousePos[1];

	//printf("%1.3f, %1.3f\n", delta[0], delta[1]);
}


void mainController::mouseFlag(bool yn)
{
	isMouseDown = yn;
}

void mainController::arrowKey_hit(int keyNum, int shiftFlag)
{
	// Y-Z axis is flipped

	bool isShift = true;
	if (shiftFlag >= 0)
	{
		// no shift
		isShift = false;
	}


	// change movement direction by angle mode
	int D_XY[4][2];
	switch (ANGLE_MODE)
	{
	case 0:
		D_XY[0][0] = -1;	D_XY[0][1] = 0; // left
		D_XY[1][0] = 0;		D_XY[1][1] = -1; // up
		D_XY[2][0] = 1;		D_XY[2][1] = 0; // right
		D_XY[3][0] = 0;		D_XY[3][1] = 1; // down
		break;
	case 1:
		D_XY[0][0] = 0;		D_XY[0][1] = 1; // left
		D_XY[1][0] = -1;	D_XY[1][1] = 0; // up
		D_XY[2][0] = 0;		D_XY[2][1] = -1; // right
		D_XY[3][0] = 1;		D_XY[3][1] = 0; // down
		break;
	case 2:
		D_XY[0][0] = 0;		D_XY[0][1] = -1; // left
		D_XY[1][0] = 1;		D_XY[1][1] = 0; // up
		D_XY[2][0] = 0;		D_XY[2][1] = 1; // right
		D_XY[3][0] = -1;	D_XY[3][1] = 0; // down
		break;
	case 3:
		D_XY[0][0] = 1;		D_XY[0][1] = 0; // left
		D_XY[1][0] = 0;		D_XY[1][1] = 1; // up
		D_XY[2][0] = -1;	D_XY[2][1] = 0; // right
		D_XY[3][0] = 0;		D_XY[3][1] = -1; // down
		break;
	default:
		break;
	}


	// move cursor position
	switch (keyNum)
	{
	case 37: // left key
		CURSOR_XYZ[0] += D_XY[0][0];
		CURSOR_XYZ[2] += D_XY[0][1];
		break;

	case 38: // up key
		if (isShift == false) // plus z
		{
			CURSOR_XYZ[0] += D_XY[1][0];
			CURSOR_XYZ[2] += D_XY[1][1];
		}
		else // plus y ( with shift key )
		{
			CURSOR_XYZ[1] += 1;
		}
		break;

	case 39: // right key
		CURSOR_XYZ[0] += D_XY[2][0];
		CURSOR_XYZ[2] += D_XY[2][1];
		break;

	case 40: // down key
		if (isShift == false) // minus z
		{
			CURSOR_XYZ[0] += D_XY[3][0];
			CURSOR_XYZ[2] += D_XY[3][1];
		}
		else // minus y ( with shift key )
		{
			CURSOR_XYZ[1] -= 1;
		}
		break;

	default:
		break;
	}


	// limit range
	CURSOR_XYZ[0] = fminl(CURSOR_XYZ[0], (GRID_X - 1) );
	CURSOR_XYZ[0] = fmaxl(CURSOR_XYZ[0], 0);

	CURSOR_XYZ[1] = fminl(CURSOR_XYZ[1], (GRID_Y - 1));
	CURSOR_XYZ[1] = fmaxl(CURSOR_XYZ[1], 0);

	CURSOR_XYZ[2] = fminl(CURSOR_XYZ[2], (GRID_Z - 1));
	CURSOR_XYZ[2] = fmaxl(CURSOR_XYZ[2], 0);

}


void mainController::spaceKey_hit(int keyNum)
{
	if (keyNum == 32)
	{
		// check canPut
		int IDX = CURSOR_XYZ[0];
		int IDY = CURSOR_XYZ[1];
		int IDZ = CURSOR_XYZ[2];

		if (canPut[IDX][IDY][IDZ])
		{
			// put cube (0=false, 1=true, 2... support cube
			isCube[IDX][IDY][IDZ] = 1;
		}

		// update canPut array
		this->update_canPut_flag();

	}
}


void mainController::deleteKey_hit(int keyNum)
{
	if (keyNum == 8 || keyNum == 46) // delete or backspace key
	{
		int IDX = CURSOR_XYZ[0];
		int IDY = CURSOR_XYZ[1];
		int IDZ = CURSOR_XYZ[2];

		isCube[IDX][IDY][IDZ] = 0; // 0-false, 1-cube, 2.. support cube

		this->update_canPut_flag();
	}
}


////////////////////////////////////////////////////////
// utility function ************************************
////////////////////////////////////////////////////////

void mainController::update_canPut_flag()
{
	// bottom cube are always true
	for (int x = 0; x < GRID_X; x++)
	{
		for (int z = 0; z < GRID_Z; z++)
		{
			canPut[x][0][z] = true;
		}
	}


	// check 4 edge at bottom layer
	bool btm_edge[5] = { false, false, false, false, false };

	for (int y = 1; y < GRID_Y; y++)
	{
		for (int x = 0; x < GRID_X; x++)
		{
			for (int z = 0; z < GRID_Z; z++)
			{
				// select target 5 box
				int tX[5];
				int tZ[5];

				tX[0] = x - 1;	tX[1] = x;		tX[2] = x + 1;	tX[3] = x;		tX[4] = x;
				tZ[0] = z;		tZ[1] = z - 1;	tZ[2] = z;		tZ[3] = z + 1;	tZ[4] = z;

				// first *************************************
				if (tX[0] < 0) { btm_edge[0] = false; } // if out of range
				else {
					if (isCube[tX[0]][y - 1][tZ[0]] == 1) // if normal cube exist
					{
						btm_edge[0] = true;
					}
					else
					{
						btm_edge[0] = false;
					}
				}
				// second ************************************
				if (tZ[1] < 0) { btm_edge[1] = false; } // out of range
				else
				{
					if (isCube[tX[1]][y - 1][tZ[1]] == 1) // if normal cube exits
					{
						btm_edge[1] = true;
					}
					else
					{
						btm_edge[1] = false;
					}
				}
				// third **************************************
				if (tX[2] >= GRID_X) { btm_edge[2] = false; } // out of range
				else
				{
					if (isCube[tX[2]][y - 1][tZ[2]] == 1) // if normal cube exist
					{
						btm_edge[2] = true;
					}
					else
					{
						btm_edge[2] = false;
					}
				}
				// fourth ************************************
				if (tZ[3] >= GRID_Z) { btm_edge[3] = false; } // out of range
				else
				{
					if (isCube[tX[3]][y - 1][tZ[3]] == 1) // if normal cube exist
					{
						btm_edge[3] = true;
					}
					else
					{
						btm_edge[3] = false;
					}
				}
				// fifth ****************************************
				// just under the cube
				if (isCube[x][y - 1][z] == 1)// if normal cube exist
				{
					btm_edge[4] = true;
				}
				else
				{
					btm_edge[4] = false;
				}


				//*********************************************
				// change canPut flag
				//*********************************************
				if (btm_edge[0] || btm_edge[1] || btm_edge[2] || btm_edge[3] || btm_edge[4])
				{
					canPut[x][y][z] = true;
				}
				else
				{
					canPut[x][y][z] = false;
				}
				//************************************************
				//************************************************
				//************************************************

			} // for z
		} // for x

		this->delete_cube_by_flag(y);

	}// for y



	//*********************************
	// setup cubes
	 
	this->setup_cubes();
	this->setup_support_block();
	this->culc_support_normal();
	//*********************************
}



void mainController::delete_cube_by_flag(int yLayer)
{
	// reset isCube by canPut flag
	// ( when cube is put, result is same )
	// ( if cube is deleted, result is changed )
	for (int x = 0; x < GRID_X; x++)
	{
		for (int z = 0; z < GRID_Z; z++)
		{
			if (canPut[x][yLayer][z] == false)
			{
				isCube[x][yLayer][z] = 0; // delete cube
			}
		}
	}
}

//********************************************************
//********************************************************
//********************************************************
//********************************************************
//********************************************************
//********************************************************

void mainController::setup_cubes()
{
	// copy pointer
	GLfloat* pass_ptr = cubes_ptr;
	GLfloat* pass_n_ptr = cubes_norm_ptr;
	// count up the number of cubes
	int NC = 0;

	for (int x = 0; x < GRID_X; x++)
	{
		for (int y = 0; y < GRID_Y; y++)
		{
			for (int z = 0; z < GRID_Z; z++)
			{
				if (isCube[x][y][z] == 1)// count up normal cube
				{
					pass_ptr = this->set_cube_vertex(x, y, z, pass_ptr);
					pass_n_ptr = this->set_cube_normal(x, y, z, pass_n_ptr);
					NC++;
				}
			}
		}
	}

	NUM_CUBE = NC;

	printf("num of normal cube %d\n", NC);

}


GLfloat* mainController::set_cube_vertex(int xID, int yID, int zID, GLfloat* ptr)
{
	GLfloat* wPtr = ptr;

	// start corner of cube
	GLfloat sx = (GLfloat)xID - (GLfloat)(GRID_X / 2);
	GLfloat sy = (GLfloat)yID - (GLfloat)(GRID_Y / 2);
	GLfloat sz = (GLfloat)zID - (GLfloat)(GRID_Z / 2);

	GLfloat cbx[8];
	GLfloat cby[8];
	GLfloat cbz[8];

	// cube 8 corner
	// -y plane
	cbx[0] = sx;		cby[0] = sy;	cbz[0] = sz;			//  4 - -7
	cbx[1] = sx;		cby[1] = sy;	cbz[1] = sz + 1.0;		// /	/
	cbx[2] = sx + 1.0;	cby[2] = sy;	cbz[2] = sz + 1.0;		//5 - -6
	cbx[3] = sx + 1.0;	cby[3] = sy;	cbz[3] = sz;			//    
	// +y plane													// 0 - - 3
	cbx[4] = sx;		cby[4] = sy+1.0;	cbz[4] = sz;		// /	/
	cbx[5] = sx;		cby[5] = sy+1.0;	cbz[5] = sz + 1.0;	//1 - - 2
	cbx[6] = sx + 1.0;	cby[6] = sy+1.0;	cbz[6] = sz + 1.0;
	cbx[7] = sx + 1.0;	cby[7] = sy+1.0;	cbz[7] = sz;

	int ELEM[36] = {4, 0, 5, 5, 0, 1, // -x plane
					6, 2, 3, 3, 7, 6, // +x plane
					7, 3, 0, 0, 4, 7, // -z plane
					5, 1, 6, 6, 1, 2, // +z plane
					0, 3, 1, 1, 3, 2, // -y plane
					4, 5, 7, 7, 5, 6};// +y plane


	// set vertes
	for (int i = 0; i < 36; i++)
	{
		*wPtr = cbx[ELEM[i]];	wPtr++;
		*wPtr = cby[ELEM[i]];	wPtr++;
		*wPtr = cbz[ELEM[i]];	wPtr++;
		*wPtr = 1.0;	wPtr++;
	}

	return wPtr;
}


GLfloat* mainController::set_cube_normal(int xID, int yID, int zID, GLfloat* nPtr)
{
	GLfloat* wPtr = nPtr;

	GLfloat N[6][3];
	N[0][0] = -1.0;		N[0][1] = 0.0;		N[0][2] = 0.0; // -x
	N[1][0] = 1.0;		N[1][1] = 0.0;		N[1][2] = 0.0; // +x
	N[2][0] = 0.0;		N[2][1] = 0.0;		N[2][2] = -1.0; // -z
	N[3][0] = 0.0;		N[3][1] = 0.0;		N[3][2] = 1.0; // +z
	N[4][0] = 0.0;		N[4][1] = -1.0;		N[4][2] = 0.0; // -y
	N[5][0] = 0.0;		N[5][1] = 1.0;		N[5][2] = 0.0; // +y

	for (int i = 0; i < 36; i++)
	{
		int ID = i / 6; // 0 - 5
		*wPtr = N[ID][0];	wPtr++; // nx
		*wPtr = N[ID][1];	wPtr++; // ny
		*wPtr = N[ID][2];	wPtr++; // nz
		*wPtr = 0.0;	wPtr++; // N/A
	}

	return wPtr;
}



//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////

void mainController::setup_support_block()
{
	// reset NUM point
	NUM_POINT_SUPPORT = 0;
	GLfloat* wPtr = support_ptr;
	GLfloat* nPtr = support_norm_ptr;

	// clear support
	for (int x = 0; x < GRID_X; x++)
	{
		for (int y = 0; y < GRID_Y; y++)
		{
			for (int z = 0; z < GRID_Z; z++)
			{
				isSupport[x][y][z] = 0;
			}
		}
	}

	// check cube position
	for (int y = 1; y < GRID_Y; y++) // y range 1 ~ 
	{
		for (int x = 0; x < GRID_X; x++)
		{
			for (int z = 0; z < GRID_Z; z++)
			{
				// cube flag
				int yn = isCube[x][y][z];

				if (yn == 0)
				{
					// do nothing
				}
				else if (yn == 1)
				{
					// cube exist, check under layer
					int supportID = this->check_support_pattern(x, y, z);
					// set support block
					isSupport[x][y-1][z] = supportID;

					// set support block vertex
					if (supportID != 0)
					{
						// vertex
						wPtr = this->set_support_vertex(x, (y - 1), z, supportID, wPtr);
					}
				}
			} // for z
		} // for x
	} // for y
}



int mainController::check_support_pattern(int xID, int yID, int zID)
{
	int supportBlockID = 0;
	char LOGIC = 0;
	if (isCube[xID][yID - 1][zID] == 1) // cube exist
	{
		// check just under the cube
		supportBlockID = 0;
	}
	else // no cube just under the cube
	{
		// check 4 direction
		int tX[4];
		int tZ[4];

		tX[0] = xID - 1;	tX[1] = xID;		tX[2] = xID + 1;	tX[3] = xID;
		tZ[0] = zID;		tZ[1] = zID - 1;	tZ[2] = zID;		tZ[3] = zID + 1;

		// check 1
		if (tX[0] >= 0)
		{
			if (isCube[tX[0]][yID - 1][tZ[0]] == 1)
			{
				LOGIC |= 1; // bit | 00000001
			}
		}
		// check 2
		if (tZ[1] >= 0)
		{
			if (isCube[tX[1]][yID - 1][tZ[1]] == 1)
			{
				LOGIC |= 2; // | 00000010
			}
		}
		// check 3
		if (tX[2] < GRID_X)
		{
			if (isCube[tX[2]][yID - 1][tZ[2]] == 1)
			{
				LOGIC |= 4;	// | 00000100
			}
		}
		// check 4
		if (tZ[3] < GRID_Z)
		{
			if (isCube[tX[3]][yID - 1][tZ[3]] == 1)
			{
				LOGIC |= 8;  // | 00001000
			}
		}
	}

	// decide support block
	switch (LOGIC)
	{
	case 1: // 0001 ( -x)
		supportBlockID = 1;
		break;
	case 2: // 0010 ( -z )
		supportBlockID = 2;
		break;
	case 4: // 0100 ( +x )
		supportBlockID = 3;
		break;
	case 8: // 1000 ( +z )
		supportBlockID = 4;
		break;
	case 5: // 0101 (+-x)
		supportBlockID = 11;
		break;
	case 10: //  1010 (+-z)
		supportBlockID = 12;
		break;
	case 3: // 0011 (-x-z)
		supportBlockID = 21;
		break;
	case 6: // 0110 (-z+x)
		supportBlockID = 22;
		break;
	case 12: // 1100 (+x+z)
		supportBlockID = 23;
		break;
	case 9: // 1001 (+z-x)
		supportBlockID = 24;
		break;
	case 14: // 1110 (-z+x+z)
		supportBlockID = 31;
		break;
	case 13: // 1101 (-x+x+z)
		supportBlockID = 32;
		break;
	case 11: // 1011(-x-z+z)
		supportBlockID = 33;
		break;
	case 7: // 0111(-x-z+x)
		supportBlockID = 34;
		break;
	}

	printf("block ID %d\n", supportBlockID);

	return supportBlockID;
}


GLfloat* mainController::set_support_vertex(int xID, int yID, int zID, int sID, GLfloat* ptr)
{
	GLfloat* vP = ptr;
	GLfloat SFT[3];
	int ELEM[100];
	int N_ELEM = 0;

	SFT[0] = (GLfloat)xID - (GLfloat)(GRID_X / 2);
	SFT[1] = (GLfloat)yID - (GLfloat)(GRID_Y / 2);
	SFT[2] = (GLfloat)zID - (GLfloat)(GRID_Z / 2);

	//     0 -- 1 -- 2
	//    /         /
	//   3 -- 4 -- 5
 	//  /         /
	// 6 -- 7 -- 8
	// |    |    |
	//
	//
	//     9 -- 10 -11
	//    /         /
	//   12--13 --14
	//  /         /
	// 15--16 --17 
	// 	
	SPV[0][0] = 0.0;	SPV[0][1] = 1.0;	SPV[0][2] = 0.0;	SPV[0][3] = 1.0;
	SPV[1][0] = 0.5;	SPV[1][1] = 1.0;	SPV[1][2] = 0.0;	SPV[1][3] = 1.0;
	SPV[2][0] = 1.0;	SPV[2][1] = 1.0;	SPV[2][2] = 0.0;	SPV[2][3] = 1.0;
	SPV[3][0] = 0.0;	SPV[3][1] = 1.0;	SPV[3][2] = 0.5;	SPV[3][3] = 1.0;
	SPV[4][0] = 0.5;	SPV[4][1] = 1.0;	SPV[4][2] = 0.5;	SPV[4][3] = 1.0;
	SPV[5][0] = 1.0;	SPV[5][1] = 1.0;	SPV[5][2] = 0.5;	SPV[5][3] = 1.0;
	SPV[6][0] = 0.0;	SPV[6][1] = 1.0;	SPV[6][2] = 1.0;	SPV[6][3] = 1.0;
	SPV[7][0] = 0.5;	SPV[7][1] = 1.0;	SPV[7][2] = 1.0;	SPV[7][3] = 1.0;
	SPV[8][0] = 1.0;	SPV[8][1] = 1.0;	SPV[8][2] = 1.0;	SPV[8][3] = 1.0;

	SPV[9][0] = 0.0;	SPV[9][1] = 0.0;	SPV[9][2] = 0.0;	SPV[9][3] = 1.0;
	SPV[10][0] = 0.5;	SPV[10][1] = 0.0;	SPV[10][2] = 0.0;	SPV[10][3] = 1.0;
	SPV[11][0] = 1.0;	SPV[11][1] = 0.0;	SPV[11][2] = 0.0;	SPV[11][3] = 1.0;
	SPV[12][0] = 0.0;	SPV[12][1] = 0.0;	SPV[12][2] = 0.5;	SPV[12][3] = 1.0;
	SPV[13][0] = 0.5;	SPV[13][1] = 0.0;	SPV[13][2] = 0.5;	SPV[13][3] = 1.0;
	SPV[14][0] = 1.0;	SPV[14][1] = 0.0;	SPV[14][2] = 0.5;	SPV[14][3] = 1.0;
	SPV[15][0] = 0.0;	SPV[15][1] = 0.0;	SPV[15][2] = 1.0;	SPV[15][3] = 1.0;
	SPV[16][0] = 0.5;	SPV[16][1] = 0.0;	SPV[16][2] = 1.0;	SPV[16][3] = 1.0;
	SPV[17][0] = 1.0;	SPV[17][1] = 0.0;	SPV[17][2] = 1.0;	SPV[17][3] = 1.0;

	switch (sID)
	{
	case 1: //  ( -x)
		// add num point
		N_ELEM = 24; // 24point
		// set element
		ELEM[0] = 0;	ELEM[1] = 9;	ELEM[2] = 6;	ELEM[3] = 6;	ELEM[4] = 9;	ELEM[5] = 15;
		ELEM[6] = 0;	ELEM[7] = 6;	ELEM[8] = 8;	ELEM[9] = 8;	ELEM[10] = 2;	ELEM[11] = 0;
		ELEM[12] = 15;	ELEM[13] = 8;	ELEM[14] = 6;
		ELEM[15] = 2;	ELEM[16] = 9;	ELEM[17] = 0;
		ELEM[18] = 8;	ELEM[19] = 15;	ELEM[20] = 2;	ELEM[21] = 2;	ELEM[22] = 15;	ELEM[23] = 9;
		break;
	case 2: //  ( -z )
		N_ELEM = 24;
		ELEM[0] = 2;	ELEM[1] = 11;	ELEM[2] = 0;	ELEM[3] = 0;	ELEM[4] = 11;	ELEM[5] = 9;
		ELEM[6] = 0;	ELEM[7] = 6;	ELEM[8] = 2;	ELEM[9] = 2;	ELEM[10] = 6;	ELEM[11] = 8;
		ELEM[12] = 9;	ELEM[13] = 6;	ELEM[14] = 0;
		ELEM[15] = 8;	ELEM[16] = 11;	ELEM[17] = 2;
		ELEM[18] = 6;	ELEM[19] = 9;	ELEM[20] = 8;	ELEM[21] = 8;	ELEM[22] = 9;	ELEM[23] = 11;
		break;
	case 3: //  ( +x )
		N_ELEM = 24;
		ELEM[0] = 8;	ELEM[1] = 17;	ELEM[2] = 2;	ELEM[3] = 2;	ELEM[4] = 17;	ELEM[5] = 11;
		ELEM[6] = 6;	ELEM[7] = 8;	ELEM[8] = 0;	ELEM[9] = 0;	ELEM[10] = 8;	ELEM[11] = 2;
		ELEM[12] = 6;	ELEM[13] = 17;	ELEM[14] = 8;
		ELEM[15] = 11;	ELEM[16] = 0;	ELEM[17] = 2;
		ELEM[18] = 0;	ELEM[19] = 11;	ELEM[20] = 6;	ELEM[21] = 6;	ELEM[22] = 11;	ELEM[23] = 17;
		break;
	case 4: //  ( +z )
		N_ELEM = 24;
		ELEM[0] = 6;	ELEM[1] = 15;	ELEM[2] = 8;	ELEM[3] = 8;	ELEM[4] = 15;	ELEM[5] = 17;
		ELEM[6] = 0;	ELEM[7] = 6;	ELEM[8] = 2;	ELEM[9] = 2;	ELEM[10] = 6;	ELEM[11] = 8;
		ELEM[12] = 17;	ELEM[13] = 2;	ELEM[14] = 8;
		ELEM[15] = 0;	ELEM[16] = 15;	ELEM[17] = 6;
		ELEM[18] = 2;	ELEM[19] = 17;	ELEM[20] = 0;	ELEM[21] = 0;	ELEM[22] = 17;	ELEM[23] = 15;
		break;
	case 11: // ( +-x )
		N_ELEM = 48;
		ELEM[0] = 0;	ELEM[1] = 9;	ELEM[2] = 6;	ELEM[3] = 6;	ELEM[4] = 9;	ELEM[5] = 15;
		ELEM[6] = 1;	ELEM[7] = 0;	ELEM[8] = 7;	ELEM[9] = 7;	ELEM[10] = 0;	ELEM[11] = 6;
		ELEM[12] = 15;	ELEM[13] = 7;	ELEM[14] = 6;
		ELEM[15] = 1;	ELEM[16] = 9;	ELEM[17] = 0;
		ELEM[18] = 7;	ELEM[19] = 15;	ELEM[20] = 1;	ELEM[21] = 1;	ELEM[22] = 15;	ELEM[23] = 9;
		ELEM[24] = 8;	ELEM[25] = 17;	ELEM[26] = 2;	ELEM[27] = 2;	ELEM[28] = 17;	ELEM[29] = 11;
		ELEM[30] = 7;	ELEM[31] = 8;	ELEM[32] = 1;	ELEM[33] = 1;	ELEM[34] = 8;	ELEM[35] = 2;
		ELEM[36] = 7;	ELEM[37] = 17;	ELEM[38] = 8;
		ELEM[39] = 11;	ELEM[40] = 1;	ELEM[41] = 2;
		ELEM[42] = 1;	ELEM[43] = 11;	ELEM[44] = 7;	ELEM[45] = 7;	ELEM[46] = 11;	ELEM[47] = 17;
		break;
	case 12: // ( +-z )
		N_ELEM = 48;
		ELEM[0] = 6;	ELEM[1] = 15;	ELEM[2] = 8;	ELEM[3] = 8;	ELEM[4] = 15;	ELEM[5] = 17;
		ELEM[6] = 3;	ELEM[7] = 6;	ELEM[8] = 5;	ELEM[9] = 5;	ELEM[10] = 6;	ELEM[11] = 8;
		ELEM[12] = 17;	ELEM[13] = 5;	ELEM[14] = 8;
		ELEM[15] = 3;	ELEM[16] = 15;	ELEM[17] = 6;
		ELEM[18] = 5;	ELEM[19] = 17;	ELEM[20] = 3;	ELEM[21] = 3;	ELEM[22] = 17;	ELEM[23] = 15;
		ELEM[24] = 2;	ELEM[25] = 11;	ELEM[26] = 0;	ELEM[27] = 0;	ELEM[28] = 11;	ELEM[29] = 9;
		ELEM[30] = 0;	ELEM[31] = 3;	ELEM[32] = 2;	ELEM[33] = 2;	ELEM[34] = 3;	ELEM[35] = 5;
		ELEM[36] = 5;	ELEM[37] = 11;	ELEM[38] = 2;
		ELEM[39] = 9;	ELEM[40] = 3;	ELEM[41] = 0;
		ELEM[42] = 3;	ELEM[43] = 9;	ELEM[44] = 5;	ELEM[45] = 5;	ELEM[46] = 9;	ELEM[47] = 11;
		break;
	case 21: // (-x-z )
		N_ELEM = 18;
		ELEM[0] = 0;	ELEM[1] = 6;	ELEM[2] = 2;	ELEM[3] = 2;	ELEM[4] = 6;	ELEM[5] = 8;
		ELEM[6] = 9;	ELEM[7] = 6;	ELEM[8] = 0;
		ELEM[9] = 0;	ELEM[10] = 2;	ELEM[11] = 9;
		ELEM[12] = 9;	ELEM[13] = 8;	ELEM[14] = 6;
		ELEM[15] = 8;	ELEM[16] = 9;	ELEM[17] = 2;
		break;
	case 22: // (-z+x)
		N_ELEM = 18;
		ELEM[0] = 0;	ELEM[1] = 6;	ELEM[2] = 2;	ELEM[3] = 2;	ELEM[4] = 6;	ELEM[5] = 8;
		ELEM[6] = 11;	ELEM[7] = 0;	ELEM[8] = 2;
		ELEM[9] = 2;	ELEM[10] = 8;	ELEM[11] = 11;
		ELEM[12] = 6;	ELEM[13] = 11;	ELEM[14] = 8;
		ELEM[15] = 0;	ELEM[16] = 11;	ELEM[17] = 6;
		break;
	case 23: // (+x+z)
		N_ELEM = 18;
		ELEM[0] = 0;	ELEM[1] = 6;	ELEM[2] = 2;	ELEM[3] = 2;	ELEM[4] = 6;	ELEM[5] = 8;
		ELEM[6] = 6;	ELEM[7] = 17;	ELEM[8] = 8;
		ELEM[9] = 8;	ELEM[10] = 17;	ELEM[11] = 2;
		ELEM[12] = 0;	ELEM[13] = 17;	ELEM[14] = 6;
		ELEM[15] = 2;	ELEM[16] = 17;	ELEM[17] = 0;
		break;
	case 24: // (+z-x)
		N_ELEM = 18;
		ELEM[0] = 0;	ELEM[1] = 6;	ELEM[2] = 2;	ELEM[3] = 2;	ELEM[4] = 6;	ELEM[5] = 8;
		ELEM[6] = 15;	ELEM[7] = 8;	ELEM[8] = 6;
		ELEM[9] = 6;	ELEM[10] = 0;	ELEM[11] = 15;
		ELEM[12] = 15;	ELEM[13] = 2;	ELEM[14] = 8;
		ELEM[15] = 0;	ELEM[16] = 2;	ELEM[17] = 15;
		break;
	case 31: // (eg -x)
		N_ELEM = 48;
		ELEM[0] = 6;	ELEM[1] = 15;	ELEM[2] = 8;	ELEM[3] = 8;	ELEM[4] = 15;	ELEM[5] = 17;
		ELEM[6] = 3;	ELEM[7] = 6;	ELEM[8] = 5;	ELEM[9] = 5;	ELEM[10] = 6;	ELEM[11] = 8;
		ELEM[12] = 17;	ELEM[13] = 5;	ELEM[14] = 8;
		ELEM[15] = 3;	ELEM[16] = 15;	ELEM[17] = 6;
		ELEM[18] = 5;	ELEM[19] = 17;	ELEM[20] = 3;	ELEM[21] = 3;	ELEM[22] = 17;	ELEM[23] = 15;
		ELEM[24] = 2;	ELEM[25] = 11;	ELEM[26] = 0;	ELEM[27] = 0;	ELEM[28] = 11;	ELEM[29] = 9;
		ELEM[30] = 0;	ELEM[31] = 3;	ELEM[32] = 2;	ELEM[33] = 2;	ELEM[34] = 3;	ELEM[35] = 5;
		ELEM[36] = 5;	ELEM[37] = 11;	ELEM[38] = 2;
		ELEM[39] = 9;	ELEM[40] = 3;	ELEM[41] = 0;
		ELEM[42] = 3;	ELEM[43] = 9;	ELEM[44] = 5;	ELEM[45] = 5;	ELEM[46] = 9;	ELEM[47] = 11;
		break;
	case 32: // (eg -z )
		N_ELEM = 48;
		ELEM[0] = 0;	ELEM[1] = 9;	ELEM[2] = 6;	ELEM[3] = 6;	ELEM[4] = 9;	ELEM[5] = 15;
		ELEM[6] = 1;	ELEM[7] = 0;	ELEM[8] = 7;	ELEM[9] = 7;	ELEM[10] = 0;	ELEM[11] = 6;
		ELEM[12] = 15;	ELEM[13] = 7;	ELEM[14] = 6;
		ELEM[15] = 1;	ELEM[16] = 9;	ELEM[17] = 0;
		ELEM[18] = 7;	ELEM[19] = 15;	ELEM[20] = 1;	ELEM[21] = 1;	ELEM[22] = 15;	ELEM[23] = 9;
		ELEM[24] = 8;	ELEM[25] = 17;	ELEM[26] = 2;	ELEM[27] = 2;	ELEM[28] = 17;	ELEM[29] = 11;
		ELEM[30] = 7;	ELEM[31] = 8;	ELEM[32] = 1;	ELEM[33] = 1;	ELEM[34] = 8;	ELEM[35] = 2;
		ELEM[36] = 7;	ELEM[37] = 17;	ELEM[38] = 8;
		ELEM[39] = 11;	ELEM[40] = 1;	ELEM[41] = 2;
		ELEM[42] = 1;	ELEM[43] = 11;	ELEM[44] = 7;	ELEM[45] = 7;	ELEM[46] = 11;	ELEM[47] = 17;
		break;
	case 33: // (eg +x )
		N_ELEM = 48;
		ELEM[0] = 6;	ELEM[1] = 15;	ELEM[2] = 8;	ELEM[3] = 8;	ELEM[4] = 15;	ELEM[5] = 17;
		ELEM[6] = 3;	ELEM[7] = 6;	ELEM[8] = 5;	ELEM[9] = 5;	ELEM[10] = 6;	ELEM[11] = 8;
		ELEM[12] = 17;	ELEM[13] = 5;	ELEM[14] = 8;
		ELEM[15] = 3;	ELEM[16] = 15;	ELEM[17] = 6;
		ELEM[18] = 5;	ELEM[19] = 17;	ELEM[20] = 3;	ELEM[21] = 3;	ELEM[22] = 17;	ELEM[23] = 15;
		ELEM[24] = 2;	ELEM[25] = 11;	ELEM[26] = 0;	ELEM[27] = 0;	ELEM[28] = 11;	ELEM[29] = 9;
		ELEM[30] = 0;	ELEM[31] = 3;	ELEM[32] = 2;	ELEM[33] = 2;	ELEM[34] = 3;	ELEM[35] = 5;
		ELEM[36] = 5;	ELEM[37] = 11;	ELEM[38] = 2;
		ELEM[39] = 9;	ELEM[40] = 3;	ELEM[41] = 0;
		ELEM[42] = 3;	ELEM[43] = 9;	ELEM[44] = 5;	ELEM[45] = 5;	ELEM[46] = 9;	ELEM[47] = 11;
		break;
	case 34: // (eg +z )
		N_ELEM = 48;
		ELEM[0] = 0;	ELEM[1] = 9;	ELEM[2] = 6;	ELEM[3] = 6;	ELEM[4] = 9;	ELEM[5] = 15;
		ELEM[6] = 1;	ELEM[7] = 0;	ELEM[8] = 7;	ELEM[9] = 7;	ELEM[10] = 0;	ELEM[11] = 6;
		ELEM[12] = 15;	ELEM[13] = 7;	ELEM[14] = 6;
		ELEM[15] = 1;	ELEM[16] = 9;	ELEM[17] = 0;
		ELEM[18] = 7;	ELEM[19] = 15;	ELEM[20] = 1;	ELEM[21] = 1;	ELEM[22] = 15;	ELEM[23] = 9;
		ELEM[24] = 8;	ELEM[25] = 17;	ELEM[26] = 2;	ELEM[27] = 2;	ELEM[28] = 17;	ELEM[29] = 11;
		ELEM[30] = 7;	ELEM[31] = 8;	ELEM[32] = 1;	ELEM[33] = 1;	ELEM[34] = 8;	ELEM[35] = 2;
		ELEM[36] = 7;	ELEM[37] = 17;	ELEM[38] = 8;
		ELEM[39] = 11;	ELEM[40] = 1;	ELEM[41] = 2;
		ELEM[42] = 1;	ELEM[43] = 11;	ELEM[44] = 7;	ELEM[45] = 7;	ELEM[46] = 11;	ELEM[47] = 17;
		break;
	}


	// set vertex and add num point ********************
	NUM_POINT_SUPPORT += N_ELEM;

	for (int i = 0; i < N_ELEM; i++)
	{
		*vP = SPV[ELEM[i]][0] + SFT[0];	vP++; // x
		*vP = SPV[ELEM[i]][1] + SFT[1];	vP++; // y
		*vP = SPV[ELEM[i]][2] + SFT[2];	vP++; // z
		*vP = SPV[ELEM[i]][3];	vP++; // w
	}
	//****************************************************

	return vP;
}



void mainController::culc_support_normal()
{
	printf("num support point %d\n", NUM_POINT_SUPPORT);

	GLfloat* vPtr = support_ptr;
	GLfloat* nPtr = support_norm_ptr;

	int CT = 0;

	GLfloat tempV[3][4];
	GLfloat vec[2][3];
	GLfloat cross[3];

	while (1)
	{
		// v1
		tempV[0][0] = *vPtr;	vPtr++;
		tempV[0][1] = *vPtr;	vPtr++;
		tempV[0][2] = *vPtr;	vPtr++;
		tempV[0][3] = *vPtr;	vPtr++;
		CT++;
		// v2
		tempV[1][0] = *vPtr;	vPtr++;
		tempV[1][1] = *vPtr;	vPtr++;
		tempV[1][2] = *vPtr;	vPtr++;
		tempV[1][3] = *vPtr;	vPtr++;
		CT++;
		// v2
		tempV[2][0] = *vPtr;	vPtr++;
		tempV[2][1] = *vPtr;	vPtr++;
		tempV[2][2] = *vPtr;	vPtr++;
		tempV[2][3] = *vPtr;	vPtr++;
		CT++;

		// calc 2 vec
		vec[0][0] = tempV[2][0] - tempV[1][0];
		vec[0][1] = tempV[2][1] - tempV[1][1];
		vec[0][2] = tempV[2][2] - tempV[1][2];

		vec[1][0] = tempV[0][0] - tempV[1][0];
		vec[1][1] = tempV[0][1] - tempV[1][1];
		vec[1][2] = tempV[0][2] - tempV[1][2];


		// calc cross product
		cross[0] = vec[0][1] * vec[1][2] - vec[0][2] * vec[1][1];
		cross[1] = vec[0][2] * vec[1][0] - vec[0][0] * vec[1][2];
		cross[2] = vec[0][0] * vec[1][1] - vec[0][1] * vec[1][0];

		// normalize cross vec
		double dist = sqrt(cross[0] * cross[0] + cross[1] * cross[1] + cross[2] * cross[2]);
		cross[0] /= dist;
		cross[1] /= dist;
		cross[2] /= dist;

		// set normal
		*nPtr = cross[0];	nPtr++;
		*nPtr = cross[1];	nPtr++;
		*nPtr = cross[2];	nPtr++;
		*nPtr = 1.0;	nPtr++;

		*nPtr = cross[0];	nPtr++;
		*nPtr = cross[1];	nPtr++;
		*nPtr = cross[2];	nPtr++;
		*nPtr = 1.0;	nPtr++;

		*nPtr = cross[0];	nPtr++;
		*nPtr = cross[1];	nPtr++;
		*nPtr = cross[2];	nPtr++;
		*nPtr = 1.0;	nPtr++;

		if (CT >= NUM_POINT_SUPPORT)
		{
			break;
		}
	}

}



void mainController::save_button()
{
	// check num block
	if (NUM_CUBE == 0)
	{
		// show alert window
		printf("NO CUBE EXIST.\n");
	}
	else
	{
		// save STL file
		std::wstring* savePath = save_load_obj->open_save_panel(h_mainWin, TEXT("stl"), TEXT("STL files\0*.stl\0\0"));
		wprintf(L"%ls\n", savePath->c_str());

		// check STL data status
		int NPT_CUBE = NUM_CUBE * 36;
		int NPT_SUPPORT = NUM_POINT_SUPPORT;


		// open FILE
		FILE* fp;
		
		if (_wfopen_s(&fp, savePath->c_str(), L"wb") != 0)
		{
			printf("can't open file\n");
			return;
		}

		//***************************************************
		///// write STL file ********************************
		//***************************************************

		// write header (80 byte)
		char head[80] = "This is STL file header.";
		fwrite(head, sizeof(char), 80, fp);

		// num of triangles ( 4byte, unsigned int )
		unsigned int NTRI = (NUM_CUBE * 12) + (NUM_POINT_SUPPORT / 3);
		fwrite(&NTRI, sizeof(unsigned int), 1, fp);

		// get normal&vertex data (from cubes_norm_ptr )
		float* readV = cubes_ptr;
		float* readN = cubes_norm_ptr;
		char unusedArea[2] = {0, 0};


		// cube
		for (int i = 0; i < NUM_CUBE; i++)
		{
			for (int PLANE = 0; PLANE < 6; PLANE++)
			{
				for (int TRI = 0; TRI < 2; TRI++)
				{
					//******************
					//** YZ axis flipped
					//******************

					float N1[3];
					N1[0] = *readN;	readN++; // nx
					N1[2] = *readN;	readN++; // ny
					N1[1] = -(*readN);	readN++; // nz
					readN++;
					readN++;	readN++;	readN++;	readN++;// skip point2
					readN++;	readN++;	readN++;	readN++;// skip point3
					// write normal
					fwrite(N1, sizeof(float), 3, fp);

					float V1[3][3];
					V1[0][0] = (*readV)*10.0;	readV++;
					V1[0][2] = (*readV)*10.0;	readV++;
					V1[0][1] = (*readV)*-10.0;	readV++;
					readV++;

					V1[1][0] = (*readV)*10.0;	readV++;
					V1[1][2] = (*readV)*10.0;	readV++;
					V1[1][1] = (*readV)*-10.0;	readV++;
					readV++;

					V1[2][0] = (*readV)*10.0;	readV++;
					V1[2][2] = (*readV)*10.0;	readV++;
					V1[2][1] = (*readV)*-10.0;	readV++;
					readV++;

					fwrite(&V1[0][0], sizeof(float), 3, fp);
					fwrite(&V1[1][0], sizeof(float), 3, fp);
					fwrite(&V1[2][0], sizeof(float), 3, fp);

					// skip 2byte
					fwrite(unusedArea, sizeof(char), 2, fp);
				}
			}
		}


		int ST = NUM_POINT_SUPPORT / 3;
		float* readSV = support_ptr;
		float* readSN = support_norm_ptr;

		for (int T = 0; T < ST; T++)
		{
			float N[3];
			N[0] = *readSN; readSN++;
			N[2] = *readSN;	readSN++; // yz flipped
			N[1] = -(*readSN);	readSN++;
			readSN++;
			readSN++;	readSN++;	readSN++;	readSN++; // skip p1
			readSN++;	readSN++;	readSN++;	readSN++; // skip p2

			fwrite(N, sizeof(float), 3, fp);

			float V[3][3];
			V[0][0] = (*readSV) * 10.0;	readSV++;
			V[0][2] = (*readSV) * 10.0;	readSV++;
			V[0][1] = (*readSV) * -10.0;	readSV++;
			readSV++;
			V[1][0] = (*readSV) * 10.0;	readSV++;
			V[1][2] = (*readSV) * 10.0;	readSV++;
			V[1][1] = (*readSV) * -10.0;	readSV++;
			readSV++;
			V[2][0] = (*readSV) * 10.0;	readSV++;
			V[2][2] = (*readSV) * 10.0;	readSV++;
			V[2][1] = (*readSV) * -10.0;	readSV++;
			readSV++;

			fwrite(&V[0][0], sizeof(float), 3, fp);
			fwrite(&V[1][0], sizeof(float), 3, fp);
			fwrite(&V[2][0], sizeof(float), 3, fp);

			// skip 2byte
			fwrite(unusedArea, sizeof(char), 2, fp);
		}


		////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////

		// write out file
		fclose(fp);


	}
	
	// focus back to mainWindow to activate key-down
	SetFocus(h_mainWin);
}
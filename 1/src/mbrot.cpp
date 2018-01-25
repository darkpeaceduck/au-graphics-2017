#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glui.h>

extern "C" {
#include "util.h"

void draw(void);
void idle_handler(void);
void key_handler(unsigned char key, int x, int y);
void bn_handler(int bn, int state, int x, int y);
void mouse_handler(int x, int y);

unsigned int prog;
float cx = 0.7, cy = 0.0;
float scale = 2.2;
int iter = 70;
const float zoom_factor = 0.025;
float th_rad = 4.0;
GLUI_Spinner *iter_spinner;
GLUI_Spinner *rad_spinner;

int main(int argc, char **argv) {
	void *img;
	
	/* initialize glut */
	glutInitWindowSize(800, 600);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	int win = glutCreateWindow("Mandelbrot");
	GLUI *glui = GLUI_Master.create_glui_subwindow(win,
	                     GLUI_SUBWINDOW_TOP);
	 iter_spinner =
	    new GLUI_Spinner( glui, "Iterations:", &iter);
	  iter_spinner->set_int_limits( 0, 500 );
	  iter_spinner->set_alignment( GLUI_ALIGN_RIGHT );
	  iter_spinner->set_int_val(iter);
	  iter_spinner->set_speed(0.02f);
	 rad_spinner =
	    new GLUI_Spinner( glui, "Radious:", &th_rad);
	 rad_spinner->set_float_limits( 0, 100.0f);
	 rad_spinner->set_alignment( GLUI_ALIGN_RIGHT );
	 rad_spinner->set_int_val(th_rad * 10);
	 rad_spinner->set_speed(0.1f);

	glui->set_main_gfx_window(win);
	GLUI_Master.auto_set_viewport();

	glutDisplayFunc(draw);
	glutIdleFunc(idle_handler);
	glutKeyboardFunc(key_handler);
	glutMouseFunc(bn_handler);
	glutMotionFunc(mouse_handler);

	/* load the 1D palette texture */
	glBindTexture(GL_TEXTURE_1D, 1);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	
	if(!(img = load_image("res/pal.ppm", 0, 0))) {
		return EXIT_FAILURE;
	}
	glTexImage1D(GL_TEXTURE_1D, 0, 4, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, img);
	free(img);

	glEnable(GL_TEXTURE_1D);

	/* load and set the mandelbrot shader */
	if(!(prog = setup_shader("res/mbrot.glsl"))) {
		return EXIT_FAILURE;
	}
	set_uniform1i(prog, "iter", iter);

	glutMainLoop();
	return 0;
}

void update_iter() {
    set_uniform1i(prog, "iter", iter);
}

void update_rad() {
    set_uniform1f(prog, "rad", th_rad );
}

void update() {
    iter = iter_spinner->get_int_val();
    th_rad = rad_spinner->get_float_val() / 10;
    update_iter();
    update_rad();
}

void draw(void) {
    update();

	set_uniform2f(prog, "center", cx, cy);
	set_uniform1f(prog, "scale", scale);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();

	glutSwapBuffers();
}

void idle_handler(void) {
	glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y) {
	switch(key) {
	case 27:
	case 'q':
	case 'Q':
		exit(0);

	case '=':
		if(1) {
			iter += 10;
		} else {
	case '-':
			iter -= 10;
			if(iter < 0) iter = 0;
		}
		update_iter();
		break;

	default:
		break;
	}
}

int which_bn;
float px, py;

void bn_handler(int bn, int state, int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	px = 2.0 * ((float)x / (float)xres - 0.5);
	py = 2.0 * ((float)y / (float)yres - 0.5);
	which_bn = bn;

	if(which_bn == 3) {
		scale *= 1 - zoom_factor * 2.0;
	} else if(which_bn == 4) {
		scale *= 1 + zoom_factor * 2.0;;
	}
	draw();
}

void mouse_handler(int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	float fx = 2.0 * ((float)x / (float)xres - 0.5);
	float fy = 2.0 * ((float)y / (float)yres - 0.5);

	if(which_bn == 0) {
		cx += (fx - px) * scale / 2.0;
		cy -= (fy - py) * scale / 2.0;
	} else if(which_bn == 1) {
		scale *= (fy - py < 0.0) ? 1 - zoom_factor : 1 + zoom_factor;
	}

	px = fx;
	py = fy;
	draw();
}
}

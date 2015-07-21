/*
 * desktop_controller.c
 *
 *  Created on: Apr 17, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <linux/input.h>

#define DESKTOP_CONTROLLER_NODE "/dev/input/event5"
#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2

/*
#define BTN_TRIGGER		0x120
#define BTN_THUMB		0x121
#define BTN_THUMB2		0x122
#define BTN_TOP			0x123
#define BTN_TOP2		0x124
#define BTN_PINKIE		0x125
#define BTN_BASE		0x126
#define BTN_BASE2		0x127
#define BTN_BASE3		0x128
#define BTN_BASE4		0x129
#define BTN_BASE5		0x12a
#define BTN_BASE6		0x12b
 */

typedef struct color {
	float r, g, b;
} COLOR;


int fd_desktop_controller;
int position[3];
int length_x, length_y;
int max_x = -1000, min_x = 1000;
int max_y = -1000, min_y = 1000;
int winId;
int point_radius = 10;
int width, height;
bool isSync = false, isExist = false;
struct input_event ev[64];
pthread_mutex_t mutex;
COLOR color;
FILE *cali;

void *receive_desktop_controller() {
	int i, temp_x, temp_y;
	int center_x, center_y;
	while(!isExist) {
		size_t rb = read(fd_desktop_controller, ev, sizeof(ev));

		for(i=0; i<(int)(rb/sizeof(struct input_event));i++) {
			center_x = (max_x + min_x)/2;
			center_y = (max_y + min_y)/2;
			//printf("code %d, type: %d \n", ev[i].code, ev[i].type);
			//printf("-->type: %d, code 0x%x, (%d)\n", ev[i].type, ev[i].code, ev[i].value);

			if(EV_KEY == ev[i].type) {
				if(ev[i].value == KEY_PRESS) {
					printf("KEY press\n");
					//printf("code %d \n", ev[i].code);
					switch(ev[i].code) {
						case BTN_TRIGGER:
							color.r = 1;
							color.g = 0;
							color.b = 0;
							printf("BTN_TRIGGER\n");
							break;
						case BTN_THUMB:
							color.r = 1;
							color.g = 0;
							color.b = 0.5;
							printf("BTN_THUMB\n");
							break;
						case BTN_THUMB2:
							color.r = 1;
							color.g = 0.5;
							color.b = 0;
							printf("BTN_THUMB2\n");
							break;
						case BTN_TOP:
							color.r = 1;
							color.g = 1;
							color.b = 0;
							printf("BTN_TOP\n");
							break;
						case BTN_TOP2:
							color.r = 1;
							color.g = 1;
							color.b = 0.7;
							printf("BTN_TOP2\n");
							break;
						case BTN_PINKIE:
							color.r = 0;
							color.g = 1;
							color.b = 0;
							printf("BTN_PINKIE\n");
							break;
						case BTN_BASE:
							color.r = 0.6;
							color.g = 1;
							color.b = 0;
							printf("BTN_BASE\n");
							break;
						case BTN_BASE2:
							color.r = 0;
							color.g = 1;
							color.b = 0.6;
							printf("BTN_BASE2\n");
							break;
						case BTN_BASE3:
							color.r = 1;
							color.g = 0;
							color.b = 1;
							printf("BTN_BASE3\n");
							break;
						case BTN_BASE4:
							color.r = 1;
							color.g = 0.7;
							color.b = 1;
							printf("BTN_BASE4\n");
							break;
						case BTN_BASE5:
							color.r = 0.5;
							color.g = 0;
							color.b = 1;
							printf("BTN_BASE5\n");
							printf("max_x: %d, min_x: %d\n", max_x, min_x);
							printf("max_y: %d, min_y: %d\n", max_y, min_y);
							fseek(cali, 0, SEEK_SET);
							fprintf(cali, "%d %d %d %d\n", max_x, min_x, max_y, min_y);

							break;
						case BTN_BASE6:
							color.r = 0;
							color.g = 0.5;
							color.b = 1;
							printf("BTN_BASE6\n");
							break;
						case BTN_START:
							printf("max_x: %d, min_x: %d\n", max_x, min_x);
							printf("max_y: %d, min_y: %d\n", max_y, min_y);
							fseek(cali, 0, SEEK_SET);
							fprintf(cali, "%d %d %d %d\n", max_x, min_x, max_y, min_y);
							break;
					}
					//write(m_fd_rfcomm, &key, 1);
				}else if(ev[i].value == KEY_RELEASE) {
					printf("Key release\n");
				}
			}
			else if (EV_ABS == ev[i].type) {
				if (ABS_X == ev[i].code) {
					temp_x = ev[i].value;
					printf("x: %d\n", temp_x);
					if (temp_x < min_x)
						min_x = temp_x;
					else if (temp_x > max_x)
						max_x = temp_x;

					position[0] = temp_x - center_x;

				} else if (ABS_Y == ev[i].code) {
					temp_y = ev[i].value;
					if (temp_y < min_y)
						min_y = temp_y;
					else if (temp_y > max_y)
						max_y = temp_y;

					position[1] = ev[i].value - center_y;
					printf("y: %d\n", temp_y);
				}
			}
			else if (EV_SYN == ev[i].type)
				isSync = true;

		}
		pthread_mutex_unlock(&mutex);
		isSync = false;
		usleep(100);
	}
	printf("exit receive_desktop_controller\n");
	//pthread_exit(NULL);
	return NULL;
}

void init() {
	char str_event[20], cmd[50];
	char *tmp;
	FILE *pipe_dev;

	pipe_dev = popen("cat /proc/bus/input/devices | grep -A10 \"Microsoft X-Box 360 pad\" | grep Handlers | awk '{print $2}'", "r");
	//pipe_dev = popen("cat /proc/bus/input/devices | grep -A10 \"CH PRODUCTS CH PRODUCTS IP DESKTOP CONTROLLER\" | grep Handlers | awk '{print $2}'", "r");
	if (!pipe_dev) {
		printf("Can't find Desktop controller device\n");
		pclose(pipe_dev);
		exit(0);
	}

	fgets(str_event, sizeof(str_event)-1, pipe_dev);
	tmp = strstr(str_event, "event");
	tmp[strlen(tmp)-1] = '\0';
	sprintf(cmd, "/dev/input/%s", strstr(str_event, "event"));
	pclose(pipe_dev);
	if((fd_desktop_controller=open(cmd,O_RDONLY))<0) {
		printf("Couldn't open Desktop controller device\n");

		exit(0);
	}
	else
		printf("Desktop controller open success!!\n");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);


//	glutInitWindowSize(500, 500);
	cali=fopen("cali.txt", "r+w");
	if (!cali) {
		printf("cali.txt not exist\n");
		cali = fopen("cali.txt", "w");
		glutInitWindowSize(500, 500);
	}
	else {
		fseek(cali, 0, 0);
		fscanf(cali, "%d %d %d %d", &max_x, &min_x, &max_y, &min_y);
		printf("max_x: %d, min_x: %d\n", max_x, min_x);
		printf("max_y: %d, min_y: %d\n", max_y, min_y);
		glutInitWindowSize((max_x-min_x)/2+point_radius, (max_y-min_y)/2+point_radius);
	}


	glutInitWindowPosition(200, 200);
	winId = glutCreateWindow("Desktop Controller");

	glClearColor(1, 1, 1, 1);
	glShadeModel(GL_FLAT);

	pthread_t rev_thread;
	color.r = 1;
	color.g = 0;
	color.b = 0;

	pthread_mutex_init(&mutex, NULL);

	int ret = pthread_create(&rev_thread, NULL, receive_desktop_controller, NULL);
	if (ret != 0) {
		perror("Thread creation failed\n");
		exit(0);
	}
}

void display() {
	printf("display........\n");
	while (true) {
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0, 1.0, 1.0);

		/*X-axis*/
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(width, 0, 0);
		glVertex3f(-width, 0, 0);
		glEnd();
		/*Y-axis*/
		glColor3f(0, 0.0, 0);
		glBegin(GL_LINES);
		glVertex3f(0, height, 0);
		glVertex3f(0, -height, 0);
		glEnd();

		pthread_mutex_lock(&mutex);

		glPushMatrix();
		glColor3f(color.r, color.g, color.b);
		glTranslatef(position[0]/2, position[1]/2, position[2]);
		glutSolidSphere(point_radius, 20, 160);
		glPopMatrix();

		pthread_mutex_unlock(&mutex);
		glutSwapBuffers();

		usleep(100);
	}
}

void reshape(int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	printf("w, h=%d, %d\n", w, h);
	//printf("fov = %f\n", atan((float)h/w)/M_PI*180);
	//gluPerspective(atan((float)h/w)/M_PI*180, h/w, 0, 1100);
	glOrtho(-w/2, w/2, h/2, -h/2, 10, -5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0, 0, 1020, 0, 0, 0, 0, 1, 0);
}

void control_event(int sig){
	printf("ctrl + c event\n");
	isExist = true;
	usleep(1000);

	fclose(cali);
	close(fd_desktop_controller);
	kill(getpid(), SIGTERM);
	//glutDestroyWindow(winId);
}


int main(int argc, char **argv)
{
	(void)signal(SIGINT,control_event);
	(void)signal(SIGQUIT,control_event);

	glutInit(&argc, argv);

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	fclose(cali);
	return 0;
}

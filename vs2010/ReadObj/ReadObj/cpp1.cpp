  #define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include "glut.h"
#include "object.h" 
#pragma comment(linker,"/subsystem:windows /entry:mainCRTStartup")

map<string,Object> objmap;
set<string> objname;
string cd;
map<string,Material> matname;
GLfloat dx = 0,dy = 0,dz = 0;
GLfloat ax = 0,ay = 0,az = 0;
GLint mx = 0,my = 0;
GLint MouseDown = 0;
GLfloat aspect = 1;

int facenum = 0;
double height = 120 / 2;
double frontbot = 195 / 2;
double width = 200 / 2;
double tempx, tempy, tempz;
double tempxmax = 0.0, tempymax = 0.0, tempzmax = 0.0;
void myIdle()
{
	Sleep(10);
	glutPostRedisplay();
}

void myReshape(int width,int height)
{
	aspect = (float)width/(height?height:1);
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75,aspect,1,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,10,0,0,0,0,1,0);
}

void myKeyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 'w':
		dz+=5.0f;
		break;
	case 's':
		dz-=5.0f;
		break;
	case 'a':
		dx-=5.0f;
		break;
	case 'd':
		dx+=5.0f;
		break;
	case 'q':
		dy+=5.0f;
		break;
	case 'e':
		dy-=5.0f;
		break;
	}
}

void myMouse(int button,int state,int x,int y)
{
	if(button==GLUT_DOWN) MouseDown = 1,mx = x,my = y;
	else if(button==GLUT_WHEEL_UP)
	{
		dz+=10.0f;
	}
	else if(button==GLUT_WHEEL_DOWN)
	{
		dz-=10.0f;
	}
	else
	{
		MouseDown = 0;
	}
}

void myMotion(int x,int y)
{
	if(MouseDown)
	{
		ax += (y-my)/5.0f;
		ay += (x-mx)/5.0f;
		mx = x;
		my = y;
	}
}

void setLight()
{
	static const GLfloat light_position[] = {50.0f,50.0f,50.0f,0.0f};
	static const GLfloat light_ambient[] = {0.0f,0.0f,0.0f,0.0f};
	static const GLfloat light_diffuse[] = {1.0f,0.9f,0.9f,0.0f};
	static const GLfloat light_specular[] = {1.0f,1.0f,1.0f,0.0f};
	static const GLfloat light_direction[] = {-1.0f,-1.0f,-1.0f};

	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light_direction);
	glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,10.0f);
	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,120.0f);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void setMaterial(Material &mat)
{
	glMaterialfv(GL_FRONT,GL_AMBIENT,mat.ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat.diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat.specular);
	glMaterialfv(GL_FRONT,GL_EMISSION,mat.emission);
	glMaterialf(GL_FRONT,GL_SHININESS,100);
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	glTranslatef(dx,dy,dz);
	glRotatef(ax,1.0f,0.0f,0.0f);
	glRotatef(ay,0.0f,1.0f,0.0f);
	glScalef(0.02,0.02,0.02);

	for(set<string>::iterator it=objname.begin();it!=objname.end();++it)
	{
		Object temp = objmap[*it];
	//	setMaterial(matname[temp.material]);
	//	glBindTexture(GL_TEXTURE_2D, 1);
	//	glBindTexture(GL_TEXTURE_2D, 3);
		//glbindtexture面
		//除三边，确定面，标记面，
		static const 
			int t[] = { 0, 3, 3, 2, 2, 1, 1 };
		vector<int>::iterator iter = temp.faces.begin();
		if(temp.col==1)
		{
			while(iter!=temp.faces.end())
			{
				glVertex3f(temp.vertexs[*iter-1].x,temp.vertexs[*iter-1].y,temp.vertexs[*iter-1].z);
				++iter;
			}
		}
		else if(temp.col==2)
		{
			while(iter!=temp.faces.end())
			{
				glTexCoord2f(temp.texcoords[*(iter+1)-1].first,temp.texcoords[*(iter+1)-1].second);
				glVertex3f(temp.vertexs[*iter-1].x,temp.vertexs[*iter-1].y,temp.vertexs[*iter-1].z);
				iter+=2;
			}
		}
		else if(temp.col==3&&!temp.texcoords.empty())
		{
			
			while (iter != temp.faces.end())
			{
				if (abs(temp.vertexs[*iter - 1].x) > tempxmax) tempxmax = abs(temp.vertexs[*iter - 1].x);
				if (abs(temp.vertexs[*iter - 1].y) > tempymax) tempymax = abs(temp.vertexs[*iter - 1].y);
				if (abs(temp.vertexs[*iter - 1].z) > tempzmax) tempzmax = abs(temp.vertexs[*iter - 1].z);
				iter += 3;
			}
			iter = temp.faces.begin();
			
			while(iter!=temp.faces.end())
			{

				tempx = abs(height / temp.normals[*(iter + 2) - 1].z)*temp.normals[*(iter + 2) - 1].x;
				tempy = abs(height / temp.normals[*(iter + 2) - 1].z)*temp.normals[*(iter + 2) - 1].y;
				tempz = abs(height / temp.normals[*(iter + 2) - 1].z)*temp.normals[*(iter + 2) - 1].z;
				if ((abs(tempx) <= width) && (abs(tempy) <= frontbot) && (tempz>0)) 
					facenum = 1;
				if ((abs(tempx) <= width) && (abs(tempy) <= frontbot) && (tempz<0)) 
					facenum = 2;

				tempx = abs(frontbot / temp.normals[*(iter + 2) - 1].y)*temp.normals[*(iter + 2) - 1].x;
				tempy = abs(frontbot / temp.normals[*(iter + 2) - 1].y)*temp.normals[*(iter + 2) - 1].y;
				tempz = abs(frontbot / temp.normals[*(iter + 2) - 1].y)*temp.normals[*(iter + 2) - 1].z;
				if ((abs(tempx) <= width) && (abs(tempz) <= height) && (tempy>0)) 
					facenum = 4;
				if ((abs(tempx) <= width) && (abs(tempz) <= height) && (tempy<0)) 
					facenum = 3;

				tempx = abs(width / temp.normals[*(iter + 2) - 1].x)*temp.normals[*(iter + 2) - 1].x;
				tempy = abs(width / temp.normals[*(iter + 2) - 1].x)*temp.normals[*(iter + 2) - 1].y;
				tempz = abs(width / temp.normals[*(iter + 2) - 1].x)*temp.normals[*(iter + 2) - 1].z;
				if ((abs(tempz) <= height) && (abs(tempy) <= frontbot) && (tempx>0)) 
					facenum = 5;
				if ((abs(tempz) <= height) && (abs(tempy) <= frontbot) && (tempx<0)) 
					facenum = 6;
				

				setMaterial(matname[temp.material]);
				glBindTexture(GL_TEXTURE_2D, t[facenum]);
				glBegin(GL_QUADS);

				glNormal3f(temp.normals[*(iter + 2) - 1].x, temp.normals[*(iter + 2) - 1].y, temp.normals[*(iter + 2) - 1].z);
				if ((facenum == 1) || (facenum==2))
					glTexCoord2f((temp.vertexs[*iter - 1].y + tempymax) / (2 * tempymax), (temp.vertexs[*iter - 1].z + tempzmax) / (2 * tempzmax));
				if ((facenum == 3) || (facenum == 4))
					glTexCoord2f((temp.vertexs[*iter - 1].x + tempxmax) / (2 * tempxmax), (temp.vertexs[*iter - 1].z + tempzmax) / (2 * tempzmax));
				if ((facenum == 5) || (facenum == 6))
					glTexCoord2f((temp.vertexs[*iter - 1].y + tempymax) / (2 * tempymax), (temp.vertexs[*iter - 1].x + tempxmax) / (2 * tempxmax));
				glVertex3f(temp.vertexs[*iter - 1].x, temp.vertexs[*iter - 1].y, temp.vertexs[*iter - 1].z);
				iter += 3;
			}
		}
		else
		{
			while(iter!=temp.faces.end())
			{
				glNormal3f(temp.normals[*(iter+2)-1].x,temp.normals[*(iter+2)-1].y,temp.normals[*(iter+2)-1].z);
				glVertex3f(temp.vertexs[*iter-1].x,temp.vertexs[*iter-1].y,temp.vertexs[*iter-1].z);
				iter+=3;
			}
		}
		glBindTexture(GL_TEXTURE_2D,0);
		glEnd();
	}
	glPopMatrix();
	glutSwapBuffers();
}

void init()
{
	ReadObj(cd,".\\tank\\tryit.obj",objmap,objname,matname);
	//glutFullScreen();
	glClearColor(0.93,0.94,0.98,1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	setLight();
}

int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(500,700);
	glutCreateWindow("OpenGl");
	glutReshapeFunc(&myReshape);
	glutKeyboardFunc(&myKeyboard);
	glutMouseFunc(&myMouse);
	glutMotionFunc(&myMotion);
	glutDisplayFunc(&myDisplay);
	glutIdleFunc(&myIdle);
	init();
	glutMainLoop();
	return 0;
}
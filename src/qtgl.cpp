#include "qtgl.h"
#include <iostream>
using namespace std;

QtGL::QtGL(QWidget *parent):QOpenGLWidget(parent){
	view_mode = TEX_MODE;
	CAM_X = 0.0f;										//ͼ��ƽ�Ʒ���
	CAM_Z = 0.0f;
	CAM_Y = 0.0f;
	CAM_TX = 1.0f;										//��ͼ��С����������
	CAM_TY = 1.0f;
	CAM_TZ = 1.0f;
    CAM_DELTAX = 0;
    CAM_DELTAY = 0;

	SELECTED_POINT = 0;
	SELECTED_POLY = 0;
	SELECTED_MODEL = 0;
	MOUSE_BUTTON = 0;
	WINDOW_HEIGHT = 800;
	WINDOW_WIDTH = 800;
	setFocusPolicy(Qt::StrongFocus);

}

void QtGL::initializeGL(){

    TEX_ID = LoadTexture("model//s.png");
    Model md = loadObj("model//wolverine_obj.obj");
	models.push_back(md);
	Model cu = GetCube(2,2,2);
	models.push_back(cu);

	//��2D��ͼ״̬
	glEnable(GL_TEXTURE_2D);
	//��ɫ
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	//�����
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glHint(GL_FOG_HINT, GL_NICEST);

	// ������
	GLfloat light_a[4] = {0.4,0.4,0.4,1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_a);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void QtGL::paintGL(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(CAM_DELTAX, 0.0, 1.0, 0.0);					//x����ת
	glRotatef(CAM_DELTAY, 1.0, 0.0, 0.0);					//y����ת
	glTranslatef(CAM_X, CAM_Y, CAM_Z);
	glScalef(CAM_TX, CAM_TY, CAM_TZ);


	// ���¾���
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	DrawGround();
	DrawAxis();


	glPushMatrix();
	glTranslatef(1,0,1);
	SetLight();
	glPopMatrix();
	// �߿�ģ��
	//md.DrawLines();
	
	// ��ģ��
	
	for (Model &md : models){
		switch (view_mode){
			case TEX_MODE:
				glBindTexture(GL_TEXTURE_2D, TEX_ID);
			case FACE_MODE:
				md.Draw();
				break;
			case LINE_MODE:
				md.DrawLines();
				break;
			case POINT_MODE:
				md.DrawPoints();
				break;
		};
	}
	glBindTexture(GL_TEXTURE_2D, 0);


}

void QtGL::resizeGL(int w, int h){
	WINDOW_WIDTH = w;
	WINDOW_HEIGHT = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	//glLoadMatrixf(projection);
	glLoadIdentity();
	glOrtho(-2, 2, -2, 2, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void QtGL::SetLight(){
	const GLfloat am = 0.0f;
	const GLfloat l = 0.5f;
	GLfloat light_position[] = {0,0,-1,0.0f};
	GLfloat light_ambient[] = {am, am, am, 1.0f};
	GLfloat light_diffuse[] = {l,l,l,l};
	GLfloat light_specular[] = {l,l,l,l};
	GLuint lid = GL_LIGHT0;
	glLightfv(lid, GL_POSITION, light_position);
	glLightfv(lid, GL_AMBIENT, light_ambient); // ���չ���
	glLightfv(lid, GL_DIFFUSE, light_diffuse); // ������
	glLightfv(lid, GL_SPECULAR, light_specular); // ���淴��
	glEnable(lid);
	//glEnable(GL_LIGHTING);
}

void QtGL::KeyDeleteDown(){
	if (SELECTED_POINT){
		int id = SELECTED_POINT->id;
		vector<objPoly> &t = SELECTED_MODEL->ps; // vec<�����>
		bool changed = false;
		for (auto o = t.begin();o != t.end();++o){
			auto &p = o->points;
			for (auto v = p.begin();v != p.end();++v){
				if (v->id == id){
					v = p.erase(v);
					cout << "+" << p.size() << endl;
					changed = true;
					if (v == p.end())break;
				}
			}
		}
		if (changed)SELECTED_MODEL->Rebuild();
		SELECTED_POINT = 0;
		update();
	}
}

void QtGL::mousePressEvent(QMouseEvent *event){
	QPoint p = event->pos();
	int x = p.x();
	int y = p.y();
	MOUSE_BUTTON = event->button();
    CAM_OLDMX = x; CAM_OLDMY = y;
    if (MOUSE_BUTTON == Qt::LeftButton || MOUSE_BUTTON == Qt::RightButton){
		if (MOUSE_BUTTON == Qt::LeftButton){
			double best = MIN_SELECTED_PIXEL * MIN_SELECTED_PIXEL;
			double bestz = -1000;
			if (!SELECTED_POINT){
				for (Model &md : models){
					for (objPoly &p : md.ps){
						for (objPoint &v : p.points){
							glm::vec3 c = md.GetVertex(v.id);
							double sx,sy,sz;
							gluProject(c.x,c.y,c.z,modelview,projection,viewport,&sx,&sy,&sz);
							double dx = sx - x;
							double dy = sy - (viewport[3] - y);
							double f = dx * dx + dy * dy;
							if (f < best || ((f - best) < CROSS_PIXEL && sz > bestz)){
								best = f;
								bestz = sz;
								SELECTED_POINT = &v;
								SELECTED_POLY = &p;
								SELECTED_MODEL = &md;
							}
						}
					}
				}
			}

			cout << SELECTED_POINT << endl;
		}
	}
	event->accept();
}

void QtGL::mouseMoveEvent(QMouseEvent *event){
	QPoint p = event->pos();
	int x = p.x();
	int y = p.y();

	if (MOUSE_BUTTON == Qt::RightButton){
        CAM_DELTAX += 360.0 * (x - CAM_OLDMX) / 600.0;
        CAM_DELTAY += 360.0 * (y - CAM_OLDMY) / 600.0;
        CAM_OLDMX = x;
        CAM_OLDMY = y;
    }else if (MOUSE_BUTTON == Qt::LeftButton){
		if (SELECTED_POINT){
			glm::vec3 &v = SELECTED_MODEL->GetVertex(SELECTED_POINT->id);
			double sx,sy,sz;
			gluProject(v.x,v.y,v.z,modelview,projection,viewport,&sx,&sy,&sz);
			double object_x = 0,object_y = 0,object_z = 0;     //3D����

			float winX=(float)x;
			float winY=(float)viewport[3]-(float)y;
			gluUnProject((GLdouble)winX,(GLdouble)winY,(GLdouble)sz,modelview,projection,viewport,&object_x,&object_y,&object_z);

			v = glm::vec3(object_x, object_y, object_z);

		}
	}
	event->accept();
	update();
}

void QtGL::mouseReleaseEvent(QMouseEvent *event){
	SELECTED_POINT = 0;
	event->accept();
}

void QtGL::wheelEvent(QWheelEvent *event){
	if (event->delta() < 0){
		CAM_TX = CAM_TX / 1.1;
		CAM_TY = CAM_TY / 1.1;
		CAM_TZ = CAM_TZ / 1.1;
	}else{
		CAM_TX = CAM_TX * 1.1;
		CAM_TY = CAM_TY * 1.1;
		CAM_TZ = CAM_TZ * 1.1;
	}
	event->accept();
	update();
}

void QtGL::keyPressEvent(QKeyEvent *event){
	switch (event->key()) {
	case Qt::Key_Plus:
	case Qt::Key_Equal:
		CAM_TX = CAM_TX * 1.1;
		CAM_TY = CAM_TY * 1.1;
		CAM_TZ = CAM_TZ * 1.1;
		update();
		break;
	case Qt::Key_Minus:
		CAM_TX = CAM_TX / 1.1;
		CAM_TY = CAM_TY / 1.1;
		CAM_TZ = CAM_TZ / 1.1;
		update();
		break;
	case Qt::Key_Delete:
		KeyDeleteDown();
		break;
	case Qt::Key_Up:
		CAM_Y += 0.1;update();break;
	case Qt::Key_Down:
		CAM_Y += 0.1;update();break;
	case Qt::Key_Left:
		CAM_X -= 0.1;update();break;
	case Qt::Key_Right:
		CAM_X += 0.1;update();break;
	}
	//event->accept();
	QWidget::keyPressEvent(event);

}

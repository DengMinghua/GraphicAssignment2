#ifndef QTGL_H
#define QTGL_H

#include <Windows.h>
#include <gl/glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QWidget>
#include <iostream>
#include <vector>
#include <string>
#include "loadobj.h"
#include "model.h"
#include "shape.h"

using namespace std;

enum VIEW_MODE{
	TEX_MODE, FACE_MODE, LINE_MODE, POINT_MODE
};


class QtGL : public QOpenGLWidget{
    Q_OBJECT
public:
    explicit QtGL(QWidget *parent = 0);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
private:
	void SetLight();
	void KeyDeleteDown();
private:
	double modelview[16];
	double projection[16];
	int viewport[4];
private:
	vector<Model> models;
	GLuint TEX_ID;
	VIEW_MODE view_mode = TEX_MODE;

	GLfloat CAM_X;										//ͼ��ƽ�Ʒ���
	GLfloat CAM_Z;
	GLfloat CAM_Y;
	GLfloat CAM_TX;										//��ͼ��С����������
	GLfloat CAM_TY;
	GLfloat CAM_TZ;
	GLfloat CAM_OLDMY, CAM_OLDMX;									//��갴�µ�λ�ã�x,y)
    float CAM_DELTAX, CAM_DELTAY;									//�ͷź�x��y�ƶ�����

	objPoint *SELECTED_POINT;
	objPoly *SELECTED_POLY;
	Model *SELECTED_MODEL;
	int MOUSE_BUTTON;
	int WINDOW_HEIGHT;
	int WINDOW_WIDTH;

	const double MIN_SELECTED_PIXEL = 64;
	const double CROSS_PIXEL = 24;

};

#endif // QTGL_H

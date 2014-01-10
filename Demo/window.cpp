#include "window.h"

#include <QtGui>

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    glwidget = new GLWidget();
    QPushButton* t_button = new QPushButton();
    t_button->setText("Triangles");
    connect(t_button, SIGNAL(clicked()), glwidget, SLOT(triangles()));
    QPushButton* l_button = new QPushButton();
    l_button->setText("Line Loop");
    connect(l_button, SIGNAL(clicked()), glwidget, SLOT(lineLoop()));
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(glwidget);
    vbox->addWidget(t_button);
    vbox->addWidget(l_button);
    setLayout(vbox);
}

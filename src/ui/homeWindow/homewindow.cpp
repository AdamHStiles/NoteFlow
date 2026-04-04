#include "homewindow.h"
#include "./ui_homewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

HomeWindow::HomeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    QWidget *central = new QWidget();
    setCentralWidget(central);


    // initlizing layouts
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    QVBoxLayout *titleLayout = new QVBoxLayout();
    QVBoxLayout *buttonLayout = new QVBoxLayout();


    // set up layouts, add child layouts to main layout
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(buttonLayout);


    // adds title widget to titleLayout and formats it
    title = new QLabel("NoteFlow");
    titleLayout->addStretch();
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    title->setContentsMargins(10, 10, 10, 10);
    title->setAlignment(Qt::AlignCenter);


    // adds button widgets to button layout and formats it
    // adding in openButton
    openButton = new QPushButton("Open Study Group");
    buttonLayout->addStretch();
    buttonLayout->addWidget(openButton);
    buttonLayout->addStretch();
    openButton->setContentsMargins(5,5,5,5);
    buttonLayout->setAlignment(openButton, Qt::AlignCenter);

    // adding in createButton
    createButton = new QPushButton("Create Study Group");
    buttonLayout->addStretch();
    buttonLayout->addWidget(createButton);
    buttonLayout->addStretch();
    createButton->setContentsMargins(5,5,5,5);
    buttonLayout->setAlignment(createButton, Qt::AlignCenter);





}

HomeWindow::~HomeWindow()
{
    delete ui;
}

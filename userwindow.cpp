#include "userwindow.h"
#include "usersmodel.h"
#include "userentry.h"


UserWindow::UserWindow(QWidget *parent)
    : QWidget(parent),
    m_model(new QLightDM::UsersModel(this))
{
    initUI();
}

void UserWindow::initUI()
{
    m_vlayout = new QVBoxLayout(this);
    m_hlayout = new QHBoxLayout(this);
    m_pageIndicator = new QLabel(this);
    m_prevLabel = new QLabel(this);
    m_nextLabel = new QLabel(this);
    m_hlayout->addWidget(m_prevLabel);
    m_hlayout->addWidget(m_nextLabel);
    m_hlayout->setStretchFactor(m_prevLabel, 1);

    m_hlayout->setStretchFactor(m_nextLabel, 1);
    m_vlayout->addLayout(m_hlayout);
    m_vlayout->addWidget(m_pageIndicator);
    m_vlayout->setStretchFactor(m_hlayout, 9);
    m_vlayout->setStretchFactor(m_pageIndicator, 1);

}


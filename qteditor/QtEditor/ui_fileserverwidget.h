/********************************************************************************
** Form generated from reading UI file 'fileserverwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILESERVERWIDGET_H
#define UI_FILESERVERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileServerWidget
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidget;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QCheckBox *checkBox;
    QLineEdit *filter;
    QCheckBox *filterCB;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDockWidget *FileServerWidget)
    {
        if (FileServerWidget->objectName().isEmpty())
            FileServerWidget->setObjectName(QStringLiteral("FileServerWidget"));
        FileServerWidget->resize(593, 300);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        tableWidget = new QTableWidget(dockWidgetContents);
        if (tableWidget->columnCount() < 6)
            tableWidget->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setSortingEnabled(true);
        tableWidget->horizontalHeader()->setDefaultSectionSize(150);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(tableWidget);

        widget = new QWidget(dockWidgetContents);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);

        checkBox = new QCheckBox(widget);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setChecked(true);

        horizontalLayout->addWidget(checkBox);

        filter = new QLineEdit(widget);
        filter->setObjectName(QStringLiteral("filter"));

        horizontalLayout->addWidget(filter);

        filterCB = new QCheckBox(widget);
        filterCB->setObjectName(QStringLiteral("filterCB"));
        filterCB->setChecked(true);

        horizontalLayout->addWidget(filterCB);

        horizontalSpacer = new QSpacerItem(245, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);

        FileServerWidget->setWidget(dockWidgetContents);

        retranslateUi(FileServerWidget);

        QMetaObject::connectSlotsByName(FileServerWidget);
    } // setupUi

    void retranslateUi(QDockWidget *FileServerWidget)
    {
        FileServerWidget->setWindowTitle(QApplication::translate("FileServerWidget", "File server", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("FileServerWidget", "Time", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("FileServerWidget", "Event", 0));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("FileServerWidget", "Handle", 0));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("FileServerWidget", "Filename", 0));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("FileServerWidget", "Parameter", 0));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("FileServerWidget", "Return", 0));
        pushButton->setText(QApplication::translate("FileServerWidget", "Clear", 0));
        checkBox->setText(QApplication::translate("FileServerWidget", "Enabled", 0));
        filterCB->setText(QApplication::translate("FileServerWidget", "Filter", 0));
    } // retranslateUi

};

namespace Ui {
    class FileServerWidget: public Ui_FileServerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILESERVERWIDGET_H

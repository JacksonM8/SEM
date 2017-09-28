#include "dataeditwidget.h"
#include <QVBoxLayout>


#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QPlainTextEdit>
#include "../theme.h"

#define SMALL_SQUARE 24
DataEditWidget::DataEditWidget(QString label, SETTING_TYPE type, QVariant data, QWidget *parent) : QWidget(parent)
{
    isHighlighted = false;
    this->label = label;
    this->currentData = data;
    this->newData = data;
    this->type = type;

    setContentsMargins(0,0,0,0);
    setupLayout();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}
void DataEditWidget::updateIcon(){
    Theme* theme = Theme::theme();
    if(icon_path.first != "" && icon_path.second != ""){
        switch(type){
            case SETTING_TYPE::FONT:
            case SETTING_TYPE::BUTTON:{
                auto button = qobject_cast<QToolButton*>(editWidget_1);
                if(button){
                    button->setIcon(theme->getIcon(icon_path));
                }
                break;
            }   
            default:
                iconLabel->setPixmap(theme->getIcon(icon_path).pixmap(16,16));
                break;

        }
    }
}

void DataEditWidget::setIcon(QString path, QString name){
    icon_path.first = path;
    icon_path.second = name;
    updateIcon();
    showIcon();
}
void DataEditWidget::showIcon(){

    switch(type){
        case SETTING_TYPE::FONT:
        case SETTING_TYPE::BUTTON:{
            break;
        }
        default:
        if(iconLabel){
            iconLabel->show();
        }
        break;
    }
}
void DataEditWidget::hideIcon(){
    if(iconLabel){
        iconLabel->hide();
    }
}

SETTING_TYPE DataEditWidget::getType()
{
    return type;
}
QString DataEditWidget::getLabel(){
    return label;
}

void DataEditWidget::setHighlighted(bool highlighted)
{
    isHighlighted = highlighted;
    themeChanged();
}

int DataEditWidget::getMinimumLabelWidth()
{
    if(editLabel){
        return editLabel->fontMetrics().width(editLabel->text());
    }
    return -1;
}

void DataEditWidget::setLabelWidth(int width)
{
    if(editLabel){
        editLabel->setFixedWidth(width);
    }
}

void DataEditWidget::setValue(QVariant data)
{
    dataChanged(data);

    switch(type){
    case SETTING_TYPE::BOOL:{
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(editWidget_1);
        if(checkBox){
            checkBox->setChecked(newData.toBool());
        }
        break;
    }
    case SETTING_TYPE::INT:{
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editWidget_1);
        if(spinBox){
            spinBox->setValue(newData.toInt());
        }
        break;
    }
    case SETTING_TYPE::COLOR:{
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);

        //Validate color;
        QColor color(newData.toString());
        if(color.isValid()){
            if(lineEdit){
                lineEdit->setText(newData.toString());
            }
            themeChanged();
        }
        break;
    }
    case SETTING_TYPE::STRING:{
        QPlainTextEdit* textEdit = qobject_cast<QPlainTextEdit*>(editWidget_1);
        if(textEdit){
            textEdit->setPlainText(newData.toString());
        }
        break;
    }
    case SETTING_TYPE::FILE:
    case SETTING_TYPE::PATH:{
        //Do the same things.
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);
        if(lineEdit){
            lineEdit->setText(newData.toString());
        }
        break;
    }
    default:
        break;

    }

    currentData = newData;
    emit valueChanged(newData);
}

QVariant DataEditWidget::getValue()
{
    return currentData;
}

void DataEditWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    if(toolbar){
        toolbar->setStyleSheet(theme->getToolBarStyleSheet());
    }

    QToolButton* button = qobject_cast<QToolButton*>(editWidget_2);
    if(button){
        QString style = theme->getToolBarStyleSheet();
        //QString style = "border:1px solid " + theme->getAltBackgroundColorHex() + ";";
        switch(type){
        case SETTING_TYPE::FILE:{
            button->setIcon(Theme::theme()->getIcon("Icons", "file"));
            break;
        }
        case SETTING_TYPE::PATH:{
            button->setIcon(Theme::theme()->getIcon("Icons", "folder"));
            break;
        }
        case SETTING_TYPE::COLOR:{
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);
            if(lineEdit){
                style += "QToolButton{background: " + lineEdit->text() + ";}";
            }
            break;
        }
        
        default:{
            break;
        }

        }
        button->setStyleSheet(style);
    }

    QString labelStyleSheet = "";

    if(isHighlighted){
        labelStyleSheet += "color: " + theme->getHighlightColorHex() + "; text-decoration:underline;";
    }else{
        labelStyleSheet += "color: " + theme->getTextColorHex() + "; text-decoration:normal;";
    }

    if(editLabel){
        editLabel->setStyleSheet(labelStyleSheet);
    }



    if(editWidget_1){
        QString style = "color:" + Theme::theme()->getTextColorHex() + ";";
        if(type == SETTING_TYPE::BOOL){
            style = labelStyleSheet;
        }else{
            style += "background:" + Theme::theme()->getAltBackgroundColorHex() + "; border: 1px solid " + theme->getAltBackgroundColorHex() + ";";
        }

        

        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);
        QPlainTextEdit* textEdit = qobject_cast<QPlainTextEdit*>(editWidget_1);
        QToolButton* button = qobject_cast<QToolButton*>(editWidget_1);

        if(lineEdit){
            lineEdit->setStyleSheet(theme->getLineEditStyleSheet());
        }else if(button){
            button->setStyleSheet(theme->getToolButtonStyleSheet());
            button->setIcon(theme->getIcon(icon_path));
        }else if(textEdit){
            textEdit->setStyleSheet(theme->getTextEditStyleSheet());
        }else{
            editWidget_1->setStyleSheet(style);
        }

    }

    
    updateIcon();
}

void DataEditWidget::dataChanged(QVariant value)
{
    newData = value;
}

void DataEditWidget::editFinished()
{
    switch(type){
    case SETTING_TYPE::BOOL:{
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(editWidget_1);
        if(checkBox){
            //Call dataChanged first
            dataChanged(checkBox->isChecked());
        }
        break;
    }
    case SETTING_TYPE::INT:{
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editWidget_1);
        if(spinBox){
            //Call data Changed first
            dataChanged(spinBox->value());
        }
        break;
    }
        case SETTING_TYPE::COLOR:{

        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);

        //Validate color;
        QColor color(newData.toString());
        if(color.isValid()){
            if(lineEdit){
                lineEdit->setText(newData.toString());
            }
            themeChanged();
            break;
        }else{
            newData = currentData;
            lineEdit->setText(currentData.toString());
            return;
        }
        break;
    }
    case SETTING_TYPE::STRING:{
        QPlainTextEdit* lineEdit = qobject_cast<QPlainTextEdit*>(editWidget_1);
        dataChanged(lineEdit->toPlainText());
        break;
    }
    case SETTING_TYPE::FILE:
    case SETTING_TYPE::PATH:{
        //Do the same things.
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);
        if(lineEdit){
            lineEdit->setText(newData.toString());
        }
        break;
    }
    default:
        break;

    }

    currentData = newData;
    emit valueChanged(newData);
}

void DataEditWidget::pickColor()
{
    //Get the current Color from the string.
    QColor currentColor(currentData.toString());

    //Select the new Color.
    QColor newColor  = QColorDialog::getColor(currentColor);
    if(newColor.isValid()){
        dataChanged(newColor.name());
        editFinished();
    }
}

void DataEditWidget::pickFont()
{
    //Get the current Color from the string.
    QFont current_font(currentData.value<QFont>());

    bool okay = false;
    //Select the new Color.
    auto new_font  = QFontDialog::getFont(&okay, current_font);
    if(okay){
        dataChanged(new_font);
        editFinished();
    }
}

void DataEditWidget::pickPath()
{
    QString path = "";
    switch(type){
        case SETTING_TYPE::PATH:
        path = QFileDialog::getExistingDirectory(this, "Select Path" , currentData.toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        break;
    case SETTING_TYPE::FILE:
        path = QFileDialog::getOpenFileName(this, "Select File" , currentData.toString());
        break;
    default:
        return;
    }
    if(path != ""){
        dataChanged(path);
        editFinished();
    }
}

void DataEditWidget::setupLayout()
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    QHBoxLayout* editLayout = new QHBoxLayout();
    verticalLayout->setMargin(0);
    editLayout->setMargin(0);


    verticalLayout->addLayout(editLayout);

    editWidget_1 = 0;
    editWidget_2 = 0;
    editLabel = new QLabel(label, this);
    iconLabel = new QLabel(this);
    editLayout->addWidget(iconLabel);
    editLayout->addWidget(editLabel);
    iconLabel->hide();

    switch(type){
        case SETTING_TYPE::STRING:{
        QPlainTextEdit* textEdit = new QPlainTextEdit(this);
        textEdit->setPlainText(currentData.toString());

        textEdit->setTabChangesFocus(true);

        textEdit->setFixedHeight(SMALL_SQUARE);

        //Any CChange should change
        connect(textEdit, &QPlainTextEdit::textChanged, this, [textEdit, this](){dataChanged(textEdit->toPlainText()); editFinished();});

        editWidget_1 = textEdit;
        editLayout->addWidget(editWidget_1, 1);
        break;
    }
    case SETTING_TYPE::BOOL:{
        QCheckBox* checkBox = new QCheckBox(label, this);
        checkBox->setChecked(currentData.toBool());

        connect(checkBox, &QCheckBox::clicked, this, &DataEditWidget::editFinished);

        editWidget_1 = checkBox;
        editLayout->removeWidget(editLabel);

        editLayout->addWidget(editWidget_1, 1);
        editLabel->deleteLater();
        editLabel = 0;
        break;
    }
    case SETTING_TYPE::INT:{
        QSpinBox* spinBox = new QSpinBox(this);
        spinBox->setMaximum(10000);
        spinBox->setValue(currentData.toInt());

        editWidget_1 = spinBox;

        connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [spinBox, this](int value){editFinished();});

        editLayout->addWidget(editWidget_1, 1);
        break;
    }
    case SETTING_TYPE::PATH:
    case SETTING_TYPE::FILE:{
        QLineEdit* lineEdit = new QLineEdit(this);

        lineEdit->setText(currentData.toString());

        QToolButton* button = new QToolButton(this);
        button->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);

        toolbar = new QToolBar(this);
        toolbar->addWidget(button);
        //button->setFlat(true);

        editWidget_1 = lineEdit;
        editWidget_2 = button;
        editLayout->addWidget(editWidget_1, 1);
        editLayout->addWidget(toolbar);

        connect(button, &QToolButton::released, this, &DataEditWidget::pickPath);
        connect(lineEdit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
        connect(lineEdit, &QLineEdit::editingFinished, this, &DataEditWidget::editFinished);
        break;
    }
    case SETTING_TYPE::COLOR:{
        QLineEdit* lineEdit = new QLineEdit(this);
        lineEdit->setText(currentData.toString());



        QToolButton* button = new QToolButton(this);
        button->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);

        editWidget_1 = lineEdit;
        editWidget_2 = button;

        connect(button, &QToolButton::released, this, &DataEditWidget::pickColor);//, Qt::QueuedConnection);
        connect(lineEdit, &QLineEdit::textEdited, this, &DataEditWidget::dataChanged);
        connect(lineEdit, &QLineEdit::editingFinished, this, &DataEditWidget::editFinished);

        editLayout->addWidget(editWidget_1, 1);
        editLayout->addWidget(editWidget_2);
        break;
    }
    case SETTING_TYPE::FONT:{
        QToolButton* button = new QToolButton(this);
        button->setText(label);
        button->setFixedHeight(SMALL_SQUARE);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        connect(button, &QToolButton::released, this, &DataEditWidget::pickFont);

        toolbar = new QToolBar(this);
        toolbar->addWidget(button);
        editWidget_1 = button;
        editLayout->removeWidget(editLabel);

        editLayout->addWidget(toolbar, 1);
        editLabel->deleteLater();
        editLabel = 0;
        break;
    }
    case SETTING_TYPE::BUTTON:{
        QToolButton* button = new QToolButton(this);
        button->setText(label);
        button->setFixedHeight(SMALL_SQUARE);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        connect(button, &QToolButton::released, this, &DataEditWidget::editFinished);

        toolbar = new QToolBar(this);
        toolbar->addWidget(button);
        editWidget_1 = button;
        editLayout->removeWidget(editLabel);

        editLayout->addWidget(toolbar, 1);
        editLabel->deleteLater();
        editLabel = 0;
        break;
    }
    default:
        break;
    }

    if(editWidget_1){
        editWidget_1->setAttribute(Qt::WA_MacShowFocusRect, false);
        if(editLabel){
            editLabel->setFocusPolicy(Qt::ClickFocus);
            editLabel->setFocusProxy(editWidget_1);
        }
        editWidget_1->setFixedHeight(SMALL_SQUARE);
    }
    if(editWidget_2){
        editWidget_2->setFixedHeight(SMALL_SQUARE);
    }
}

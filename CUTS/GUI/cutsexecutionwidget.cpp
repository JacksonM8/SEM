#include "cutsexecutionwidget.h"
#include "../cutsmanager.h"


#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QLayout>
#include <QProgressBar>
#include <QAction>
#include <QMenu>
#include <QProgressDialog>
#include <QDebug>

#include <QFileDialog>
#include <QTimer>
#include <QTime>
#include <QMessageBox>


CUTSExecutionWidget::CUTSExecutionWidget(QWidget *parent, CUTSManager *cutsManager)
{
    this->cutsManager = cutsManager;
    graphmlPathEdit = 0;
    outputPathEdit = 0;
    graphmlPathOk = false;
    outputPathOk = false;

    setWindowTitle("Launch CUTS Execution");
    setWindowIcon(QIcon(":/Actions/Cut.png"));
    setupLayout();

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
    setModal(true);

    setStyleSheet("font-family: Helvetica, Arial, sans-serif; background-color:white;  font-size: 13px; color: #333;");

    //Turn off the Other Buttons.
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
    connect(cutsManager, SIGNAL(fileToGenerate(QString)), this, SLOT(fileToBeGenerated(QString)));
    connect(cutsManager, SIGNAL(fileIsGenerated(QString,bool)), this, SLOT(fileGenerated(QString, bool)));
    connect(cutsManager, SIGNAL(executedXSLGeneration(bool,QString)), this, SLOT(generationFinished(bool,QString)));
    connect(cutsManager, SIGNAL(executedMWCGeneration(bool, QString)), this, SLOT(generationFinished(bool,QString)));
    connect(cutsManager, SIGNAL(executedCPPCompilation(bool,QString)), this, SLOT(generationFinished(bool,QString)));
    connect(cutsManager, SIGNAL(executedCUTS(bool,QString)), this, SLOT(generationFinished(bool,QString)));

    connect(cutsManager, SIGNAL(gotLiveCPPOutput(QString)), this, SLOT(gotLiveMakeOutput(QString)));
    connect(cutsManager, SIGNAL(gotLiveMWCOutput(QString)), this, SLOT(gotLiveMWCOutput(QString)));
    connect(cutsManager, SIGNAL(gotLiveCUTSOutput(QString)), this, SLOT(gotLiveCUTSOutput(QString)));



    loadingMovie = new QMovie(this);
    loadingMovie->setFileName(":/Actions/jenkins_waiting.gif");
    loadingMovie->start();

    //Set State
    state = INITIAL;
    setState(INITIAL);

    setGraphMLPath("/Users/dan/Desktop/model.graphml");
    setOutputPath("c:/Test2/");

    currentTime.start();
    currentTimer = new QTimer(this);
    connect(currentTimer, SIGNAL(timeout()), this, SLOT(tick()));

}

CUTSExecutionWidget::~CUTSExecutionWidget()
{

}

void CUTSExecutionWidget::stateFinished(int code)
{
    qCritical() << code;
    if(code == 0){
        stepState();
    }
}

void CUTSExecutionWidget::setGraphMLPath(QString path)
{
    //Check file.
    QFile file(path);
    QFileInfo fileInfo(file);

    bool isGraphML = false;
    bool isFile = fileInfo.isFile();
    if(path.endsWith(".graphml")){
        isGraphML = true;
    }

    if(isGraphML && isFile){
        graphmlPathEdit->setText(fileInfo.absoluteFilePath());

        jobLabel->setText("Local Deployment [" + fileInfo.fileName() + "]");
        setIconSuccess(graphmlPathIcon, true);
        graphmlPathOk = true;
    }else{
        if(fileInfo.isDir()){
            graphmlPathEdit->setText("");
        }else{
            graphmlPathEdit->setText(fileInfo.absoluteFilePath());
        }

        setIconSuccess(graphmlPathIcon, false);
        graphmlPathOk = false;
    }

    updateButtons();
}

void CUTSExecutionWidget::setOutputPath(QString outputPath)
{
    //Check file.
    QFile file(outputPath);
    QFileInfo fileInfo(file);

    QString path = fileInfo.absoluteFilePath();
    if(!path.endsWith("/")){
        path += "/";
    }
    outputPathEdit->setText(path);
    if(fileInfo.isDir()){
        setIconSuccess(outputPathIcon, true);
        outputPathOk = true;
    }else{
        setIconSuccess(outputPathIcon, false);
        outputPathOk = false;
    }
    updateButtons();
}

void CUTSExecutionWidget::runGeneration()
{

}

void CUTSExecutionWidget::nextButtonPressed()
{
    stepState();
}

void CUTSExecutionWidget::fileToBeGenerated(QString filePath)
{
    files.append(filePath);
    addFileToLayout(filePath);
}

void CUTSExecutionWidget::fileGenerated(QString filePath, bool success)
{

    FileExtension fE = fileExtensionLayouts[getExtension(filePath)];

    fE.filesGenerated.append(filePath);

    if(!success){
        fE.allSuccess = false;
    }

    if(fE.filesGenerated.size() == fE.files.size()){
        setIconSuccess(fE.icon, fE.allSuccess);
    }

    //Update fileCount
    fE.finishedLabel->setText(QString::number(fE.filesGenerated.count()));

    //Update the Layouts
    fileExtensionLayouts[getExtension(filePath)] = fE;

    //Check
    filesGenerated.append(filePath);

    //CHECK STATE.
    if(filesGenerated.size() == files.size()){
       setState(RAN_XSL);
    }
}

void CUTSExecutionWidget::generationFinished(bool success, QString errorString)
{
    if(success){
        stepState();
    }else{
        QMessageBox::critical(this, "CRITICAL", errorString, QMessageBox::Ok);
        setState(INITIAL);
    }
}

void CUTSExecutionWidget::gotLiveMWCOutput(QString data)
{
    if(mwcTextBrowser){
        mwcTextBrowser->append(data);
    }
}

void CUTSExecutionWidget::gotLiveMakeOutput(QString data)
{
    if(makeTextBrowser){
        makeTextBrowser->append(data);
    }
}

void CUTSExecutionWidget::gotLiveCUTSOutput(QString data)
{
    if(executeText){
        executeText->append(data);
    }
}

void CUTSExecutionWidget::outputPathEdited()
{
    setOutputPath(outputPathEdit->text());
}

void CUTSExecutionWidget::graphmlPathEdited()
{
    setGraphMLPath(graphmlPathEdit->text());
}

void CUTSExecutionWidget::tick()
{
    if(state == RUN_MWC || state == RUN_XSL || state == RUN_MWC || state == RUN_EXECUTION){
        updateTimeText(currentTime);
    }
}

QWidget *CUTSExecutionWidget::setupGenerateWidget()
{
    //Construct a ScrollArea
    generateScrollArea = new QScrollArea();
    generateWidget = new QWidget();

    generateScrollArea->setWidgetResizable(true);
    generateScrollArea->setWidget(generateWidget);
    generateScrollArea->setStyleSheet("QScrollArea{border: none;}");


    QVBoxLayout* vLayout = new QVBoxLayout(generateWidget);
    generateLayout = new QVBoxLayout();
    vLayout->addLayout(generateLayout);
    vLayout->addStretch(1);
    return generateScrollArea;
}

QWidget *CUTSExecutionWidget::setupBuildWidget()
{
    buildScrollArea = new QScrollArea();
    buildWidget = new QWidget();

    buildScrollArea->setWidgetResizable(true);
    buildScrollArea->setWidget(buildWidget);
    buildScrollArea->setStyleSheet("QScrollArea{border: none;}");


    QVBoxLayout* vLayout = new QVBoxLayout(buildWidget);
    QHBoxLayout* mwcLayout = new QHBoxLayout();
    mwcIcon = new QLabel();
    mwcIcon->setFixedSize(16,16);
    mwcIcon->setScaledContents(true);
    mwcLabel = new QLabel("MWC Generation");
    mwcLayout->addWidget(mwcIcon);
    mwcLayout->addWidget(mwcLabel);





    mwcTextBrowser = new QTextBrowser();
    mwcTextBrowser->setPlaceholderText("MWC GENERATION OUTPUT");
    vLayout->addLayout(mwcLayout);
    vLayout->addWidget(mwcTextBrowser);



    QHBoxLayout* makeLayout = new QHBoxLayout();
    makeIcon = new QLabel();
    makeIcon->setFixedSize(16,16);
    makeIcon->setScaledContents(true);
    //makeIcon->setIcon

    makeLabel = new QLabel("Compiling C++ Code");
    makeLayout->addWidget(makeIcon);
    makeLayout->addWidget(makeLabel);



    makeTextBrowser = new QTextBrowser();
    makeTextBrowser->setStyleSheet("background-color:black; color:white; font-family:Lucida Console;font-weight: bold; ");
    makeTextBrowser->setPlaceholderText("C++ Compilation Output");
    vLayout->addLayout(makeLayout);
    vLayout->addWidget(makeTextBrowser);

    return buildScrollArea;
}

QWidget *CUTSExecutionWidget::setupExecuteWidget()
{
    executeScrollArea = new QScrollArea();
    executeWidget = new QWidget();

    executeScrollArea->setWidgetResizable(true);
    executeScrollArea->setWidget(executeWidget);
    executeScrollArea->setStyleSheet("QScrollArea{border: none;}");


    QVBoxLayout* vLayout = new QVBoxLayout(executeWidget);
    executeText = new QTextBrowser();
    executeText->setPlaceholderText("Execute");
    vLayout->addWidget(executeText);
    return executeScrollArea;
}

QString CUTSExecutionWidget::getExtension(QString filePath)
{
    return filePath.split(".").last();
}

void CUTSExecutionWidget::updateButtons()
{
    if(graphmlPathOk && outputPathOk){
        setState(PARAMETERS_OKAY);
    }else{
        setState(INITIAL);
    }

}

void CUTSExecutionWidget::stepState()
{
    CUTS_EXECUTION_STATES newState = (CUTS_EXECUTION_STATES) (int(state) + 1);
    setState(newState);
}

void CUTSExecutionWidget::addFileToLayout(QString filePath)
{
    //Get File Extension
    QString extension = getExtension(filePath);

    FileExtension fE;

    if(!fileExtensionLayouts.contains(extension)){
        fE.allSuccess = true;
        fE.layout = new QHBoxLayout();
        fE.icon = new QLabel();
        fE.finishedLabel = new QLabel("0");
        fE.slashLabel = new QLabel("/");
        fE.totalLabel = new QLabel();
        fE.label = new QLabel("*." + extension + " Files");
        fE.icon = new QLabel();
        fE.icon->setFixedSize(16,16);
        fE.icon->setScaledContents(true);

        fE.icon->setMovie(loadingMovie);

        fE.layout->addWidget(fE.icon);
        fE.layout->addWidget(fE.finishedLabel);
        fE.layout->addWidget(fE.slashLabel);
        fE.layout->addWidget(fE.totalLabel);
        fE.layout->addWidget(fE.label,1);

        generateLayout->addLayout(fE.layout);
        fileExtensionLayouts[extension] = fE;
    }
    fE = fileExtensionLayouts[extension];

    fE.files.append(filePath);
    //Update fileCount
    fE.totalLabel->setText(QString::number(fE.files.count()));
    //Update the Layouts
    fileExtensionLayouts[extension] = fE;
}


void CUTSExecutionWidget::selectGraphMLPath()
{
    QString directory = getDirectory(graphmlPathEdit->text());
    QString graphmlFile = QFileDialog::getOpenFileName(this, "Select GraphML File.", directory, "GraphML Documents (*.graphml)");
    setGraphMLPath(graphmlFile);
}

void CUTSExecutionWidget::selectOutputPath()
{
    QString directory = getDirectory(outputPathEdit->text());
    QString path = QFileDialog::getExistingDirectory(this, "Select Output Directory.", directory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    setOutputPath(path);

}

void CUTSExecutionWidget::setupLayout()
{
    //Construct a Vertical Layout.
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    //Setup Title.
    titleWidget = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
    verticalLayout->addWidget(titleWidget);

    //Set up a QLabel for the name of the Jenkins Job
    jobLabel = new QLabel("Local Deployment");
    QLabel* iconLabel = new QLabel();

    iconLabel->setPixmap(QPixmap::fromImage(QImage(":/Actions/Job_Build.png")));

    jobLabel->setStyleSheet("font-family: Helvetica, Arial, sans-serif; font-size: 18px;  font-weight: bold;");

    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(jobLabel);
    titleLayout->addStretch();


    QHBoxLayout* modelLayout = new QHBoxLayout();
    QLabel* modelLabel = new QLabel("Model:");
    modelLabel->setToolTip("The absolute path to the model to execute.");
    graphmlPathIcon = new QLabel();
    graphmlPathIcon->setFixedSize(25,25);
    graphmlPathIcon->setScaledContents(true);
    graphmlPathEdit = new QLineEdit();
    setIconSuccess(graphmlPathIcon, false);

    graphmlPathButton = new QPushButton();
    graphmlPathButton->setIconSize(QSize(20,20));
    graphmlPathButton->setIcon(QIcon(":/Actions/Search_Folder.png"));
    connect(graphmlPathButton, SIGNAL(clicked()), this, SLOT(selectGraphMLPath()));
    modelLayout->addWidget(graphmlPathIcon);
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(graphmlPathEdit, 1);
    connect(graphmlPathEdit, SIGNAL(editingFinished()), this, SLOT(graphmlPathEdited()));
    modelLayout->addWidget(graphmlPathButton);



    verticalLayout->addLayout(modelLayout);

    QHBoxLayout* outputLayout = new QHBoxLayout();
    QLabel* outputLabel = new QLabel("Output:");
    outputLabel->setToolTip("The absolute path to the directory to build code in.");
    outputPathIcon = new QLabel();
    outputPathIcon->setFixedSize(25,25);
    outputPathIcon->setScaledContents(true);
    outputPathEdit = new QLineEdit();
    setIconSuccess(outputPathIcon, false);

    outputPathButton = new QPushButton();
    outputPathButton->setIconSize(QSize(20,20));
    outputPathButton->setIcon(QIcon(":/Actions/Search_Folder.png"));

    connect(outputPathButton, SIGNAL(clicked()), this, SLOT(selectOutputPath()));
    outputLayout->addWidget(outputPathIcon);
    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(outputPathEdit, 1);
    connect(outputPathEdit, SIGNAL(editingFinished()), this, SLOT(outputPathEdited()));
    outputLayout->addWidget(outputPathButton);

    verticalLayout->addLayout(outputLayout);

    tabWidget = new QTabWidget();
    verticalLayout->addWidget(tabWidget, 1);
    tabWidget->addTab(setupGenerateWidget(), QIcon(":/Actions/Generate.png"), "Generate");
    tabWidget->addTab(setupBuildWidget(), QIcon(":/Actions/Build.png"), "Build");
    tabWidget->addTab(setupExecuteWidget(),QIcon(":/Actions/Forward.png"),  "Execute");





    //Button Groups.
    QHBoxLayout* buttonGroups = new QHBoxLayout();
    verticalLayout->addLayout(buttonGroups);
    currentTaskTime = new QLabel("00:00");
    currentTaskTime->setVisible(false);
    nextButton = new QPushButton("Next");
    cancelButton = new QPushButton("Cancel");

    connect(nextButton, SIGNAL(clicked()), SLOT(nextButtonPressed()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    buttonGroups->addStretch(1);
    buttonGroups->addWidget(currentTaskTime);
    buttonGroups->addWidget(nextButton);
    buttonGroups->addWidget(cancelButton);

}

QString CUTSExecutionWidget::getDirectory(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo(file);
    return fileInfo.absolutePath();
}

void CUTSExecutionWidget::setIconSuccess(QLabel *label, bool success)
{
    QString fileName = "cross";
    if(success){
        fileName = "tick";
    }
    if(label){
        label->setPixmap(QPixmap::fromImage(QImage(":/Actions/" + fileName + ".png")));
    }
}

void CUTSExecutionWidget::updateTimeText(QTime time)
{
    if(currentTaskTime){
        int totalSeconds = time.elapsed() / 1000;

        //Calculate the seconds/mins
        int mins = totalSeconds / 60;
        int secs = totalSeconds - (mins * 60);


        QString timeText;

        timeText = QString::number(mins) + ":";
        if(secs < 10){
            timeText += "0";
        }
        timeText += QString::number(secs);

        currentTaskTime->setText(timeText);
        if(!currentTaskTime->isVisible()){
            currentTaskTime->setVisible(true);
        }
    }
}

void CUTSExecutionWidget::setIconLoading(QLabel *label)
{
    if(label && loadingMovie){
        label->setMovie(loadingMovie);
    }
}

void CUTSExecutionWidget::setState(CUTS_EXECUTION_STATES newState)
{
    if(state == INITIAL){
        if(newState == PARAMETERS_OKAY){
            //ENABLE BUTTON
            nextButton->setEnabled(true);
            state = newState;
        }
    }else if(state == PARAMETERS_OKAY){
        //RESET
        if(newState == INITIAL){
            state = newState;
        }else if(newState == RUN_XSL){
            state = newState;
            //Start the timer
            currentTimer->start(500);
            currentTime.restart();
            updateTimeText(currentTime);

            //Disable the Path/GraphML Button.
             nextButton->setEnabled(false);
            graphmlPathEdit->setEnabled(false);
            outputPathEdit->setEnabled(false);
            graphmlPathButton->setEnabled(false);
            outputPathButton->setEnabled(false);

            //If our Parameters are okay, we can proceed to RUN_XSL
            cutsManager->executeXSLGeneration(graphmlPathEdit->text(), outputPathEdit->text());
        }
    }else if(state == RUN_XSL){
        if(newState == RAN_XSL){
            state = newState;

            //Allow the Tab Widget.
            tabWidget->setTabEnabled(1, true);

            //Update Next Button
            nextButton->setIcon(tabWidget->tabIcon(1));
            nextButton->setText(tabWidget->tabText(1));
            nextButton->setEnabled(true);
       }else if(newState == INITIAL){
            state = newState;
        }
    }else if(state == RAN_XSL){
        if(newState == RUN_MWC){
            state = newState;

            currentTime.restart();
            updateTimeText(currentTime);

            //Move to the Build Tab.
            tabWidget->setCurrentIndex(1);
            setIconLoading(mwcIcon);
            cutsManager->executeMWCGeneration(outputPathEdit->text());
        }
    }else if(state == RUN_MWC){
        if(newState == RAN_MWC){
            state = newState;
            setIconSuccess(mwcIcon, true);
        }
    }else if(state == RAN_MWC){
        if(newState == RUN_MAKE){
            state = newState;
            currentTime.restart();
            updateTimeText(currentTime);

            setIconLoading(makeIcon);
            cutsManager->executeCPPCompilation(outputPathEdit->text());
            //DO SHIFT
        }
    }else if(state == RUN_MAKE){
        if(newState == RAN_MAKE){
            state = newState;
            setIconSuccess(makeIcon, true);
            //Allow the Tab Widget.
            tabWidget->setTabEnabled(2, true);

            //Update Next Button
            nextButton->setIcon(tabWidget->tabIcon(2));
            nextButton->setText(tabWidget->tabText(2));
            nextButton->setEnabled(true);
            currentTime.restart();
        }
    }else if(state == RAN_MAKE){
        if(newState == RUN_EXECUTION){
            state = newState;

            currentTime.restart();
            updateTimeText(currentTime);

            //Move to the Build Tab.
            tabWidget->setCurrentIndex(2);
            cutsManager->executeCUTS(outputPathEdit->text());
        }
    }else if(state == RUN_EXECUTION){
        if(newState == RAN_EXECUTION){
            state = newState;

            nextButton->setIcon(tabWidget->tabIcon(2));
            nextButton->setText("Re-Execute");
        }
    }else if(state == RAN_EXECUTION){
        if(newState == RERUN_EXECUTION){
            state = RAN_MAKE;
            setState(RUN_EXECUTION);
        }
    }

    //IF WE HAVE BEEN RESET
    if(state == INITIAL){
        //Disable all Tabs
        tabWidget->setTabEnabled(0, true);
        tabWidget->setTabEnabled(1, false);
        tabWidget->setTabEnabled(2, false);

        //Enable the Path Buttons again.
        graphmlPathEdit->setEnabled(true);
        outputPathEdit->setEnabled(true);
        graphmlPathButton->setEnabled(true);
        outputPathButton->setEnabled(true);


        //Update Next Button
        nextButton->setIcon(tabWidget->tabIcon(0));
        nextButton->setText(tabWidget->tabText(0));
        nextButton->setEnabled(false);
        nextButton->setVisible(true);
    }
}


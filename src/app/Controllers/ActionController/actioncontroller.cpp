#include "actioncontroller.h"
#include "../ViewController/viewcontroller.h"
#include "../../theme.h"
#include "../../Utils/filehandler.h"
#include "../../Utils/rootaction.h"
#include <QDebug>
#include "../../../modelcontroller/nodekinds.h"
#include "../../../modelcontroller/modelcontroller.h"
#include <QShortcut>
#include <QApplication>
ActionController::ActionController(ViewController* vc) : QObject(vc)
{
    

    viewController = vc;
    shortcutDialog = 0;
    recentProjectMapper = 0;

    selectionController = viewController->getSelectionController();

    got_valid_jenkins = false;
    got_java = false;
    setupActions();

    setupMainMenu();
    setupApplicationToolbar();
    setupContextToolbar();

    setupRecentProjects();


    connect(SettingsController::settings(), &SettingsController::settingChanged, this, &ActionController::settingChanged);
    connect(Theme::theme(), &Theme::theme_Changed, this, &ActionController::themeChanged);


    themeChanged();
    connectViewController(vc);
    connectSelectionController();

    updateActions();
}

void ActionController::connectViewController(ViewController *controller)
{

    if(viewController){
        connect(controller, &ViewController::vc_ActionFinished, this, &ActionController::actionFinished);
        connect(controller, &ViewController::vc_controllerReady, this, &ActionController::ModelControllerReady);
        connect(controller, &ViewController::GotJenkins, this, &ActionController::jenkinsValidated);
        connect(controller, &ViewController::GotJava, this, &ActionController::gotJava);
        connect(controller, &ViewController::GotRe, this, &ActionController::gotRe);

        connect(controller, &ViewController::mc_undoRedoUpdated, this, &ActionController::updateUndoRedo);
        connect(controller, &ViewController::vc_addProjectToRecentProjects, this, &ActionController::addRecentProject);
        connect(controller, &ViewController::vc_removeProjectFromRecentProjects, this, &ActionController::removeRecentProject);


        connect(file_newProject, &QAction::triggered, viewController, &ViewController::newProject);
    
        connect(file_openProject, &QAction::triggered, viewController, &ViewController::OpenProject);
        connect(file_closeProject, &QAction::triggered, viewController, &ViewController::closeProject);
        connect(file_saveProject, &QAction::triggered, viewController, &ViewController::saveProject);
        connect(file_saveAsProject, &QAction::triggered, viewController, &ViewController::saveAsProject);
        connect(file_importGraphML, &QAction::triggered, viewController, &ViewController::importProjects);
        connect(file_exit, &QAction::triggered, viewController, &ViewController::closeMEDEA);

        connect(edit_undo, &QAction::triggered, viewController, &ViewController::vc_undo);
        connect(edit_redo, &QAction::triggered, viewController, &ViewController::vc_redo);
        connect(edit_cut, &QAction::triggered, viewController, &ViewController::cut);
        connect(edit_copy, &QAction::triggered, viewController, &ViewController::copy);
        connect(edit_paste, &QAction::triggered, viewController, &ViewController::paste);
        connect(edit_replicate, &QAction::triggered, viewController, &ViewController::replicate);
        connect(view_fitView, &QAction::triggered, [=](){emit viewController->vc_fitToScreen(true);});
        connect(view_fitAllViews, &QAction::triggered, [=](){emit viewController->vc_fitToScreen(false);});

        connect(edit_alignHorizontal, &QAction::triggered, viewController, &ViewController::alignSelectionHorizontal);
        connect(edit_alignVertical, &QAction::triggered, viewController, &ViewController::alignSelectionVertical);


        connect(view_centerOn, &QAction::triggered, viewController, &ViewController::centerSelection);
        connect(edit_delete, &QAction::triggered, viewController, &ViewController::deleteSelection);
        connect(edit_renameActiveSelection, &QAction::triggered, viewController, &ViewController::editLabel);
        connect(toolbar_replicateCount, &QAction::triggered, viewController, &ViewController::editReplicationCount);

        connect(view_centerOnImpl, &QAction::triggered, viewController, &ViewController::centerImpl);
        connect(view_centerOnDefn, &QAction::triggered, viewController, &ViewController::centerDefinition);
        connect(view_viewDefnInNewWindow, &QAction::triggered, viewController, &ViewController::popupDefinition);
        connect(view_viewImplInNewWindow, &QAction::triggered, viewController, &ViewController::popupImpl);
        connect(view_viewInNewWindow, &QAction::triggered, viewController, &ViewController::popupSelection);
        connect(view_viewConnections, &QAction::triggered, viewController, &ViewController::selectAndCenterConnectedEntities);


        connect(toolbar_wiki, &QAction::triggered, viewController, &ViewController::showWikiForSelectedItem);
        connect(help_wiki, &QAction::triggered, viewController, &ViewController::showWiki);
        connect(help_reportBug, &QAction::triggered, viewController, &ViewController::reportBug);
        connect(help_aboutQt, &QAction::triggered, viewController, &ViewController::aboutQt);
        connect(help_aboutMedea, &QAction::triggered, viewController, &ViewController::aboutMEDEA);


        connect(edit_expand, &QAction::triggered, viewController, &ViewController::expandSelection);
        connect(edit_contract, &QAction::triggered, viewController, &ViewController::contractSelection);



        connect(jenkins_importNodes, &QAction::triggered, viewController, &ViewController::RequestJenkinsNodes);
        connect(jenkins_executeJob, &QAction::triggered, viewController, &ViewController::RequestJenkinsBuildJob);
        connect(jenkins_showBrowser, &QAction::triggered, viewController, &ViewController::showExecutionMonitor);
        connect(jenkins_listJobs, &QAction::triggered, viewController, &ViewController::ListJenkinsJobs);



        connect(options_settings, &QAction::triggered, SettingsController::settings(), &SettingsController::showSettingsWidget);

        connect(toolbar_addDDSQOSProfile, &QAction::triggered, viewController, &ViewController::constructDDSQOSProfile);

        connect(model_getCodeForComponent, &QAction::triggered, viewController, &ViewController::getCodeForComponent);
        connect(model_validateModel, &QAction::triggered, viewController, &ViewController::validateModel);
        connect(model_selectModel, &QAction::triggered, viewController, &ViewController::selectModel);
        connect(model_generateModelWorkspace, &QAction::triggered, viewController, &ViewController::generateWorkspace);
        connect(model_executeLocalJob, &QAction::triggered, viewController, &ViewController::executeModelLocal);

        //connect(model_executeLocalJob, &QAction::triggered, viewController, &ViewController::launchLocalDeployment);
        //connect(file_importXME, &QAction::triggered, viewController, &ViewController::importXMEProject);
        //connect(file_importXMI, &QAction::triggered, viewController, &ViewController::importXMIProject);
        connect(file_importIdl, &QAction::triggered, viewController, &ViewController::importIdlFiles);
        
        connect(file_recentProjects_clearHistory, &QAction::triggered, this, &ActionController::clearRecentProjects);

        connect(help_shortcuts, &QAction::triggered, this, &ActionController::showShortcutDialog);

        connect(edit_incrementIndex, &QAction::triggered, [=](){viewController->incrementSelectedKey("index");});
        connect(edit_decrementIndex, &QAction::triggered, [=](){viewController->decrementSelectedKey("index");});

        connect(edit_incrementRow, &QAction::triggered, [=](){viewController->incrementSelectedKey("row");});
        connect(edit_decrementRow, &QAction::triggered, [=](){viewController->decrementSelectedKey("row");});
    }
}

void ActionController::connectSelectionController()
{
    if(selectionController){
        connect(selectionController, &SelectionController::selectionChanged, this, &ActionController::selectionChanged);
        connect(edit_CycleActiveSelectionForward, SIGNAL(triggered(bool)), selectionController, SLOT(cycleActiveSelectionForward()));
        connect(edit_CycleActiveSelectionBackward, SIGNAL(triggered(bool)), selectionController, SLOT(cycleActiveSelectionBackward()));
        connect(edit_selectAll, SIGNAL(triggered(bool)), selectionController, SIGNAL(selectAll()));

        connect(view_zoomIn, &QAction::triggered, selectionController, &SelectionController::zoomIn);
        connect(view_zoomOut, &QAction::triggered, selectionController, &SelectionController::zoomOut);
        
        
        connect(edit_clearSelection, SIGNAL(triggered(bool)), selectionController, SIGNAL(clearSelection()));
    }
}


/**
 * @brief ActionController::getRootAction
 * This will return a copy of the root action specified by kind if it exists.
 * If it doesn't, it will construct the root action and then return a copy of it.
 * @param action
 * @param stealth
 * @return
 */
RootAction* ActionController::getRootAction(QString actionKey)
{

    RootAction* action = 0;
    if(actionHash.contains(actionKey)){
        action = actionHash[actionKey];
    }
    return action;
}

RootAction *ActionController::createRootAction(QString category, QString name, QString hashKey, QString iconPath, QString aliasPath)
{
    RootAction* action = new RootAction(category, name, this);
    action->setIconPath(iconPath, aliasPath);
    if(hashKey != ""){
        actionHash[hashKey] = action;
    }
    if(!actionCategoryMap.contains(category, action)){
        actionCategoryMap.insertMulti(category, action);
    }
    allActions.append(action);
    return action;
}

void ActionController::showShortcutDialog()
{
    if(!shortcutDialog){
        shortcutDialog = new ShortcutDialog(0);

        auto list = actionCategoryMap.uniqueKeys();
        std::sort(list.begin(), list.end());
        
        for(auto key : list){
            QList<RootAction*> actions; 
            
            for(auto action : actionCategoryMap.values(key)){
                if(action && !action->shortcut().isEmpty()){
                    actions.append(action);
                }
            }

            std::sort(actions.begin(), actions.end(), [](const QAction* a, const QAction* b){
                return a->text() < b->text();
                });

            if(actions.length()){
                shortcutDialog->addTitle(key, "Icons", key);
                for(auto action : actions){
                    shortcutDialog->addShortcut(action->shortcut().toString(), action->text(), action->getIconPath(), action->getIconAlias());
                }
            }
        }
        shortcutDialog->resizeTable();
    }
    shortcutDialog->show();
}

void ActionController::clearRecentProjects()
{
    SettingsController::settings()->setSetting(SETTINGS::GENERAL_RECENT_PROJECTS, QStringList());
}


void ActionController::addRecentProject(QString file_path){
    FileHandler::sanitizeFilePath(file_path);
    auto project_list = recentProjectKeys;

    //Get the index of the filename opened (if it exists)
    project_list.removeAll(file_path);
    project_list.prepend(file_path);

    //Shrink the size to 8
    while(project_list.size() > 8){
        project_list.removeLast();
    }

    //Update the settings
    SettingsController::settings()->setSetting(SETTINGS::GENERAL_RECENT_PROJECTS, project_list);
}

void ActionController::removeRecentProject(QString file_path){
    FileHandler::sanitizeFilePath(file_path);
    auto project_list = recentProjectKeys;
    if(project_list.removeAll(file_path)){
        //Only update if things have changed
        SettingsController::settings()->setSetting(SETTINGS::GENERAL_RECENT_PROJECTS, project_list);
    }
}

void ActionController::settingChanged(SETTINGS key, QVariant value)
{
    bool boolVal = value.toBool();

    QAction* action = getSettingAction(key);

    if(action){
        action->setVisible(boolVal);
    }

    if(key == SETTINGS::GENERAL_RECENT_PROJECTS){
        recentProjectsChanged();
    }
}

void ActionController::jenkinsValidated(bool success)
{
    if(got_valid_jenkins != success){
        got_valid_jenkins = success;
        updateJenkinsActions();
    }
}

void ActionController::gotJava(bool java)
{
    if(got_java != java){
        got_java = java;
        updateJenkinsActions();
    }
}

void ActionController::gotRe(bool re)
{
    if(got_re != re){
        got_re = re;
        updateReActions();
    }
}

void ActionController::selectionChanged(int selection_size)
{
    if(selectionController){
        QSet<SELECTION_PROPERTIES> selection_properties;
        auto selection = selectionController->getSelectionIDs();
        selection_size = selection.size();

        
        auto model_controller = viewController->getModelController();
        if(model_controller){
            auto id = selectionController->getActiveSelectedID();
            selection_properties = model_controller->getSelectionProperties(id, selection);
        }

        bool controller_ready = viewController->isControllerReady();
        bool model_actions = controller_ready;


        bool got_selection = selection_size > 0;
        bool got_single_selection = selection_size == 1;
        bool got_multi_selection = selection_size > 1;

        //New checks
        edit_cut->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_CUT));
        edit_copy->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_COPY));
        edit_paste->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_PASTE));
        edit_replicate->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_REPLICATE));
        edit_delete->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_REMOVE));
        edit_renameActiveSelection->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_RENAME));

        edit_incrementIndex->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_CHANGE_INDEX));
        edit_decrementIndex->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_CHANGE_INDEX));

        edit_incrementRow->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_CHANGE_ROW));
        edit_decrementRow->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::CAN_CHANGE_ROW));


        

        //Active selection based.
        view_centerOnDefn->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::GOT_DEFINITION));
        view_viewDefnInNewWindow->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::GOT_DEFINITION));
        view_centerOnImpl->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::GOT_IMPLEMENTATION));
        view_viewImplInNewWindow->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::GOT_IMPLEMENTATION));
        view_viewConnections->setEnabled(selection_properties.contains(SELECTION_PROPERTIES::GOT_EDGES));

    
        //Selection based.
        toolbar_wiki->setEnabled(got_selection);
        edit_expand->setEnabled(got_selection);
        edit_contract->setEnabled(got_selection);


        edit_clearSelection->setEnabled(got_selection);
        view_centerOn->setEnabled(got_selection);
        
        //Single Selection
        edit_selectAll->setEnabled(got_single_selection);
        view_viewInNewWindow->setEnabled(got_single_selection);

        //Multi Selection
        edit_alignHorizontal->setEnabled(got_multi_selection);
        edit_alignVertical->setEnabled(got_multi_selection);
        edit_CycleActiveSelectionForward->setEnabled(got_multi_selection);
        edit_CycleActiveSelectionBackward->setEnabled(got_multi_selection);
        
        view_fitView->setEnabled(got_selection);
        view_fitAllViews->setEnabled(controller_ready);

        auto active_item = selectionController->getActiveSelectedItem();
        
        if(active_item && active_item->isNode()){
            auto node_item = (NodeViewItem*) active_item;
            auto node_kind = node_item->getNodeKind();
            toolbar_replicateCount->setEnabled(node_kind == NODE_KIND::COMPONENT_ASSEMBLY);
            model_getCodeForComponent->setEnabled(got_java && (node_kind == NODE_KIND::COMPONENT || node_kind == NODE_KIND::COMPONENT_INSTANCE || node_kind == NODE_KIND::COMPONENT_IMPL));
        }

        //applicationToolbar->updateSpacers();
    }
}

void ActionController::actionFinished()
{
    selectionChanged(-1);
}

void ActionController::ModelControllerReady(bool)
{
    updateActions();
}

void ActionController::themeChanged()
{
    Theme* theme = Theme::theme();

    foreach(RootAction* action, allActions){
        updateIcon(action, theme);
    }

    foreach(RootAction* action, recentProjectActions.values()){
        updateIcon(action, theme);
    }


    menu_file_recentProjects->setIcon(theme->getIcon("Icons", "clock"));
}

void ActionController::updateJenkinsActions()
{
    bool controller_ready = viewController->isControllerReady();

    jenkins_importNodes->setEnabled(controller_ready && got_valid_jenkins);
    jenkins_executeJob->setEnabled(controller_ready && got_valid_jenkins);
    jenkins_listJobs->setEnabled(controller_ready && got_valid_jenkins);
    jenkins_showBrowser->setEnabled(controller_ready && got_valid_jenkins);
    
    model_generateModelWorkspace->setEnabled(controller_ready && got_java);
    model_executeLocalJob->setEnabled(controller_ready && got_java);
    
    model_validateModel->setEnabled(controller_ready && got_java);
}
void ActionController::updateReActions(){
    bool controller_ready = viewController->isControllerReady();
    model_executeLocalJob->setEnabled(controller_ready && got_re && got_java);
}

void ActionController::updateUndoRedo()
{
    if(viewController){
        bool controller_ready = viewController->isControllerReady();
        edit_undo->setEnabled(controller_ready && viewController->canUndo());
        edit_redo->setEnabled(controller_ready && viewController->canRedo());
    }
}

QAction *ActionController::getSettingAction(SETTINGS key)
{
    switch(key){
    case SETTINGS::TOOLBAR_CONTEXT:
        return toolbar_context;
    case SETTINGS::TOOLBAR_UNDO:
        return toolbar_undo;
    case SETTINGS::TOOLBAR_REDO:
        return toolbar_redo;
    case SETTINGS::TOOLBAR_CUT:
        return toolbar_cut;
    case SETTINGS::TOOLBAR_COPY:
        return toolbar_copy;
    case SETTINGS::TOOLBAR_PASTE:
        return toolbar_paste;
    case SETTINGS::TOOLBAR_REPLICATE:
        return toolbar_replicate;
    case SETTINGS::TOOLBAR_FIT_TO_SCREEN:
        return toolbar_fitToScreen;
    case SETTINGS::TOOLBAR_CENTER_SELECTION:
        return toolbar_centerOn;
    case SETTINGS::TOOLBAR_VIEW_IN_NEWWINDOW:
        return toolbar_viewInNewWindow;
    case SETTINGS::TOOLBAR_DELETE:
        return toolbar_delete;
    case SETTINGS::TOOLBAR_ALIGN_HORIZONTAL:
        return toolbar_alignVertical;
    case SETTINGS::TOOLBAR_ALIGN_VERTICAL:
        return toolbar_alignHorizontal;
    case SETTINGS::TOOLBAR_SEARCH:
        return toolbar_search;
    case SETTINGS::TOOLBAR_CONTRACT:
        return toolbar_contract;
    case SETTINGS::TOOLBAR_EXPAND:
        return toolbar_expand;
    case SETTINGS::TOOLBAR_VALIDATE:
        return toolbar_validate;
    default:
        return 0;
    }
}

void ActionController::updateActions()
{
    bool controller_ready = viewController->isControllerReady();

    //Always Enabled
    file_newProject->setEnabled(true);
    file_openProject->setEnabled(true);
    file_closeProject->setEnabled(true);

    file_importGraphML->setEnabled(controller_ready);
    
    file_importIdl->setEnabled(controller_ready);
    //file_importXME->setEnabled(modelActions);
    //file_importXMI->setEnabled(modelActions);
    file_saveProject->setEnabled(controller_ready);
    file_saveAsProject->setEnabled(controller_ready);
    file_closeProject->setEnabled(controller_ready);

    model_selectModel->setEnabled(controller_ready);
    model_validateModel->setEnabled(controller_ready);
    model_generateModelWorkspace->setEnabled(controller_ready);

    edit_search->setEnabled(controller_ready);
    view_fitView->setEnabled(controller_ready);
    view_fitAllViews->setEnabled(controller_ready);

    toolbar_contextToolbar->setEnabled(controller_ready);
    toolbar_addDDSQOSProfile->setEnabled(controller_ready);

    //Update the selection changed with a special thing.
    selectionChanged(-1);

    updateUndoRedo();
    updateJenkinsActions();
    updateReActions();
}

void ActionController::createRecentProjectAction(QString fileName)
{
    if(!recentProjectActions.contains(fileName)){
        RootAction* action = new RootAction("Project", fileName, this);
        action->setIconPath("Icons", "file");
        updateIcon(action);
        recentProjectActions.insert(fileName, action);
        connect(action, &QAction::triggered, recentProjectMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        recentProjectMapper->setMapping(action, fileName);
    }
}

void ActionController::recentProjectsChanged()
{
    if(!recentProjectMapper){
        recentProjectMapper = new QSignalMapper(this);
        connect(recentProjectMapper, static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), viewController, &ViewController::OpenExistingProject);
    }

    //Load in the defaults.
    QStringList list = SettingsController::settings()->getSetting(SETTINGS::GENERAL_RECENT_PROJECTS).toStringList();

    QStringList orderedKeys;

    foreach(QString filepath, list){
        FileHandler::sanitizeFilePath(filepath);
        createRecentProjectAction(filepath);
        orderedKeys.append(filepath);
    }
    foreach(QString oldKey, recentProjectKeys){
        if(orderedKeys.contains(oldKey)){
            continue;
        }else{
            RootAction* action = recentProjectActions.value(oldKey, 0);
            if(action){
                recentProjectActions.remove(oldKey);
                action->deleteLater();
            }
        }
    }
    if(orderedKeys != recentProjectKeys){

        //Update Menus
        menu_file_recentProjects->clear();

        foreach(QString key, orderedKeys){
            RootAction* action = recentProjectActions.value(key, 0);
            if(action){
                menu_file_recentProjects->addAction(action);
            }
        }

        menu_file_recentProjects->addSeparator();
        menu_file_recentProjects->addAction(file_recentProjects_clearHistory);
        recentProjectKeys = orderedKeys;
        emit recentProjectsUpdated();
    }

}

void ActionController::updateIcon(RootAction *action, Theme *theme)
{
    if(theme && action){
        auto pair = action->getIconPair();
        auto icon = theme->getIcon(pair);
        if(!icon.isNull()){
            action->setIcon(icon);
        }
    }
}

QList<RootAction *> ActionController::getRecentProjectActions()
{
    QList<RootAction*> actions;

    foreach(QAction* action, menu_file_recentProjects->actions()){
        if(action->isSeparator()){
            continue;
        }
        if(action == file_recentProjects_clearHistory){
            continue;
        }
        RootAction* a = qobject_cast<RootAction*>(action);
        if(a){
            actions.append(a);
        }
    }

    return actions;
}


QList<QAction*> ActionController::getAllActions(){
    QList<QAction*> actions;
    for(auto action : allActions){
        actions.append(action);
    }
    return actions;
}

QList<QAction *> ActionController::getNodeViewActions()
{
    return view_actions;
}

QList<QAction *> ActionController::getGlobalActions(){
    QList<QAction*> actions;
    for(auto action : allActions){
        if(action->shortcutContext() == Qt::ApplicationShortcut){
            actions.append(action);
        }
    }
    return actions;
}

void ActionController::setupActions()
{
    file_newProject = createRootAction("Project", "New Project", "", "Icons", "file");
    file_newProject->setToolTip("Construct a new project.");
    file_newProject->setShortcutContext(Qt::ApplicationShortcut);
    file_newProject->setShortcut(QKeySequence::New);

    file_openProject = createRootAction("Project", "Open Project", "", "Icons", "folder");
    file_openProject->setToolTip("Open an existing project.");
    file_openProject->setShortcutContext(Qt::ApplicationShortcut);
    file_openProject->setShortcut(QKeySequence::Open);

    file_recentProjects_clearHistory = createRootAction("Misc", "Clear History", "", "Icons", "cross");

    file_saveProject = createRootAction("Project", "Save Project", "", "Icons", "floppyDisk");
    file_saveProject->setToolTip("Save current project.");
    file_saveProject->setShortcutContext(Qt::ApplicationShortcut);
    file_saveProject->setShortcut(QKeySequence::Save);


    file_saveAsProject = createRootAction("Project", "Save Project As", "", "Icons", "floppyDisk");
    file_saveAsProject->setToolTip("Save current project into a different file.");
    file_saveAsProject->setShortcutContext(Qt::ApplicationShortcut);
    file_saveAsProject->setShortcut(QKeySequence::SaveAs);

    file_closeProject = createRootAction("Project", "Close Project", "", "Icons", "cross");
    file_closeProject->setToolTip("Close current project.");
    file_closeProject->setShortcutContext(Qt::ApplicationShortcut);
    file_closeProject->setShortcut(QKeySequence::Close);


    file_importGraphML = createRootAction("Project", "Import Project", "", "Icons", "clipboardDown");
    file_importGraphML->setToolTip("Import Project into current project.");
    file_importGraphML->setShortcutContext(Qt::ApplicationShortcut);
    file_importGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));


    
    file_importIdl = createRootAction("Project", "Import IDL Files", "", "Icons", "clipboardDown");
    file_importIdl->setToolTip("Import IDL files into current project.");
    //file_importXME = createRootAction("Project", "Import XME Project", "", "Icons", "gme");
    //file_importXME->setToolTip("Import XME Project into current project.");
    //file_importXMI = createRootAction("Project", "Import UML XMI Project", "", "Icons", "uml");
    //file_importXMI->setToolTip("Import XMI Project into current project.");

  
    edit_undo = createRootAction("Edit", "Undo", "", "Icons", "arrowUndo");
    edit_undo->setToolTip("Undo last model change.");
    edit_undo->setShortcutContext(Qt::ApplicationShortcut);
    edit_undo->setShortcut(QKeySequence::Undo);

    edit_redo = createRootAction("Edit", "Redo", "", "Icons", "arrowRedo");
    edit_redo->setToolTip("Redo last undo.");
    edit_redo->setShortcutContext(Qt::ApplicationShortcut);
    edit_redo->setShortcut(QKeySequence::Redo);


    edit_cut = createRootAction("Edit", "Cut", "", "Icons", "scissors");
    edit_cut->setToolTip("Cut selection.");
    edit_cut->setShortcut(QKeySequence::Cut);

    edit_copy = createRootAction("Edit", "Copy", "", "Icons", "copy");
    edit_copy->setToolTip("Copy selection.");
    edit_copy->setShortcut(QKeySequence::Copy);

    edit_paste = createRootAction("Edit", "Paste", "", "Icons", "clipboard");
    edit_paste->setToolTip("Paste clipboard into selected entity.");
    edit_paste->setShortcut(QKeySequence::Paste);

    edit_replicate = createRootAction("Edit", "Replicate", "", "Icons", "copyList");
    edit_replicate->setToolTip("Replicate the selected entities.");
    edit_replicate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    edit_delete = createRootAction("Edit", "Delete", "Delete", "Icons", "bin");
    edit_delete->setToolTip("Delete the selected entities.");
    edit_delete->setShortcut(QKeySequence::Delete);

    edit_renameActiveSelection = createRootAction("Edit", "Rename", "Rename", "Icons", "letterA");
    edit_renameActiveSelection->setToolTip("Rename the selected entity.");
    edit_renameActiveSelection->setShortcut(QKeySequence(Qt::Key_F2));


    edit_search = createRootAction("Edit", "Search", "Root_Search", "Icons", "zoom");
    edit_search->setToolTip("Search model.");
    edit_search->setShortcutContext(Qt::ApplicationShortcut);
    edit_search->setShortcut(QKeySequence::Find);



    //edit_sort = createRootAction("Edit", "Sort", "", "Icons", "letterAZ");
    //edit_sort->setToolTip("Sort selection.");

    edit_alignVertical = createRootAction("Edit", "Align Vertically", "", "Icons", "alignVertical");
    edit_alignVertical->setToolTip("Align selection vertically.");

    edit_alignHorizontal = createRootAction("Edit", "Align Horizontally", "", "Icons", "alignHorizontal");
    edit_alignHorizontal->setToolTip("Align selection horizontally.");


    edit_CycleActiveSelectionForward = createRootAction("Selection", "Cycle Next Selected Item", "", "Icons", "arrowHeadRight");
    edit_CycleActiveSelectionForward->setToolTip("Cycle between active selected entities.");
    edit_CycleActiveSelectionForward->setShortcut(QKeySequence::NextChild);

    edit_CycleActiveSelectionBackward = createRootAction("Selection", "Cycle Prev Selected Item", "", "Icons", "arrowHeadLeft");
    edit_CycleActiveSelectionBackward->setToolTip("Cycle(Back) between active selected entities.");
    edit_CycleActiveSelectionBackward->setShortcut(QKeySequence::PreviousChild);

    edit_selectAll = createRootAction("Selection", "Select All", "", "Icons", "gridSelect");
    edit_selectAll->setToolTip("Select all child entities of selection.");
    edit_selectAll->setShortcut(QKeySequence::SelectAll);

    edit_clearSelection = createRootAction("Selection", "Clear Selection", "", "Icons", "cross");
    edit_clearSelection->setToolTip("Clear selection.");
    edit_clearSelection->setShortcut(QKeySequence(Qt::Key_Escape));

    edit_incrementIndex = createRootAction("View", "Increment Index", "", "Icons", "plus");
    edit_decrementIndex = createRootAction("View", "Decrement Index", "", "Icons", "lineHorizontal");
    
    edit_incrementRow = createRootAction("View", "Increment Row", "", "Icons", "plus");
    edit_decrementRow = createRootAction("View", "Decrement Row", "", "Icons", "lineHorizontal");

    

    view_fitView = createRootAction("View", "Fit View", "", "Icons", "screenResize");
    view_fitView->setToolTip("Center all entities in active view.");
    view_fitView->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));

    view_fitAllViews = createRootAction("View", "Fit All Views", "", "Icons", "screenResize");
    view_fitAllViews->setToolTip("Center all entities in all views.");
    view_fitAllViews->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Space));

    view_centerOn = createRootAction("View", "Center On Selection", "", "Icons", "crosshair");
    
    view_zoomIn = createRootAction("View", "Zoom In", "", "Icons", "zoomIn");
    view_zoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Equal));

    view_zoomOut = createRootAction("View", "Zoom Out", "", "Icons", "zoomOut");
    view_zoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));

    edit_expand = createRootAction("Toolbar", "Expand Selection", "", "Icons", "triangleSouthEast");
    edit_expand->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_E));
    edit_contract = createRootAction("Toolbar", "Contract Selection", "", "Icons", "triangleNorthWest");
    edit_contract->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_C));

    view_centerOnDefn = createRootAction("View", "Center On Definition", "", "Icons", "bracketsCurly");
    view_centerOnDefn->setToolTip("Center selected entity's Definition.");
    view_centerOnDefn->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));

    view_centerOnImpl = createRootAction("View", "Center On Implementation", "", "Icons", "gears");
    view_centerOnImpl->setToolTip("Center selected entity's Implementation.");
    view_centerOnImpl->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));

    view_viewDefnInNewWindow = createRootAction("View", "Show Definition in New Window", "", "Icons", "bracketsCurly");
    view_viewDefnInNewWindow->setToolTip("Popout selected entity's Definition.");
    view_viewDefnInNewWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_D));

    view_viewImplInNewWindow = createRootAction("View", "Show Implementation in New Window", "", "Icons", "gears");
    view_viewImplInNewWindow->setToolTip("Popout selected entity's Implementation.");
    view_viewImplInNewWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));


    view_viewConnections = createRootAction("View", "Select and Center Items Connections", "", "Icons", "connectFork");
    view_viewConnections->setToolTip("Center selected entity's connected entities.");
    view_viewConnections->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_E));


    view_viewInNewWindow = createRootAction("View", "View In New Window", "", "Icons", "popOut");
    view_viewInNewWindow->setToolTip("Popout selected entity.");

    model_selectModel= createRootAction("Selection", "Select Model", "", "Icons", "medeaLogo");
    model_selectModel->setToolTip("Select the Model entity.");
    model_selectModel->setShortcutContext(Qt::ApplicationShortcut);
    model_selectModel->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_M));

    model_validateModel = createRootAction("Model", "Validate Model", "", "Icons", "shieldTick");
    model_validateModel->setToolTip("Validate the current project.");
    model_validateModel->setShortcutContext(Qt::ApplicationShortcut);
    model_validateModel->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));

    dock_addPart = createRootAction("Dock", "Open Add Part Dock", "", "Icons", "plus");
    dock_addPart->setToolTip("Open the add parts dock");
    dock_addPart->setShortcutContext(Qt::ApplicationShortcut);
    dock_addPart->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));

    dock_deploy = createRootAction("Dock", "Open Deploy Dock", "", "Icons", "screenTwoTone");
    dock_deploy->setToolTip("Open the deploy dock");
    dock_deploy->setShortcutContext(Qt::ApplicationShortcut);
    dock_deploy->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_D));

    

    model_getCodeForComponent = createRootAction("Model", "Generate Code for Component", "", "Icons", "bracketsAngled");
    model_getCodeForComponent->setToolTip("Generate the C++ Impl code for the selected Component");
    model_getCodeForComponent->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));

    model_generateModelWorkspace = createRootAction("Model", "Generate Model Workspace", "", "Icons", "briefcase");
    model_generateModelWorkspace->setToolTip("Generate all the C++ artifacts for the model");

    model_executeLocalJob = createRootAction("Model", "Launch: Local Deployment", "", "Icons", "jobBuild");
    model_executeLocalJob->setToolTip("Executes the current project on the local machine.");
    model_executeLocalJob->setShortcutContext(Qt::ApplicationShortcut);
    model_executeLocalJob->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    




    jenkins_importNodes = createRootAction("Jenkins", "Import Jenkins Nodes", "", "EntityIcons", "HardwareNode");
    jenkins_importNodes->setToolTip("Imports the nodes from the Jenkins Server.");
    jenkins_importNodes->setShortcutContext(Qt::ApplicationShortcut);
    jenkins_importNodes->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));


    jenkins_executeJob = createRootAction("Jenkins", "Launch: Jenkins Job", "", "Icons", "jobBuild");
    jenkins_executeJob->setToolTip("Executes the current project on the Jenkins Server.");
    jenkins_executeJob->setShortcutContext(Qt::ApplicationShortcut);
    jenkins_executeJob->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B));

    jenkins_listJobs = createRootAction("Jenkins", "Launch other Jenkins Job", "", "Icons", "jobBuild");
    jenkins_listJobs->setToolTip("Builds a Jenkins Job.");
    


    jenkins_showBrowser = createRootAction("Jenkins", "Show Jenkins Execution Monitor", "", "Icons", "bracketsAngled");
    jenkins_showBrowser->setToolTip("Shows Jenkins Execution Monitor.");
    jenkins_showBrowser->setShortcutContext(Qt::ApplicationShortcut);


    

    help_shortcuts = createRootAction("Help", "App Shortcuts", "", "Icons", "keyboard");

    help_reportBug = createRootAction("Help", "Report Bug", "", "Icons", "bug");

    help_wiki = createRootAction("Help", "Wiki", "", "Icons", "book");
    help_wiki->setToolTip("Show the help wiki.");
    help_wiki->setShortcutContext(Qt::ApplicationShortcut);
    help_wiki->setShortcut(QKeySequence::HelpContents);


    help_aboutMedea = createRootAction("Help", "About MEDEA", "", "Icons", "circleInfo");
    help_aboutQt = createRootAction("Help", "About Qt", "", "Icons", "qt");

    options_settings = createRootAction("Help", "Settings", "", "Icons", "gear");
    options_settings->setToolTip("Show application settings.");
    options_settings->setShortcutContext(Qt::ApplicationShortcut);
    options_settings->setShortcut(QKeySequence(Qt::Key_F10));
    



    file_exit = createRootAction("Project", "Exit", "", "Icons", "arrowIntoBox");

    toolbar_contextToolbar = createRootAction("Toolbar", "Show Context Toolbar", "", "Icons", "gearDark");

    toolbar_addChild = createRootAction("Toolbar", "Add Child Entity", "", "Icons", "plus");
    toolbar_connect = createRootAction("Toolbar", "Connect Selection", "", "Icons", "connect");
    toolbar_popOutDefn = createRootAction("Toolbar", "Popout Definition", "", "Icons", "popOut");
    toolbar_popOutImpl = createRootAction("Toolbar", "Popout Implementation", "", "Icons", "popOut");
   




    toolbar_wiki = createRootAction("Toolbar", "View Wiki", "", "Icons", "book");
    toolbar_replicateCount = createRootAction("Toolbar", "Change Replicate Count", "", "Icons", "copyX");
    toolbar_displayedChildrenOption = createRootAction("Toolbar", "Change Displayed Nodes Settings", "", "Icons", "dotsVertical");

    toolbar_addDDSQOSProfile = createRootAction("Toolbar", "Add Profile", "", "Icons", "plus");
    toolbar_removeDDSQOSProfile = createRootAction("Toolbar", "Remove Profile", "", "Icons", "bin");

    toggleDock = createRootAction("Misc", "Show/Hide Dock", "", "Icons", "dotsVertical");


    view_actions.append(edit_cut);
    view_actions.append(edit_copy);
    view_actions.append(edit_paste);
    view_actions.append(edit_replicate);
    view_actions.append(edit_delete);
    
    view_actions.append(edit_clearSelection);
    view_actions.append(edit_selectAll);
    view_actions.append(edit_alignHorizontal);
    view_actions.append(edit_alignVertical);
    view_actions.append(edit_CycleActiveSelectionForward);
    view_actions.append(edit_CycleActiveSelectionBackward);
    view_actions.append(edit_renameActiveSelection);
    view_actions.append(edit_expand);
    view_actions.append(edit_contract);

    view_actions.append(view_fitView);
    view_actions.append(view_fitAllViews);

    view_actions.append(view_centerOn);
    view_actions.append(view_centerOnDefn);
    view_actions.append(view_viewDefnInNewWindow);
    view_actions.append(view_centerOnImpl);
    view_actions.append(view_viewImplInNewWindow);
    view_actions.append(view_viewConnections);
    view_actions.append(view_viewInNewWindow);
    
    
    

    view_actions.append(model_getCodeForComponent);

    for(auto action : view_actions){
        action->setShortcutContext(Qt::WidgetShortcut);
    }

}
void ActionController::setupMainMenu()
{
    menu_file = new QMenu("File");
    menu_edit = new QMenu("Edit");
    //menu_edit->setPlatformMenu();
    menu_view = new QMenu("View");
    menu_model = new QMenu("Model");
    menu_jenkins = new QMenu("Jenkins");
    menu_options = new QMenu("Options");
    menu_help = new QMenu("Help");

    // File Menu
    menu_file->addAction(file_newProject);
    menu_file->addAction(file_openProject);
    menu_file_recentProjects = menu_file->addMenu("Recent Projects");
    menu_file_recentProjects->addAction(file_recentProjects_clearHistory);
    menu_file->addSeparator();
    menu_file->addAction(file_saveProject);
    menu_file->addAction(file_saveAsProject);
    menu_file->addAction(file_closeProject);
    menu_file->addSeparator();
    menu_file->addAction(file_importGraphML);
    menu_file->addAction(file_importIdl);
    
    //menu_file->addAction(file_importXME);
    //menu_file->addAction(file_importXMI);
    //menu_file->addSeparator();

    menu_file->addSeparator();
    menu_file->addAction(file_exit);

    // Edit Menu
    menu_edit->addAction(edit_undo);
    menu_edit->addAction(edit_redo);
    menu_edit->addSeparator();
    menu_edit->addAction(edit_cut);
    menu_edit->addAction(edit_copy);
    menu_edit->addAction(edit_paste);
    menu_edit->addAction(edit_replicate);
    menu_edit->addSeparator();
    menu_edit->addAction(edit_delete);
    menu_edit->addAction(edit_renameActiveSelection);

    menu_edit->addAction(edit_search);
    menu_edit->addSeparator();
    //menu_edit->addAction(edit_sort);
    menu_edit->addAction(edit_alignHorizontal);
    menu_edit->addAction(edit_alignVertical);
    menu_edit->addSeparator();
    menu_edit->addAction(edit_selectAll);
    menu_edit->addAction(edit_clearSelection);
    menu_edit->addAction(edit_CycleActiveSelectionForward);
    menu_edit->addAction(edit_CycleActiveSelectionBackward);
    menu_edit->addSeparator();
    menu_edit->addAction(edit_incrementIndex);
    menu_edit->addAction(edit_decrementIndex);
    menu_edit->addAction(edit_incrementRow);
    menu_edit->addAction(edit_decrementRow);
    

     // View Menu
    menu_view->addAction(view_fitView);
    menu_view->addAction(view_fitAllViews);
    menu_view->addSeparator();
    menu_view->addAction(view_centerOn);
    menu_view->addAction(view_centerOnDefn);
    menu_view->addAction(view_centerOnImpl);
    menu_view->addSeparator();
    menu_view->addAction(view_viewInNewWindow);
    menu_view->addAction(view_viewDefnInNewWindow);
    menu_view->addAction(view_viewImplInNewWindow);
    menu_view->addAction(view_viewConnections);
    menu_view->addSeparator();
    menu_view->addAction(view_zoomIn);
    menu_view->addAction(view_zoomOut);

    // Model Menu
    menu_model->addAction(model_selectModel);

    menu_model->addAction(model_validateModel);
    menu_model->addAction(model_getCodeForComponent);
    menu_model->addAction(model_generateModelWorkspace);
    menu_model->addAction(model_executeLocalJob);

    // Jenkins Menu

    menu_jenkins->addAction(jenkins_importNodes);
    menu_jenkins->addAction(jenkins_executeJob);
    menu_jenkins->addAction(jenkins_showBrowser);
    menu_jenkins->addAction(jenkins_listJobs);
    

    // Options Menu
    menu_options->addAction(options_settings);

    menu_help->addAction(help_shortcuts);
    menu_help->addAction(help_wiki);
    menu_help->addAction(help_reportBug);
    menu_help->addSeparator();
    menu_help->addAction(help_aboutMedea);
    menu_help->addAction(help_aboutQt);
}

void ActionController::setupApplicationToolbar()
{
    applicationToolbar = new ActionGroup(this);


    toolbar_undo = applicationToolbar->addAction(edit_undo->constructSubAction(false));
    toolbar_redo = applicationToolbar->addAction(edit_redo->constructSubAction(false));
    applicationToolbar->addSeperator();
    toolbar_cut = applicationToolbar->addAction(edit_cut->constructSubAction(false));
    toolbar_copy = applicationToolbar->addAction(edit_copy->constructSubAction(false));
    toolbar_paste = applicationToolbar->addAction(edit_paste->constructSubAction(false));
    toolbar_replicate = applicationToolbar->addAction(edit_replicate->constructSubAction(false));
    applicationToolbar->addSeperator();
    toolbar_fitToScreen = applicationToolbar->addAction(view_fitView->constructSubAction(false));
    toolbar_centerOn = applicationToolbar->addAction(view_centerOn->constructSubAction(false));
    toolbar_viewInNewWindow = applicationToolbar->addAction(view_viewInNewWindow->constructSubAction(false));
    applicationToolbar->addSeperator();
    //toolbar_sort = applicationToolbar->addAction(edit_sort->constructSubAction(false));
    toolbar_alignVertical = applicationToolbar->addAction(edit_alignVertical->constructSubAction(false));
    toolbar_alignHorizontal = applicationToolbar->addAction(edit_alignHorizontal->constructSubAction(false));
    toolbar_contract = applicationToolbar->addAction(edit_contract->constructSubAction(false));
    toolbar_expand = applicationToolbar->addAction(edit_expand->constructSubAction(false));
    applicationToolbar->addSeperator();
    toolbar_delete = applicationToolbar->addAction(edit_delete->constructSubAction(false));
    toolbar_context = applicationToolbar->addAction(toolbar_contextToolbar->constructSubAction(false));
    toolbar_validate = applicationToolbar->addAction(model_validateModel->constructSubAction(false));
    toolbar_search = applicationToolbar->addAction(edit_search->constructSubAction(false));


    

    

    SettingsController* s = SettingsController::settings();
    foreach(SETTINGS key, s->getSettingsKeys("Toolbar", "Visible Buttons")){
        settingChanged(key, s->getSetting(key));
    }
    applicationToolbar->updateSpacers();
}

void ActionController::setupContextToolbar()
{
    contextToolbar = new ActionGroup(this);

    contextToolbar->addAction(toolbar_addChild);
    contextToolbar->addAction(edit_delete->constructSubAction());
    //contextToolbar->addAction(toolbar_hardware);
    //contextToolbar->addAction(toolbar_disconnectHardware);
    contextToolbar->addSeperator();
    //contextToolbar->addSeperator();
    contextToolbar->addAction(view_centerOnDefn->constructSubAction());
    contextToolbar->addAction(view_centerOnImpl->constructSubAction());
    contextToolbar->addSeperator();
    contextToolbar->addAction(toolbar_displayedChildrenOption);
    contextToolbar->addAction(toolbar_replicateCount);
    
    contextToolbar->addSeperator();
    contextToolbar->addAction(view_viewConnections->constructSubAction());
    contextToolbar->addAction(model_getCodeForComponent->constructSubAction());
    contextToolbar->addAction(view_viewInNewWindow->constructSubAction());
    contextToolbar->addAction(toolbar_wiki);

}

void ActionController::setupRecentProjects()
{
    recentProjects = new ActionGroup(this);
    recentProjectsChanged();
}


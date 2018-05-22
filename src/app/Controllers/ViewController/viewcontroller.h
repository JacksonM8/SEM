#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "../ActionController/actioncontroller.h"
#include "../SelectionController/selectioncontroller.h"
#include "../SelectionController/selectionhandler.h"
#include "../NotificationManager/notificationmanager.h"
#include "../../Widgets/DockWidgets/basedockwidget.h"

#include "../ExecutionManager/executionmanager.h"
#include "viewitem.h"
#include "nodeviewitem.h"
#include "edgeviewitem.h"

#include "../../../modelcontroller/kinds.h"
#include "../../../modelcontroller/nodekinds.h"
#include "../../../modelcontroller/edgekinds.h"
#include "../../../modelcontroller/dataupdate.h"
#include "../../../modelcontroller/viewcontrollerint.h"
#include "../JenkinsManager/jenkinsmanager.h"
#include <QTimer>

enum class MODEL_SEVERITY;
class NotificationManager;
class ModelController;
class NodeView;
class JenkinsManager;
class ContextMenu;
class DefaultDockWidget;
class CodeBrowser;
class JobMonitor;

class ViewController : public ViewControllerInterface
{
    Q_OBJECT

public:
    ViewController();
    ~ViewController();

    DefaultDockWidget* constructDockWidget(QString label, QString icon_path, QString icon_name, QWidget* widget, BaseWindow* window = 0);

    static QList<ViewItem*> ToViewItemList(QList<NodeViewItem*> &items);
    static QList<ViewItem*> ToViewItemList(QList<EdgeViewItem*> &items);

    bool isWelcomeScreenShowing();
    ContextMenu* getContextMenu();

    

    JenkinsManager* getJenkinsManager();
    ExecutionManager* getExecutionManager();
    SelectionController* getSelectionController();
    ActionController* getActionController();

    QList<ViewItem*> getConstructableNodeDefinitions(NODE_KIND node_kind, EDGE_KIND edge_kind);

    QStringList _getSearchSuggestions();

    QMap<QString, ViewItem*> getSearchResults(QString query, QList<ViewItem*> view_items = {});
    QList<ViewItem*> filterList(QString query, QList<ViewItem*> view_items);
    QList<ViewItem*> filterList(QString query, QList<NodeViewItem*> view_items){
        return filterList(query, ViewController::ToViewItemList(view_items));
    }
    QList<ViewItem*> filterList(QString query, QList<EdgeViewItem*> view_items){
        return filterList(query, ViewController::ToViewItemList(view_items));
    }

    QHash<EDGE_DIRECTION, ViewItem*> getValidEdges(EDGE_KIND kind);
    QMultiMap<EDGE_DIRECTION, ViewItem*> getExistingEndPointsOfSelection(EDGE_KIND kind);

    ViewDockWidget* constructViewDockWidget(QString title, QWidget* parent);
    

    QList<NodeViewItem*> getNodeKindItems();
    QList<EdgeViewItem*> getEdgeKindItems();
    NodeViewItem* getNodeItem(NODE_KIND kind);

    QList<ViewItem*> getViewItemParents(QList<ViewItem*> items);


    ModelController* getModelController();

    QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > getValidEdgeKinds(QList<int> ids);
    
    QSet<NODE_KIND> getValidNodeKinds();
    QSet<EDGE_KIND> getCurrentEdgeKinds();

    QList<ViewItem*> getExistingEdgeEndPointsForSelection(EDGE_KIND kind);

    QList<QVariant> getValidValuesForKey(int ID, QString keyName);
    static void SetDefaultIcon(ViewItem& viewItem);
    
    ViewItem* getModel();
    bool isControllerReady();

    bool canUndo();
    bool canRedo();

    void RequestJenkinsNodes();
    void RequestJenkinsBuildJob();
    void RequestJenkinsBuildJobName(QString job_name);
    void ShowJenkinsBuildDialog(QString job_name, QList<Jenkins_Job_Parameter> paramaters);
    void ShowJenkinsBuildDialog(QStringList jobs);

    QVector<ViewItem*> getOrderedSelection(QList<int> selection);

    void setController(ModelController* c);


    bool isNodeAncestor(int ID, int ID2);
    VIEW_ASPECT getNodeViewAspect(int ID);
    QVariant getEntityDataValue(int ID, QString key_name);
    void constructEdges(int id, EDGE_KIND edge_kind, EDGE_DIRECTION edge_direction);
private:
    void SetParentNode(ViewItem* parent, ViewItem* child);
    void notification_Added(QSharedPointer<NotificationObject> obj);
    void notification_Destructed(QSharedPointer<NotificationObject> obj);

signals:
    void vc_showWelcomeScreen(bool);
    void GotJava(bool);
    void GotRe(bool);
    void GotJenkins(bool);
    void vc_controllerReady(bool);
    void vc_viewItemConstructed(ViewItem* viewItem);
    void vc_viewItemDestructing(int ID, ViewItem* item);
    void vc_showToolbar(QPoint globalPos, QPointF itemPos = QPointF());
    void vc_gotSearchSuggestions(QStringList suggestions);
    void vc_editTableCell(int ID, QString keyName);
    void vc_centerItem(int ID);
    void vc_selectAndCenterConnectedEntities(ViewItem* item);
    void vc_fitToScreen(bool if_active_view = false);
    void vc_addProjectToRecentProjects(QString filePath);
    void vc_removeProjectFromRecentProjects(QString filePath);
    void vc_highlightItem(int ID, bool highlight);
public slots:
    void incrementSelectedKey(QString key_name);
    void decrementSelectedKey(QString key_name);
    
    void welcomeScreenToggled(bool visible);
    void highlight(QList<int> ids);


    void modelValidated(QString reportPath);
    void importGraphMLFile(QString graphmlPath);
    void importGraphMLExtract(QString data);
    

    void showCodeViewer(QString tabName, QString content);

    JobMonitor* getExecutionMonitor();
    void showExecutionMonitor();
    void RefreshExecutionMonitor(QString job_name);
    void ListJenkinsJobs();

    void jenkinsManager_GotJenkinsNodesList(QString graphmlData);


    void getCodeForComponent();
    void validateModel();
    void selectModel();

    //Interface
protected:
    void ModelReady(bool ready);
    void NodeConstructed(int parent_id, int id, NODE_KIND node_kind);
    void EdgeConstructed(int id, EDGE_KIND edge_kind, int src_id, int dst_id);
    void EntityDestructed(int id, GRAPHML_KIND kind);
    void DataChanged(int id, DataUpdate data);
    void DataRemoved(int id, QString key_name);
    void NodeEdgeKindsChanged(int id);
    void NodeTypesChanged(int id);
    void AddNotification(MODEL_SEVERITY severity, QString title, QString description, int ID);
public slots:

    void setClipboardData(QString data);

    void newProject();
    bool OpenProject();
    bool OpenExistingProject(QString file_path);

    void importProjects();
   
    void autoSaveProject();
    void saveProject();
    void saveAsProject();
    void closeProject();
    void closeMEDEA();


    void importIdlFiles();

    void generateWorkspace();
    void executeModelLocal();

    void centerSelection();
    void alignSelectionVertical();
    void alignSelectionHorizontal();
    void selectAndCenterConnectedEntities();
    void centerOnID(int ID);

    void showWiki();
    void reportBug();
    void showWikiForSelectedItem();

    void centerImpl();
    void centerDefinition();

    void popupDefinition();
    void popupImpl();
    void popupSelection();
    void popupItem(int ID);

    void aboutQt();
    void aboutMEDEA();

    void cut();
    void copy();
    void paste();
    void replicate();
    void deleteSelection();
    void expandSelection();
    void contractSelection();

    void editLabel();
    void editReplicationCount();

    void constructDDSQOSProfile();
    void requestSearchSuggestions();

    void setControllerReady(bool ready);
    void openURL(QString url);

private slots:
    void initializeController();
    void table_dataChanged(int ID, QString key, QVariant data);

    

private:
    void StoreViewItem(ViewItem* view_item);
    QList<ViewItem*> getSearchableEntities();

    void setupEntityKindItems();
    void _showGitHubPage(QString relURL="");
    void _showWebpage(QString URL);
    void _showWiki(ViewItem* item=0);
    QString getTempFileForModel();
    void spawnSubView(ViewItem *item);
    bool destructViewItem(ViewItem* item);
    QList<ViewItem*> getViewItems(QList<int> IDs);
    ViewItem* getActiveSelectedItem() const;

    QList<NodeView*> getNodeViewsContainingID(int ID);

    NodeViewItem* getNodeViewItem(int ID);

    NodeViewItem* getNodesImpl(int ID);
    NodeViewItem* getNodesDefinition(int ID);

    NodeViewItem* getSharedParent(NodeViewItem* node1, NodeViewItem* node2);

    NodeView* getActiveNodeView();
    void TeardownController();


    bool newProjectUsed;


    bool _newProject(QString file_path="");
    bool _saveProject();
    bool _saveAsProject(QString file_path = "");
    bool _closeProject(bool show_welcome=false);
    void _importProjects();
    void _importProjectFiles(QStringList fileName);
    bool _openProject(QString filePath = "");

    QList<ViewItem*> getItemsOfKind(NODE_KIND kind);
    QList<ViewItem*> getItemsOfKind(EDGE_KIND kind);


    QHash<NODE_KIND, NodeViewItem*> nodeKindItems;
    QHash<EDGE_KIND, EdgeViewItem*> edgeKindItems;

    bool _controllerReady = false;


    bool clearVisualItems();


    ViewItem* getViewItem(int ID);

    QHash<QString, int> treeLookup;
    QMultiMap<NODE_KIND, int> nodeKindLookups;
    QMultiMap<EDGE_KIND, int> edgeKindLookups;

    QHash<int, ViewItem*> viewItems;
    QList<int> topLevelItems;
    ViewItem* rootItem;

    BaseDockWidget* codeViewer = 0;
    CodeBrowser* codeBrowser = 0;
    BaseDockWidget* execution_monitor = 0;
    JobMonitor* job_monitor = 0;

    

    SelectionController* selectionController;
    ActionController* actionController;
    ExecutionManager* execution_manager;
    JenkinsManager* jenkins_manager;

    ContextMenu* menu = 0;
    ModelController* controller = 0;
    QMutex mutex;
    QTimer* autosave_timer_ = 0;
    int autosave_id_ = 0;
    bool showSearchSuggestions;
    
    bool showingWelcomeScreen = true;
};

#endif // VIEWCONTROLLER_H

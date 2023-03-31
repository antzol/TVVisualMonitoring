#ifndef TREEITEM_H
#define TREEITEM_H

#include <vector>

#include <QMenu>
#include <QString>
#include <QVariant>
#include <QVector>

#include "loggable.h"

class TreeItem : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Default = -1,
        RootItem = 0,
        GlobalSettings,
        UiSettings,
        ServiceWidgetSettings,
        ConfigsFolder,
        SourcesFolder,
        ViewerWindowsFolder,
        Config,
        Source,
        Service,
        ViewerWindow
    };

    enum Column {
        Name = 0
    };

    explicit TreeItem(const QString &name, TreeItem *parent = nullptr);
    virtual ~TreeItem();

    void setParent(TreeItem *item);
    TreeItem *parent();
    TreeItem *child(int number);
    const TreeItem *child(int number) const;
    int childCount() const;
    int columnCount() const;
    void appendChild(TreeItem *item);
    bool removeChild(int position);
    bool removeChildren(int position, int count);

    int childNumber() const;

    virtual QVariant data(int column) const;
    bool setData(int column, const QVariant &value);

    virtual std::shared_ptr<QMenu> getContextMenu();
    int getType() const;

signals:
    void treeDataUpdated();

protected:
    QList<QVariant> itemData;

    int type{Type::Default};

    Loggable loggable;

private:
    QList<TreeItem*> childItems;

    TreeItem *parentItem;

};

#endif // TREEITEM_H

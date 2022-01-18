#include "CompleterProxy.h"

CompleterProxy::CompleterProxy(QObject *parent)
    : QObject(parent)
{
    filter.setSourceModel(&model);
    //CaseInsensitive不区分大小写
    filter.setFilterCaseSensitivity(Qt::CaseInsensitive);
    filter.sort(0);
}

QSortFilterProxyModel *CompleterProxy::getFilterModel()
{
    return &filter;
}

QStringList CompleterProxy::getKeywords() const
{
    return model.stringList();
}

void CompleterProxy::setKeywords(const QStringList &keys)
{
    model.setStringList(keys);
    emit keywordsChanged();
}

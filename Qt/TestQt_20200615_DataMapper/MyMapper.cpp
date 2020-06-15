#include "MyMapper.h"

MyMapper::MyMapper(QObject *parent) : QObject(parent)
{

}

void MyMapper::setModel(QStandardItemModel *model)
{
    this->model=model;
}

void MyMapper::addMapping(QWidget *w, int column, const QByteArray &property)
{
    mapperList.push_back(MapperItem{w,property,column});
}

void MyMapper::setRow(int row)
{
    if(row>=0&&model&&model->rowCount()>row){
        for(MapperItem & item:mapperList){
            //将model的值设置给widget属性
            item.widget->setProperty(item.property,model->data(model->index(row,item.column),Qt::DisplayRole));
        }
    }
}

void MyMapper::toFirst()
{
    currentRow=0;
    setRow(currentRow);
}

void MyMapper::toNext()
{
    if(model&&model->rowCount()>currentRow+1){
        ++currentRow;
        setRow(currentRow);
    }
}

void MyMapper::toPrev()
{
    if(currentRow>0){
        --currentRow;
        setRow(currentRow);
    }
}

void MyMapper::submit()
{
    if(currentRow>=0&&model&&model->rowCount()>currentRow){
        for(MapperItem & item:mapperList){
            //将widget的属性值设置到model
            model->setData(model->index(currentRow,item.column),item.widget->property(item.property),Qt::EditRole);
        }
    }
}

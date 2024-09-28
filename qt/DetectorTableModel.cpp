/*---------------------------------------------------------*\
| DetectorTableModel.cpp                                    |
|                                                           |
|   Table model for detector list                           |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "DetectorTableModel.h"
#include "SettingsManager.h"
#include <QBrush>

DetectorTableModel::DetectorTableModel(QObject* parent) : QAbstractTableModel(parent)
{

    reload();
}

int DetectorTableModel::columnCount(const QModelIndex&) const
{
    /*-----------------------------------------------------*\
    | The table has two columns - detector name and enable  |
    \*-----------------------------------------------------*/
    return(2);
}

int DetectorTableModel::rowCount(const QModelIndex&) const
{
    /*-----------------------------------------------------*\
    | The number of rows is equal to the number of detectors|
    \*-----------------------------------------------------*/
    return((int)detectors.size());
}

QVariant DetectorTableModel::data(const QModelIndex& index, int role) const
{
    switch(role)
    {
        /*-----------------------------------------------------*\
        | Column 0 is the detector name, 1 is the enable flag   |
        \*-----------------------------------------------------*/
        case Qt::DisplayRole:
            switch(index.column())
            {
                case 0:
                    return(detectors[index.row()].name.c_str());
                case 1:
                    return(detectors[index.row()].enabled);
            }
            return(QVariant());

        case Qt::CheckStateRole:
            switch(index.column())
            {
                case 1:
                    return(2 * detectors[index.row()].enabled);
            }
            return(QVariant());
        case Qt::BackgroundRole:
            if(detectors[index.row()].discarded)
            {
                return QBrush(QColor(170, 64, 16));
            }
            return QVariant();
    }
    return(QVariant());
}

bool DetectorTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    /*-----------------------------------------------------*\
    | Update detector value for column 1                    |
    \*-----------------------------------------------------*/
    if(index.column() == 1 && role == Qt::CheckStateRole)
    {
        detectors[index.row()].enabled = value.toBool();
        emit dataChanged(index, index);
    }
    return(false);
}

QVariant DetectorTableModel::headerData(int index, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        switch(orientation)
        {
            case Qt::Vertical:
                return(index + 1);

            case Qt::Horizontal:
                switch(index)
                {
                    case 0:
                        return(tr("Name"));
                    case 1:
                        return(tr("Enabled"));
                }
        }
    }
    return(QVariant());
}

Qt::ItemFlags DetectorTableModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags fl = Qt::ItemIsEnabled;

    if(index.column() == 1)
    {
        fl |= Qt::ItemIsUserCheckable;
    }

    return(fl);
}

void DetectorTableModel::applySettings()
{
    /*-----------------------------------------------------*\
    | Read the detector list from the settings manager      |
    \*-----------------------------------------------------*/
    json settings = ResourceManager::get()->GetSettingsManager()->GetSettings("Detectors");

    /*-----------------------------------------------------*\
    | Loop through all detectors in the list and update the |
    | value in the settings                                 |
    \*-----------------------------------------------------*/
    if(settings.contains("detectors"))
    {
        for(unsigned int detector_idx = 0; detector_idx < detectors.size(); detector_idx++)
        {
            settings["detectors"][detectors[detector_idx].name] = detectors[detector_idx].enabled;
        }
    }

    /*-----------------------------------------------------*\
    | Set and save the settings                             |
    \*-----------------------------------------------------*/
    ResourceManager::get()->GetSettingsManager()->SetSettings("Detectors", settings);
    ResourceManager::get()->GetSettingsManager()->SaveSettings();
}

void DetectorTableModel::toggleAll(const bool state, QSortFilterProxyModel* detectorSortModel)
{
    for(unsigned int detector_idx = 0; detector_idx < detectors.size(); detector_idx++)
    {
        if(detectorSortModel->mapFromSource(index(detector_idx, 0)).isValid())
        {
            setData(index(detector_idx,1), state, Qt::CheckStateRole);
        }
    }
}
void DetectorTableModel::reload()
{
    beginResetModel();

    detectors.clear();

    /*-----------------------------------------------------*\
    | Read the detector list from the settings manager      |
    \*-----------------------------------------------------*/
    json settings = ResourceManager::get()->GetSettingsManager()->GetSettings("Detectors");
    std::vector<std::string> discardedList = ResourceManager::get()->GetDiscarded();

    if(settings.contains("detectors"))
    {
        for(json::const_iterator it = settings["detectors"].begin(); it != settings["detectors"].end(); it++)
        {
            DetectorTableValue new_entry;

            new_entry.name      = it.key();
            new_entry.enabled   = it.value();
            new_entry.discarded = (std::find(discardedList.begin(), discardedList.end(), it.key()) != discardedList.end());

            detectors.push_back(new_entry);
        }
    }

    /*-----------------------------------------------------*\
    | If settings contains the detectors list, fill in rows |
    \*-----------------------------------------------------*/
    endResetModel();
}

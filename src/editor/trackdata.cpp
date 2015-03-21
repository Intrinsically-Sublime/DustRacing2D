// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "trackdata.hpp"
#include "tracktile.hpp"
#include "undostackitembase.hpp"

TrackData::TrackData(QString name, bool isUserTrack, unsigned int cols, unsigned int rows)
: TrackDataBase(name, isUserTrack)
, m_map(*this, cols, rows)
, m_route()
, m_undoStack()
, m_undoStackPosition()
{}

QString TrackData::fileName() const
{
    return m_fileName;
}

void TrackData::setFileName(QString newFileName)
{
    m_fileName = newFileName;
}

Route & TrackData::route()
{
    return m_route;
}

const Route & TrackData::route() const
{
    return m_route;
}

MapBase & TrackData::map()
{
    return m_map;
}

const MapBase & TrackData::map() const
{
    return m_map;
}

Objects & TrackData::objects()
{
    return m_objects;
}

const Objects & TrackData::objects() const
{
    return m_objects;
}

void TrackData::enlargeHorSize()
{
    m_map.resize(m_map.cols() + 1, m_map.rows());
}

void TrackData::enlargeVerSize()
{
    m_map.resize(m_map.cols(), m_map.rows() + 1);
}

void TrackData::insertColumn(unsigned int at)
{
    m_map.insertColumn(at);

    moveObjectsAfterColumnInsert(at);

    moveTargetNodesAfterColumnInsert(at);
}

void TrackData::insertRow(unsigned int at)
{
    m_map.insertRow(at);

    moveObjectsAfterRowInsert(at);

    moveTargetNodesAfterRowInsert(at);
}

void TrackData::moveObjectsAfterColumnInsert(unsigned int at)
{
    for (unsigned int i = 0; i < m_objects.count(); i++)
    {
        ObjectPtr object = m_objects.object(i);
        if (object->location().x() > TrackTile::TILE_W * at)
        {
            object->setLocation(QPointF(object->location().x() + TrackTile::TILE_W, object->location().y()));
        }
    }
}

void TrackData::moveObjectsAfterRowInsert(unsigned int at)
{
    for (unsigned int i = 0; i < m_objects.count(); i++)
    {
        ObjectPtr object = m_objects.object(i);
        if (object->location().y() > TrackTile::TILE_H * at)
        {
            object->setLocation(QPointF(object->location().x(), object->location().y() + TrackTile::TILE_H));
        }
    }
}

void TrackData::moveTargetNodesAfterColumnInsert(unsigned int at)
{
    for (unsigned int i = 0; i < m_route.numNodes(); i++)
    {
        TargetNodePtr tnode = m_route.get(i);
        if (tnode->location().x() > TrackTile::TILE_W * at)
        {
            tnode->setLocation(QPointF(tnode->location().x() + TrackTile::TILE_W, tnode->location().y()));
        }
    }
}

void TrackData::moveTargetNodesAfterRowInsert(unsigned int at)
{
    for (unsigned int i = 0; i < m_route.numNodes(); i++)
    {
        TargetNodePtr tnode = m_route.get(i);
        if (tnode->location().y() > TrackTile::TILE_H * at)
        {
            tnode->setLocation(QPointF(tnode->location().x(), tnode->location().y() + TrackTile::TILE_H));
        }
    }
}

void TrackData::addItemToUndoStack(UndoStackItemBase * item)
{
    // "Redo" actions are lost when new items are added to undo stack.
    m_undoStack.erase(m_undoStackPosition, m_undoStack.end());

    m_undoStack.push_back(std::shared_ptr< UndoStackItemBase >(item));

    m_undoStackPosition = m_undoStack.end();
}

bool TrackData::undo(const ObjectModelLoader & loader)
{
    UndoStack::const_iterator begin = m_undoStack.begin();

    if (m_undoStackPosition != begin)
    {
        --m_undoStackPosition;
        m_undoStackPosition->get()->executeUndo(this, loader);
    }

    return m_undoStackPosition != begin;
}

bool TrackData::redo(const ObjectModelLoader & loader)
{
    UndoStack::const_iterator end = m_undoStack.end();

    if (m_undoStackPosition != end)
    {
        m_undoStackPosition->get()->executeRedo(this, loader);
        ++m_undoStackPosition;
    }

    return m_undoStackPosition != end;
}

TrackData::~TrackData()
{
}

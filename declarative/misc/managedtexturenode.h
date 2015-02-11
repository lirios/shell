/*
 *   Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MANAGEDTEXTURENODE_H
#define MANAGEDTEXTURENODE_H

#include <qglobal.h>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QSharedPointer>

/**
 * @short Node that contains a reference counted texture
 *
 * Usually when assigning textures within a node, we'll want to delete the
 * texture with the node. This class will take a shared texture and display it
 * within the node.
 *
 * It's especially interesting to use this class together with the ImageTexturesCache
 * that will offer us shareable textures and cache them transparently, when asking
 * it to create the texture.
 *
 * @see ImageTexturesCache
 */

class ManagedTextureNode : public QSGSimpleTextureNode
{
Q_DISABLE_COPY(ManagedTextureNode)
public:
    ManagedTextureNode();

    void setTexture(QSharedPointer<QSGTexture> texture);

private:
    QSharedPointer<QSGTexture> m_texture;
};

#endif

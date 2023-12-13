// SPDX-FileCopyrightText: 2021-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QQmlParserStatus>
#include <QWindow>

namespace Aurora {

namespace Client {

class WlrLayerSurfaceV1Private;

class WlrLayerSurfaceV1 : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WlrLayerSurfaceV1)
    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(Aurora::Client::WlrLayerSurfaceV1::Layer layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY roleChanged)
    Q_PROPERTY(Aurora::Client::WlrLayerSurfaceV1::Anchors anchors READ anchors WRITE setAnchors NOTIFY anchorsChanged)
    Q_PROPERTY(qint32 exclusiveZone READ exclusiveZone WRITE setExclusiveZone NOTIFY exclusiveZoneChanged)
    Q_PROPERTY(QMargins margins READ margins NOTIFY marginsChanged)
    Q_PROPERTY(qint32 leftMargin READ leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qint32 rightMargin READ rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(qint32 topMargin READ topMargin WRITE setTopMargin NOTIFY topMarginChanged)
    Q_PROPERTY(qint32 bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)
    Q_PROPERTY(Aurora::Client::WlrLayerSurfaceV1::KeyboardInteractivity keyboardInteractivity READ keyboardInteractivity WRITE setKeyboardInteractivity NOTIFY keyboardInteractivityChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    enum Layer {
        BackgroundLayer = 0,
        BottomLayer = 1,
        TopLayer = 2,
        OverlayLayer = 3
    };
    Q_ENUM(Layer)

    enum Anchor {
        TopAnchor = 1,
        BottomAnchor = 2,
        LeftAnchor = 4,
        RightAnchor = 8
    };
    Q_ENUM(Anchor)
    Q_DECLARE_FLAGS(Anchors, Anchor)
    Q_FLAG(Anchors)

    enum KeyboardInteractivity {
        NoKeyboardInteractivity = 0,
        ExclusiveKeyboardInteractivity = 1,
        OnDemandKeyboardInteractivity
    };
    Q_ENUM(KeyboardInteractivity)

    WlrLayerSurfaceV1(QObject *parent = nullptr);
    ~WlrLayerSurfaceV1();

    bool isInitialized() const;

    QWindow *window() const;
    void setWindow(QWindow *window);

    WlrLayerSurfaceV1::Layer layer() const;
    void setLayer(WlrLayerSurfaceV1::Layer layer);

    QString role() const;
    void setRole(const QString &role);

    WlrLayerSurfaceV1::Anchors anchors() const;
    void setAnchors(WlrLayerSurfaceV1::Anchors anchors);

    qint32 exclusiveZone() const;
    void setExclusiveZone(qint32 zone);

    QMargins margins() const;

    qint32 leftMargin() const;
    void setLeftMargin(qint32 margin);

    qint32 rightMargin() const;
    void setRightMargin(qint32 margin);

    qint32 topMargin() const;
    void setTopMargin(qint32 margin);

    qint32 bottomMargin() const;
    void setBottomMargin(qint32 margin);

    WlrLayerSurfaceV1::KeyboardInteractivity keyboardInteractivity() const;
    void setKeyboardInteractivity(WlrLayerSurfaceV1::KeyboardInteractivity keyboardInteractivity);

    void initialize();

    void setLayerEnabled(bool enabled);

    static WlrLayerSurfaceV1 *get(QWindow *window);

Q_SIGNALS:
    void windowChanged(QWindow *window);
    void layerChanged(WlrLayerSurfaceV1::Layer layer);
    void roleChanged(const QString &role);
    void anchorsChanged(WlrLayerSurfaceV1::Anchors anchors);
    void exclusiveZoneChanged(qint32 exclusiveZone);
    void marginsChanged(const QMargins &margins);
    void leftMarginChanged(qint32 margin);
    void rightMarginChanged(qint32 margin);
    void topMarginChanged(qint32 margin);
    void bottomMarginChanged(qint32 margin);
    void keyboardInteractivityChanged(Aurora::Client::WlrLayerSurfaceV1::KeyboardInteractivity keyboardInteractivity);

protected:
    void classBegin() override {}
    void componentComplete() override;

private:
    QScopedPointer<WlrLayerSurfaceV1Private> const d_ptr;
};

} // Client

} // Aurora

Q_DECLARE_OPERATORS_FOR_FLAGS(Aurora::Client::WlrLayerSurfaceV1::Anchors)

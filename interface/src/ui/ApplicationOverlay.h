//
//  ApplicationOverlay.h
//  interface/src/ui/overlays
//
//  Created by Benjamin Arnold on 5/27/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ApplicationOverlay_h
#define hifi_ApplicationOverlay_h

#include <gpu/Texture.h>
class QOpenGLFramebufferObject;

// Handles the drawing of the overlays to the screen
// TODO, move divide up the rendering, displaying and input handling
// facilities of this class
class ApplicationOverlay : public QObject {
    Q_OBJECT
public:
    ApplicationOverlay();
    ~ApplicationOverlay();

    void renderOverlay(RenderArgs* renderArgs);
    GLuint getOverlayTexture();

private:
    void renderStatsAndLogs(RenderArgs* renderArgs);
    void renderDomainConnectionStatusBorder(RenderArgs* renderArgs);
    void renderRearViewToFbo(RenderArgs* renderArgs);
    void renderRearView(RenderArgs* renderArgs);
    void renderQmlUi(RenderArgs* renderArgs);
    void renderOverlays(RenderArgs* renderArgs);
    void buildFramebufferObject();

    float _alpha{ 1.0f };
    float _trailingAudioLoudness{ 0.0f };
    GLuint _uiTexture{ 0 };

    int _domainStatusBorder;
    int _magnifierBorder;

    ivec2 _previousBorderSize{ -1 };
    QOpenGLFramebufferObject* _overlayFramebuffer{ nullptr };
};

#endif // hifi_ApplicationOverlay_h

#pragma once
#include "stdafx.h"

class CanvasScene : public wxControl, public sf::RenderWindow
{
public:

    CanvasScene(wxWindow* parent);
    ~CanvasScene();
private:
    void OnIdle(wxIdleEvent&);
    void OnPaint(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
    void OnSize(wxSizeEvent&);

    void Grid();

    sf::Vector2f position;
    sf::Clock deltaClock;
    sf::Font font;
    float deltatime;

    std::vector<sf::RectangleShape> grid = {};
    void CreateGrid();
    void DrawSceneHUD();

    DECLARE_EVENT_TABLE()
};


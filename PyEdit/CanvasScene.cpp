#include "CanvasScene.h"

CanvasScene::CanvasScene(wxWindow* parent) : wxControl()
{
    this->Create(parent, wxID_ANY, wxDefaultPosition, wxSize(500, 0));
    sf::RenderWindow::create(this->GetHandle());


    std::string agencyFont(wxGetOSDirectory().append("\\Fonts\\AGENCYR.TTF"));
    this->font.loadFromFile(agencyFont);
}

CanvasScene::~CanvasScene()
{
}

void CanvasScene::OnIdle(wxIdleEvent&)
{
    this->Refresh();
}

void CanvasScene::OnPaint(wxPaintEvent&)
{
    this->deltatime = deltaClock.restart().asSeconds();
    this->clear(sf::Color::Black);

    if (this->HasFocus())
    {
        float move = 200.f * deltatime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        {
            this->position.y -= move;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            this->position.y += move;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            this->position.x -= move;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            this->position.x += move;
        }
    }
    

    sf::CircleShape player;
    player.setRadius(10);
    player.setFillColor(sf::Color::Green);
    player.setPosition(this->position + sf::Vector2f(0,0));

    this->draw(player);


    this->DrawSceneHUD();
    this->display();
}
void CanvasScene::CreateGrid()
{
    this->grid.clear();

    int row = 50;
    int col = 50;

    for (int i = -row / 2; i < this->getView().getSize().x; i += row)
    {
        for (int k = -col / 2; k < this->getView().getSize().y; k += col)
        {
            sf::RectangleShape rect;
            rect.setFillColor(sf::Color(0, 0, 0, 0));
            rect.setOutlineColor(sf::Color(255, 255, 255, 50));
            rect.setOutlineThickness(1);

            rect.setSize(sf::Vector2f(row, col));
            rect.setPosition(sf::Vector2f(i, k));

            this->grid.push_back(rect);
        }
    }
}
void CanvasScene::DrawSceneHUD()
{
    for (const sf::RectangleShape& rect : grid)
    {
        this->draw(rect);
    }

    sf::Text text;
    text.setFont(font);
    text.setString(std::to_string((int)(1 / deltatime)) + " FPS");
    text.setCharacterSize(20);
    text.setPosition(sf::Vector2f(0, 0));
    this->draw(text);
}


void CanvasScene::OnEraseBackground(wxEraseEvent&)
{
    this->clear(sf::Color::Black);
}

void CanvasScene::OnSize(wxSizeEvent& args)
{
    setSize(sf::Vector2u(args.GetSize().x, args.GetSize().y));
    setView(sf::View(sf::FloatRect(0, 0, args.GetSize().x, args.GetSize().y)));
    this->CreateGrid();
}

void CanvasScene::Grid()
{
    
}

BEGIN_EVENT_TABLE(CanvasScene, wxControl)

EVT_PAINT(CanvasScene::OnPaint)
EVT_IDLE(CanvasScene::OnIdle)
EVT_ERASE_BACKGROUND(CanvasScene::OnEraseBackground)
EVT_SIZE(CanvasScene::OnSize)

END_EVENT_TABLE()
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "SMS_File.h"
#include "SMS_Map.h"

using namespace std;

int main(int argc, char** argv) {
    sf::RenderWindow App(sf::VideoMode(800, 600), "Hello World", sf::Style::Close); 
    //App.PreserveOpenGLStates(true);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, App.GetWidth(), App.GetHeight(), 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sf::Image TileSet;
    if (!TileSet.LoadFromFile("data/tileset.png")) {
        return EXIT_FAILURE;
    }
    TileSet.SetSmooth(true);
    sf::Sprite TileSetSpr(TileSet);

    int i  = 0;
    App.SetFramerateLimit(60);
    SMS_TileSet ts;
    ts.SetImage("data/tileset.png");
    ts.SetAlphaImage("data/tileset_alpha.png");
    ts.SetTileSize(32, 32);
    ts.ReloadImages();
    if (ts.HasError()) {
        cout << "Error: " << ts.GetErrorString() << endl;
        return 1;
    }
    while (App.IsOpened()) {
        sf::Event Event;
        while (App.GetEvent(Event)) {
            if (Event.Type == sf::Event::Closed)
                App.Close();
            else if ((Event.Type == sf::Event::KeyPressed) && 
                     (Event.Key.Code == sf::Key::Escape))
                App.Close();
        }
        App.Clear(sf::Color(255, 0, 255));
        TileSetSpr.SetX(0);
        TileSetSpr.SetY(0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
		//glTranslatef(0.375f, 0.375f, 0.0f);
        glColor4f(1.f, 1.f, 1.f, 1.f);
        TileSet.Bind();
        ts.DrawTile(0, 50, 50);

        ++i;
        App.Display();
    }
    return EXIT_SUCCESS;
}

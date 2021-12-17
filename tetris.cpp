#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
using namespace sf;

const int LENGTH = 20;
const int WIDTH = 10;

int field[LENGTH][WIDTH] = { 0 };

struct Point
{
    int x, y;
} a[4], b[4], c[4];

int figures[7][4] =
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};


////Prevent object from moving out of the screen
bool check()
{
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= WIDTH || a[i].y >= LENGTH)
            return 0;
        else if (field[a[i].y][a[i].x])
            return 0;

    return 1;
};


int main()
{
    //Set up score counter
    Text score;
    Font font;
    font.loadFromFile("Forwa_font.TTF");
    score.setFont(font);
    score.setCharacterSize(15);
    score.setFillColor(Color::White);
    score.setPosition(300, 360);
    Vector2<float> score_scale(1.5f, 1.5f);
    score.setScale(score_scale);
    score.setString("Lines: 0");

    //Set up preview window
    RectangleShape preview_border(Vector2f(90, 90));
    preview_border.setFillColor(Color::White);
    preview_border.setOutlineThickness(-1);
    preview_border.setPosition(300, 90);
    Text next;
    next.setFont(font);
    next.setCharacterSize(15);
    next.setFillColor(Color::Red);
    next.setPosition(290, 200);
    next.setString("Next Figure");

    //Set up music
    Music track;
    track.openFromFile("tetris_theme.wav");
    track.play();
    track.setLoop(true);

    //Set up sound effect
    SoundBuffer buffer;
    buffer.loadFromFile("line.wav");
    Sound line;
    line.setBuffer(buffer);

    SoundBuffer buffer2;
    buffer2.loadFromFile("fall.wav");
    Sound fall;
    fall.setBuffer(buffer2);

    //Set up and open window
    RenderWindow window(VideoMode(480, 420), "TETRIS");

    Texture t1, t2, t3;
    t1.loadFromFile("tiles.png");
    t2.loadFromFile("background.png");
    t3.loadFromFile("frame.png");

    Sprite s(t1), background(t2), frame(t3);

    int dx = 0; bool rotate = 0; int colorNum = 1, next_colorNum = 1;
    float timer = 0, delay = 0.3;
    int scoreCounter = 0;

    //Set up clock
    srand(time(0));
    Clock clock;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed) {
                window.close();
                track.stop();
            }

            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::Up) rotate = true;
                else if (e.key.code == Keyboard::Left) dx = -1;
                else if (e.key.code == Keyboard::Right) dx = 1;
        }

        if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

        //// <- Move -> ///
        for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }
        if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];

        //////Rotate//////
        if (rotate)
        {
            Point p = a[1]; //center of rotation
            for (int i = 0; i < 4; i++)
            {
                int x = a[i].y - p.y;
                int y = a[i].x - p.x;
                a[i].x = p.x - x;
                a[i].y = p.y + y;
            }
            if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];
        }

        ///////Tick//////

        if (timer > delay)
        {
            for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }

            if (!check())
            {
                fall.play();
                for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;

                colorNum = next_colorNum;
                int n = rand() % 7;
                for (int i = 0; i < 4; i++)
                {
                    a[i].x = c[i].x;
                    a[i].y = c[i].y;
                    c[i].x = figures[n][i] % 2;
                    c[i].y = figures[n][i] / 2;
                }
                next_colorNum = 1 + rand() % 7;
            }

            timer = 0;
        }

        ///////check lines//////////
        int k = LENGTH - 1;
        for (int i = LENGTH - 1; i > 0; i--)
        {
            int count = 0;
            for (int j = 0; j < WIDTH; j++)
            {
                if (field[i][j]) count++;
                field[k][j] = field[i][j];
            }
            if (count < WIDTH) {
                k--;
            }
        }
        scoreCounter += k;
        for (int i = 0; i < k; ++i) {
            line.play();
        }
        char temp[256];
        sprintf_s(temp, "Lines: %i", scoreCounter);
        score.setString(temp);

        //If game over, then open lose window
        for (int i = 0; i < WIDTH; ++i) {
            if (field[0][i]) {
                window.close();
                track.stop();
                RenderWindow Lose_window(VideoMode(700, 525), "YOU LOSE");
                while (Lose_window.isOpen())
                {
                    Event event;
                    while (Lose_window.pollEvent(event))
                    {
                        if (event.type == Event::Closed) {
                            Lose_window.close();
                        }
                    }
                }
            }
        }

        //Reset the move, rotate and delay counter
        dx = 0; rotate = 0; delay = 0.3;

        /////////draw//////////
        window.clear(Color::White);
        window.draw(background);

        //Draw preview window for next figure 
        window.draw(preview_border);
        window.draw(next);
        for (int i = 0; i < 4; ++i) {
            s.setTextureRect(IntRect(next_colorNum * 18, 0, 18, 18));
            s.setPosition(c[i].x * 18 + 300, c[i].y * 18 + 65);
            s.move(28, 31); //offset
            window.draw(s);
        }

        for (int i = 0; i < LENGTH; i++) {
            for (int j = 0; j < WIDTH; j++)
            {
                if (field[i][j] == 0) continue;
                s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
                s.setPosition(j * 18, i * 18);
                s.move(28, 31); //offset
                window.draw(s);
            }
        }

        for (int i = 0; i < 4; i++)
        {
            s.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
            s.setPosition(a[i].x * 18, a[i].y * 18);
            s.move(28, 31); //offset
            window.draw(s);
        }

        window.draw(frame);
        window.draw(score);
        window.display();
    }

    return 0;
}
#include "game.h"
#include <iostream>
#include <time.h>
#include <cmath>

int tile;
float mouseX, mouseY;
float mouseRX, mouseRY;
float mouseRXend, mouseRYend;
float mouseRXstart, mouseRYstart;
bool isRightPressed;

int gridX;
int gridY;
int gridSpacing;

int radiusOfCircle = 10;

int lineThickness = 5;

bool p = false;

struct Circle
{
    float x, y;
    float prevX, prevY;
    int radius;
    bool pinned;
};
void renderCircle(SDL_Renderer *rendere, Circle *c);
bool isInCircle(float x, float y, Circle *c);
void drawThickLine(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, int thickness);
void initGrid(int rows, int cols, float spacing, int radius);
bool lineIntersectsLine(float x1, float y1, float x2, float y2,
                        float x3, float y3, float x4, float y4);
void showLineThicknessSlider();
void     showGridControls();
void startSimulation();


std::vector<Circle *> circles;
struct Connection
{
    Circle *a;
    Circle *b;
    float length;
};

std::vector<Connection> pinnedConnections;

Game::Game(int width, int height)
    : WINDOW_W(width), WINDOW_H(height), isRunning(false) {}

Game::~Game()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::init(const char *title)
{
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        SDL_Log("SDL Init failed: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    isRunning = true;
    return true;
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        
        if (event.type == SDL_EVENT_QUIT)
        {
            isRunning = false;
        }
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            continue; 
        
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            SDL_GetMouseState(&mouseX, &mouseY);

            Circle *c = new Circle();
            c->x = mouseX;
            c->y = mouseY;
            c->radius = radiusOfCircle;
            c->prevX = c->x;
            c->prevY = c->y;
            circles.push_back(c);
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_MIDDLE)
        {

            SDL_GetMouseState(&mouseRX, &mouseRY);
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT)
        {
            SDL_GetMouseState(&mouseRXstart, &mouseRYstart);
            isRightPressed = true;
        }
        else if (event.type == SDL_EVENT_MOUSE_MOTION && isRightPressed)
        {
            mouseRXend = event.motion.x;
            mouseRYend = event.motion.y;
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_RIGHT)
        {
            bool connectionDeleted = false;

            std::vector<Connection> remainingConnections;

            for (const auto &conn : pinnedConnections)
            {
                if (lineIntersectsLine(mouseRXstart, mouseRYstart, mouseRXend, mouseRYend,
                                       conn.a->x, conn.a->y, conn.b->x, conn.b->y))
                {
                    connectionDeleted = true;
                    continue;
                }
                remainingConnections.push_back(conn);
            }

            if (connectionDeleted)
            {
                pinnedConnections = remainingConnections;
            }
            else
            {
                Circle *startCircle = nullptr;
                Circle *endCircle = nullptr;

                for (auto c : circles)
                {
                    if (isInCircle(mouseRXstart, mouseRYstart, c))
                    {
                        startCircle = c;
                    }
                    if (isInCircle(mouseRXend, mouseRYend, c))
                    {
                        endCircle = c;
                    }
                }

                if (startCircle && endCircle && startCircle != endCircle)
                {
                    float dx = endCircle->x - startCircle->x;
                    float dy = endCircle->y - startCircle->y;
                    float dist = sqrt(dx * dx + dy * dy);

                    pinnedConnections.push_back({startCircle, endCircle, dist});
                }
            }

            isRightPressed = false;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE)
        {
            p = !p;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_S)
        {
            initGrid(gridX,gridY, gridSpacing,radiusOfCircle);
        }
    }
}

void Game::update()
{
    for(auto it: circles){
        it->radius = radiusOfCircle;
    }
    if (p)
    {
        float dt = 1.0f; // delta time
        float gravity = 0.5f;
        
        for (auto &c : circles)
        {
            if (c->pinned)
            continue;
            
            float vx = c->x - c->prevX;
            float vy = c->y - c->prevY;
            
            c->prevX = c->x;
            c->prevY = c->y;
            
            c->x += vx;
            c->y += vy + gravity;
        }
        
        for (int i = 0; i < 5; ++i)
        {
            for (auto &conn : pinnedConnections)
            {
                Circle *a = conn.a;
                Circle *b = conn.b;
                
                float dx = b->x - a->x;
                float dy = b->y - a->y;
                float dist = sqrt(dx * dx + dy * dy);
                float desiredDist = conn.length;
                
                if (dist == 0.0f)
                continue;
                
                float diff = (dist - desiredDist) / dist;
                float offsetX = dx * 0.5f * diff;
                float offsetY = dy * 0.5f * diff;
                
                if (!a->pinned)
                {
                    a->x += offsetX;
                    a->y += offsetY;
                }
                
                if (!b->pinned)
                {
                    b->x -= offsetX;
                    b->y -= offsetY;
                }
            }
        }
    }
    
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    showLineThicknessSlider();
    showGridControls();
    startSimulation();
}

void Game::render()
{
    ImGui::Render();


    // Set background color (dark gray to make walls/tiles stand out)
    SDL_SetRenderDrawColor(renderer, 160, 32, 240, 255);
    SDL_RenderClear(renderer);

    if(showGrid){

        if (isRightPressed)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
            drawThickLine(renderer, mouseRXstart, mouseRYstart, mouseRXend, mouseRYend, 3);
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255); // Light red
            for (const auto &conn : pinnedConnections)
            {
                if (lineIntersectsLine(mouseRXstart, mouseRYstart, mouseRXend, mouseRYend,
                                       conn.a->x, conn.a->y, conn.b->x, conn.b->y))
                {
                    drawThickLine(renderer, conn.a->x, conn.a->y, conn.b->x, conn.b->y, lineThickness);
                }
            }
        }
    }
    for (auto &conn : pinnedConnections)
    {
        drawThickLine(renderer, conn.a->x, conn.a->y, conn.b->x, conn.b->y, lineThickness);
    }

    for (auto it : circles)
    {
        if (isInCircle(mouseRX, mouseRY, it))
        {
            std::cout << "circle on --> " << it->x << " , " << it->y << std::endl;
            if (it->pinned)
            {
                it->pinned = false;
            }
            else
                it->pinned = true;
            mouseRX = -1;
            mouseRY = -1;
        }
        renderCircle(renderer, it);
    }

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Game::clean()
{
    for (auto c : circles)
    {
        delete c;
    }
    circles.clear();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void renderCircle(SDL_Renderer *rendere, Circle *c)
{

    SDL_SetRenderDrawColor(rendere, 255, 255, 255, 255);

    for (int i = -c->radius; i <= c->radius; i++)
    {
        for (int j = -c->radius; j <= c->radius; j++)
        {
            if (i * i + j * j <= c->radius * c->radius)
            {
                if (c->pinned)
                {
                    SDL_SetRenderDrawColor(rendere, 255, 0, 0, 0);
                }
                SDL_RenderPoint(rendere, c->x + i, c->y + j);
            }
        }
    }
}

bool isInCircle(float x, float y, Circle *c)
{
    float dx = x - c->x;
    float dy = y - c->y;
    float distanceSquared = dx * dx + dy * dy;

    return distanceSquared <= c->radius * c->radius;
}

void drawThickLine(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, int thickness)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
    for (int i = -thickness / 2; i <= thickness / 2; ++i)
    {
        SDL_RenderLine(renderer,
                       static_cast<int>(x1 + i), static_cast<int>(y1),
                       static_cast<int>(x2 + i), static_cast<int>(y2));
        SDL_RenderLine(renderer,
                       static_cast<int>(x1), static_cast<int>(y1 + i),
                       static_cast<int>(x2), static_cast<int>(y2 + i));
    }
}

void initGrid(int rows, int cols, float spacing, int radius)
{
    circles.clear();
    pinnedConnections.clear();

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            Circle *c = new Circle();
            c->x = 100 + x * spacing;
            c->y = 100 + y * spacing;
            c->radius = radius;
            c->pinned = (y == 0); // pin top row

            // Set previous position to account for initial gravity
            c->prevX = c->x;
            c->prevY = c->y - 0.5f * 9.8f * (1.0f / 60.0f) * (1.0f / 60.0f); // Small downward offset

            circles.push_back(c);
        }
    }

    // Add connections
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            int index = y * cols + x;
            Circle *current = circles[index];

            // Right neighbor
            if (x < cols - 1)
            {
                Circle *right = circles[index + 1];
                float dx = right->x - current->x;
                float dy = right->y - current->y;
                float dist = sqrt(dx * dx + dy * dy);
                pinnedConnections.push_back({current, right, dist});
            }

            // Bottom neighbor
            if (y < rows - 1)
            {
                Circle *down = circles[index + cols];
                float dx = down->x - current->x;
                float dy = down->y - current->y;
                float dist = sqrt(dx * dx + dy * dy);
                pinnedConnections.push_back({current, down, dist});
            }
        }
    }
}

bool lineIntersectsLine(float x1, float y1, float x2, float y2,
                        float x3, float y3, float x4, float y4)
{

    auto orientation = [](float x1, float y1, float x2, float y2, float x3, float y3)
    {
        float val = (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2);
        if (val == 0)
            return 0;            
        return (val > 0) ? 1 : 2;
    };

    int o1 = orientation(x1, y1, x2, y2, x3, y3);
    int o2 = orientation(x1, y1, x2, y2, x4, y4);
    int o3 = orientation(x3, y3, x4, y4, x1, y1);
    int o4 = orientation(x3, y3, x4, y4, x2, y2);

    if (o1 != o2 && o3 != o4)
        return true;

    return false;
}

void showLineThicknessSlider(){
    ImGui::Begin("CHANGE VARIABLES");
    float value = lineThickness;
    ImGui::SliderFloat("Line Thickness", &value, 1.0f, 10.0f);
    lineThickness = (int)value;
    float dummyRadius = radiusOfCircle;
    ImGui::SliderFloat("Radius", &dummyRadius, 1.0f, 30.0f);
    radiusOfCircle = (int)dummyRadius;
    ImGui::End();
}

void showGridControls(){
    ImGui::Begin("Set your grid");
    ImGui::SliderInt("No of Circle in X", &gridX, 1, 30);
    ImGui::SliderInt("No of Circle in Y", &gridY, 1, 30);
    ImGui::SliderInt("Spacing between circles", &gridSpacing, 10, 200);
    if (ImGui::Button("Start Cloth")) {
        std::cout << "Start Cloth button pressed!\n";
        initGrid(gridX, gridY, gridSpacing, radiusOfCircle);
    }
    
    ImGui::End();
}

void startSimulation(){
    ImGui::Begin("Start Simulation");
    if(ImGui::Button("StartSimulation")){
        p = true;
    }
    if(ImGui::Button("StopSimulation")){
        p = false;
    }
    ImGui::End();
}
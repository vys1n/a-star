#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <random>
#include <raylib.h>
#include <cstddef>

class AStar {
public:
    static constexpr size_t NODES_ROW_AMOUNT { 20 };
    static constexpr size_t NODES_PX_SIZE { 50 };
    static constexpr size_t OBSTACLES_AMT { NODES_ROW_AMOUNT * 5 };

public:
    AStar() {
        for (int y {}; y < NODES_ROW_AMOUNT; y++) {
            for (int x {}; x < NODES_ROW_AMOUNT; x++) {
                Node node { x, y };
                grid[y][x] = node;
            }
        }

        start_node = &grid[0][0];
        end_node = &grid[NODES_ROW_AMOUNT - 1][NODES_ROW_AMOUNT - 1];

        start_node->g_cost = 0;
        start_node->h_cost = heuristic(start_node, end_node);
        start_node->f_cost = start_node->h_cost;

        std::mt19937 generator { std::random_device()() };
        std::uniform_int_distribution<int> range { 0, NODES_ROW_AMOUNT - 1};

        for (size_t i {}; i < OBSTACLES_AMT;) {
            int x { range(generator) };
            int y { range(generator) };

            auto* node { get_node(x, y) };

            if (node == start_node || node == end_node)
                continue;

            if (node->is_obstacle)
                continue;

            node->is_obstacle = true;

            i++;
        }
    }

    void update() {
    }

    void draw() {
        for (int y {}; y < NODES_ROW_AMOUNT; y++) {
            for (int x {}; x < NODES_ROW_AMOUNT; x++) {
                const auto* node { get_node(x, y) };
                
                Color color { GRAY };
                if (node->is_obstacle)
                    color = BLACK;

                DrawRectangle(x * NODES_PX_SIZE, y * NODES_PX_SIZE, NODES_PX_SIZE, NODES_PX_SIZE, color);
            }
        }

        DrawRectangle(start_node->x * NODES_PX_SIZE, start_node->y * NODES_PX_SIZE, NODES_PX_SIZE, NODES_PX_SIZE, GREEN);
        DrawRectangle(end_node->x * NODES_PX_SIZE, end_node->y * NODES_PX_SIZE, NODES_PX_SIZE, NODES_PX_SIZE, RED);
    }

private:
    struct Node {
        int x {};
        int y {};

        float h_cost {};
        float g_cost { std::numeric_limits<float>::max() };
        float f_cost { std::numeric_limits<float>::max() };

        bool is_obstacle {};
    };

private:
    float heuristic(const Node* a, const Node* b) {
        return std::abs(a->y - b->y) + std::abs(a->x - b->x);
    }

    Node* get_node(int x, int y) {
        if (x < 0 || y < 0)
            return nullptr;
        
        if (x >= NODES_ROW_AMOUNT || y >= NODES_ROW_AMOUNT )
            return nullptr;

        return &grid[y][x];
    }

private:
    std::array<std::array<Node, NODES_ROW_AMOUNT>, NODES_ROW_AMOUNT> grid {};
    Node* start_node {};
    Node* end_node {};
};

int main() {
    InitWindow(1000, 1000, "a-star-algorithm");
    SetTargetFPS(60);

    AStar a_star {};

    while(!WindowShouldClose()) {
        PollInputEvents();
        a_star.update();

        BeginDrawing();
        ClearBackground(BLACK);
        a_star.draw();
        
        EndDrawing();
    }

    return 0;
}

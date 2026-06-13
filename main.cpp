#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <queue>
#include <random>
#include <raylib.h>
#include <vector>

class AStar {
public:
    static constexpr size_t NODES_ROW_AMOUNT{ 20 * 5 };
    static constexpr size_t NODES_PX_SIZE{ 50 / 5 };
    static constexpr size_t OBSTACLES_AMT{ NODES_ROW_AMOUNT * 20 };

public:
    AStar() {
        for (int y{}; y < NODES_ROW_AMOUNT; y++) {
            for (int x{}; x < NODES_ROW_AMOUNT; x++) {
                Node node{x, y};
                grid[y][x] = node;
            }
        }

        start_node = &grid[0][0];
        end_node = &grid[NODES_ROW_AMOUNT - 1][NODES_ROW_AMOUNT - 1];

        start_node->g_cost = 0;
        start_node->h_cost = heuristic(start_node, end_node);
        start_node->f_cost = start_node->h_cost;

        start_node->is_open = true;

        opened.push({ start_node });

        std::mt19937 generator{std::random_device()()};
        std::uniform_int_distribution<int> range{0, NODES_ROW_AMOUNT - 1};

        for (size_t i{}; i < OBSTACLES_AMT;) {
            int x{range(generator)};
            int y{range(generator)};

            auto *node{get_node(x, y)};

            if (node == start_node || node == end_node)
                continue;

            if (node->is_obstacle)
                continue;

            node->is_obstacle = true;

            i++;
        }
    }

    void update() {
        if (finished) {
            return;
        }
        
        if (opened.empty()) {
            finished = true;
            return;
        }

        auto current { opened.top() };
        opened.pop();

        if (current.node->is_closed)
            return;

        current.node->is_closed = true;

        if (current.node == end_node) {
            reconstruct_path();
            finished = true;
            return;
        }

        expand_node(current.node);
    }

    void draw() {
        for (int y{}; y < NODES_ROW_AMOUNT; y++) {
            for (int x{}; x < NODES_ROW_AMOUNT; x++) {
                const auto *node{get_node(x, y)};
                Color color{GRAY};

                if (node->is_obstacle)
                    color = BLACK;

                if (node->is_open)
                    color = ORANGE;
                
                if (node->is_closed)
                    color = BLUE;

                DrawRectangle(x * NODES_PX_SIZE, y * NODES_PX_SIZE, NODES_PX_SIZE, NODES_PX_SIZE, color);
            }
        }

        for (const auto* node : final_path) {
            DrawRectangle(node->x * NODES_PX_SIZE, node->y * NODES_PX_SIZE, NODES_PX_SIZE, NODES_PX_SIZE, YELLOW);
        }

        DrawRectangle(start_node->x * NODES_PX_SIZE, start_node->y * NODES_PX_SIZE, NODES_PX_SIZE, NODES_PX_SIZE, GREEN);
        DrawRectangle(end_node->x * NODES_PX_SIZE, end_node->y * NODES_PX_SIZE, NODES_PX_SIZE, NODES_PX_SIZE, RED);
    }

private:
    struct Node {
        int x{};
        int y{};

        float h_cost{};
        float g_cost{std::numeric_limits<float>::max()};
        float f_cost{std::numeric_limits<float>::max()};

        bool is_obstacle{};
        bool is_open {};
        bool is_closed {};

        Node* parent {};
    };

    struct OpenNode {
        Node* node {};

        bool operator<(const OpenNode& other) const {
            return node->f_cost > other.node->f_cost;
        }
    };

private:
    float heuristic(const Node *a, const Node *b) {
        return std::abs(a->y - b->y) + std::abs(a->x - b->x);
    }

    Node* get_node(int x, int y) {
        if (x < 0 || y < 0)
            return nullptr;

        if (x >= NODES_ROW_AMOUNT || y >= NODES_ROW_AMOUNT)
            return nullptr;

        return &grid[y][x];
    }

    void expand_node(Node* node) {
        for (auto* neighbour : get_neighbours(node)) {
            if (neighbour->is_obstacle)
                continue;

            const float new_g_cost { node->g_cost + 1 };
        
            if (new_g_cost >= neighbour->g_cost)
                continue;

            neighbour->g_cost = new_g_cost;
            neighbour->h_cost = heuristic(neighbour, end_node);
            neighbour->f_cost = neighbour->h_cost + neighbour->g_cost;

            neighbour->is_open = true;

            neighbour->parent = node;

            opened.push({ neighbour });
        }
    }

    std::vector<Node*> get_neighbours(Node* node) {
        std::vector<Vector2> directions { { 0 , 1}, { 1, 0 }, { 0 , -1}, { -1 , 0 } };
        std::vector<Node*> output {};
        for (const auto& direction : directions) {
            auto* possible_neighbour { get_node(node->x + direction.x , node->y + direction.y) };
            if (!possible_neighbour)
                continue;
            output.push_back(possible_neighbour);
        }

        return output;
    }

    void reconstruct_path() {
        auto* current { end_node };
        while (current->parent) {
            final_path.push_back(current);
            current = current->parent;
        }
        final_path.push_back(current);
    }

private:
    std::array<std::array<Node, NODES_ROW_AMOUNT>, NODES_ROW_AMOUNT> grid{};
    std::priority_queue<OpenNode> opened {};
    std::vector<Node*> final_path {};

    bool finished {};

    Node *start_node{};
    Node *end_node{};
};

int main() {
    InitWindow(1000, 1000, "a-star-algorithm");
    SetTargetFPS(600);

    AStar a_star{};

    while (!WindowShouldClose()) {
        PollInputEvents();
        a_star.update();

        BeginDrawing();
        ClearBackground(BLACK);
        a_star.draw();

        EndDrawing();
    }

    return 0;
}

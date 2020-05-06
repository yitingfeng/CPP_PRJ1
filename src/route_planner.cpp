#include "route_planner.h"
#include <algorithm>
#include <new>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    m_Model = model;

    RouteModel::Node* start = new RouteModel::Node;
    start = &model.FindClosestNode(start_x, start_y);
    start_node = start;
    RouteModel::Node* end = new RouteModel::Node;
    end = &model.FindClosestNode(end_x, end_y);
    end_node = end;
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    float dist = node->distance(*end_node);
    return dist;
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for (auto neighbor:(current_node->neighbors)) {
        // if (neighbor->visited == false) {
        float n_h_value = CalculateHValue(neighbor);
        float n_g_value = current_node->g_value + current_node->distance(*neighbor);

        if (n_h_value + n_g_value < neighbor->h_value + neighbor->g_value) {    
            neighbor->parent = current_node;
            neighbor->h_value = CalculateHValue(neighbor);
            neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
            open_list.push_back(neighbor);
            neighbor->visited = true;
        }
        // } 
    }
    // current_node->visited = true;
}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

RouteModel::Node *RoutePlanner::NextNode() {
    std::sort (open_list.begin(), open_list.end(), [] (const auto &lhs, const auto &rhs) {
        return (lhs->h_value + lhs->g_value > rhs->h_value + rhs->g_value); 
    });
    auto current = open_list.back();
    open_list.pop_back();
    // current->visited = true;
    return current;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
    while(current_node->parent) {
        path_found.push_back(*current_node);
        distance += current_node->distance(*current_node->parent);
        current_node = current_node->parent;
    }
    path_found.push_back(*current_node);
    std::reverse(path_found.begin(), path_found.end());

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    // std::cout << "Distance = " << distance << "\n";
    // std::cout << "Path = " << path_found.size() << "\n";
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = start_node;

    // TODO: Implement your solution here.

    // open_list.push_back(start_node);
    start_node->visited = true;
    while (open_list.size() > 0 || current_node == start_node) {
        AddNeighbors(current_node);
        // std::cout << "After adding size = " << open_list.size() << "\n";
        current_node = NextNode();
        if (current_node->x == end_node->x && current_node->y == end_node->y) {
            // std::cout << "123 Path size = " << m_Model.path.size() << "\n";
            m_Model.path = ConstructFinalPath(current_node);
            return;
        }
    }
    // std::cout << "128 Path size = " << m_Model.path.size() << "\n";
}
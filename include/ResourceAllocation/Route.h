/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Route.h
 * Author: BrunoVinicius
 *
 * Created on November 15, 2018, 10:40 PM
 */

#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <cassert>
#include <iostream>
#include <memory>
#include "../GeneralClasses/Def.h"

class Link;
class ResourceAlloc;
class Topology;
class Node;

/**
 * @brief Class Route that storage the path as all nodes in this path.
 */
class Route {
    
    friend std::ostream& operator<<(std::ostream& ostream, 
    const Route* route);
    
public:
    /**
     * @brief Standard constructor of the class object.
     * @param rsaAlg ResourceAlloc object used in this route.
     * @param path Vector of integers representing the path nodes Ids.
     */
    Route(ResourceAlloc* rsaAlg, const std::vector<int>& path);
    /**
     * @brief Default destructor of a Route object.
     */
    virtual ~Route();
    /**
     * @brief Comparison operator that checks if two routes have the same path.
     * @param right Route in the right side of the relational equation.
     * @return True if the the routes have the same path.
     */
    bool operator==(const Route& right) const;
    
    bool operator<(const Route& right) const;
    
    bool operator>(const Route& right) const;
    
    bool checkShareLink(Route* route) const;


    /**
     * @brief Function that return the source node Id of this route.
     * @return Source node Id.
     */
    int GetOrNodeId() const;
    /**
     * @brief Function that return the source node pointer of this route.
     * @return Source node pointer.
     */
    Node* GetOrNode() const;
    /**
     * @brief Function that return the destination node Id of this route.
     * @return Source node Id.
     */
    int GetDeNodeId() const;
    /**
     * @brief Function that return the destination node pointer of this route.
     * @return Destination node pointer.
     */
    Node* GetDeNode() const;
    /**
     * @brief Function that return a specified node Id of this route.
     * @param index Path index of the route.
     * @return Node Id.
     */
    int GetNodeId(unsigned int index) const;
    /**
     * @brief Function that return a specified node of this route.
     * @param index Path index of the route.
     * @return Node pointer.
     */
    Node* GetNode(unsigned int index) const;
    
    std::vector<Node*> GetNodes() const;
    /**
     * @brief Get the number of hops of this route.
     * @return Number of hops.
     */
    unsigned int GetNumHops() const;
    /**
     * @brief Get the number of node of this route.
     * @return Number of nodes.
     */
    unsigned int GetNumNodes() const;
    /**
     * @brief Get the path, vector of node Id, of this route.
     * @return Container of nodes Id.
     */
    std::vector<int> GetPath() const;
    /**
     * @brief Get the total cost of this route.
     * @return Route cost.
     */
    double GetCost() const;
    /**
     * @brief Set the total cost of this route.
     * @param cost Route cost.
     */
    void SetCost(double cost);
    /**
     * @brief Get the total cost of this route in terms of Hops.
     * @return Route cost.
     */
    double GetCostHop() const;
    /**
     * @brief Set the total cost of this route in terms of Hops.
     * @param cost Route cost.
     */
    void SetCostHop(double costHop);
    /**
     * @brief Get the total cost of this route in terms of Length.
     * @return Route cost.
     */
    double GetCostLength() const;
    /**
     * @brief Set the total cost of this route in terms of Length
     * @param cost Route cost.
     */
    void SetCostLength(double costLength);
    /**
     * @brief Set the total cost of this route. The cost is calculated based on
     * the cost of links that compose the route.
     */
    void SetCost();
    /**
    * @brief Set the total cost of this route in terms of Hops.
    */
    void SetCostHop();
    /**
    * @brief Set the total cost of this route in terms of Lenth.
    */
    void SetCostLength();
    /**
     * @brief Function that return a specified link of this route.
     * @param index Link index.
     * @return Link pointer.
     */
    Link* GetLink(unsigned int index) const;
    /**
     * @brief Function that return all links of this route.
     * @param route Link index.
     * @return all Link pointers vector.
     */
    std::vector<Link*> GetLinks(std::shared_ptr<Route>& route) const;

    /**
     * @brief Set all nodes in this route as working.
     */
    void SetAllNodesWorking();
    /**
     * @brief Function that create a new route from this, based on 
     * the path indexes.
     * @param ind1 First index.
     * @param ind2 Second index.
     * @return Created route.
     */
    std::shared_ptr<Route> CreatePartialRoute(unsigned int ind1, 
                                              unsigned int ind2);
    /**
     * @brief Create a new route, adding a specified route to this route.
     * The last index of the first route needs to be the same as the first index
     * of the second.
     * @param route Route to be added.
     * @return Created route.
     */
    std::shared_ptr<Route> AddRoute(std::shared_ptr<Route>& route);
    /**
     * @brief Check if the node is into route path vector.
     * @param node Node to check.
     * @return True if node is into route path vector or false to otherwise.
     */
    bool IsNode(NodeIndex node);
    /**
     * @brief Add the node in the end of route path vector.
     * @param node Node to add.
     */
    void AddNodeAtEnd(NodeIndex node);


private:
    /**
     * @brief ResourceAlloc used by this route.
     */
    ResourceAlloc* resourceAlloc;
    /**
     * @brief Topology used in this route.
     */
    Topology* topology;
    /**
     * @brief Container of the nodes indexes of this path.
     */
    std::vector<int> path;
    /**
     * @brief Container of the nodes pointers of this path.
     */
    std::vector<Node*> pathNodes;
    
    std::vector<Link*> pathLinks;
    /**
     * @brief Cost of the route.
     */
    double cost;
    /**
     * @brief Cost of the route in terms of Hops.
     */
    double costHop;
    /**
     * @brief Cost of the route in terms of Length.
     */
    double costLength;
};

#endif /* ROUTE_H */


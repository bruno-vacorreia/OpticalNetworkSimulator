/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Node.h
 * Author: bruno
 *
 * Created on August 4, 2018, 1:14 AM
 */

#ifndef NODE_H
#define NODE_H

#include <iostream>

class Topology;

#include "../GeneralClasses/Def.h"

/**
 * @brief Class Node represents a node inside a topology.
 */
class Node {
    
    friend std::ostream& operator<<(std::ostream& ostream,
    Node* node);
public:
    /**
     * @brief Standard constructor for a Node object.
     * @param topPointer pointer to a Topology object that
     * owns this node.
     * @param nodeId node index.
     */
    Node(Topology* topPointer,  NodeIndex nodeId);
    /**
     * @brief Virtual destructor of a Node object.
     */
    virtual ~Node();
    /**
     * @brief Function to compare two nodes.
     * @param right Node in the right side of the operator.
     * @return True if the nodes are the same.
     */
    bool operator==(const Node& right) const;
    /**
     * @brief Initialize the node, setting the start values
     * contained in it.
     */
    virtual void Initialize();
    
    /**
     * @brief Get the node index.
     * @return Node index.
     */
    NodeIndex GetNodeId() const;
    /**
     * @brief Checks if the node is functional.
     * @return True if the node is working.
     */
    bool IsNodeWorking() const;
    /**
     * @brief Set the node state (working or not).
     * @param NodeWorking Node state.
     */
    void SetNodeState(bool NodeWorking);
    /**
     * @brief Function to add a node to the neighbor nodes container.
     * @param node Node to add.
     */
    void AddNeighborNode(Node* node);
    /**
     * @brief Function to get thee node degree. The degree indicates how many
     * nodes this node is connected.
     * @return Node degree.
     */
    unsigned int GetNodeDegree();
    
    /**
     * @brief Function to get the topology in which this node is inserted.
     * @return Physical topology pointer.
     */
    Topology* GetTopology() const;
private:
    /**
     * @brief Pointer to a Topology object that
     * owns this node
     */
    Topology* topology;
    /**
     * @brief Node index.
     */
    const NodeIndex nodeId;
    /**
     * @brief Boolean variable to indicate the node state.
     */
    bool nodeWorking;
    /**
     * @brief Container of nodes this node is connected.
     */
    std::vector<Node*> neighborNodes;
};

#endif /* NODE_H */


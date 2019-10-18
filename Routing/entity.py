# William Trace LaCour (WTL6C)
# Justin Vaughn Javier (JTJ5GS)
#
# Networks - Routing Assignment
# 4/2/19

'''
Code for an entity in the network. This is where you should implement the
distance-vector algorithm.
'''

import packet


class Entity:
    '''
    Entity that represents a node in the network.

    Each function should be implemented so that the Entity can be instantiated
    multiple times and successfully run a distance-vector routing algorithm.
    '''

    def __init__(self, entity_index, number_entities):
        '''
        This initialization function will be called at the beginning of the
        simulation to setup all entities.

        Arguments:
        - `entity_index`:    The id of this entity.
        - `number_entities`: Number of total entities in the network.

        Return Value: None.
        '''

        # Save state
        self.index = entity_index
        self.number_of_entities = number_entities
        self.cost = []
        self.neighbors = []
        self.nextHop = [999] * number_entities

    def initialize_costs(self, neighbor_costs):
        '''
        This function will be called at the beginning of the simulation to
        provide a list of neighbors and the costs on those one-hop links.

        Arguments:
        - `neighbor_costs`:  Array of (entity_index, cost) tuples for
                             one-hop neighbors of this entity in this network.

        Return Value: This function should return an array of `Packet`s to be
        sent from this entity (if any) to neighboring entities.
        '''

        dest_arr = []
        ret_arr = []
        cost_arr = [999] * self.number_of_entities
        cost_arr[self.index] = 0

        y = 0
        while y < len(neighbor_costs):
            cost_arr[neighbor_costs[y][0]] = neighbor_costs[y][1]
            dest_arr.append(neighbor_costs[y][0])
            self.nextHop[neighbor_costs[y][0]] = neighbor_costs[y][0]
            y += 1

        z = 0
        while z < len(dest_arr):
            ret_arr.append(packet.Packet(dest_arr[z], cost_arr))
            z += 1

        self.nextHop[self.index] = self.index
        self.cost = cost_arr
        self.neighbors = dest_arr

        return ret_arr

    def update(self, pkt):
        '''
        This function is called when a packet arrives for this entity.

        Arguments:
        - `packet`: The incoming packet of type `Packet`.

        Return Value: This function should return an array of `Packet`s to be
        sent from this entity (if any) to neighboring entities.
        '''

        ret_arr = []
        changed = 0

        x = 0
        while x < len(pkt.costs):
            temp = self.cost[pkt.source] + pkt.costs[x]

            if self.cost[x] > temp:
                self.cost[x] = temp
                self.nextHop[x] = self.nextHop[pkt.source]
                changed = 1
            x += 1

        x = 0
        while x < len(self.neighbors):
            if changed == 1:
                ret_arr.append(packet.Packet(self.neighbors[x], self.cost))
            x += 1

        return ret_arr

    def get_all_costs(self):
        '''
        This function is used by the simulator to retrieve the calculated routes
        and costs from an entity. This is most useful at the end of the
        simulation to collect the resulting routing state.

        Return Value: This function should return an array of (next_hop, cost)
        tuples for _every_ entity in the network based on the entity's current
        understanding of those costs. The array should be sorted such that the
        first element of the array is the next hop and cost to entity index 0,
        second element is to entity index 1, etc.
        '''

        tupplin = []

        x = 0
        while x < self.number_of_entities:
            temp = (self.nextHop[x], self.cost[x])
            tupplin.append(temp)
            x += 1

        return tupplin

    def forward_next_hop(self, destination):
        '''
        Return the best next hop for a packet with the given destination.

        Arguments:
        - `destination`: The final destination of the packet.

        Return Value: The index of the best neighboring entity to use as the
        next hop.
        '''

        return self.nextHop[destination]

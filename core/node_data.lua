-- import the additional library
-- require("../additional/add_node_data.lua")

-- list of all availible nodes which will be accessed via C++
AVAILIBLE_NODES = {}

NODE_DATA_TEMPLATE = {
    name = "abc",

    on_land = function() end,
    on_leave = function() end
}

-- will be expanded to include mod data
function create_node()
    -- append a new node
    return NODE_DATA_TEMPLATE
end

-- function to add a node to the table
function add_node(node)
    AVAILIBLE_NODES[#AVAILIBLE_NODES + 1] = node
end

node = create_node()
add_node(node)
add_node(node)
add_node(node)
add_node(node)
add_node(node)
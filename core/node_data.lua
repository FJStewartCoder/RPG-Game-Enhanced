-- import the additional library
-- require("../additional/add_node_data.lua")

-- list of all availible nodes which will be accessed via C++
AVAILIBLE_NODES = {}

NODE_DATA_TEMPLATE = {
    name = "abc",

    on_land = function() end,
    on_leave = function() end
}

function create_node()
    -- append a new node
    AVAILIBLE_NODES[#AVAILIBLE_NODES + 1] = NODE_DATA_TEMPLATE
end

create_node()
create_node()
create_node()
create_node()
create_node()
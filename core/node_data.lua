-- list of all availible nodes which will be accessed via C++
AVAILIBLE_NODES = {}

NODE_DATA_TEMPLATE = {
    name = "abc",

    on_land = function() end,
    on_leave = function() end
}

----------------------------------------------------------------------------

-- gets the player name
function start(data, node, player)
    print("Hello this is a node type of " ..node.name)
    node.abc = "New thing"

    print("New property added ABC with value " ..node.abc)

    print("Got data data1 " ..data.data1)

    print("The player's name is " ..player.name)
end

function start_leave(data, node, player)
    -- manage position from the script
    player.position_id = 3
end

----------------------------------------------------------------------------

function build()
    add_node({name = "Start", on_land = start, on_leave = start_leave, random = 2})
    add_node({name = "2", on_land = function () end, on_leave = function () end})
    add_node({name = "3", on_land = function () end, on_leave = function () end})
    add_node({name = "4", on_land = function () end, on_leave = function () end})

    return 0
end
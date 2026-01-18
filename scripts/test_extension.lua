function start(data, node, player)
    print("Hello this is a node type of " ..node.name)
    node.abc = "New thing"

    print("New property added ABC with value " ..node.abc)

    -- print("Got data data1 " ..data.data1)

    print("The player's name is " ..player.name)
end
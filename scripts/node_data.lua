-- gets the player name
function start(data, node, player)
    print("Hello this is a node type of " ..node.name)
    node.abc = "New thing"

    print("New property added ABC with value " ..node.abc)

    -- print("Got data data1 " ..data.data1)

    print("The player's name is " ..player.name)

    selection = alt_menu("ABC", "ADSKJDAHS", {"ABC", "ABC2", "ABC3"})
    print(selection)
end

function start_leave(data, node, player)
    -- manage position from the script
    player.position_id = 3
end
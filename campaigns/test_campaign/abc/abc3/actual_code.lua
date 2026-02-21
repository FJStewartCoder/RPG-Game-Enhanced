-- gets the player name
function start(data, player)
    new_table = {
        name = "hello"
    }

    other_table = copy_table( new_table )
    other_table.name = "not hello"

    show_table(new_table)
    show_table(other_table)


    ----------------------------------------------------------------


    node = {
        name = "NO LONGER IMPLEMENTED"
    }

    print("Hello this is a node type of " ..node.name)
    node.abc = "New thing"

    print("New property added ABC with value " ..node.abc)

    -- print("Got data data1 " ..data.data1)

    print("The player's name is " ..player.name)

    selection = alt_menu("ABC", "ADSKJDAHS", {"ABC", "ABC2", "ABC3"})
    print(selection)

    player.position.x = 2
    player.position.y = 5
    player.position.z = 1
end

function start_leave(data, player)
    -- manage position from the script
    --player.position.x = 3

    player.position.x = 2
    player.position.y = 5
    player.position.z = 1

    table1 = {
        name = "hello",
        bob = 3,
        secret_toad = {
            name = "Bob",
            age = 5
        }
    }

    table2 = {
        name = 10,
        bob = 7,
        new_thing = "TOAD",
        secret_toad = {
            chicken = 123
        }
    }

    -- play ground for merging tables
    table3 = merge_table( table1, table2, "oad" )
    show_table(table3)
end
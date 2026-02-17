CAMPAIGN_NAME = "Sample Game"

-- array of functions for building the environment
-- essentially a lazy namespace
build_env = {

--  ps
-- pSs
-- HH
-- (H)ouse, (S)tart, (p)ath, (s)hop

village = function ()
    build_node("Path", 0, 0, 1, {}, "")
    build_node(
        "Shop",
        1, 0, 1,
        {
            name = "Items",
            items = {
                { name = "Health Potion", cost = 100 }
            }
        },
        "!l"
    )  -- only allow entrance left

    build_node("Path", -1, 0, 0, {}, "")
    build_node("Start", 0, 0, 0, {}, "")
    build_node(
        "Shop",
        1, 0, 0,
        {
            name = "Weapons",
            items = {
                { name = "Sword", cost = 100 },
                { name = "Shield", cost = 75 },
                { name = "Rock", cost = 0 }
            }
        },
        "!l"
    )  -- only allow entrance left

    build_node(
        "House",
        -1, 0, -1,
        {
            name = "Bob"
        },
        "!f"
    )  -- only allow entrace from the front

    build_node("House", 0, 0, -1, {}, "!f")  -- only allow entrace from the front
end,

--  d
-- VVV
-- VVV
-- VVV
-- (V)illage, (d)ragon

dragon = function ()
    build_node("Fight", 0, 0, 2, {}, "")
end

}

function build()
    add_node({name = "Start", on_land = land_start, on_leave = leave_start})
    add_node({name = "Fight", on_land = land_fight, on_leave = leave_fight})
    add_node({name = "Shop", on_land = land_shop, on_leave = leave_shop})
    add_node({name = "House", on_land = land_house, on_leave = leave_house})
    add_node({name = "Path"})
end

function extend()
    extend_player({
        health = 100,
        money = 0,
        wanted_level = 0,
        inventory = {},
        bosses_defeated = {
            dragon = false
        }
    })
end

function environment()
    build_env.village()
    build_env.dragon()
end